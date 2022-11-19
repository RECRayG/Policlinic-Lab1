#pragma once

#include "Patient.h"
#include "Doctor.h"
#include "Reception.h"
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
#define DATE_LEN 255
#define MED_LEN 255
#define DIAG_LEN 8192
#define PROC_LEN 8192
#define AN_LEN 1024
#define IS_LEN 8
#define SQL_LEN 512
#define ADDRESS_LEN 1020
#define NAME_LEN 255

class ReceptionRepo : public InterfaceRepo<Reception, long> {
private:
	SQLINTEGER id_reception;
	SQLWCHAR dateOfReception[DATE_LEN];
	SQLWCHAR complaints[DIAG_LEN];
	SQLWCHAR diagnosis[DIAG_LEN];
	SQLWCHAR medication_name[MED_LEN];
	SQLWCHAR procedure_name[PROC_LEN];
	SQLWCHAR analysis_name[AN_LEN];
	SQLWCHAR analysis_result[AN_LEN];
	SQLWCHAR dateOfExtract[DATE_LEN];
	SQLINTEGER isDone;
	SQLWCHAR timeOfReception[MED_LEN];

	SQLINTEGER id_doctor;
	SQLWCHAR doctor_full_name[NAME_LEN * 3];
	SQLWCHAR specialization[NAME_LEN];
	SQLWCHAR cabinet[NAME_LEN];
	SQLINTEGER plot;

	SQLINTEGER id_patient;
	SQLWCHAR patient_full_name[NAME_LEN * 3];
	SQLWCHAR address[NAME_LEN * 4];
public:
	ReceptionRepo(DB_Operations& dbOperations);

	// Вспомогательные методы
	//void alterSeqPatients(long idStart);
	std::vector<Reception*> findAll() override; // Список всех приёмов
	void alterSeqReceptions(long idStart);
	int getMaxReceptionId(); // Получить значение максимального существующего id в таблице receptions

	// Основные методы из меню
	void add(const Reception& reception) override; // Add/Edit - Сохранить пациента (вставка если новый, апдейт если уже есть)
	void remove(const Reception& reception) override; // Удалить пациента из базы
	void edit(const Reception& reception) override; // Изменить существующего в базе пациента

	//int getMaxReceptionId(); // Получить значение максимального существующего id в таблице receptions
	int whatDayOfWeek(string Date); // По дате определить день недели
	bool checkEmployment(const Doctor doctor, string timeReception, string dateReception);

	void addTreatment(const Reception& reception);
	vector<string> getAnalyses();
	Reception* getReceptionVectorItem(const vector<Reception*>& receptions, int id); // Поиск элемента по id

	int getIdByValueMedication(string medication);
	int getIdByValueProcedure(string procedure);
	int getIdByValueAnalysis(string analysis);
	int getIdByIdAnalysis(int id_analysis);

	void setIsDone(const Reception& reception);
};

int ReceptionRepo::whatDayOfWeek(string Date)
{
	list<int> words;
	int Year;
	int Month;
	int Day;

	// Замена "-" на " "
	for (int i = 0; i < Date.length(); i++) {
		if (Date[i] == '-') {
			Date[i] = ' ';
		}
	}

	// Алгоритм разделения строк по пробелу
	istringstream ist(Date);
	while (ist >> Date) {
		words.push_back(atoi(Date.c_str()));
	}

	list<int>::iterator it = words.begin();
	Year = *next(it, 0);
	Month = *next(it, 1);
	Day = *next(it, 2);

	int a, y, m, R;
	a = (14 - Month) / 12;
	y = Year - a;
	m = Month + 12 * a - 2;
	R = 7000 + (Day + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12);

	return R % 7;
}

bool ReceptionRepo::checkEmployment(const Doctor doctor, string timeReception, string dateReception) {
	char buf[SQL_LEN];
	SQLWCHAR times[SQL_LEN];

	sprintf_s(buf, "SELECT r.time_of_reception\
					FROM receptions r\
					INNER JOIN doc_rec_identity dri ON dri.id_reception = r.id_reception\
					INNER JOIN doctors d ON d.id_doctor = dri.id_doctor\
					WHERE dri.id_doctor = %d AND\
						r.is_done = false AND\
						r.date_of_reception = '%s' AND\
						r.time_of_reception = '%s';",
					doctor.getId(), dateReception.c_str(), timeReception.c_str());

	SQLHSTMT statement = dbOperations.execute(buf);
	SQLRETURN retcode;
	retcode = SQLBindCol(statement, 1, SQL_C_CHAR, &times, SQL_LEN, nullptr);

	while (true) {
		retcode = SQLFetch(statement);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			SQLFreeStmt(statement, SQL_CLOSE);
			memset(buf, 0, SQL_LEN);
			return true;
		}
		else {
			if (retcode != SQL_NO_DATA) {
				std::cout << "Error fetching receptions\n";
			}
			break;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);
	memset(buf, 0, SQL_LEN);

	return false;
}

