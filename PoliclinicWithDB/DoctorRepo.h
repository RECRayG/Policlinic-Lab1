#pragma once

#include "Doctor.h"
#include "InterfaceRepo.h"

#include "PoliclinicStorage.h"

#include <iostream>
#include <list>
#include <algorithm>
#include <functional>
#include <iterator>
#include <string>
#include <sstream>

#define ID_LEN 8
#define NAME_LEN 255
#define SQL_LEN 512

class DoctorRepo : public InterfaceRepo<Doctor, long> {
private:
	SQLINTEGER id_doctor, id_specialization, id_cabinet, id_plot;
	SQLWCHAR last_name[NAME_LEN];
	SQLWCHAR first_name[NAME_LEN];
	SQLWCHAR middle_name[NAME_LEN];
	SQLWCHAR full_name[NAME_LEN*3];
	SQLWCHAR specialization[NAME_LEN];
	SQLWCHAR cabinet[NAME_LEN];
	SQLINTEGER plot;
public:
	DoctorRepo(DB_Operations& dbOperations);

	// Вспомогательные методы
	void alterSeqDoctors(long idStart);
	//Doctor* findById(long id) override; // Поиск доктора по идентификатору
	std::vector<Doctor*> findAll() override; // Список всех врачей
	//Doctor findDoctor();

	// Основные методы из меню
	void add(const Doctor& doctor) override; // Add/Edit - Сохранить доктора (вставка если новый, апдейт если уже есть)
	void remove(const Doctor& doctor) override; // Удалить доктора из базы
	void edit(const Doctor& doctor) override; // Изменить существующего в базе доктора

	std::string getAllSpecializations(); // Получить список всех специальностей врачей ( 1)  Хирург\n 2)  Педиатр\n и т.д )
	int getCountSpecializations(); // Вернуть результат работы оператора COUNT(*) в PostgreSQL для специальностей

	std::string getAllCabinets();
	int getCountCabinets();

	int getCountPlots(); // Участки можно выстроить циклом по количеству, т.к. это всегда просто чилса по возрастанию

	std::string getAllDaysOfWeek();
	int getIdByValueDaysOfWeek(string day);

	std::string getAllTimesOfJob();
	int getCountTimesOfJob();
	int getIdByValueTimesOfJob(string time);
	vector<string> getDifferenceAppendTimetable(vector<pair<string, pair<string, string>>> first, vector<pair<string, pair<string, string>>> second);
	void removeTimetablePosition(Doctor& doctor, vector<int> pos);
	vector<pair<string,string>> getTimetableToDoctor(const Doctor& doctor);

	pair<string, pair<string, string>> checkJobAtDayOfWeek(Doctor* doctor, int idDay);

	vector<int> getOtherIdForDoctor(const Doctor& doctor); // Получить другие id, использующиеся для таблицы doctors
	int getMaxDoctorId(); // Получить значение максимального существующего id в таблице doctors
	vector<string> splitFullNameDoctor(const Doctor& doctor); // Разделение строки с полным именем на 3 или 2 составляющие
	vector<pair<string, pair<string, string>>> splitDoctorTimetable(const Doctor& doctor);
};

DoctorRepo::DoctorRepo(DB_Operations& dbOperations) :
	InterfaceRepo(dbOperations),
	id_doctor(),
	id_specialization(),
	id_cabinet(), 
	id_plot(),
	last_name(),
	first_name(),
	middle_name(),
	full_name(),
	specialization(),
	cabinet(),
	plot() {
}

