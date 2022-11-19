#pragma once

#include <iostream>
#include "windows.h"
#include <sql.h>
#include <sqlext.h>
#include <string>
#include <odbcinst.h>
#include <stdlib.h>

using namespace std;

#define ID_LEN 8
#define NAME_LEN 255
#define SQL_LEN 512

class DB_Operations {

private:
	SQLHENV environment = SQL_NULL_HENV;
	SQLHDBC connection = SQL_NULL_HDBC; // Дескриптор подключения
	SQLHSTMT statement = SQL_NULL_HSTMT;

	//SQLHANDLE SQLConnectionHandle = NULL;
	SQLRETURN retCode = 0;

private:
	void connect();

public:
	DB_Operations();
	~DB_Operations();

	const SQLHSTMT& execute(const char* sql);
	void checkBD();
	void createTables();
};

void showSQLError(unsigned int handleType, const SQLHANDLE& handle) {
	SQLWCHAR SQLState[1024];
	SQLWCHAR message[1024];
	if (SQLGetDiagRec(handleType, handle, 1, SQLState, NULL, message, 1024, NULL) == SQL_SUCCESS) {
		cout << "SQL driver message: " << message << endl << "SQL State: " << SQLState << "." << endl;
	}
}

void DB_Operations::connect() {
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &environment) == SQL_ERROR)
	{
		fwprintf(stderr, L"Unable to allocate an environment handle\n");
		exit(-1);
	}

	if (SQLSetEnvAttr(environment, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0) == SQL_ERROR) {
		fwprintf(stderr, L"Unable to set an environment variable\n");
		exit(-1);
	}

	if (SQLAllocHandle(SQL_HANDLE_DBC, environment, &connection) == SQL_ERROR) {
		fwprintf(stderr, L"Unable to allocate an connection handle\n");
		exit(-1);
	}
/*
	if (SQLConnect(connection, (SQLWCHAR*)L"PostgreSQL 15", SQL_NTS, (SQLWCHAR*)L"odbc_user", SQL_NTS, (SQLWCHAR*)L"12345", SQL_NTS) == SQL_ERROR) {
		fwprintf(stderr, L"Unable to connect\n");
		exit(-1);
	}
	//*/
	
	//или PostgreSQL ODBC Driver(UNICODE)
	///*
	SQLWCHAR retConString[1024];
	switch (SQLDriverConnect(connection, NULL, (SQLWCHAR*)TEXT("Dsn=PostgreSQL 15;\
																Driver={PostgreSQL ODBC Driver(UNICODE)};\
																Uid=odbc_user;\
																Pwd=12345;"), 
		SQL_NTS, retConString, 1024, NULL, SQL_DRIVER_NOPROMPT)) {
	case SQL_SUCCESS:
		retCode = 0;
		break;
	case SQL_SUCCESS_WITH_INFO:
		break;
	case SQL_NO_DATA_FOUND:
		showSQLError(SQL_HANDLE_DBC, connection);
		retCode = -1;
		break;
	case SQL_INVALID_HANDLE:
		showSQLError(SQL_HANDLE_DBC, connection);
		retCode = -1;
		break;
	case SQL_ERROR:
		showSQLError(SQL_HANDLE_DBC, connection);
		retCode = -1;
		fwprintf(stderr, L"Unable to connect\n");
		exit(-1);
		break;
	default: 
		break;
	}
	//*/

	if (SQLAllocHandle(SQL_HANDLE_STMT, connection, &statement) == SQL_ERROR) {
		fwprintf(stderr, L"Unable to allocate stmt handle\n");
		exit(-1);
	}

	checkBD();
}

DB_Operations::DB_Operations() {
	connect();
}

