#pragma once

#include "Genre.h"
#include "BaseRepo.h"

#include "FilmStorage.h"

#define ID_LEN 8
#define NAME_LEN 256


class GenreRepo : public BaseRepo<Genre, long> {

private:
	SQLINTEGER id;
	SQLCHAR name[NAME_LEN];

public:
	GenreRepo(DB_Operations& dbOperations);
	void save(const Genre& genre) override;
	Genre* findById(long id) override;
	std::vector<Genre*> findAll() override;
	void remove(long id) override;

	std::vector<Genre*> findByFilmId(long i);
};

GenreRepo::GenreRepo(DB_Operations& dbOperations) :
	BaseRepo(dbOperations),
	id(),
	name()
{
}

void GenreRepo::save(const Genre& genre)
{
	char buf[512];
	if (genre.getId() != -1) {
		sprintf_s(buf, "update genre set name = '%s' where id = %d", genre.getName().c_str(), genre.getId());
	}
	else {
		sprintf_s(buf, "insert into genre(name) values ('%s')", genre.getName().c_str());
	}
	dbOperations.execute(buf);
}

Genre* GenreRepo::findById(long id)
{
	char buf[64];
	sprintf_s(buf, "select id, name from genre where id = %d", id);

	SQLHSTMT statement = dbOperations.execute(buf);
	SQLRETURN retcode;
	retcode = SQLBindCol(statement, 1, SQL_C_LONG, &this->id, 2, nullptr);
	retcode = SQLBindCol(statement, 2, SQL_C_CHAR, &name, NAME_LEN, nullptr);

	retcode = SQLFetch(statement);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		Genre genre;
		genre.setId(id);
		genre.setName(reinterpret_cast<char*>(name));
		FilmStorage::getInstance()->putGenre(genre);
		SQLFreeStmt(statement, SQL_CLOSE);
		return FilmStorage::getInstance()->getGenre(genre.getId());
	}
	else if (retcode != SQL_NO_DATA) {
		std::cout << "Error fetching genre\n";
	}
	return nullptr;
}

std::vector<Genre*> GenreRepo::findAll() {

	std::vector<Genre*> genres;

	SQLHSTMT statement = dbOperations.execute("select id, name from genre");
	SQLRETURN retcode;
	retcode = SQLBindCol(statement, 1, SQL_C_LONG, &id, 2, nullptr);
	retcode = SQLBindCol(statement, 2, SQL_C_CHAR, &name, NAME_LEN, nullptr);

	while (true) {
		retcode = SQLFetch(statement);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			Genre genre;
			genre.setId(id);
			genre.setName(reinterpret_cast<char*>(name));
			FilmStorage::getInstance()->putGenre(genre);
			genres.push_back(FilmStorage::getInstance()->getGenre(genre.getId()));
		}
		else {
			if (retcode != SQL_NO_DATA) {
				std::cout << "Error fetching genres\n";
			}
			break;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);
	return genres;
}

void GenreRepo::remove(long id)
{
	char buf[64];
	sprintf_s(buf, "delete from genre where id = %d", id);

	dbOperations.execute(buf);
}

std::vector<Genre*> GenreRepo::findByFilmId(long id)
{
	std::vector<Genre*> genres;

	char buf[255];
	sprintf_s(buf, "select g.id, g.name from film f join film_genre fg on fg.film_id = f.id join genre g on g.id = fg.genre_id where f.id = %d", id);

	SQLHSTMT statement = dbOperations.execute(buf);
	SQLRETURN retcode;
	retcode = SQLBindCol(statement, 1, SQL_C_LONG, &id, 2, nullptr);
	retcode = SQLBindCol(statement, 2, SQL_C_CHAR, &name, NAME_LEN, nullptr);

	while (true) {
		retcode = SQLFetch(statement);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			Genre genre;
			genre.setId(id);
			genre.setName(reinterpret_cast<char*>(name));
			FilmStorage::getInstance()->putGenre(genre);
			genres.push_back(FilmStorage::getInstance()->getGenre(genre.getId()));
		}
		else {
			if (retcode != SQL_NO_DATA) {
				std::cout << "Error fetching genres\n";
			}
			break;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);
	return genres;
}
