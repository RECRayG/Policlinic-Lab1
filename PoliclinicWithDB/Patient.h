#pragma once

// Класс пациента

#include <iostream>
#include <string>

class Patient {
private:
	long id;
	std::string full_name;
	std::string address;
public:
	Patient();
	Patient(const Patient* patient);
	~Patient();

	long getId() const;
	void setId(long);

	std::string getName() const;
	void setName(std::string name);

	std::string getAddress() const;
	void setAddress(std::string address);

	friend bool operator==(const Patient& left, const Patient& right);
	friend bool operator!=(const Patient& left, const Patient& right);
};

Patient::Patient() :
	id(-1),
	full_name(),
	address() {
}
Patient::Patient(const Patient* patient) {
	this->id = patient->getId();
	this->full_name = patient->getName();
	this->address = patient->getAddress();
}
Patient::~Patient() {
}

bool operator==(const Patient& left, const Patient& right) {
	return
		left.id == right.id &&
		left.full_name == right.full_name &&
		left.address == right.address;
}

bool operator!=(const Patient& left, const Patient& right) {
	return
		left.id != right.id ||
		left.full_name != right.full_name ||
		left.address != right.address;
}

long Patient::getId() const {
	return this->id;
}
void Patient::setId(long id) {
	this->id = id;
}

std::string Patient::getName() const {
	return this->full_name;
}
void Patient::setName(std::string full_name) {
	this->full_name = full_name;
}

std::string Patient::getAddress() const {
	return this->address;
}
void Patient::setAddress(std::string address) {
	this->address = address;
}