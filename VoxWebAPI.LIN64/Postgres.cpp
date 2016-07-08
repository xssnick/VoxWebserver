//
// Created by kek on 08.07.16.
//

#include <iostream>
#include "Postgres.h"

Postgres::Postgres(string database, string user, string password, unsigned short port, string host)
{
	pthread_mutex_init(&cs, NULL);
	try
	{
		pqxx::connection *C = new pqxx::connection("user=" + user + " "
				"host=" + host + " "
														   "port=" + to_string(port) + " "
														   "password=" + password + " "
														   "dbname=" + database + " ");

		if (C->is_open())
		{
			cout << "Opened database successfully: " << C->dbname() << endl;
			con = C;
		} else
		{
			cout << "Can't open database" << endl;
			return;
		}
	} catch (const std::exception &e)
	{
		cerr << e.what() << std::endl;
		return;
	}
}

pqxx::result Postgres::GetQuery(string query)
{
	pqxx::result rs;
	pthread_mutex_lock(&cs);
	try
	{
		pqxx::nontransaction N(*con);
		rs = N.exec(query);
	} catch (const std::exception &e)
	{
		cerr << e.what() << std::endl;
	}
	pthread_mutex_unlock(&cs);
	return rs;
}

pqxx::result Postgres::AffectQuery(string query)
{
	pqxx::result rs;
	pthread_mutex_lock(&cs);
	try
	{
		pqxx::work W(*con);
		rs = W.exec(query);
		W.commit();
	} catch (const std::exception &e)
	{
		cerr << e.what() << std::endl;
	}
	pthread_mutex_unlock(&cs);
	return rs;
}





