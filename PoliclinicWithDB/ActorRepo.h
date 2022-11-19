#pragma once

#include "Actor.h"
#include "BaseRepo.h"

#include "FilmStorage.h"

#define ID_LEN 8
#define NAME_LEN 256


class ActorRepo : public BaseRepo<Actor, long> {

private:
	SQLINTEGER id;
	SQLCHAR name[NAME_LEN];

public:
	ActorRepo(DB_Operations& dbOperations);
	void save(const Actor& genre) override;
	Actor* findById(long id) override;
	std::vector<Actor*> findAll() override;
	void remove(long id) override;
	std::vector<Actor*> findByFilmId(long id);
};


ActorRepo::ActorRepo(DB_Operations& dbOperations) :
	BaseRepo(dbOperations),
	id(),
	name()
{
}

void ActorRepo::save(const Actor& genre)
{
	char buf[512];
	if (genre.getId() != -1) {
		sprintf_s(buf, "update actor set name = '%s' where id = %d", genre.getName().c_str(), genre.getId());
	}
	else {
		sprintf_s(buf, "insert into actor(name) values ('%s')", genre.getName().c_str());
	}
	dbOperations.execute(buf);
}

Actor* ActorRepo::findById(long id)
{
	char buf[64];
	sprintf_s(buf, "select id, name from actor where id = %d", id);

	SQLHSTMT statement = dbOperations.execute("select id, name from actor where id =");
	SQLRETURN retcode;
	retcode = SQLBindCol(statement, 1, SQL_C_LONG, &this->id, 2, nullptr);
	retcode = SQLBindCol(statement, 2, SQL_C_CHAR, &name, NAME_LEN, nullptr);

	retcode = SQLFetch(statement);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		Actor actor;
		actor.setId(id);
		actor.setName(reinterpret_cast<char*>(name));
		FilmStorage::getInstance()->putActor(actor);
		SQLFreeStmt(statement, SQL_CLOSE);
		return FilmStorage::getInstance()->getActor(actor.getId());
	}
	else if (retcode != SQL_NO_DATA) {
		std::cout << "Error fetching actor\n";
	}
	return nullptr;
}

std::vector<Actor*> ActorRepo::findAll() {

	std::vector<Actor*> actors;

	SQLHSTMT statement = dbOperations.execute("select id, name from actor");
	SQLRETURN retcode;
	retcode = SQLBindCol(statement, 1, SQL_C_LONG, &id, 2, nullptr);
	retcode = SQLBindCol(statement, 2, SQL_C_CHAR, &name, NAME_LEN, nullptr);

	while (true) {
		retcode = SQLFetch(statement);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			Actor actor;
			actor.setId(id);
			actor.setName(reinterpret_cast<char*>(name));
			FilmStorage::getInstance()->putActor(actor);
			actors.push_back(FilmStorage::getInstance()->getActor(actor.getId()));
		}
		else {
			if (retcode != SQL_NO_DATA) {
				std::cout << "Error fetching actors\n";
			}
			break;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);
	return actors;
}

void ActorRepo::remove(long id)
{
	char buf[64];
	sprintf_s(buf, "delete from actor where id = %d", id);

	dbOperations.execute(buf);
}

std::vector<Actor*> ActorRepo::findByFilmId(long id)
{
	std::vector<Actor*> actors;

	char buf[255];
	sprintf_s(buf, "select a.id, a.name from film f join film_actor fa on fa.film_id = f.id join actor a on a.id = fa.actor_id where f.id = %d", id);

	SQLHSTMT statement = dbOperations.execute(buf);
	SQLRETURN retcode;
	retcode = SQLBindCol(statement, 1, SQL_C_LONG, &id, 2, nullptr);
	retcode = SQLBindCol(statement, 2, SQL_C_CHAR, &name, NAME_LEN, nullptr);

	while (true) {
		retcode = SQLFetch(statement);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			Actor actor;
			actor.setId(id);
			actor.setName(reinterpret_cast<char*>(name));
			FilmStorage::getInstance()->putActor(actor);
			actors.push_back(FilmStorage::getInstance()->getActor(actor.getId()));
		}
		else {
			if (retcode != SQL_NO_DATA) {
				std::cout << "Error fetching actors\n";
			}
			break;
		}
	}
	SQLFreeStmt(statement, SQL_CLOSE);
	return actors;
}
