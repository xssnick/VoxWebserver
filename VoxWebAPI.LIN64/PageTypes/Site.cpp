//
// Created by root on 14.06.16.
//
#include "../Network/Webserver.h"


Site::Site(string name, DomainList domains)
{
	//Settings.Log = new Logger(name.c_str());
	Name = name;
	for (char *vl : domains.List)
	{
		Domains.push_back(string(vl));
	}
}

vector<Page *> Site::GetPages()
{
	return Pages;
}

vector<string> Site::GetDomains()
{
	return Domains;
}

bool Site::IsAcceptProtocol(Protocol proto)
{
	return Settings.Protocols & proto;
}

bool Site::PreInit(HttpRequest *request, Page *page)
{
	return true;
}

bool Site::AfterInit(HttpRequest *request, Page *page)
{
	return true;
}

void Site::AddPage(Page *page)
{
	Pages.push_back(page);
}

Page *Site::FindPage(string url)
{
	return nullptr;
}

Page *Site::Get404()
{
	return Settings.PageNotFound;
}

Page *Site::Get400()
{
	return Settings.PageBadRequest;
}