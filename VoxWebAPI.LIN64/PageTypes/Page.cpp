//
// Created by root on 14.06.16.
//
#include "../RequestTypes/HttpRequest.h"

Page::Page(string url, bool dynamic)
{
	URL = url;
	DynamicURL = dynamic;
}

bool Page::IsDynamicURL()
{
	return DynamicURL;
}

bool Page::OnRequest(HttpRequest *request)
{
	request->SendResult("Page: " + URL);
	return false;
}

bool Page::OnWebSocket(WebSocket *request)
{
	return false;
}

string Page::GetURL()
{
	return URL;
}

Site *Page::GetSite()
{
	return lsite;
}

bool Page::CheckURL(string url)
{
	if (DynamicURL)
	{
		if (URL.size() > url.size())
			return false;

		return url.substr(0, URL.size()) == URL;
	} else
	{
		return URL == url;
	}
}

void Page::SetSite(Site *site)
{
	lsite = site;
}

string Page::GetTitle()
{
	return Title;
}