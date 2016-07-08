//
// Created by kek on 02.07.16.
//

#ifndef VOXWEBSERVER_LIN64_PROCESSPAGE_H
#define VOXWEBSERVER_LIN64_PROCESSPAGE_H

#include "VoxWebAPI.h"

class ProcessPage : public Page
{
public:
	typedef bool (*OnRequestPrc)(HttpRequest *);

	ProcessPage(string url, OnRequestPrc func);

	bool OnRequest(HttpRequest *request);

private:
	OnRequestPrc func_cll;
};


#endif //VOXWEBSERVER_LIN64_PROCESSPAGE_H