std::vector<Doctor*> DoctorRepo::findAll() {
	char buf[SQL_LEN];
	std::vector<Doctor*> doctors;

	vector<pair<string,string>> timetableLocal;

	SQLHSTMT statement = dbOperations.execute("SELECT doc.id_doctor,\
												CONCAT(doc.last_name, ' ', doc.first_name, ' ', doc.middle_name) AS full_name,\
												sp.specialization_name, cab.cabinet_number, pl.plot_number\
												FROM doctors doc\
												INNER JOIN specializations sp ON sp.id_specialization = doc.id_specialization\
												INNER JOIN cabinets cab ON cab.id_cabinet = doc.id_cabinet\
												INNER JOIN plots pl ON pl.id_plot = doc.id_plot;");
	SQLRETURN retcode;
	retcode = SQLBindCol(statement, 1, SQL_C_LONG, &id_doctor, ID_LEN, nullptr);
	retcode = SQLBindCol(statement, 2, SQL_C_CHAR, &full_name, NAME_LEN, nullptr);
	retcode = SQLBindCol(statement, 3, SQL_C_CHAR, &specialization, NAME_LEN, nullptr);
	retcode = SQLBindCol(statement, 4, SQL_C_CHAR, &cabinet, NAME_LEN, nullptr);
	retcode = SQLBindCol(statement, 5, SQL_C_LONG, &plot, ID_LEN, nullptr);

	while (true) {
		retcode = SQLFetch(statement);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			Doctor doctor;
			doctor.setId(id_doctor);
			doctor.setName(reinterpret_cast<char*>(full_name));
			doctor.setSpecialization(reinterpret_cast<char*>(specialization));
			doctor.setCabinet(reinterpret_cast<char*>(cabinet));
			doctor.setPlot(plot);
			
			PoliclinicStorage::getInstance()->putDoctor(doctor);
			doctors.push_back(PoliclinicStorage::getInstance()->getDoctor(doctor.getId()));
		}
		else {
			if (retcode != SQL_NO_DATA) {
				std::cout << "Error fetching doctors\n";
			}
			break;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);
	memset(buf, 0, SQL_LEN); // Очистка буфера для запросов

	for (int i = 0; i < doctors.size(); i++) {
		doctors.at(i)->setTimetable(getTimetableToDoctor(doctors.at(i)));
		PoliclinicStorage::getInstance()->updateDoctor(doctors.at(i));
	}
	
	return doctors;
}

int DoctorRepo::getCountSpecializations() {
	SQLINTEGER countSp;
	int count = 0;

	SQLHSTMT statement = dbOperations.execute("SELECT COUNT(*) FROM specializations;");
	SQLRETURN retcode = SQLBindCol(statement, 1, SQL_C_LONG, &countSp, ID_LEN, nullptr);

	retcode = SQLFetch(statement);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		count = countSp;
	}
	else {
		if (retcode != SQL_NO_DATA) {
			std::cout << "Error fetching specializations\n";
			SQLFreeStmt(statement, SQL_CLOSE);
			return count;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);

	return count;
}
std::string DoctorRepo::getAllSpecializations() {
	SQLWCHAR sqlStr[NAME_LEN];
	std::string str = "";

	SQLHSTMT statement = dbOperations.execute("SELECT specialization_name FROM specializations;");
	SQLRETURN retcode = SQLBindCol(statement, 1, SQL_C_CHAR, &sqlStr, NAME_LEN, nullptr);

	int i = 0;
	while (true) {
		retcode = SQLFetch(statement);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			str += std::to_string(i+1) + ")" + " " + reinterpret_cast<char*>(sqlStr) + "\n";
		}
		else {
			if (retcode != SQL_NO_DATA) {
				std::cout << "Error fetching specializations\n";
			}
			break;
		}

		i++;
	}
	SQLFreeStmt(statement, SQL_CLOSE);

	return str;
}

int DoctorRepo::getCountCabinets() {
	SQLINTEGER countCab;
	int count = 0;

	SQLHSTMT statement = dbOperations.execute("SELECT COUNT(*) FROM cabinets;");
	SQLRETURN retcode = SQLBindCol(statement, 1, SQL_C_LONG, &countCab, ID_LEN, nullptr);

	retcode = SQLFetch(statement);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		count = countCab;
	}
	else {
		if (retcode != SQL_NO_DATA) {
			std::cout << "Error fetching cabinets\n";
			SQLFreeStmt(statement, SQL_CLOSE);
			return count;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);

	return count;
}
std::string DoctorRepo::getAllCabinets() {
	SQLWCHAR sqlStr[NAME_LEN];
	std::string str = "";

	SQLHSTMT statement = dbOperations.execute("SELECT cabinet_number FROM cabinets;");
	SQLRETURN retcode = SQLBindCol(statement, 1, SQL_C_CHAR, &sqlStr, NAME_LEN, nullptr);

	int i = 0;
	while (true) {
		retcode = SQLFetch(statement);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			str += std::to_string(i + 1) + ")" + " " + reinterpret_cast<char*>(sqlStr) + "\n";
		}
		else {
			if (retcode != SQL_NO_DATA) {
				std::cout << "Error fetching cabinets\n";
			}
			break;
		}

		i++;
	}
	SQLFreeStmt(statement, SQL_CLOSE);

	return str;
}


