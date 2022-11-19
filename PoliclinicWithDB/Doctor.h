#pragma once

// Класс доктора

#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <iomanip>
#include <vector>

class Doctor {
private:
	long id;
	std::string full_name;
	std::string specialization;
	std::string cabinet;
	int plot;
	//std::map<std::string, std::string> timetable;
	std::vector<std::pair<std::string, std::string>> timetable; // День недели (ключ) + , например, "8:00/17:00" (значение)
public:
	Doctor();
	~Doctor();
	Doctor(const Doctor* doctor);

	long getId() const;
	void setId(long);

	std::string getName() const;
	void setName(std::string full_name);

	std::string getSpecialization() const;
	void setSpecialization(std::string specialization);

	std::string getCabinet() const;
	void setCabinet(std::string cabinet);

	int getPlot() const;
	void setPlot(int plot);

	//std::map<std::string, std::string> getTimetable() const;
	// setTimetable(std::map<std::string, std::string> timetable);

	std::vector<std::pair<std::string, std::string>> getTimetable() const;
	void setTimetable(std::vector<std::pair<std::string, std::string>> gop);
	void addTimetableToCurrent(std::vector<std::pair<std::string, std::string>> newValues);
	void sortTimetable();

	friend bool operator==(const Doctor& left, const Doctor& right);
	friend bool operator!=(const Doctor& left, const Doctor& right);
};

Doctor::Doctor() :
	id(-1),
	full_name(),
	specialization(),
	cabinet(),
	plot(),
	timetable() {
}
Doctor::~Doctor() {
}
Doctor::Doctor(const Doctor* doctor) {
	this->id = doctor->getId();
	this->full_name = doctor->getName();
	this->specialization = doctor->getSpecialization();
	this->cabinet = doctor->getCabinet();
	this->plot = doctor->getPlot();
	this->timetable = doctor->getTimetable();
}

bool operator==(const Doctor& left, const Doctor& right) {
	return
		left.id == right.id &&
		left.full_name == right.full_name &&
		left.specialization == right.specialization &&
		left.cabinet == right.cabinet &&
		left.plot == right.plot &&
		left.timetable == right.timetable;
}

bool operator!=(const Doctor& left, const Doctor& right) {
	return
		left.id != right.id ||
		left.full_name != right.full_name ||
		left.specialization != right.specialization ||
		left.cabinet != right.cabinet ||
		left.plot != right.plot;//&&
		//left.timetable != right.timetable;
}

void Doctor::sortTimetable() {
	std::vector<std::pair<int, std::string>> dictionary;
	std::vector<std::pair<std::string, std::string>> localTimetable;

	dictionary.push_back(std::make_pair(0,"Понедельник"));
	dictionary.push_back(std::make_pair(1, "Вторник"));
	dictionary.push_back(std::make_pair(2, "Среда"));
	dictionary.push_back(std::make_pair(3, "Четверг"));
	dictionary.push_back(std::make_pair(4, "Пятница"));
	dictionary.push_back(std::make_pair(5, "Суббота"));
	dictionary.push_back(std::make_pair(6, "Воскресенье"));

	for (int i = 0; i < dictionary.size(); i++) {
		for (int j = 0; j < this->timetable.size(); j++) {
			if (dictionary.at(i).second == this->timetable.at(j).first) {
				localTimetable.push_back(std::make_pair(this->timetable.at(j).first, this->timetable.at(j).second));
			}
		}
	}

	this->timetable = localTimetable;
}

void Doctor::addTimetableToCurrent(std::vector<std::pair<std::string, std::string>> newValues) {
	/*for (int i = 0; i < this->timetable.size(); i++) {
		for (int j = 0; j < newValues.size(); j++) {
			if (this->timetable.at(i).first == newValues.at(j).first) {
				this->timetable.at(i).second = newValues.at(j).second;
			}
			else {
				this->timetable.insert(this->timetable.begin() + i, make_pair(newValues.at(j).first, newValues.at(j).second));
			}
		}
	}*/

	// Замена существующих записей
	for (int i = 0; i < newValues.size(); i++) {
		for (int j = 0; j < this->timetable.size(); j++) {
			if (this->timetable.at(j).first == newValues.at(i).first) {
				this->timetable.at(j).second = newValues.at(i).second;
			}
		}
	}

	// Добавление новых
	int count = 0;
	int insert = 0;
	for (int i = 0; i < newValues.size(); i++) {
		for (int j = 0; j < this->timetable.size(); j++) {
			if (this->timetable.at(j).first == newValues.at(i).first) {
				count++;
				break;
			}
		}

		if (count == 1) {
			count = 0;
			continue;
		}
		else if (count == 0) {
				this->timetable.push_back(make_pair(newValues.at(i).first, newValues.at(i).second));
		}
	}
}

long Doctor::getId() const {
	return this->id;
}
void Doctor::setId(long id) {
	this->id = id;
}

std::string Doctor::getName() const {
	return this->full_name;
}
void Doctor::setName(std::string full_name) {
	this->full_name = full_name;
}

std::string Doctor::getSpecialization() const {
	return this->specialization;
}
void Doctor::setSpecialization(std::string specialization) {
	this->specialization = specialization;
}

std::string Doctor::getCabinet() const {
	return this->cabinet;
}
void Doctor::setCabinet(std::string cabinet) {
	this->cabinet = cabinet;
}

int Doctor::getPlot() const {
	return this->plot;
}
void Doctor::setPlot(int plot) {
	this->plot = plot;
}

std::vector<std::pair<std::string, std::string>> Doctor::getTimetable() const {
	return this->timetable;
}
void Doctor::setTimetable(std::vector<std::pair<std::string, std::string>> timetable) {
	this->timetable = timetable;
}