ReceptionRepo::ReceptionRepo(DB_Operations& dbOperations) :
	InterfaceRepo(dbOperations),
	id_reception(),
	dateOfReception(),
	complaints(),
	diagnosis(),
	medication_name(),
	procedure_name(),
	analysis_name(),
	analysis_result(),
	dateOfExtract(),
	isDone(),
	
	id_doctor(), 
	doctor_full_name(),
	specialization(),
	cabinet(),
	plot(), 

	id_patient(),
	patient_full_name(),
	address() {
}

std::vector<Reception*> ReceptionRepo::findAll() {
	char buf[SQL_LEN * 2];
	SQLLEN nullbufComp;
	SQLLEN nullbufDiag;
	SQLLEN nullbufDate;
	std::vector<Reception*> receptions;
	std::vector<std::string> medications;
	std::vector<std::string> procedures;
	std::map<std::string, std::string> analyses;

	/*
		SELECT doc.id_doctor, CONCAT(doc.last_name, ' ', doc.first_name, ' ', doc.middle_name) AS doctor_full_name,
			sp.specialization_name, cab.cabinet_number, pl.plot_number,
	   
			pat.id_patient, CONCAT(pat.last_name, ' ', pat.first_name, ' ', pat.middle_name) AS patient_full_name,
			CONCAT(pat.city, ',', pat.street, ',', pat.building, ',', pat.apartment) AS patient_address,
	   
			rec.id_reception, rec.date_of_reception, rec.complaints, rec.diagnosis, rec.date_of_extract,
			rec.is_done, rec.time_of_reception,
	   
			med.medication_name, 
			proc.procedure_name,
			an.analysis_name,
			ar.analysis_result
		FROM receptions rec
		INNER JOIN doc_rec_identity dri ON dri.id_reception = rec.id_reception
		INNER JOIN doctors doc ON dri.id_doctor = doc.id_doctor
		INNER JOIN pat_rec_identity pri ON pri.id_reception = rec.id_reception
		INNER JOIN patients pat ON pri.id_patient = pat.id_patient

		INNER JOIN specializations sp ON sp.id_specialization = doc.id_specialization
		INNER JOIN cabinets cab ON cab.id_cabinet = doc.id_cabinet
		INNER JOIN plots pl ON pl.id_plot = doc.id_plot

		INNER JOIN rec_tr_identity rti ON rti.id_reception = rec.id_reception
		INNER JOIN treatments tr ON tr.id_treatment = rti.id_treatment
		INNER JOIN medications med ON med.id_medication = tr.id_medication
		INNER JOIN proceduress proc ON proc.id_procedure = tr.id_procedure

		INNER JOIN analyses_results ar ON ar.id_patient = pat.id_patient
		INNER JOIN analyses an ON an.id_analysis = ar.id_analysis;



		"med.medication_name, "
		"proc.procedure_name, "
		"an.analysis_name, "
		"ar.analysis_result "

		"INNER JOIN rec_tr_identity rti ON rti.id_reception = rec.id_reception "
		"INNER JOIN treatments tr ON tr.id_treatment = rti.id_treatment "
		"INNER JOIN medications med ON med.id_medication = tr.id_medication "
		"INNER JOIN proceduress proc ON proc.id_procedure = tr.id_procedure "

		"INNER JOIN analyses_results ar ON ar.id_patient = pat.id_patient "
		"INNER JOIN analyses an ON an.id_analysis = ar.id_analysis;
	*/

	// main info
	sprintf_s(buf, "SELECT DISTINCT doc.id_doctor, CONCAT(doc.last_name, ' ', doc.first_name, ' ', doc.middle_name) AS doctor_full_name, "
					"sp.specialization_name, cab.cabinet_number, pl.plot_number, "
					"pat.id_patient, CONCAT(pat.last_name, ' ', pat.first_name, ' ', pat.middle_name) AS patient_full_name, "
					"CONCAT(pat.city, ',', pat.street, ',', pat.building, ',', pat.apartment) AS patient_address, "

					"rec.id_reception, rec.date_of_reception, rec.complaints, rec.diagnosis, rec.date_of_extract, "
					"rec.is_done, rec.time_of_reception "

					"FROM receptions rec "
					"INNER JOIN doc_rec_identity dri ON dri.id_reception = rec.id_reception "
					"INNER JOIN doctors doc ON dri.id_doctor = doc.id_doctor "
					"INNER JOIN pat_rec_identity pri ON pri.id_reception = rec.id_reception "
					"INNER JOIN patients pat ON pri.id_patient = pat.id_patient "
					"INNER JOIN specializations sp ON sp.id_specialization = doc.id_specialization "
					"INNER JOIN cabinets cab ON cab.id_cabinet = doc.id_cabinet "
					"INNER JOIN plots pl ON pl.id_plot = doc.id_plot;");

	SQLHSTMT statement = dbOperations.execute(buf);
												
	SQLRETURN retcode;
	retcode = SQLBindCol(statement, 1, SQL_C_LONG, &id_doctor, ID_LEN, nullptr);
	retcode = SQLBindCol(statement, 2, SQL_C_CHAR, &doctor_full_name, NAME_LEN * 3, nullptr);
	retcode = SQLBindCol(statement, 3, SQL_C_CHAR, &specialization, NAME_LEN, nullptr);
	retcode = SQLBindCol(statement, 4, SQL_C_CHAR, &cabinet, NAME_LEN, nullptr);
	retcode = SQLBindCol(statement, 5, SQL_C_LONG, &plot, ID_LEN, nullptr);

	retcode = SQLBindCol(statement, 6, SQL_C_LONG, &id_patient, ID_LEN, nullptr);
	retcode = SQLBindCol(statement, 7, SQL_C_CHAR, &patient_full_name, NAME_LEN * 3, nullptr);
	retcode = SQLBindCol(statement, 8, SQL_C_CHAR, &address, ADDRESS_LEN * 4, nullptr);

	retcode = SQLBindCol(statement, 9, SQL_C_LONG, &id_reception, ID_LEN, nullptr);
	retcode = SQLBindCol(statement, 10, SQL_C_CHAR, &dateOfReception, DATE_LEN, nullptr);


	retcode = SQLBindCol(statement, 11, SQL_C_CHAR, &complaints, DIAG_LEN, &nullbufComp);
	retcode = SQLBindCol(statement, 12, SQL_C_CHAR, &diagnosis, DIAG_LEN, &nullbufDiag);
	retcode = SQLBindCol(statement, 13, SQL_C_CHAR, &dateOfExtract, DATE_LEN, &nullbufDate);


	retcode = SQLBindCol(statement, 14, SQL_C_LONG, &isDone, ID_LEN, nullptr);
	retcode = SQLBindCol(statement, 15, SQL_C_CHAR, &timeOfReception, MED_LEN, nullptr);

	while (true) {
		retcode = SQLFetch(statement);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			Doctor doctor;
			Patient patient;
			Reception reception;

			doctor.setId(id_doctor);
			doctor.setName(reinterpret_cast<char*>(doctor_full_name));
			doctor.setSpecialization(reinterpret_cast<char*>(specialization));
			doctor.setCabinet(reinterpret_cast<char*>(cabinet));
			doctor.setPlot(plot);

			patient.setId(id_patient);
			patient.setName(reinterpret_cast<char*>(patient_full_name));
			patient.setAddress(reinterpret_cast<char*>(address));

			reception.setId(id_reception);
			reception.setDateOfReception(reinterpret_cast<char*>(dateOfReception));

				reception.setComplaints(reinterpret_cast<char*>(complaints));
				reception.setDiagnosis(reinterpret_cast<char*>(diagnosis));
				reception.setDateOfExtract(reinterpret_cast<char*>(dateOfExtract));
			
			reception.setIsDone(isDone);
			reception.setTimeOfReception(reinterpret_cast<char*>(timeOfReception));
			reception.setDoctor(doctor);
			reception.setPatient(patient);

			PoliclinicStorage::getInstance()->putReception(reception);
			receptions.push_back(PoliclinicStorage::getInstance()->getReception(reception.getId()));
		}
		else {
			if (retcode != SQL_NO_DATA) {
				std::cout << "Error fetching receptions\n";
			}
			break;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);
	memset(buf, 0, SQL_LEN * 2);

	// medications
	SQLRETURN retcode2;
	for (int i = 0; i < receptions.size(); i++) {
		sprintf_s(buf, "SELECT DISTINCT med.medication_name "
						"FROM receptions rec "
						"INNER JOIN pat_rec_identity pri ON pri.id_reception = rec.id_reception "
						"INNER JOIN patients pat ON pri.id_patient = pat.id_patient "

						"INNER JOIN rec_med_identity rmi ON rmi.id_reception = rec.id_reception "

						"INNER JOIN medications med ON med.id_medication = rmi.id_medication "
						"WHERE rec.id_reception = %d;", receptions.at(i)->getId());

		statement = dbOperations.execute(buf);
		retcode2 = SQLBindCol(statement, 1, SQL_C_CHAR, &medication_name, MED_LEN, nullptr);

		while (true) {
			retcode2 = SQLFetch(statement);
			if (retcode2 == SQL_SUCCESS || retcode2 == SQL_SUCCESS_WITH_INFO) {
				medications.push_back(reinterpret_cast<char*>(medication_name));
			}
			else {
				if (retcode2 != SQL_NO_DATA) {
					std::cout << "Error fetching receptions medications\n";
				}
				break;
			}
		}

		receptions.at(i)->setMedications(medications);
		PoliclinicStorage::getInstance()->updateReception(receptions.at(i));

		medications.clear();
		SQLFreeStmt(statement, SQL_CLOSE);
		memset(buf, 0, SQL_LEN * 2);
	}
	
	// procedures
	SQLRETURN retcode3;
	for (int i = 0; i < receptions.size(); i++) {
		sprintf_s(buf, "SELECT DISTINCT proc.procedure_name "
						"FROM receptions rec "
						"INNER JOIN pat_rec_identity pri ON pri.id_reception = rec.id_reception "
						"INNER JOIN patients pat ON pri.id_patient = pat.id_patient "

						"INNER JOIN rec_proc_identity rpi ON rpi.id_reception = rec.id_reception "

						"INNER JOIN proceduress proc ON proc.id_procedure = rpi.id_procedure "
						"WHERE rec.id_reception = %d;", receptions.at(i)->getId());

		statement = dbOperations.execute(buf);
		retcode3 = SQLBindCol(statement, 1, SQL_C_CHAR, &procedure_name, PROC_LEN, nullptr);

		while (true) {
			retcode3 = SQLFetch(statement);
			if (retcode3 == SQL_SUCCESS || retcode3 == SQL_SUCCESS_WITH_INFO) {
				procedures.push_back(reinterpret_cast<char*>(procedure_name));
			}
			else {
				if (retcode3 != SQL_NO_DATA) {
					std::cout << "Error fetching receptions procedures\n";
				}
				break;
			}
		}

		receptions.at(i)->setProcedures(procedures);
		PoliclinicStorage::getInstance()->updateReception(receptions.at(i));

		procedures.clear();
		SQLFreeStmt(statement, SQL_CLOSE);
		memset(buf, 0, SQL_LEN * 2);
	}

	// analyses
	SQLRETURN retcode4;
	for (int i = 0; i < receptions.size(); i++) {
		sprintf_s(buf, "SELECT DISTINCT an.analysis_name, ar.analysis_result "
						"FROM receptions rec "
						"INNER JOIN pat_rec_identity pri ON pri.id_reception = rec.id_reception "
						"INNER JOIN patients pat ON pri.id_patient = pat.id_patient "

						"INNER JOIN analyses_results ar ON ar.id_patient = pat.id_patient "
						"INNER JOIN analyses an ON an.id_analysis = ar.id_analysis "
						"WHERE rec.id_reception = %d;", receptions.at(i)->getId());

		statement = dbOperations.execute(buf);
		retcode4 = SQLBindCol(statement, 1, SQL_C_CHAR, &analysis_name, AN_LEN, nullptr);
		retcode4 = SQLBindCol(statement, 2, SQL_C_CHAR, &analysis_result, AN_LEN, nullptr);

		while (true) {
			retcode4 = SQLFetch(statement);
			if (retcode4 == SQL_SUCCESS || retcode4 == SQL_SUCCESS_WITH_INFO) {
				analyses.insert(std::pair<string, string>(reinterpret_cast<char*>(analysis_name), 
														  reinterpret_cast<char*>(analysis_result)));
			}
			else {
				if (retcode4 != SQL_NO_DATA) {
					std::cout << "Error fetching receptions analyses\n";
				}
				break;
			}
		}

		receptions.at(i)->setAnalyses(analyses);
		PoliclinicStorage::getInstance()->updateReception(receptions.at(i));

		analyses.clear();
		SQLFreeStmt(statement, SQL_CLOSE);
		memset(buf, 0, SQL_LEN * 2);
	}

	return receptions;
}