int DoctorRepo::getCountPlots() {
	SQLINTEGER countPl;
	int count = 0;

	SQLHSTMT statement = dbOperations.execute("SELECT COUNT(*) FROM plots;");
	SQLRETURN retcode = SQLBindCol(statement, 1, SQL_C_LONG, &countPl, ID_LEN, nullptr);

	retcode = SQLFetch(statement);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		count = countPl;
	}
	else {
		if (retcode != SQL_NO_DATA) {
			std::cout << "Error fetching plots\n";
			SQLFreeStmt(statement, SQL_CLOSE);
			return count;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);

	return count;
}

std::string DoctorRepo::getAllDaysOfWeek() {
	SQLCHAR sqlStr[NAME_LEN];
	std::string str = "";

	SQLHSTMT statement = dbOperations.execute("SELECT day_of_week FROM days_of_week;");
	SQLRETURN retcode = SQLBindCol(statement, 1, SQL_C_CHAR, &sqlStr, NAME_LEN, nullptr);

	int i = 0;
	while (true) {
		retcode = SQLFetch(statement);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			str += std::to_string(i + 1) + ")" + " " + reinterpret_cast<char*>(sqlStr) + "\n";
		}
		else {
			if (retcode != SQL_NO_DATA) {
				std::cout << "Error fetching days_of_week\n";
			}
			break;
		}

		i++;
	}
	SQLFreeStmt(statement, SQL_CLOSE);

	return str;
}


int DoctorRepo::getCountTimesOfJob() {
	SQLINTEGER countTj;
	int count = 0;

	SQLHSTMT statement = dbOperations.execute("SELECT COUNT(*) FROM times_of_job;");
	SQLRETURN retcode = SQLBindCol(statement, 1, SQL_C_LONG, &countTj, ID_LEN, nullptr);

	retcode = SQLFetch(statement);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		count = countTj;
	}
	else {
		if (retcode != SQL_NO_DATA) {
			std::cout << "Error fetching times_of_job\n";
			SQLFreeStmt(statement, SQL_CLOSE);
			return count;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);

	return count;
}
std::string DoctorRepo::getAllTimesOfJob() {
	SQLWCHAR sqlStr[NAME_LEN];
	std::string str = "";

	SQLHSTMT statement = dbOperations.execute("SELECT time_value FROM times_of_job;");
	SQLRETURN retcode = SQLBindCol(statement, 1, SQL_C_CHAR, &sqlStr, NAME_LEN, nullptr);

	int i = 0;
	while (true) {
		retcode = SQLFetch(statement);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			str += std::to_string(i + 1) + ")" + " " + reinterpret_cast<char*>(sqlStr) + "\n";
		}
		else {
			if (retcode != SQL_NO_DATA) {
				std::cout << "Error fetching times_of_job\n";
			}
			break;
		}

		i++;
	}
	SQLFreeStmt(statement, SQL_CLOSE);

	return str;
}

int DoctorRepo::getMaxDoctorId() {
	SQLINTEGER countTj;
	int count = 0;

	SQLHSTMT statement = dbOperations.execute("SELECT MAX(id_doctor) FROM doctors;");
	SQLRETURN retcode = SQLBindCol(statement, 1, SQL_C_LONG, &countTj, ID_LEN, nullptr);

	retcode = SQLFetch(statement);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		count = countTj;
	}
	else {
		if (retcode != SQL_NO_DATA) {
			std::cout << "Error fetching doctors\n";
			SQLFreeStmt(statement, SQL_CLOSE);
			return count;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);

	return count;
}



