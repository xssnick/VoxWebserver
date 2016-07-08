//
// Created by kek on 02.07.16.
//

#include "ProcessPage.h"

ProcessPage::ProcessPage(string url, OnRequestPrc templ) : Page(url, false)
{
	func_cll = templ;
}

bool ProcessPage::OnRequest(HttpRequest *request)
{
	return func_cll(request);
}