void ReceptionRepo::add(const Reception& reception) {
	char buf[SQL_LEN];
	string isDone;

	if (!reception.getIsDone())
		isDone = "false";
	else
		isDone = "true";

	sprintf_s(buf, "INSERT INTO receptions(date_of_reception, complaints, diagnosis, date_of_extract, is_done, time_of_reception)\
					VALUES('%s', NULL, NULL, NULL, %s, '%s');", 
					reception.getDateOfReception().c_str(), isDone.c_str(), reception.getTimeOfReception().c_str());

	SQLHSTMT statement = dbOperations.execute(buf);
	SQLFreeStmt(statement, SQL_CLOSE);
	memset(buf, 0, SQL_LEN);


	sprintf_s(buf, "INSERT INTO doc_rec_identity(id_doctor, id_reception)\
					VALUES(%d, %d);",
					reception.getDoctor().getId(), reception.getId());

	statement = dbOperations.execute(buf);
	SQLFreeStmt(statement, SQL_CLOSE);
	memset(buf, 0, SQL_LEN);


	sprintf_s(buf, "INSERT INTO pat_rec_identity(id_patient, id_reception)\
					VALUES(%d, %d);",
					reception.getPatient().getId(), reception.getId());

	statement = dbOperations.execute(buf);
	SQLFreeStmt(statement, SQL_CLOSE);
	memset(buf, 0, SQL_LEN);


	PoliclinicStorage::getInstance()->putReception(reception);
}