void DoctorRepo::add(const Doctor& doctor) {
	char buf[SQL_LEN];
	vector<int> ids;
	vector<string> splitName;
	vector<pair<string, pair<string, string>>> splitTimeJob;
	int idWeek;
	int idTimeBegin;
	int idTimeEnd;

	ids = getOtherIdForDoctor(doctor);
	splitName = splitFullNameDoctor(doctor);
	splitTimeJob = splitDoctorTimetable(doctor);

	if (splitName.size() == 3) {
		sprintf_s(buf, "INSERT INTO doctors(last_name,first_name,middle_name,id_specialization,id_cabinet,id_plot)\
		VALUES ('%s', '%s', '%s', %d, %d, %d);", splitName.at(0).c_str(), splitName.at(1).c_str(), splitName.at(2).c_str(), ids.at(0), ids.at(1), ids.at(2));
	}
	else {
		sprintf_s(buf, "INSERT INTO doctors(last_name,first_name,middle_name,id_specialization,id_cabinet,id_plot)\
		VALUES ('%s', '%s', NULL, %d, %d, %d);", splitName.at(0).c_str(), splitName.at(1).c_str(), ids.at(0), ids.at(1), ids.at(2));
	}
	
	SQLHSTMT statement = dbOperations.execute(buf);
	SQLFreeStmt(statement, SQL_CLOSE);
	memset(buf, 0, SQL_LEN);

	if (!doctor.getTimetable().empty()) {
		for (int i = 0; i < splitTimeJob.size(); i++) {
			idWeek = getIdByValueDaysOfWeek(splitTimeJob.at(i).first);
			idTimeBegin = getIdByValueTimesOfJob(splitTimeJob.at(i).second.first);
			idTimeEnd = getIdByValueTimesOfJob(splitTimeJob.at(i).second.second);

			sprintf_s(buf, "INSERT INTO dw_doc_identity(id_day_of_week, id_doctor, id_begin, id_end) VALUES\
			(%d, %d, %d, %d);", idWeek, doctor.getId(), idTimeBegin, idTimeEnd);

			statement = dbOperations.execute(buf);
			SQLFreeStmt(statement, SQL_CLOSE);
			memset(buf, 0, SQL_LEN);
		}
	}

	PoliclinicStorage::getInstance()->putDoctor(doctor);
}

vector<int> DoctorRepo::getOtherIdForDoctor(const Doctor& doctor) {
	char buf[SQL_LEN];
	vector<int> ids;

	sprintf_s(buf, "SELECT id_specialization FROM specializations WHERE specialization_name = '%s';", doctor.getSpecialization().c_str());
	SQLHSTMT statement = dbOperations.execute(buf);
	SQLRETURN retcode = SQLBindCol(statement, 1, SQL_C_LONG, &id_specialization, ID_LEN, nullptr);

	retcode = SQLFetch(statement);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		ids.push_back(id_specialization);
		SQLFreeStmt(statement, SQL_CLOSE);
	}
	else {
		if (retcode != SQL_NO_DATA) {
			std::cout << "Error fetching specializations\n";
			SQLFreeStmt(statement, SQL_CLOSE);
			ids.push_back(-1);
			return ids;
		}
	}
	memset(buf, 0, SQL_LEN);

	sprintf_s(buf, "SELECT id_cabinet FROM cabinets WHERE cabinet_number = '%s';", doctor.getCabinet().c_str());
	statement = dbOperations.execute(buf);
	retcode = SQLBindCol(statement, 1, SQL_C_LONG, &id_cabinet, ID_LEN, nullptr);

	retcode = SQLFetch(statement);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		ids.push_back(id_cabinet);
		SQLFreeStmt(statement, SQL_CLOSE);
	}
	else {
		if (retcode != SQL_NO_DATA) {
			std::cout << "Error fetching cabinets\n";
			SQLFreeStmt(statement, SQL_CLOSE);
			ids.push_back(-1);
			return ids;
		}
	}
	memset(buf, 0, SQL_LEN);

	sprintf_s(buf, "SELECT id_plot FROM plots WHERE plot_number = %d;", doctor.getPlot());
	statement = dbOperations.execute(buf);
	retcode = SQLBindCol(statement, 1, SQL_C_LONG, &id_plot, ID_LEN, nullptr);

	retcode = SQLFetch(statement);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		ids.push_back(id_plot);
		SQLFreeStmt(statement, SQL_CLOSE);
	}
	else {
		if (retcode != SQL_NO_DATA) {
			std::cout << "Error fetching plots\n";
			SQLFreeStmt(statement, SQL_CLOSE);
			ids.push_back(-1);
			return ids;
		}
	}
	memset(buf, 0, SQL_LEN);

	return ids;
}

