#pragma once

#include "Patient.h"
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
#define NAME_LEN 765
#define ADDRESS_LEN 1020
#define SQL_LEN 512

class PatientRepo : public InterfaceRepo<Patient, long> {
private:
	SQLINTEGER id_patient;
	SQLWCHAR full_name[NAME_LEN*3];
	SQLWCHAR address[NAME_LEN*4];
public:
	PatientRepo(DB_Operations& dbOperations);

	// Вспомогательные методы
	void alterSeqPatients(long idStart);
	std::vector<Patient*> findAll() override; // Список всех врачей

	// Основные методы из меню
	void add(const Patient& patient) override; // Add/Edit - Сохранить пациента (вставка если новый, апдейт если уже есть)
	void remove(const Patient& patient) override; // Удалить пациента из базы
	void edit(const Patient& patient) override; // Изменить существующего в базе пациента

	//int getIdPatientByValue(const Patient& patient); // Получить id пациента по его данным
	int getMaxPatientId(); // Получить значение максимального существующего id в таблице patients
	vector<string> splitFullNamePatient(const Patient& patient);
	vector<string> splitAddressPatient(const Patient& patient);
};

PatientRepo::PatientRepo(DB_Operations& dbOperations) :
	InterfaceRepo(dbOperations),
	id_patient(),
	full_name(),
	address() {
}

void PatientRepo::alterSeqPatients(long idStart) {
	char buf[SQL_LEN];

	sprintf_s(buf, "ALTER SEQUENCE patients_id_patient_seq RESTART WITH %d;", idStart);

	SQLHSTMT statement = dbOperations.execute(buf);
	memset(buf, 0, SQL_LEN);
	SQLFreeStmt(statement, SQL_CLOSE);
}

int PatientRepo::getMaxPatientId() {
	SQLINTEGER countP;
	int count = 0;

	SQLHSTMT statement = dbOperations.execute("SELECT MAX(id_patient) FROM patients;");
	SQLRETURN retcode = SQLBindCol(statement, 1, SQL_C_LONG, &countP, ID_LEN, nullptr);

	retcode = SQLFetch(statement);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		count = countP;
	}
	else {
		if (retcode != SQL_NO_DATA) {
			std::cout << "Error fetching patients\n";
			SQLFreeStmt(statement, SQL_CLOSE);
			return count;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);

	return count;
}
std::vector<Patient*> PatientRepo::findAll() {
	char buf[SQL_LEN];
	std::vector<Patient*> patients;

	SQLHSTMT statement = dbOperations.execute("SELECT id_patient, concat(last_name, ' ', first_name, ' ', middle_name) as full_name, \
											   concat(city, ',', street, ',', building, ',', apartment) as address \
											   FROM patients;");
	SQLRETURN retcode;
	retcode = SQLBindCol(statement, 1, SQL_C_LONG, &id_patient, ID_LEN, nullptr);
	retcode = SQLBindCol(statement, 2, SQL_C_CHAR, &full_name, NAME_LEN, nullptr);
	retcode = SQLBindCol(statement, 3, SQL_C_CHAR, &address, ADDRESS_LEN, nullptr);

	while (true) {
		retcode = SQLFetch(statement);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			Patient patient;
			patient.setId(id_patient);
			patient.setName(reinterpret_cast<char*>(full_name));
			patient.setAddress(reinterpret_cast<char*>(address));

			PoliclinicStorage::getInstance()->putPatient(patient);
			patients.push_back(PoliclinicStorage::getInstance()->getPatient(patient.getId()));
		}
		else {
			if (retcode != SQL_NO_DATA) {
				std::cout << "Error fetching patients\n";
			}
			break;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);
	memset(buf, 0, SQL_LEN); // Очистка буфера для запросов

	return patients;
}