void ReceptionRepo::remove(const Reception& reception) {
	char buf[SQL_LEN];

	sprintf_s(buf, "DELETE FROM receptions WHERE id_reception = %d", reception.getId());

	SQLHSTMT statement = dbOperations.execute(buf);
	memset(buf, 0, SQL_LEN);
	PoliclinicStorage::getInstance()->dropReception(reception);
	SQLFreeStmt(statement, SQL_CLOSE);
}

void ReceptionRepo::edit(const Reception& reception) {
	char buf[SQL_LEN];

	// Если есть различия
	if (reception != PoliclinicStorage::getInstance()->getReception(reception.getId())) {
		sprintf_s(buf, "UPDATE receptions SET complaints = '%s', diagnosis = '%s' "
					   "WHERE id_reception = %d;", reception.getComplaints().c_str(), reception.getDiagnosis().c_str(),
					   reception.getId());

		SQLHSTMT statement = dbOperations.execute(buf);
		SQLFreeStmt(statement, SQL_CLOSE);
		memset(buf, 0, SQL_LEN);

		sprintf_s(buf, "UPDATE doc_rec_identity SET id_doctor = %d "
					   "WHERE id_reception = %d;", reception.getDoctor().getId(), reception.getId());

		statement = dbOperations.execute(buf);
		SQLFreeStmt(statement, SQL_CLOSE);
		memset(buf, 0, SQL_LEN);

		for (auto& item : reception.getAnalyses()) {
			sprintf_s(buf, "UPDATE analyses_results SET analysis_result = '%s' "
							"WHERE id_analysis_result = %d;", item.second.c_str(), getIdByIdAnalysis(getIdByValueAnalysis(item.first)));

			statement = dbOperations.execute(buf);
			SQLFreeStmt(statement, SQL_CLOSE);
			memset(buf, 0, SQL_LEN);
		}

		PoliclinicStorage::getInstance()->updateReception(reception);
	}
}