vector<string> DoctorRepo::splitFullNameDoctor(const Doctor& doctor) {
	vector<string> split;

	// Алгоритм разделения строк по пробелу
	list<string> words;
	string tmp;
	istringstream ist(doctor.getName());
	while (ist >> tmp)
		words.push_back(tmp);

	list<string>::iterator it = words.begin();
	if (words.size() == 3) {
		split.push_back(*next(it, 0));
		split.push_back(*next(it, 1));
		split.push_back(*next(it, 2));
	}
	else if (words.size() == 2) {
		split.push_back(*next(it, 0));
		split.push_back(*next(it, 1));
	}

	return split;
}

void DoctorRepo::edit(const Doctor& doctor) {
	////////////////////////// !!!!!!!!!!!!!!!!!!!!!!!! Передаётся доктор с изменёнными параметрами
	char buf[SQL_LEN];
	vector<int> ids;
	vector<string> splitName;
	vector<pair<string, pair<string,string>>> splitTimeJob;
	vector<pair<string, pair<string, string>>> splitTimeJobDromSingleton;
	int idWeek;
	int idTimeBegin;
	int idTimeEnd;
	vector<string> newDays;

	ids = getOtherIdForDoctor(doctor);
	splitName = splitFullNameDoctor(doctor);

	// Если есть различия
	if (doctor != PoliclinicStorage::getInstance()->getDoctor(doctor.getId())) {
		if (splitName.size() == 3) {
			sprintf_s(buf, "UPDATE doctors SET last_name = '%s', first_name = '%s', middle_name = '%s',\
						id_specialization = %d, id_cabinet = %d, id_plot = %d\
						WHERE id_doctor = % d;", splitName.at(0).c_str(), splitName.at(1).c_str(),
				splitName.at(2).c_str(), ids.at(0), ids.at(1), ids.at(2), doctor.getId());
		}
		else {
			sprintf_s(buf, "UPDATE doctors SET last_name = '%s', first_name = '%s', middle_name = NULL,\
						id_specialization = %d, id_cabinet = %d, id_plot = %d\
						WHERE id_doctor = % d;", splitName.at(0).c_str(), splitName.at(1).c_str(),
				ids.at(0), ids.at(1), ids.at(2), doctor.getId());
		}

		SQLHSTMT statement = dbOperations.execute(buf);
		SQLFreeStmt(statement, SQL_CLOSE);
		memset(buf, 0, SQL_LEN);
	}

	// Если записи были до этого
	if (!PoliclinicStorage::getInstance()->getDoctor(doctor.getId())->getTimetable().empty()) {
		// Если размеры таблиц расписания совпадают (не было добавлено новых дней недели)
		if (!doctor.getTimetable().empty() && doctor.getTimetable().size() == PoliclinicStorage::getInstance()->getDoctor(doctor.getId())->getTimetable().size()) {
			splitTimeJob = splitDoctorTimetable(doctor);

			// Обновить существующие записи
			for (int i = 0; i < doctor.getTimetable().size(); i++) {
				sprintf_s(buf, "UPDATE dw_doc_identity SET id_begin = %d, id_end = %d\
								WHERE id_doctor = %d AND id_day_of_week = %d;", 
				getIdByValueTimesOfJob(splitTimeJob.at(i).second.first), getIdByValueTimesOfJob(splitTimeJob.at(i).second.second),
				doctor.getId(), getIdByValueDaysOfWeek(splitTimeJob.at(i).first));

				SQLHSTMT statement2 = dbOperations.execute(buf);
				SQLFreeStmt(statement2, SQL_CLOSE);
				memset(buf, 0, SQL_LEN);
			}
		} // Если размер новго расписания стал больше (добавились новые дни недели)
		else if (!doctor.getTimetable().empty() && doctor.getTimetable().size() > PoliclinicStorage::getInstance()->getDoctor(doctor.getId())->getTimetable().size()) {
			splitTimeJob = splitDoctorTimetable(doctor);
			splitTimeJobDromSingleton = splitDoctorTimetable(PoliclinicStorage::getInstance()->getDoctor(doctor.getId()));
			newDays = getDifferenceAppendTimetable(splitTimeJob, splitTimeJobDromSingleton);

			// Обновить существующие записи
			for (int i = 0; i < splitTimeJob.size(); i++) {
				for (int j = 0; j < splitTimeJobDromSingleton.size(); j++) {
					if (splitTimeJob.at(i).first == splitTimeJobDromSingleton.at(j).first) {
						sprintf_s(buf, "UPDATE dw_doc_identity SET id_begin = %d, id_end = %d\
										WHERE id_doctor = %d AND id_day_of_week = %d;",
						getIdByValueTimesOfJob(splitTimeJob.at(i).second.first), 
						getIdByValueTimesOfJob(splitTimeJob.at(i).second.second),
						doctor.getId(), getIdByValueDaysOfWeek(splitTimeJob.at(i).first));

						SQLHSTMT statement3 = dbOperations.execute(buf);
						SQLFreeStmt(statement3, SQL_CLOSE);
						memset(buf, 0, SQL_LEN);
					}
				}
			}

			if (!newDays.empty()) {
				// Добавить новые записи
				for (int i = 0; i < splitTimeJob.size(); i++) {
					for (int j = 0; j < newDays.size(); j++) {
						if (splitTimeJob.at(i).first == newDays.at(j)) {
							idWeek = getIdByValueDaysOfWeek(splitTimeJob.at(i).first);
							idTimeBegin = getIdByValueTimesOfJob(splitTimeJob.at(i).second.first);
							idTimeEnd = getIdByValueTimesOfJob(splitTimeJob.at(i).second.second);

							sprintf_s(buf, "INSERT INTO dw_doc_identity(id_day_of_week, id_doctor, id_begin, id_end) VALUES\
							(%d, %d, %d, %d);", idWeek, doctor.getId(), idTimeBegin, idTimeEnd);

							SQLHSTMT statement4 = dbOperations.execute(buf);
							SQLFreeStmt(statement4, SQL_CLOSE);
							memset(buf, 0, SQL_LEN);
						}
					}
				}
			}
		
		}
	}
	else { // Если добавляются записи в первый раз
		splitTimeJob = splitDoctorTimetable(doctor);

		for (int i = 0; i < splitTimeJob.size(); i++) {
			idWeek = getIdByValueDaysOfWeek(splitTimeJob.at(i).first);
			idTimeBegin = getIdByValueTimesOfJob(splitTimeJob.at(i).second.first);
			idTimeEnd = getIdByValueTimesOfJob(splitTimeJob.at(i).second.second);

			sprintf_s(buf, "INSERT INTO dw_doc_identity(id_day_of_week, id_doctor, id_begin, id_end) VALUES\
					(%d, %d, %d, %d);", idWeek, doctor.getId(), idTimeBegin, idTimeEnd);

			SQLHSTMT statement4 = dbOperations.execute(buf);
			SQLFreeStmt(statement4, SQL_CLOSE);
			memset(buf, 0, SQL_LEN);
		}
	}

	PoliclinicStorage::getInstance()->updateDoctor(doctor); // В singleton структуру помещаю изменённый элемент
}

