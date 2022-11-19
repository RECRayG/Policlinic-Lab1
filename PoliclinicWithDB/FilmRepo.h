#pragma once

#include "BaseRepo.h"
#include "ActorRepo.h"
#include "GenreRepo.h"
#include "Film.h"

#include "FilmStorage.h"


#define ID_LEN 8
#define NAME_LEN 256
#define RATING_LEN 8
#define SEEN_LEN 8

class FilmRepo : public BaseRepo<Film, long> {

private:
	ActorRepo& actorRepo;
	GenreRepo& genreRepo;

	SQLINTEGER id;
	SQLWCHAR name[NAME_LEN];
	SQLDOUBLE rating;
	SQLCHAR seen;

public:
	FilmRepo(DB_Operations& dbOperations, ActorRepo& actorRepo, GenreRepo& genreRepo);
	void save(const Film& genre) override;
	Film* findById(long id) override;
	std::vector<Film*> findAll() override;
	void remove(long id) override;

	void addActor(long id, long actorId);
	void addGenre(long id, long genreId);

	void removeActor(long id, long actorId);
	void removeGenre(long id, long genreId);

	std::vector<Film*> findByName(std::string& name);
	std::vector<Film*> findByActorName(std::string& actorName);
	std::vector<Film*> findByGenreName(std::string& genreName);
	std::vector<Film*> findByRatingGreaterAndSeenIsFalse(double rating);
};

FilmRepo::FilmRepo(DB_Operations& dbOperations, ActorRepo& actorRepo, GenreRepo& genreRepo) :
	BaseRepo(dbOperations),
	actorRepo(actorRepo),
	genreRepo(genreRepo),
	id(),
	name(),
	rating(),
	seen()
{
}

void FilmRepo::save(const Film& film)
{
	char buf[512];
	//if (film.getId() != -1) {
	//	sprintf_s(buf, "update film set name = '%s', rating = %f, seen = %s where id = %d", film.getName().c_str(), film.getRating(), film.isSeen() ? "true" : "false", film.getId());
	//}
	//else {
	//	sprintf_s(buf, "insert into film(name, rating, seen) values ('%s', %f, %s)", film.getName().c_str(), film.getRating(), film.isSeen() ? "true" : "false");
	//}

	sprintf_s(buf, "create table if not exists auditory("
		"id serial primary key,"
		"auditory varchar(15) unique"
		");");

	dbOperations.execute(buf);
}

Film* FilmRepo::findById(long id)
{
	return nullptr;
}

std::vector<Film*> FilmRepo::findAll() {

	std::vector<Film> films;

	SQLHSTMT statement = dbOperations.execute("select id, name, rating, seen from film");
	SQLRETURN retcode;
	retcode = SQLBindCol(statement, 1, SQL_C_LONG, &id, 2, nullptr);
	retcode = SQLBindCol(statement, 2, SQL_C_CHAR, &name, NAME_LEN, nullptr);
	retcode = SQLBindCol(statement, 3, SQL_C_DOUBLE, &rating, RATING_LEN, nullptr);
	retcode = SQLBindCol(statement, 4, SQL_C_CHAR, &seen, SEEN_LEN, nullptr);

	while (true) {
		retcode = SQLFetch(statement);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			Film film;
			film.setId(id);
			film.setName(reinterpret_cast<char*>(name));
			film.setRating(rating);
			film.setSeen(seen == '1');
			films.push_back(film);
		}
		else {
			if (retcode != SQL_NO_DATA) {
				std::cout << "Error fetching actors\n";
			}
			break;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);
	for (auto& film : films) {
		film.setActors(actorRepo.findByFilmId(film.getId()));
		film.setGenres(genreRepo.findByFilmId(film.getId()));
	}
	std::vector<Film*> ret;
	for (auto& film : films) {
		FilmStorage::getInstance()->putFilm(film);
		ret.push_back(FilmStorage::getInstance()->getFilm(film.getId()));
	}

	return ret;
}