void ReceptionRepo::alterSeqReceptions(long idStart) {
	char buf[SQL_LEN];

	sprintf_s(buf, "ALTER SEQUENCE receptions_id_reception_seq RESTART WITH %d;", idStart);

	SQLHSTMT statement = dbOperations.execute(buf);
	memset(buf, 0, SQL_LEN);
	SQLFreeStmt(statement, SQL_CLOSE);
}

int ReceptionRepo::getMaxReceptionId() {
	SQLINTEGER countR;
	int count = 0;

	SQLHSTMT statement = dbOperations.execute("SELECT MAX(id_reception) FROM receptions;");
	SQLRETURN retcode = SQLBindCol(statement, 1, SQL_C_LONG, &countR, ID_LEN, nullptr);

	retcode = SQLFetch(statement);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		count = countR;
	}
	else {
		if (retcode != SQL_NO_DATA) {
			std::cout << "Error fetching receptions\n";
			SQLFreeStmt(statement, SQL_CLOSE);
			return count;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);

	return count;
}

Reception* ReceptionRepo::getReceptionVectorItem(const vector<Reception*>& receptions, int id) {
	for (int i = 0; i < receptions.size(); i++) {
		if ((i + 1) == id) {
			return receptions.at(i);
		}
	}
}

vector<string> ReceptionRepo::getAnalyses() {
	char buf[SQL_LEN];
	vector<string> analyses;

	sprintf_s(buf, "SELECT * FROM analyses;");

	SQLHSTMT statement = dbOperations.execute(buf);
	SQLRETURN retcode = SQLBindCol(statement, 2, SQL_C_CHAR, &analysis_name, AN_LEN, nullptr);

	while (true) {
		retcode = SQLFetch(statement);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			analyses.push_back(reinterpret_cast<char*>(analysis_name));
		}
		else {
			if (retcode != SQL_NO_DATA) {
				std::cout << "Error fetching analyses\n";
			}
			break;
		}
	}

	memset(buf, 0, SQL_LEN);
	SQLFreeStmt(statement, SQL_CLOSE);

	return analyses;
}