void DoctorRepo::removeTimetablePosition(Doctor& doctor, vector<int> pos) {
	char buf[SQL_LEN];
	vector<pair<string, pair<string, string>>> splitTimeJob;
	splitTimeJob = splitDoctorTimetable(doctor);
	int idWeek;
	int idTimeBegin;
	int idTimeEnd;

	for (int i = 0; i < splitTimeJob.size(); i++) {
		for (int j = 0; j < pos.size(); j++) {
			if ((i + 1) == pos.at(j)) {
				sprintf_s(buf, "DELETE FROM dw_doc_identity WHERE id_doctor = %d AND id_day_of_week = %d;",
				doctor.getId(), getIdByValueDaysOfWeek(splitTimeJob.at(i).first));

				SQLHSTMT statement = dbOperations.execute(buf);
				SQLFreeStmt(statement, SQL_CLOSE);
				memset(buf, 0, SQL_LEN);
			}
		}
	}

	// Удаление соответствующих позиций расписания приёма из объекта doctor
	vector<pair<string, string>> localTimetable = doctor.getTimetable();
	for (int i = 0; i < localTimetable.size(); i++) {
		for (int j = 0; j < pos.size(); j++) {
			if ((i + 1) == pos.at(j)) {
				localTimetable.erase(localTimetable.begin() + i);
			}
		}
	}
	doctor.setTimetable(localTimetable);

	PoliclinicStorage::getInstance()->putDoctor(doctor); // В singleton структуру помещаю изменённый элемент
}

