//
// Created by root on 14.06.16.
//
#include <dirent.h>
#include <stdio.h>
#include <cstring>
#include <dlfcn.h>
#include <boost/algorithm/string/predicate.hpp>
#include "SiteLoader.h"
#include "../Processors/PacketProcessor.h"

void SiteLoader::LoadAll(string path)
{
	printf("Starting load sites!\n");
	DIR *d;
	struct dirent *dir;
	d = opendir(path.c_str());
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
				continue;

			if (boost::ends_with(dir->d_name, ".so"))
			{
				LoadSite(path + "/" + string(dir->d_name));
			} else if (dir->d_type == DT_DIR)
			{
				printf((path + "/" + string(dir->d_name) + "\n").c_str());
				LoadAll(path + "/" + string(dir->d_name));
			}
		}

		closedir(d);
	}
}

void SiteLoader::LoadSite(string dll_name)
{
	void *handle = dlopen(dll_name.c_str(), RTLD_LAZY);

	if (!handle)
	{
		printf("Loading %s ; ERR: %s\n", dll_name.c_str(), dlerror());
		return;
	} else
		printf("%s Connected!\n", dll_name.c_str());

	SiteLoadDEF fnc = (SiteLoadDEF) dlsym(handle, "OnLoad");

	if (fnc)
	{
		vector<Site *> sts = fnc(PacketProcessor::Web);
		for (Site *st : sts)
		{
			if (st != 0)
			{
				printf("Site %s loaded!\n", st->Name.c_str());
				PacketProcessor::Web->AddSite(st);
			} else
			{
				printf("Site %s not want to be loaded!\n", dll_name.c_str());
			}
		}
	} else
	{
		printf("OnLoad method in %p %s not found!\n", handle, (dll_name).c_str());
	}
}