DB_Operations::~DB_Operations()
{
	if (this->statement != SQL_NULL_HSTMT)
		SQLFreeHandle(SQL_HANDLE_STMT, this->statement);

	if (this->connection != SQL_NULL_HDBC) {
		SQLDisconnect(this->connection);
		SQLFreeHandle(SQL_HANDLE_DBC, this->connection);
	}

	if (this->environment != SQL_NULL_HENV)
		SQLFreeHandle(SQL_HANDLE_ENV, this->environment);
}

const SQLHSTMT& DB_Operations::execute(const char* sql)
{
	// mbstowcs: unsafe-операция (устарела и не проверяет переполнение буфера) 
	//#define в main позволяет использовать её
	size_t cSize = strlen(sql) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, sql, cSize);

	// Необходимо преобразование в UNICODE перед execute в SQLExecDirect
	SQLRETURN ret = SQLExecDirect(statement,
		(SQLWCHAR*)wc, SQL_NTS);
	if (ret == SQL_SUCCESS) {
		return this->statement;
	}
	return nullptr;
}

void DB_Operations::checkBD() {
	//vector<pair<string, bool>> have;
	//vector<string> tableNames;
	char buf[SQL_LEN];
	SQLRETURN retcode2;
	SQLWCHAR tables[NAME_LEN];

	// Получить список всех существующих таблиц в базе
	sprintf_s(buf, "SELECT table_name FROM information_schema.tables\
					WHERE table_schema NOT IN('information_schema', 'pg_catalog')\
					AND table_schema IN('public', 'myschema');");
	SQLHSTMT statement2 = execute(buf);
	retcode2 = SQLBindCol(statement2, 1, SQL_C_CHAR, &tables, NAME_LEN, nullptr);

	while (true) {
		retcode2 = SQLFetch(statement2);
		if (retcode2 == SQL_SUCCESS || retcode2 == SQL_SUCCESS_WITH_INFO) {
			//tableNames.push_back(reinterpret_cast<char*>(tables));
			SQLFreeStmt(statement2, SQL_CLOSE);
			memset(buf, 0, SQL_LEN); // Очистка буфера для запросов
			return;
		}
		else {
			if (retcode2 != SQL_NO_DATA || retcode2 == 100) {
				createTables();
				break;
			}
		}
	}
	SQLFreeStmt(statement2, SQL_CLOSE);
	memset(buf, 0, SQL_LEN); // Очистка буфера для запросов

	//return have;
}