vector<string> DoctorRepo::getDifferenceAppendTimetable(vector<pair<string, pair<string, string>>> first, vector<pair<string, pair<string, string>>> second) {
	vector<string> difference;
	string tmp;
	bool firstBigger = false;
	bool secondBigger = false;
	int count = 0;

	if (first.size() > second.size()) firstBigger = true;
	else if(first.size() < second.size()) secondBigger = true;

	if (firstBigger) {
		for (int i = 0; i < first.size(); i++) {
			for (int j = 0; j < second.size(); j++) {
				if (first.at(i).first == second.at(j).first) {
					count++;
					break;
				}
				else tmp = first.at(i).first;
			}

			if (count == 1) {
				count = 0;
				tmp = "";
				continue;
			}
			else if (count == 0) {
				difference.push_back(tmp);
			}
		}
	}
	else if (secondBigger) {
		for (int i = 0; i < second.size(); i++) {
			for (int j = 0; j < first.size(); j++) {
				if (second.at(i).first == first.at(j).first) {
					count++;
					break;
				}
				tmp = first.at(j).first;
			}

			if (count == 1) {
				count = 0;
				tmp = "";
				continue;
			}
			else if (count == 0) {
				difference.push_back(tmp);
			}
		}
	}

	return difference;
}

void DoctorRepo::remove(const Doctor& doctor) {
	char buf[SQL_LEN];

	sprintf_s(buf, "DELETE FROM doctors WHERE id_doctor = %d", doctor.getId());

	SQLHSTMT statement = dbOperations.execute(buf);
	memset(buf, 0, SQL_LEN);
	PoliclinicStorage::getInstance()->dropDoctor(doctor);
	SQLFreeStmt(statement, SQL_CLOSE);
}

int DoctorRepo::getIdByValueDaysOfWeek(string day) {
	char buf[SQL_LEN];
	SQLINTEGER tmpId;
	int id;

	sprintf_s(buf, "SELECT id_day_of_week FROM days_of_week WHERE day_of_week = '%s';", day.c_str());
	SQLHSTMT statement = dbOperations.execute(buf);
	SQLRETURN retcode = SQLBindCol(statement, 1, SQL_C_LONG, &tmpId, ID_LEN, nullptr);

	retcode = SQLFetch(statement);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		id = tmpId;
		SQLFreeStmt(statement, SQL_CLOSE);
	}
	else {
		if (retcode != SQL_NO_DATA) {
			std::cout << "Error fetching day_of_week\n";
			SQLFreeStmt(statement, SQL_CLOSE);
			return -1;
		}
	}
	memset(buf, 0, SQL_LEN);

	return id;
}

int DoctorRepo::getIdByValueTimesOfJob(string time) {
	char buf[SQL_LEN];
	SQLINTEGER tmpId;
	int id;

	sprintf_s(buf, "SELECT id_time_of_job FROM times_of_job WHERE time_value = '%s';", time.c_str());
	SQLHSTMT statement = dbOperations.execute(buf);
	SQLRETURN retcode = SQLBindCol(statement, 1, SQL_C_LONG, &tmpId, ID_LEN, nullptr);

	retcode = SQLFetch(statement);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		id = tmpId;
		SQLFreeStmt(statement, SQL_CLOSE);
	}
	else {
		if (retcode != SQL_NO_DATA) {
			std::cout << "Error fetching day_of_week\n";
			SQLFreeStmt(statement, SQL_CLOSE);
			return -1;
		}
	}
	memset(buf, 0, SQL_LEN);

	return id;
}

vector<pair<string, pair<string, string>>> DoctorRepo::splitDoctorTimetable(const Doctor& doctor) {
	vector<pair<string, pair<string, string>>> splitTime;

	// Алгоритм разделения строк по пробелу
	list<string> words;
	string tmp;
	list<string>::iterator it;
	for (int i = 0; i < doctor.getTimetable().size(); i++) {
		istringstream ist(doctor.getTimetable().at(i).second);
		while (ist >> tmp)
			words.push_back(tmp);

		it = words.begin();
		splitTime.push_back(make_pair(doctor.getTimetable().at(i).first, make_pair(*next(it, 0), *next(it, 1)))); // Например, (Понедельник/(8:00/17:00))

		words.clear();
	}

	return splitTime;
}

