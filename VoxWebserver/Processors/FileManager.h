//
// Created by root on 15.06.16.
//

#ifndef VOXWEBSERVER_LIN64_FILEMANAGER_H
#define VOXWEBSERVER_LIN64_FILEMANAGER_H


#include "RequestTypes/HttpRequest.h"

class FileManager
{
public:
	static bool ProcessFile(Site *site, HttpRequest *request);
};


#endif //VOXWEBSERVER_LIN64_FILEMANAGER_H
