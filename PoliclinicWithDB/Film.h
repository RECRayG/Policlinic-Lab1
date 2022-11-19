#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "Genre.h"
#include "Actor.h"

class Film {
private:
	long id;
	std::string name;
	std::vector<Genre*> genres;
	std::vector<Actor*> actors;

	double rating;
	bool seen;

public:
	Film();
	~Film();

	long getId() const;
	void setId(long);

	std::string getName() const;
	void setName(const std::string& name);

	std::vector<Genre*> getGenres() const;
	void setGenres(const std::vector<Genre*> genres);

	std::vector<Actor*> getActors() const;
	void setActors(const std::vector<Actor*> actors);

	double getRating() const;
	void setRating(double rating);

	bool isSeen() const;
	void setSeen(bool seen);
};

Film::Film() :
	id(-1),
	name(),
	genres(),
	actors(),
	rating(),
	seen()
{
}

Film::~Film()
{
}

long Film::getId() const
{
	return this->id;
}

void Film::setId(long id)
{
	this->id = id;
}

std::string Film::getName() const
{
	return this->name;
}

void Film::setName(const std::string& name)
{
	this->name = name;
}

std::vector<Genre*> Film::getGenres() const
{
	return this->genres;
}

void Film::setGenres(const std::vector<Genre*> genres)
{
	this->genres = genres;
}

std::vector<Actor*> Film::getActors() const
{
	return this->actors;
}

void Film::setActors(const std::vector<Actor*> actors)
{
	this->actors = actors;
}

double Film::getRating() const
{
	return this->rating;
}

void Film::setRating(double rating)
{
	this->rating = rating;
}

bool Film::isSeen() const
{
	return this->seen;
}

void Film::setSeen(bool seen)
{
	this->seen = seen;
}
