//
// Created by root on 14.06.16.
//

#ifndef VOXWEBSERVER_LIN64_PAGE_H
#define VOXWEBSERVER_LIN64_PAGE_H

#include "../VoxWebAPI.h"


class HttpRequest;

class Page
{
public:
	Page(string url, bool dynamic = false);

	virtual bool IsDynamicURL();

	virtual bool OnRequest(HttpRequest *request);

	virtual bool OnWebSocket(WebSocket *request);

	string GetURL();

	virtual Site *GetSite();

	virtual bool CheckURL(string url);

	virtual void SetSite(Site *site);

	virtual string GetTitle();

protected:
	string URL;
	bool DynamicURL = false;
	string Title;
private:
	Site *lsite;
};


#endif //VOXWEBSERVER_LIN64_PAGE_H