void ReceptionRepo::addTreatment(const Reception& reception) {
	char buf[SQL_LEN * 2];

	// Вставка в словари, если есть новые позиции
	for (int i = 0; i < reception.getMedications().size(); i++) {
		sprintf_s(buf, "INSERT INTO medications(medication_name) "
						"SELECT '%s' "
						"WHERE "
						"NOT EXISTS( "
						"SELECT id_medication FROM medications WHERE id_medication = %d "
						");",
						reception.getMedications().at(i).c_str(), getIdByValueMedication(reception.getMedications().at(i)));

		SQLHSTMT statement = dbOperations.execute(buf);
		SQLFreeStmt(statement, SQL_CLOSE);
		memset(buf, 0, SQL_LEN);
	}

	for (int i = 0; i < reception.getProcedures().size(); i++) {
		sprintf_s(buf, "INSERT INTO proceduress(procedure_name) "
						"SELECT '%s' "
						"WHERE "
						"NOT EXISTS( "
						"SELECT id_procedure FROM proceduress WHERE id_procedure = %d "
						");",
						reception.getProcedures().at(i).c_str(), getIdByValueProcedure(reception.getProcedures().at(i)));

		SQLHSTMT statement = dbOperations.execute(buf);
		SQLFreeStmt(statement, SQL_CLOSE);
		memset(buf, 0, SQL_LEN);
	}

	// Вставка в лечение
	for (int i = 0; i < reception.getMedications().size(); i++) {
		sprintf_s(buf, "INSERT INTO rec_med_identity(id_reception, id_medication) VALUES "
						"(%d, %d);",
				  reception.getId(), getIdByValueMedication(reception.getMedications().at(i)));

		SQLHSTMT statement = dbOperations.execute(buf);
		SQLFreeStmt(statement, SQL_CLOSE);
		memset(buf, 0, SQL_LEN);
	}

	for (int i = 0; i < reception.getProcedures().size(); i++) {
		sprintf_s(buf, "INSERT INTO rec_proc_identity(id_reception, id_procedure) VALUES "
						"(%d, %d);",
				  reception.getId(), getIdByValueProcedure(reception.getProcedures().at(i)));

		SQLHSTMT statement = dbOperations.execute(buf);
		SQLFreeStmt(statement, SQL_CLOSE);
		memset(buf, 0, SQL_LEN);
	}

	for (auto& item : reception.getAnalyses())
	{
		sprintf_s(buf, "INSERT INTO rec_an_identity(id_reception, id_analysis) VALUES "
						"(%d, %d);",
				  reception.getId(), getIdByValueAnalysis(item.first));

		SQLHSTMT statement = dbOperations.execute(buf);
		SQLFreeStmt(statement, SQL_CLOSE);
		memset(buf, 0, SQL_LEN);

		sprintf_s(buf, "INSERT INTO analyses_results(id_analysis, id_patient, analysis_result) VALUES(%d, %d, '%s');",
				 getIdByValueAnalysis(item.first), reception.getPatient().getId(), item.second.c_str());

		statement = dbOperations.execute(buf);
		SQLFreeStmt(statement, SQL_CLOSE);
		memset(buf, 0, SQL_LEN);
	}

	PoliclinicStorage::getInstance()->updateReception(reception);
}

