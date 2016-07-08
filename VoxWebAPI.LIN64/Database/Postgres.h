//
// Created by kek on 08.07.16.
//

#ifndef VOXWEBSERVER_LIN64_POSTGRES_H
#define VOXWEBSERVER_LIN64_POSTGRES_H

#include "VoxWebAPI.h"
#include <pqxx/pqxx>

class Postgres
{
public:
	Postgres(string database, string user, string password, unsigned short port = 5432, string host = "localhost");

	pqxx::result GetQuery(string query);

	pqxx::result AffectQuery(string query);

private:
	pqxx::connection *con;
	pthread_mutex_t cs;
};


#endif //VOXWEBSERVER_LIN64_POSTGRES_H