void PatientRepo::add(const Patient& patient) {
	char buf[SQL_LEN];
	vector<string> splitName;
	vector<string> splitAddress;

	splitName = splitFullNamePatient(patient);
	splitAddress = splitAddressPatient(patient);

	if (splitName.size() == 3) {
		sprintf_s(buf, "INSERT INTO patients(last_name, first_name, middle_name, city, street, building, apartment) \
		VALUES('%s', '%s', '%s', '%s', '%s', '%s', '%s');", 
		splitName.at(0).c_str(), splitName.at(1).c_str(), splitName.at(2).c_str(), 
		splitAddress.at(0).c_str(), splitAddress.at(1).c_str(), splitAddress.at(2).c_str(), splitAddress.at(3).c_str());
	}
	else {
		sprintf_s(buf, "INSERT INTO patients(last_name, first_name, middle_name, city, street, building, apartment) \
		VALUES('%s', '%s', NULL, '%s', '%s', '%s', '%s');",
		splitName.at(0).c_str(), splitName.at(1).c_str(),
		splitAddress.at(0).c_str(), splitAddress.at(1).c_str(), splitAddress.at(2).c_str(), splitAddress.at(3).c_str());
	}

	SQLHSTMT statement = dbOperations.execute(buf);
	SQLFreeStmt(statement, SQL_CLOSE);
	memset(buf, 0, SQL_LEN);
	PoliclinicStorage::getInstance()->putPatient(patient);
}

void PatientRepo::remove(const Patient& patient) {
	char buf[SQL_LEN];

	sprintf_s(buf, "DELETE FROM patients WHERE id_patient = %d", patient.getId());

	SQLHSTMT statement = dbOperations.execute(buf);
	memset(buf, 0, SQL_LEN);
	PoliclinicStorage::getInstance()->dropPatient(patient);
	SQLFreeStmt(statement, SQL_CLOSE);
}

void PatientRepo::edit(const Patient& patient) {
	char buf[SQL_LEN];
	vector<string> splitName;
	vector<string> splitAddress;

	splitName = splitFullNamePatient(patient);
	splitAddress = splitAddressPatient(patient);

	if (splitName.size() == 3) {
		sprintf_s(buf, "UPDATE patients SET last_name = '%s', first_name = '%s', middle_name = '%s', \
						city = '%s', street = '%s', building = '%s', apartment = '%s' \
						WHERE id_patient = % d;", 
						splitName.at(0).c_str(), splitName.at(1).c_str(), splitName.at(2).c_str(), 
						splitAddress.at(0).c_str(), splitAddress.at(1).c_str(), splitAddress.at(2).c_str(), splitAddress.at(3).c_str(),
						patient.getId());
	}
	else {
		sprintf_s(buf, "UPDATE patients SET last_name = '%s', first_name = '%s', middle_name = NULL, \
						city = '%s', street = '%s', building = '%s', apartment = '%s' \
						WHERE id_patient = % d;",
						splitName.at(0).c_str(), splitName.at(1).c_str(),
						splitAddress.at(0).c_str(), splitAddress.at(1).c_str(), splitAddress.at(2).c_str(), splitAddress.at(3).c_str(),
						patient.getId());
	}

	SQLHSTMT statement = dbOperations.execute(buf);
	SQLFreeStmt(statement, SQL_CLOSE);
	memset(buf, 0, SQL_LEN);

	PoliclinicStorage::getInstance()->putPatient(patient); // В singleton структуру помещаю изменённый элемент
}

vector<string> PatientRepo::splitFullNamePatient(const Patient& patient) {
	vector<string> split;

	// Алгоритм разделения строк по пробелу
	list<string> words;
	string tmp;
	istringstream ist(patient.getName());
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

vector<string> PatientRepo::splitAddressPatient(const Patient& patient) {
	vector<string> split;
	string str = patient.getAddress();
	
	// Замена "," на " "
	for (int i = 0; i < str.length(); i++) {
		if (str[i] == ',') {
			str[i] = ' ';
		}
	}

	// Алгоритм разделения строк по пробелу
	list<string> words;
	string tmp;
	istringstream ist(str);
	while (ist >> tmp)
		words.push_back(tmp);

	list<string>::iterator it = words.begin();
	split.push_back(*next(it, 0));
	split.push_back(*next(it, 1));
	split.push_back(*next(it, 2));
	split.push_back(*next(it, 3));

	return split;
}