void DB_Operations::createTables() {
	SQLRETURN retcode;
	retcode = SQLExecDirect(statement,
		(SQLWCHAR*)L"CREATE TABLE IF NOT EXISTS cabinets(\
					id_cabinet SERIAL NOT NULL UNIQUE,\
					cabinet_number VARCHAR(255) NOT NULL,\
					PRIMARY KEY(id_cabinet)\
					);\
					\
					DELETE FROM cabinets WHERE id_cabinet >= 1;\
					ALTER SEQUENCE cabinets_id_cabinet_seq RESTART WITH 1;\
					\
					INSERT INTO cabinets(cabinet_number) VALUES\
					('101'), ('102'), ('102а'), ('103'), ('104'), ('105'),\
					('201'), ('202'), ('202а'), ('202б'), ('203'), ('204'),\
					('301'), ('301а'), ('302'), ('303'), ('304'), ('305'),\
					('401'), ('402'), ('403'), ('405'), ('405а'), ('405б');",
					SQL_NTS);
	retcode = SQLFreeStmt(statement, SQL_CLOSE);

	retcode = SQLExecDirect(statement,
		(SQLWCHAR*)L"CREATE TABLE IF NOT EXISTS specializations(\
					id_specialization SERIAL NOT NULL UNIQUE,\
					specialization_name VARCHAR(255) NOT NULL,\
					PRIMARY KEY(id_specialization)\
					);\
					\
					DELETE FROM specializations WHERE id_specialization >= 1;\
					ALTER SEQUENCE specializations_id_specialization_seq RESTART WITH 1;\
					\
					INSERT INTO specializations(specialization_name) VALUES\
					('Гастроэнтеролог'), ('Кардиолог'), ('Офтальмолог'), ('Терапевт'), ('Педиатр'), ('Эндокринолог'),\
					('Онколог'), ('Ортодонт'), ('Психиатр'), ('Рентгенолог'), ('Стоматолог'), ('Уролог'),\
					('Гинеколог'), ('Инфекционист'), ('Ревматолог'), ('Фтизиатр'), ('Хирург'), ('Врач мануальной терапии'),\
					('Колопроктолог'), ('Нефролог');",
					SQL_NTS);
	retcode = SQLFreeStmt(statement, SQL_CLOSE);

	retcode = SQLExecDirect(statement,
		(SQLWCHAR*)L"CREATE TABLE IF NOT EXISTS plots(\
		id_plot SERIAL NOT NULL UNIQUE,\
		plot_number INT NOT NULL,\
		PRIMARY KEY(id_plot)\
		);\
		\
		DELETE FROM plots WHERE id_plot >= 1;\
		ALTER SEQUENCE plots_id_plot_seq RESTART WITH 1;\
		\
		INSERT INTO plots(plot_number) VALUES\
		(1), (2), (3), (4);",
		SQL_NTS);
	retcode = SQLFreeStmt(statement, SQL_CLOSE);

	retcode = SQLExecDirect(statement,
		(SQLWCHAR*)L"CREATE TABLE IF NOT EXISTS days_of_week(\
		id_day_of_week SERIAL NOT NULL UNIQUE,\
		day_of_week VARCHAR(255) NOT NULL,\
		PRIMARY KEY(id_day_of_week)\
		);\
		\
		DELETE FROM days_of_week WHERE id_day_of_week >= 1;\
		ALTER SEQUENCE days_of_week_id_day_of_week_seq RESTART WITH 1;\
		\
		INSERT INTO days_of_week(day_of_week) VALUES\
		('Понедельник'),('Вторник'),('Среда'),('Четверг'),('Пятница'),('Суббота'),('Воскресенье');",
		SQL_NTS);
	retcode = SQLFreeStmt(statement, SQL_CLOSE);

	retcode = SQLExecDirect(statement,
		(SQLWCHAR*)L"CREATE TABLE IF NOT EXISTS times_of_job(\
		id_time_of_job SERIAL NOT NULL UNIQUE,\
		time_value TIME WITHOUT TIME ZONE NOT NULL,\
		PRIMARY KEY(id_time_of_job)\
		);\
		\
		DELETE FROM times_of_job WHERE id_time_of_job >= 1;\
		ALTER SEQUENCE times_of_job_id_time_of_job_seq RESTART WITH 1;\
		\
		INSERT INTO times_of_job(time_value) VALUES\
		('8:00'),('8:15'),('8:30'),('8:45'),\
		('9:00'),('9:15'),('9:30'),('9:45'),\
		('10:00'),('10:15'),('10:30'),('10:45'),\
		('11:00'),('11:15'),('11:30'),('11:45'),\
		('12:00'),('12:15'),('12:30'),('12:45'),\
		('13:00'),('13:15'),('13:30'),('13:45'),\
		('14:00'),('14:15'),('14:30'),('14:45'),\
		('15:00'),('15:15'),('15:30'),('15:45'),\
		('16:00'),('16:15'),('16:30'),('16:45'),\
		('17:00'),('17:15'),('17:30'),('17:45'),\
		('18:00'),('18:15'),('18:30'),('18:45'), ('19:00');",
		SQL_NTS);
	retcode = SQLFreeStmt(statement, SQL_CLOSE);

	retcode = SQLExecDirect(statement,
		(SQLWCHAR*)L"CREATE TABLE IF NOT EXISTS doctors(\
		id_doctor SERIAL NOT NULL UNIQUE,\
		last_name VARCHAR(255) NOT NULL,\
		first_name VARCHAR(255) NOT NULL,\
		middle_name VARCHAR(255),\
		id_specialization INT NOT NULL DEFAULT 0,\
		id_cabinet INT NOT NULL DEFAULT 0,\
		id_plot INT NOT NULL DEFAULT 0,\
		PRIMARY KEY(id_doctor),\
		FOREIGN KEY(id_specialization)\
		REFERENCES specializations(id_specialization) ON UPDATE CASCADE ON DELETE SET DEFAULT,\
		FOREIGN KEY(id_cabinet)\
		REFERENCES cabinets(id_cabinet) ON UPDATE CASCADE ON DELETE SET DEFAULT,\
		FOREIGN KEY(id_plot)\
		REFERENCES plots(id_plot) ON UPDATE CASCADE ON DELETE SET DEFAULT\
		);",
		SQL_NTS);
	retcode = SQLFreeStmt(statement, SQL_CLOSE);

	retcode = SQLExecDirect(statement,
		(SQLWCHAR*)L"CREATE TABLE IF NOT EXISTS dw_doc_identity(\
		id_day_of_week INT NOT NULL,\
		id_doctor INT NOT NULL,\
		id_begin INT,\
		id_end INT,\
		FOREIGN KEY(id_day_of_week)\
		REFERENCES days_of_week(id_day_of_week) ON UPDATE CASCADE ON DELETE CASCADE,\
		FOREIGN KEY(id_doctor)\
		REFERENCES doctors(id_doctor) ON UPDATE CASCADE ON DELETE CASCADE,\
		FOREIGN KEY(id_begin)\
		REFERENCES times_of_job(id_time_of_job) ON UPDATE CASCADE ON DELETE SET NULL,\
		FOREIGN KEY(id_end)\
		REFERENCES times_of_job(id_time_of_job) ON UPDATE CASCADE ON DELETE SET NULL\
		);",
		SQL_NTS);
	retcode = SQLFreeStmt(statement, SQL_CLOSE);

	retcode = SQLExecDirect(statement,
		(SQLWCHAR*)L"CREATE TABLE IF NOT EXISTS patients(\
		id_patient SERIAL NOT NULL UNIQUE,\
		last_name VARCHAR(255) NOT NULL,\
		first_name VARCHAR(255) NOT NULL,\
		middle_name VARCHAR(255),\
		city VARCHAR(255) NOT NULL,\
		street VARCHAR(255) NOT NULL,\
		building VARCHAR(255) NOT NULL,\
		apartment VARCHAR(255) NOT NULL,\
		PRIMARY KEY(id_patient)\
		);",
		SQL_NTS);
	retcode = SQLFreeStmt(statement, SQL_CLOSE);

	retcode = SQLExecDirect(statement,
		(SQLWCHAR*)L"CREATE TABLE IF NOT EXISTS medications(\
		id_medication SERIAL NOT NULL UNIQUE,\
		medication_name VARCHAR(255) NOT NULL,\
		PRIMARY KEY(id_medication)\
		);",
		SQL_NTS);
	retcode = SQLFreeStmt(statement, SQL_CLOSE);

	retcode = SQLExecDirect(statement,
		(SQLWCHAR*)L"CREATE TABLE IF NOT EXISTS proceduress(\
		id_procedure SERIAL NOT NULL UNIQUE,\
		procedure_name VARCHAR(255) NOT NULL,\
		PRIMARY KEY(id_procedure)\
		);",
		SQL_NTS);
	retcode = SQLFreeStmt(statement, SQL_CLOSE);

	retcode = SQLExecDirect(statement,
		(SQLWCHAR*)L"CREATE TABLE IF NOT EXISTS analyses(\
		id_analysis SERIAL NOT NULL UNIQUE,\
		analysis_name VARCHAR(255) NOT NULL,\
		PRIMARY KEY(id_analysis)\
		);\
		\
		DELETE FROM analyses WHERE id_analysis >= 1;\
		ALTER SEQUENCE analyses_id_analysis_seq RESTART WITH 1;\
		\
		INSERT INTO analyses(analysis_name) VALUES\
		('Общий анализ крови'), ('Биохимический анализ крови'), ('Общий анализ кала, копрограмму'), ('Эндоскопия верхнего отдела ЖКТ'),\
		('Исследование прямой кишки и дистального отдела сигмовидной кишки'), ('Инструментальное исследование прямой кишки'),\
		('ФГДС с биопсией'),('УЗИ органов брюшной полости – пищевода, желудка, кишечника'),\
		\
		('Общий анализ на холестерин, ЛПВП'),('Общий анализ мочи'),('Анализ крови на глюкозу'),\
		('Аспартатаминотрансфераза, аланинаминотрансфераза и гама-глютамлтрансфераза'),('Креатинин и мочевина'),\
		\
		('Коагулограмма (исследование показателей свертываемости крови)'),('Обнаружение вирусов гепатита В и С, ВИЧ-инфекции, сифилиса'),\
		\
		('Гормоны крови: ТТГ, свободный Т4, АТ к ТПО'),('Гликированный гемоглобин'),\
		\
		('Анализы на свертываемость крови'),('Иммунологический анализ крови: онкомаркеры'),\
		\
		('Фотопротокол'),('Прицельный снимок'),('КЛКТ'),('ОПТГ'),('РТГ'),\
		\
		('Гормоны щитовидной железы'),('Определение концентрации препарата в крови (в частности, лития)'),\
		\
		('Анализ на сифилис'),('Анализ на гепатиты B и C'),('Коагулограмма'),('Анализ на группу крови и резус-фактор'),\
		\
		('Мазок на инфекции, передающиеся половым путем'),('Забор секрета предстательной железы'),('Сбор спермы'),('Спермограмма'),\
		('Кровь на гормоны'),('Кровь на простатспецифический антиген (ПСА)'),('УЗИ мочевого пузыря'),\
		\
		('Посев и мазок на флору'),('ПЦР-диагностика'),('Серологическая диагностика на предмет кишечных и респираторных заболеваний'),\
		('Анализ на определение маркеров в крови на предмет вирусного гепатита'),('ИФА - иммуноферментный анализ крови'),\
		\
		('Анализ крови на уровень мочевой кислоты'),('Анализ крови на антинуклеарные антитела'),('Исследование крови на ревматоидный фактор'),\
		('Выявление антител к циклическому цитруллин-содержащему пептиду'),('Анализ крови на С-реактивный белок'),\
		\
		('Соскоб на яйца глистов и простейшие'),('Флюорография'),('МСКТ'),\
		\
		('Коагулограмма (АЧТВ, фибриноген, МНО, ПТИ)'),\
		\
		('Дуплексное сканирование суставов'),('Рентген'),('ЭМГ'),('Компьютерная томография'),('МРТ'),\
		\
		('УЗИ почек, мочевого тракта и органов брюшной полости'),('Компьютерную томографию почек (КТ)'),('рентгенологическое исследование почек;'),\
		('Биопсия почек'),('МРТ почек');",
		SQL_NTS);
	retcode = SQLFreeStmt(statement, SQL_CLOSE);

	retcode = SQLExecDirect(statement,
		(SQLWCHAR*)L"CREATE TABLE IF NOT EXISTS analyses_results(\
		id_analysis_result SERIAL NOT NULL UNIQUE,\
		id_analysis INT NOT NULL,\
		id_patient INT NOT NULL,\
		analysis_result VARCHAR NOT NULL,\
		PRIMARY KEY(id_analysis_result),\
		FOREIGN KEY(id_analysis)\
		REFERENCES analyses(id_analysis) ON UPDATE CASCADE ON DELETE CASCADE,\
		FOREIGN KEY(id_patient)\
		REFERENCES patients(id_patient) ON UPDATE CASCADE ON DELETE CASCADE\
		);",
		SQL_NTS);
	retcode = SQLFreeStmt(statement, SQL_CLOSE);

	retcode = SQLExecDirect(statement,
		(SQLWCHAR*)L"CREATE TABLE IF NOT EXISTS receptions(\
		id_reception SERIAL NOT NULL UNIQUE,\
		date_of_reception DATE NOT NULL,\
		complaints VARCHAR,\
		diagnosis VARCHAR,\
		date_of_extract DATE,\
		is_done BOOL NOT NULL,\
		time_of_reception TIME WITHOUT TIME ZONE NOT NULL,\
		PRIMARY KEY(id_reception)\
		);",
		SQL_NTS);
	retcode = SQLFreeStmt(statement, SQL_CLOSE);

	retcode = SQLExecDirect(statement,
		(SQLWCHAR*)L"CREATE TABLE IF NOT EXISTS rec_med_identity(\
		id_reception INT NOT NULL,\
		id_medication INT NOT NULL,\
		FOREIGN KEY(id_reception)\
		REFERENCES receptions(id_reception) ON UPDATE CASCADE ON DELETE CASCADE,\
		FOREIGN KEY(id_medication)\
		REFERENCES medications(id_medication) ON UPDATE CASCADE ON DELETE CASCADE\
		);",
		SQL_NTS);
	retcode = SQLFreeStmt(statement, SQL_CLOSE);

	retcode = SQLExecDirect(statement,
		(SQLWCHAR*)L"CREATE TABLE IF NOT EXISTS rec_proc_identity(\
		id_reception INT NOT NULL,\
		id_procedure INT NOT NULL,\
		FOREIGN KEY(id_reception)\
		REFERENCES receptions(id_reception) ON UPDATE CASCADE ON DELETE CASCADE,\
		FOREIGN KEY(id_procedure)\
		REFERENCES proceduress(id_procedure) ON UPDATE CASCADE ON DELETE CASCADE\
		);",
		SQL_NTS);
	retcode = SQLFreeStmt(statement, SQL_CLOSE);

	retcode = SQLExecDirect(statement,
		(SQLWCHAR*)L"CREATE TABLE IF NOT EXISTS rec_an_identity(\
		id_reception INT NOT NULL,\
		id_analysis INT NOT NULL,\
		FOREIGN KEY(id_reception)\
		REFERENCES receptions(id_reception) ON UPDATE CASCADE ON DELETE CASCADE,\
		FOREIGN KEY(id_analysis)\
		REFERENCES analyses(id_analysis) ON UPDATE CASCADE ON DELETE CASCADE\
		);",
		SQL_NTS);
	retcode = SQLFreeStmt(statement, SQL_CLOSE);

	retcode = SQLExecDirect(statement,
		(SQLWCHAR*)L"CREATE TABLE IF NOT EXISTS pat_rec_identity(\
		id_patient INT NOT NULL,\
		id_reception INT NOT NULL,\
		FOREIGN KEY(id_patient)\
		REFERENCES patients(id_patient) ON UPDATE CASCADE ON DELETE CASCADE,\
		FOREIGN KEY(id_reception)\
		REFERENCES receptions(id_reception) ON UPDATE CASCADE ON DELETE CASCADE\
		);",
		SQL_NTS);
	retcode = SQLFreeStmt(statement, SQL_CLOSE);

	retcode = SQLExecDirect(statement,
		(SQLWCHAR*)L"CREATE TABLE IF NOT EXISTS doc_rec_identity(\
		id_doctor INT NOT NULL,\
		id_reception INT NOT NULL,\
		FOREIGN KEY(id_doctor)\
		REFERENCES doctors(id_doctor) ON UPDATE CASCADE ON DELETE CASCADE,\
		FOREIGN KEY(id_reception)\
		REFERENCES receptions(id_reception) ON UPDATE CASCADE ON DELETE CASCADE\
		);",
		SQL_NTS);
	retcode = SQLFreeStmt(statement, SQL_CLOSE);
}