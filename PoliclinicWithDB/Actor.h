#pragma once

#include <iostream>
#include <string>

class Actor {
private:
	long id;
	std::string name;

public:
	Actor();
	~Actor();
	long getId() const;
	void setId(long);

	std::string getName() const;
	void setName(std::string name);
};

Actor::Actor() :
	id(-1),
	name()
{
}

Actor::~Actor()
{
}

long Actor::getId() const {
	return this->id;
}

void Actor::setId(long id) {
	this->id = id;
}

std::string Actor::getName() const
{
	return this->name;
}

void Actor::setName(std::string name)
{
	this->name = name;
}