void DoctorRepo::alterSeqDoctors(long idStart) {
	char buf[SQL_LEN];

	sprintf_s(buf, "ALTER SEQUENCE doctors_id_doctor_seq RESTART WITH %d;", idStart);

	SQLHSTMT statement = dbOperations.execute(buf);
	memset(buf, 0, SQL_LEN);
	SQLFreeStmt(statement, SQL_CLOSE);
}

vector<pair<string, string>> DoctorRepo::getTimetableToDoctor(const Doctor& doctor) {
	char buf[SQL_LEN];
	SQLRETURN retcode;

	SQLINTEGER idLocal[ID_LEN];
	SQLWCHAR day[NAME_LEN];
	SQLWCHAR timeBegin[NAME_LEN];
	SQLWCHAR timeEnd[NAME_LEN];
	int idLocalIdent;
	string dayOfWeek;
	string timeBeginJob;
	string timeEndJob;

	vector<pair<string, string>> timetableLocal;

	sprintf_s(buf, "SELECT doc.id_doctor, dw.day_of_week, tbegin.time_value, tend.time_value "
					"FROM dw_doc_identity ident "
					"INNER JOIN days_of_week dw ON dw.id_day_of_week = ident.id_day_of_week "
					"INNER JOIN doctors doc ON doc.id_doctor = ident.id_doctor "
					"INNER JOIN times_of_job tbegin ON tbegin.id_time_of_job = ident.id_begin "
					"INNER JOIN times_of_job tend ON tend.id_time_of_job = ident.id_end "
					"WHERE doc.id_doctor = %d;", doctor.getId());
	SQLHSTMT statement = dbOperations.execute(buf);
	retcode = SQLBindCol(statement, 1, SQL_C_LONG, &idLocal, ID_LEN, nullptr);
	retcode = SQLBindCol(statement, 2, SQL_C_CHAR, &day, NAME_LEN, nullptr);
	retcode = SQLBindCol(statement, 3, SQL_C_CHAR, &timeBegin, NAME_LEN, nullptr);
	retcode = SQLBindCol(statement, 4, SQL_C_CHAR, &timeEnd, NAME_LEN, nullptr);

	while (true) {
		retcode = SQLFetch(statement);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			idLocalIdent = reinterpret_cast<int>(idLocal);
			dayOfWeek = reinterpret_cast<char*>(day);
			timeBeginJob = reinterpret_cast<char*>(timeBegin);
			timeEndJob = reinterpret_cast<char*>(timeEnd);

			timetableLocal.push_back(make_pair(dayOfWeek, timeBeginJob + " " + timeEndJob));
		}
		else {
			if (retcode != SQL_NO_DATA) {
				std::cout << "Error fetching dw_doc_identity\n";
				break;
			}
			break;
		}
	}

	memset(buf, 0, SQL_LEN);
	SQLFreeStmt(statement, SQL_CLOSE);
	return timetableLocal;
}

pair<string, pair<string, string>> DoctorRepo::checkJobAtDayOfWeek(Doctor* doctor, int idDay) {
	vector<pair<string, pair<string, string>>> splitTimeJob;
	pair<string, pair<string, string>> voidValue;

	splitTimeJob = splitDoctorTimetable(doctor);

	for (int i = 0; i < splitTimeJob.size(); i++) {
		if (splitTimeJob.at(i).first == "Понедельник") {
			if (idDay == 1) return splitTimeJob.at(i);
		}
		else if (splitTimeJob.at(i).first == "Вторник") {
			if (idDay == 2) return splitTimeJob.at(i);
		}
		else if (splitTimeJob.at(i).first == "Среда") {
			if (idDay == 3) return splitTimeJob.at(i);
		}
		else if (splitTimeJob.at(i).first == "Четверг") {
			if (idDay == 4) return splitTimeJob.at(i);
		}
		else if (splitTimeJob.at(i).first == "Пятница") {
			if (idDay == 5) return splitTimeJob.at(i);
		}
		else if (splitTimeJob.at(i).first == "Суббота") {
			if (idDay == 6) return splitTimeJob.at(i);
		}
		else {
			if (idDay == 0) return splitTimeJob.at(i);
		}
	}
	
	voidValue = make_pair("0",make_pair("0","0"));
	return voidValue;
}