void FilmRepo::remove(long id)
{
	char buf[64];
	sprintf_s(buf, "delete from film where id = %d", id);

	dbOperations.execute(buf);
}

void FilmRepo::addActor(long id, long actorId)
{
	char buf[128];
	sprintf_s(buf, "insert into film_actor(film_id, actor_id) values (%d, %d)", id, actorId);

	dbOperations.execute(buf);
}

void FilmRepo::addGenre(long id, long genreId)
{
	char buf[128];
	sprintf_s(buf, "insert into film_genre(film_id, genre_id) values (%d, %d)", id, genreId);

	dbOperations.execute(buf);
}

void FilmRepo::removeActor(long id, long actorId)
{
	char buf[128];
	sprintf_s(buf, "delete from film_actor where (film_id = %d and actor_id = %d)", id, actorId);

	dbOperations.execute(buf);
}

void FilmRepo::removeGenre(long id, long genreId)
{
	char buf[128];
	sprintf_s(buf, "delete from film_genre where (film_id = %d and genre_id = %d)", id, genreId);

	dbOperations.execute(buf);
}

inline std::vector<Film*> FilmRepo::findByName(std::string& name)
{
	char buf[512];
	sprintf_s(buf, "select id, name, rating, seen from film where name like '%%%s%%'", name.c_str());

	std::vector<Film> films;

	SQLHSTMT statement = dbOperations.execute(buf);
	SQLRETURN retcode;
	retcode = SQLBindCol(statement, 1, SQL_C_LONG, &id, 2, nullptr);
	retcode = SQLBindCol(statement, 2, SQL_C_CHAR, &this->name, NAME_LEN, nullptr);
	retcode = SQLBindCol(statement, 3, SQL_C_DOUBLE, &rating, RATING_LEN, nullptr);
	retcode = SQLBindCol(statement, 4, SQL_C_CHAR, &seen, SEEN_LEN, nullptr);

	while (true) {
		retcode = SQLFetch(statement);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			Film film;
			film.setId(id);
			film.setName(reinterpret_cast<char*>(this->name));
			film.setRating(rating);
			film.setSeen(seen == '1');
			films.push_back(film);
		}
		else {
			if (retcode != SQL_NO_DATA) {
				std::cout << "Error fetching actors\n";
			}
			break;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);
	for (auto& film : films) {
		film.setActors(actorRepo.findByFilmId(film.getId()));
		film.setGenres(genreRepo.findByFilmId(film.getId()));
	}
	std::vector<Film*> ret;
	for (auto& film : films) {
		FilmStorage::getInstance()->putFilm(film);
		ret.push_back(FilmStorage::getInstance()->getFilm(film.getId()));
	}

	return ret;
}

inline std::vector<Film*> FilmRepo::findByActorName(std::string& actorName)
{
	char buf[512];
	sprintf_s(buf, "select f.* from film f join film_actor fa on fa.film_id = f.id join actor a on a.id = fa.actor_id where a.name like '%%%s%%'", actorName.c_str());

	std::vector<Film> films;

	SQLHSTMT statement = dbOperations.execute(buf);
	SQLRETURN retcode;
	retcode = SQLBindCol(statement, 1, SQL_C_LONG, &id, 2, nullptr);
	retcode = SQLBindCol(statement, 2, SQL_C_CHAR, &name, NAME_LEN, nullptr);
	retcode = SQLBindCol(statement, 3, SQL_C_DOUBLE, &rating, RATING_LEN, nullptr);
	retcode = SQLBindCol(statement, 4, SQL_C_CHAR, &seen, SEEN_LEN, nullptr);

	while (true) {
		retcode = SQLFetch(statement);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			Film film;
			film.setId(id);
			film.setName(reinterpret_cast<char*>(name));
			film.setRating(rating);
			film.setSeen(seen == '1');
			films.push_back(film);
		}
		else {
			if (retcode != SQL_NO_DATA) {
				std::cout << "Error fetching actors\n";
			}
			break;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);
	for (auto& film : films) {
		film.setActors(actorRepo.findByFilmId(film.getId()));
		film.setGenres(genreRepo.findByFilmId(film.getId()));
	}
	std::vector<Film*> ret;
	for (auto& film : films) {
		FilmStorage::getInstance()->putFilm(film);
		ret.push_back(FilmStorage::getInstance()->getFilm(film.getId()));
	}

	return ret;
}

