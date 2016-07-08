//
// Created by root on 14.06.16.
//

#ifndef VOXWEBSERVER_LIN64_SITELOADER_H
#define VOXWEBSERVER_LIN64_SITELOADER_H

#include "../../settings.h"

class SiteLoader
{
public:
	static void LoadAll(string path = "./sites");

	static void LoadSite(string dll_name);
};


#endif //VOXWEBSERVER_LIN64_SITELOADER_H
