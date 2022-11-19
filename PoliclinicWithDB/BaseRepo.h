#pragma once

#include "DB_Operations.h"
#include <vector>

template<class T, class IdType>
class BaseRepo {

protected:
	DB_Operations& dbOperations;


public:
	BaseRepo(DB_Operations& dbOperations);

	virtual void save(const T& data) = 0;

	virtual T* findById(IdType id) = 0;

	virtual std::vector<T*> findAll() = 0;

	virtual void remove(IdType id) = 0;
};

template<class T, class IdType>
BaseRepo<T, IdType>::BaseRepo(DB_Operations& dbOperations)
	: dbOperations(dbOperations)
{
}