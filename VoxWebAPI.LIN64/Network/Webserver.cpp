//
// Created by root on 14.06.16.
//
#include "../PageTypes/Site.h"

Webserver::Webserver(ServerMode mode)
{
	Mode = mode;
}

vector<Site *> Webserver::GetSitesList()
{
	return sites;
}

void Webserver::AddSite(Site *site)
{
	if (site != 0)
		sites.push_back(site);
}

ServerMode Webserver::GetMode()
{
	return Mode;
}

