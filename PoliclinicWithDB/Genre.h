#pragma once

#include <iostream>
#include <string>

class Genre {
private:
	long id;
	std::string name;

public:
	Genre();
	~Genre();
	long getId() const;
	void setId(long);

	std::string getName() const;
	void setName(const std::string& name);
};

Genre::Genre() :
	id(-1),
	name()
{
}

Genre::~Genre()
{
}

long Genre::getId() const {
	return this->id;
}

void Genre::setId(long id) {
	this->id = id;
}

std::string Genre::getName() const
{
	return this->name;
}

void Genre::setName(const std::string& name)
{
	this->name = name;
}
