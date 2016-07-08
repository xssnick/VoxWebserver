//
// Created by root on 14.06.16.
//

#ifndef VOXWEBSERVER_LIN64_WEBSERVER_H
#define VOXWEBSERVER_LIN64_WEBSERVER_H

#include "VoxWebAPI.h"

enum ServerMode
{
	WEBSERVER_MODE = 1, CORESERVER_MODE = 2, DATASERVER_MODE = 4
};

class Site;

class Webserver
{
public:
	Webserver(ServerMode Mode);

	vector<Site *> GetSitesList();

	void AddSite(Site *site);

	ServerMode GetMode();

	int MainEPoll;

private:
	ServerMode Mode;
	vector<Site *> sites;
};

#endif //VOXWEBSERVER_LIN64_WEBSERVER_H
