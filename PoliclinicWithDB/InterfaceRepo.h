#pragma once

#include "DB_Operations.h"
#include <vector>

template<class T, class IdType>
class InterfaceRepo {

protected:
	DB_Operations& dbOperations;


public:
	InterfaceRepo(DB_Operations& dbOperations);

	virtual void add(const T& data) = 0;

	virtual void edit(const T& data) = 0;

	virtual std::vector<T*> findAll() = 0;

	virtual void remove(const T& data) = 0;
};

template<class T, class IdType>
InterfaceRepo<T, IdType>::InterfaceRepo(DB_Operations& dbOperations) : dbOperations(dbOperations) 
{
}