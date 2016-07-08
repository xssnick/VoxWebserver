//
// Created by root on 17.06.16.
//

#ifndef VOXWEBSERVER_LIN64_SIMPLEPAGE_H
#define VOXWEBSERVER_LIN64_SIMPLEPAGE_H

#include "VoxWebAPI.h"

class SimplePage : public Page
{
public:
	typedef string (*SimplTempl)(HttpRequest *);

	SimplePage(string url, SimplTempl templ);

	bool OnRequest(HttpRequest *request);

private:
	SimplTempl func;
};


#endif //VOXWEBSERVER_LIN64_SIMPLEPAGE_H
