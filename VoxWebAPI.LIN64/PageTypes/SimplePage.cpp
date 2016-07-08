//
// Created by root on 17.06.16.
//
#include "SimplePage.h"

SimplePage::SimplePage(string url, SimplTempl templ) : Page(url, false)
{
	func = templ;
}

bool SimplePage::OnRequest(HttpRequest *request)
{
	request->SendResult(func(request));
	return false;
}