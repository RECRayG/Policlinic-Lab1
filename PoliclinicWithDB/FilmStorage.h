#pragma once

#include <map>

#include "Actor.h"
#include "Film.h"
#include "Genre.h"

class FilmStorage {
private:
	std::map<long, Actor> actors;
	std::map<long, Genre> genres;
	std::map<long, Film> films;

	FilmStorage();

	static FilmStorage* instance;

public:
	FilmStorage(FilmStorage& other) = delete;
	void operator=(const FilmStorage&) = delete;

	static FilmStorage* getInstance();

	Actor* getActor(long);

	Genre* getGenre(long);

	Film* getFilm(long);

	void putActor(Actor actor);

	void putGenre(Genre genre);

	void putFilm(Film film);


};

FilmStorage* FilmStorage::instance;

FilmStorage::FilmStorage() :
	actors(),
	genres(),
	films()
{
}

inline FilmStorage* FilmStorage::getInstance()
{
	if (instance == nullptr) {
		instance = new FilmStorage();
	}
	return instance;
}

inline Actor* FilmStorage::getActor(long id)
{
	return &(this->actors.find(id)->second);
}

inline Genre* FilmStorage::getGenre(long id)
{
	return &(this->genres.find(id)->second);
}

inline Film* FilmStorage::getFilm(long id)
{
	return &(this->films.find(id)->second);
}

inline void FilmStorage::putActor(Actor actor)
{
	auto it = this->actors.find(actor.getId());
	if (it == this->actors.end()) {
		this->actors[actor.getId()] = actor;
		return;
	}
	it->second.setName(actor.getName());
}

inline void FilmStorage::putGenre(Genre genre)
{
	auto it = this->genres.find(genre.getId());
	if (it == this->genres.end()) {
		this->genres[genre.getId()] = genre;
		return;
	}
	it->second.setName(genre.getName());
}

inline void FilmStorage::putFilm(Film film)
{
	auto it = this->films.find(film.getId());
	if (it == this->films.end()) {
		this->films[film.getId()] = film;
		return;
	}
	auto& f = it->second;
	f.setName(film.getName());
	f.setRating(film.getRating());
	f.setSeen(film.isSeen());
	f.setActors(film.getActors());
	f.setGenres(film.getGenres());
}