inline std::vector<Film*> FilmRepo::findByGenreName(std::string& genreName)
{
	char buf[512];
	sprintf_s(buf, "select f.* from film f join film_genre fg on fg.film_id = f.id join genre g on g.id = fg.genre_id where g.name like '%%%s%%'", genreName.c_str());

	std::vector<Film> films;

	SQLHSTMT statement = dbOperations.execute(buf);
	SQLRETURN retcode;
	retcode = SQLBindCol(statement, 1, SQL_C_LONG, &id, 2, nullptr);
	retcode = SQLBindCol(statement, 2, SQL_C_CHAR, &name, NAME_LEN, nullptr);
	retcode = SQLBindCol(statement, 3, SQL_C_DOUBLE, &rating, RATING_LEN, nullptr);
	retcode = SQLBindCol(statement, 4, SQL_C_CHAR, &seen, SEEN_LEN, nullptr);

	while (true) {
		retcode = SQLFetch(statement);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			Film film;
			film.setId(id);
			film.setName(reinterpret_cast<char*>(name));
			film.setRating(rating);
			film.setSeen(seen == '1');
			films.push_back(film);
		}
		else {
			if (retcode != SQL_NO_DATA) {
				std::cout << "Error fetching actors\n";
			}
			break;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);
	for (auto& film : films) {
		film.setActors(actorRepo.findByFilmId(film.getId()));
		film.setGenres(genreRepo.findByFilmId(film.getId()));
	}
	std::vector<Film*> ret;
	for (auto& film : films) {
		FilmStorage::getInstance()->putFilm(film);
		ret.push_back(FilmStorage::getInstance()->getFilm(film.getId()));
	}

	return ret;
}

std::vector<Film*> FilmRepo::findByRatingGreaterAndSeenIsFalse(double rating)
{
	char buf[512];
	sprintf_s(buf, "select id, name, rating, seen from film where rating >= %f and seen = false", rating);

	std::vector<Film> films;

	SQLHSTMT statement = dbOperations.execute(buf);
	SQLRETURN retcode;
	retcode = SQLBindCol(statement, 1, SQL_C_LONG, &id, 2, nullptr);
	retcode = SQLBindCol(statement, 2, SQL_C_CHAR, &name, NAME_LEN, nullptr);
	retcode = SQLBindCol(statement, 3, SQL_C_DOUBLE, &this->rating, RATING_LEN, nullptr);
	retcode = SQLBindCol(statement, 4, SQL_C_CHAR, &seen, SEEN_LEN, nullptr);

	while (true) {
		retcode = SQLFetch(statement);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			Film film;
			film.setId(id);
			film.setName(reinterpret_cast<char*>(name));
			film.setRating(this->rating);
			film.setSeen(seen == '1');
			films.push_back(film);
		}
		else {
			if (retcode != SQL_NO_DATA) {
				std::cout << "Error fetching actors\n";
			}
			break;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);

	for (auto& film : films) {
		film.setActors(actorRepo.findByFilmId(film.getId()));
		film.setGenres(genreRepo.findByFilmId(film.getId()));
	}

	std::vector<Film*> ret;
	for (auto& film : films) {
		FilmStorage::getInstance()->putFilm(film);
		ret.push_back(FilmStorage::getInstance()->getFilm(film.getId()));
	}

	return ret;
}
