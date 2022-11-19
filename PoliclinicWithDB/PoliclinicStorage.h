#pragma once

// Хранит всю информацию об объектах Doctor, Patient, Reception

#include <map>

#include "Doctor.h"
#include "Patient.h"
#include "Reception.h"

class PoliclinicStorage {
private:
	// id + object
	std::map<long, Doctor> doctors;
	std::map<long, Patient> patients;
	std::map<long, Reception> receptions;

	PoliclinicStorage();

	static PoliclinicStorage* instance;
public:
	PoliclinicStorage(PoliclinicStorage& other) = delete;
	void operator=(const PoliclinicStorage&) = delete;

	static PoliclinicStorage* getInstance();

	Doctor* getDoctor(long);
	Patient* getPatient(long);
	Reception* getReception(long);

	void putDoctor(Doctor doctor);
	void putPatient(Patient patient);
	void putReception(Reception reception);

	void updateDoctor(Doctor doctor);
	void updatePatient(Patient patient);
	void updateReception(Reception reception);

	void dropDoctor(Doctor doctor);
	void dropPatient(Patient patient);
	void dropReception(Reception reception);
	//void putPatient(Patient patient) {};
	//void putReception(Reception reception) {};
};

PoliclinicStorage* PoliclinicStorage::instance;

PoliclinicStorage::PoliclinicStorage() :
	doctors(),
	patients(),
	receptions()
{
}

inline PoliclinicStorage* PoliclinicStorage::getInstance()
{
	if (instance == nullptr) {
		instance = new PoliclinicStorage();
	}
	return instance;
}

inline Doctor* PoliclinicStorage::getDoctor(long id)
{
	return &(this->doctors.find(id)->second); // Получить значение по ключу из map
}

inline Patient* PoliclinicStorage::getPatient(long id)
{
	return &(this->patients.find(id)->second);
}

inline Reception* PoliclinicStorage::getReception(long id)
{
	return &(this->receptions.find(id)->second);
}

inline void PoliclinicStorage::putDoctor(Doctor doctor)
{
	auto it = this->doctors.find(doctor.getId());
	if (it == this->doctors.end()) {
		this->doctors[doctor.getId()] = doctor;
		return;
	}
	it->second.setName(doctor.getName());
	it->second.setCabinet(doctor.getCabinet());
	it->second.setPlot(doctor.getPlot());
	it->second.setSpecialization(doctor.getSpecialization());
	it->second.setTimetable(doctor.getTimetable());
}

void PoliclinicStorage::updateDoctor(Doctor doctor) {
	auto it = this->doctors.find(doctor.getId());
	if (it->second == doctor) 
		return;

	it->second.setName(doctor.getName());
	it->second.setCabinet(doctor.getCabinet());
	it->second.setPlot(doctor.getPlot());
	it->second.setSpecialization(doctor.getSpecialization());
	it->second.setTimetable(doctor.getTimetable());
}

inline void PoliclinicStorage::putPatient(Patient patient)
{
	auto it = this->patients.find(patient.getId());
	if (it == this->patients.end()) {
		this->patients[patient.getId()] = patient;
		return;
	}
	it->second.setName(patient.getName());
	it->second.setAddress(patient.getAddress());
}

void PoliclinicStorage::updatePatient(Patient patient) {
	auto it = this->patients.find(patient.getId());
	if (it->second == patient)
		return;

	it->second.setName(patient.getName());
	it->second.setAddress(patient.getAddress());
}

inline void PoliclinicStorage::putReception(Reception reception)
{
	auto it = this->receptions.find(reception.getId());
	if (it == this->receptions.end()) {
		this->receptions[reception.getId()] = reception;
		return;
	}

	auto& f = it->second;
	f.setAnalyses(reception.getAnalyses());
	f.setComplaints(reception.getComplaints());
	f.setDateOfExtract(reception.getDateOfExtract());
	f.setDateOfReception(reception.getDateOfReception());
	f.setDiagnosis(reception.getDiagnosis());
	f.setIsDone(reception.getIsDone());
	f.setMedications(reception.getMedications());
	f.setProcedures(reception.getProcedures());
	f.setDoctor(reception.getDoctor());
	f.setPatient(reception.getPatient());
}

void PoliclinicStorage::updateReception(Reception reception) {
	auto it = this->receptions.find(reception.getId());
	if (it->second == reception)
		return;

	it->second.setDateOfReception(reception.getDateOfReception());
	it->second.setComplaints(reception.getComplaints());
	it->second.setDiagnosis(reception.getDiagnosis());
	it->second.setMedications(reception.getMedications());
	it->second.setProcedures(reception.getProcedures());
	it->second.setAnalyses(reception.getAnalyses());
	it->second.setDateOfExtract(reception.getDateOfExtract());
	it->second.setIsDone(reception.getIsDone());
	it->second.setTimeOfReception(reception.getTimeOfReception());
	it->second.setDoctor(reception.getDoctor());
	it->second.setPatient(reception.getPatient());
}

inline void PoliclinicStorage::dropDoctor(Doctor doctor) {
	for (auto pos = doctors.begin(); pos != doctors.end();) {
		if (pos->first == doctor.getId()) {
			pos = doctors.erase(pos);
			break;
		}
		else {
			++pos;
		}
	}
}

inline void PoliclinicStorage::dropPatient(Patient patient) {
	for (auto pos = patients.begin(); pos != patients.end();) {
		if (pos->first == patient.getId()) {
			pos = patients.erase(pos);
			break;
		}
		else {
			++pos;
		}
	}
}

inline void PoliclinicStorage::dropReception(Reception reception) {
	for (auto pos = receptions.begin(); pos != receptions.end();) {
		if (pos->first == reception.getId()) {
			pos = receptions.erase(pos);
			break;
		}
		else {
			++pos;
		}
	}
}