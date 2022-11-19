#pragma once

// Класс приёма пациента

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <map>
#include "Doctor.h"
#include "Patient.h"

class Reception {
private:
	long id;
	std::string dateOfReception; // Дата приёма
	std::string complaints; // Жалобы
	std::string diagnosis; // Диагноз
	std::vector<std::string> medications; // Назначенные лекарства
	std::vector<std::string> procedures; // Назначенные процедуры
	std::map<std::string, std::string> analyses; // Название анализа (ключ) + результат анализа (значение)

	std::string dateOfExtract; // Дата "вывписки"
	bool isDone; // Логический идентификатор открытости приёма

	std::string timeOfReception; // Время первичной записи на приём

	Doctor doctor;
	Patient patient;
public:
	Reception();
	Reception(const Reception* reception);
	~Reception();

	long getId() const;
	void setId(long);

	std::string getDateOfReception() const;
	void setDateOfReception(std::string dateOfReception);

	std::string getComplaints() const;
	void setComplaints(std::string complaints);

	std::string getDiagnosis() const;
	void setDiagnosis(std::string diagnosis);

	std::vector<std::string> getMedications() const;
	void setMedications(std::vector<std::string> medications);

	std::vector<std::string> getProcedures() const;
	void setProcedures(std::vector<std::string> procedures);

	std::map<std::string, std::string> getAnalyses() const;
	void setAnalyses(std::map<std::string, std::string> analyses);

	std::string getDateOfExtract() const;
	void setDateOfExtract(std::string dateOfExtract);

	bool getIsDone() const;
	void setIsDone(bool isDone);

	std::string getTimeOfReception() const;
	void setTimeOfReception(std::string timeOfReception);

	Doctor getDoctor() const;
	void setDoctor(Doctor doctor);

	Patient getPatient() const;
	void setPatient(Patient patient);

	friend bool operator==(const Reception& left, const Reception& right);
	friend bool operator!=(const Reception& left, const Reception& right);
};

long Reception::getId() const
{
	return this->id;
}
void Reception::setId(long id)
{
	this->id = id;
}

Reception::Reception() :
	id(-1),
	dateOfReception(),
	complaints(),
	diagnosis(),
	medications(),
	procedures(),
	analyses(),
	dateOfExtract(),
	isDone(),
	timeOfReception(),
	doctor(),
	patient() {
}
Reception::Reception(const Reception* reception) {
	this->id = reception->getId();
	this->dateOfReception = reception->getDateOfReception();
	this->complaints = reception->getComplaints();
	this->diagnosis = reception->getDiagnosis();
	this->medications = reception->getMedications();
	this->procedures = reception->getProcedures();
	this->analyses = reception->getAnalyses();
	this->timeOfReception = reception->getTimeOfReception();
	this->dateOfExtract = reception->getDateOfExtract();
	this->isDone = reception->getIsDone();
	this->doctor = reception->getDoctor();
	this->patient = reception->getPatient();
}
Reception::~Reception() {

}

std::string Reception::getDateOfReception() const
{
	return this->dateOfReception;
}
void Reception::setDateOfReception(std::string dateOfReception)
{
	this->dateOfReception = dateOfReception;
}

std::string Reception::getComplaints() const
{
	return this->complaints;
}
void Reception::setComplaints(std::string complaints)
{
	this->complaints = complaints;
}

std::string Reception::getDiagnosis() const
{
	return this->diagnosis;
}
void Reception::setDiagnosis(std::string diagnosis)
{
	this->diagnosis = diagnosis;
}

std::vector<std::string> Reception::getMedications() const {
	return this->medications;
}
void Reception::setMedications(std::vector<std::string> medications) {
	this->medications = medications;
}

std::vector<std::string> Reception::getProcedures() const
{
	return this->procedures;
}
void Reception::setProcedures(std::vector<std::string> procedures)
{
	this->procedures = procedures;
}

std::map<std::string, std::string> Reception::getAnalyses() const
{
	return this->analyses;
}
void Reception::setAnalyses(std::map<std::string, std::string> analyses)
{
	this->analyses = analyses;
}

std::string Reception::getDateOfExtract() const
{
	return this->dateOfExtract;
}
void Reception::setDateOfExtract(std::string dateOfExtract)
{
	this->dateOfExtract = dateOfExtract;
}

bool Reception::getIsDone() const
{
	return this->isDone;
}
void Reception::setIsDone(bool isDone)
{
	this->isDone = isDone;
}

std::string Reception::getTimeOfReception() const
{
	return this->timeOfReception;
}
void Reception::setTimeOfReception(std::string timeOfReception)
{
	this->timeOfReception = timeOfReception;
}

Doctor Reception::getDoctor() const
{
	return this->doctor;
}
void Reception::setDoctor(Doctor doctor)
{
	this->doctor = doctor;
}

Patient Reception::getPatient() const
{
	return this->patient;
}
void Reception::setPatient(Patient patient)
{
	this->patient = patient;
}

bool operator==(const Reception& left, const Reception& right) {
	return
		left.id == right.id &&
		left.dateOfReception == right.dateOfReception &&
		left.complaints == right.complaints &&
		left.diagnosis == right.diagnosis &&
		left.medications == right.medications &&
		left.procedures == right.procedures &&
		left.analyses == right.analyses &&
		left.dateOfExtract == right.dateOfExtract &&
		left.isDone == right.isDone &&
		left.timeOfReception == right.timeOfReception &&
		left.doctor == right.doctor &&
		left.patient == right.patient;
}

bool operator!=(const Reception& left, const Reception& right) {
	return
		left.id != right.id ||
		left.dateOfReception != right.dateOfReception ||
		left.complaints != right.complaints ||
		left.diagnosis != right.diagnosis ||
		left.medications != right.medications ||
		left.procedures != right.procedures ||
		left.analyses != right.analyses ||
		left.dateOfExtract != right.dateOfExtract ||
		left.isDone != right.isDone ||
		left.timeOfReception != right.timeOfReception ||
		left.doctor != right.doctor ||
		left.patient != right.patient;
}