int ReceptionRepo::getIdByValueMedication(string medication) {
	char buf[SQL_LEN];
	SQLINTEGER tmpId;
	int id = 0;

	sprintf_s(buf, "SELECT id_medication FROM medications WHERE medication_name = '%s';", medication.c_str());
	SQLHSTMT statement = dbOperations.execute(buf);
	SQLRETURN retcode = SQLBindCol(statement, 1, SQL_C_LONG, &tmpId, ID_LEN, nullptr);

	retcode = SQLFetch(statement);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		id = tmpId;
		SQLFreeStmt(statement, SQL_CLOSE);
	}
	else {
		if (retcode != SQL_NO_DATA) {
			std::cout << "Error fetching medications\n";
			SQLFreeStmt(statement, SQL_CLOSE);
			return -1;
		}
	}
	memset(buf, 0, SQL_LEN);

	return id;
}

int ReceptionRepo::getIdByValueProcedure(string procedure) {
	char buf[SQL_LEN];
	SQLINTEGER tmpId;
	int id = 0;

	sprintf_s(buf, "SELECT id_procedure FROM proceduress WHERE procedure_name = '%s';", procedure.c_str());
	SQLHSTMT statement = dbOperations.execute(buf);
	SQLRETURN retcode = SQLBindCol(statement, 1, SQL_C_LONG, &tmpId, ID_LEN, nullptr);

	retcode = SQLFetch(statement);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		id = tmpId;
		SQLFreeStmt(statement, SQL_CLOSE);
	}
	else {
		if (retcode != SQL_NO_DATA) {
			std::cout << "Error fetching procedures\n";
			SQLFreeStmt(statement, SQL_CLOSE);
			return -1;
		}
	}
	memset(buf, 0, SQL_LEN);

	return id;
}

int ReceptionRepo::getIdByIdAnalysis(int id_analysis) {
	char buf[SQL_LEN];
	SQLINTEGER tmpId;
	int id;

	sprintf_s(buf, "SELECT id_analysis_result FROM analyses_results WHERE id_analysis = %d;", id_analysis);
	SQLHSTMT statement = dbOperations.execute(buf);
	SQLRETURN retcode = SQLBindCol(statement, 1, SQL_C_LONG, &tmpId, ID_LEN, nullptr);

	retcode = SQLFetch(statement);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		id = tmpId;
		SQLFreeStmt(statement, SQL_CLOSE);
	}
	else {
		if (retcode != SQL_NO_DATA) {
			std::cout << "Error fetching analyses_results\n";
			SQLFreeStmt(statement, SQL_CLOSE);
			return -1;
		}
	}
	memset(buf, 0, SQL_LEN);

	return id;
}

int ReceptionRepo::getIdByValueAnalysis(string analysis) {
	char buf[SQL_LEN];
	SQLINTEGER tmpId;
	int id;

	sprintf_s(buf, "SELECT id_analysis FROM analyses WHERE analysis_name = '%s';", analysis.c_str());
	SQLHSTMT statement = dbOperations.execute(buf);
	SQLRETURN retcode = SQLBindCol(statement, 1, SQL_C_LONG, &tmpId, ID_LEN, nullptr);

	retcode = SQLFetch(statement);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		id = tmpId;
		SQLFreeStmt(statement, SQL_CLOSE);
	}
	else {
		if (retcode != SQL_NO_DATA) {
			std::cout << "Error fetching procedures\n";
			SQLFreeStmt(statement, SQL_CLOSE);
			return -1;
		}
	}
	memset(buf, 0, SQL_LEN);

	return id;
}

void ReceptionRepo::setIsDone(const Reception& reception) {
	char buf[SQL_LEN];

	sprintf_s(buf, "UPDATE receptions SET is_done = true, date_of_extract = CURRENT_DATE "
					"WHERE id_reception = %d;", reception.getId());

	SQLHSTMT statement = dbOperations.execute(buf);
	SQLFreeStmt(statement, SQL_CLOSE);
	memset(buf, 0, SQL_LEN);

	PoliclinicStorage::getInstance()->updateReception(reception);
}