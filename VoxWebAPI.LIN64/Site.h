//
// Created by root on 14.06.16.
//

#ifndef VOXWEBSERVER_LIN64_SITE_H
#define VOXWEBSERVER_LIN64_SITE_H

#include "VoxWebAPI.h"
#include <stdarg.h>

class Page;

struct SiteSettings
{
	Page *PageNotFound = 0;
	Page *PageBadRequest = 0;
	enum Protocol Protocols = HTTP;
};

struct DomainList
{
	vector<char *> List;

	DomainList(int num, ...)
	{
		va_list vl;
		va_start(vl, num);
		for (int i = 0; i < num; i++)
		{
			List.push_back(va_arg(vl, char*));
		}
		va_end(vl);
	}
};

class Connection;

class HttpRequest;

class Webserver;

class Site
{
	//friend HttpRequest;
public:
	Site(string name, DomainList domains);

	virtual vector<Page *> GetPages();

	virtual vector<string> GetDomains();

	virtual bool IsAcceptProtocol(enum Protocol proto);

	virtual void AddPage(Page *page);

	virtual bool PreInit(HttpRequest *request, Page *page);

	virtual bool AfterInit(HttpRequest *request, Page *page);

	virtual Page *FindPage(string url);
	//virtual Logger* GetLogger();

	virtual void OnDisconnect(const Connection *Connect) { }

	string Name;
protected:
	SiteSettings Settings;
	vector<Page *> Pages;
	vector<string> Domains;
public:
	Page *Get404();

	Page *Get400();
};

typedef vector<Site *>(*SiteLoadDEF)(Webserver *webserver);

#endif //VOXWEBSERVER_LIN64_SITE_H
