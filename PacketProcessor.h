//
// Created by root on 14.06.16.
//

#ifndef VOXWEBSERVER_LIN64_PACKETPROCESSOR_H
#define VOXWEBSERVER_LIN64_PACKETPROCESSOR_H

#include "settings.h"

class PacketProcessor
{
public:
	static void OnConnect(Connection *client);

	static void OnDisonnect(Connection *client);

	static Site *FindSite(string domain);

	static Webserver *Web;
private:
	static bool ParseHeader(HttpRequest *req);
};


#endif //VOXWEBSERVER_LIN64_PACKETPROCESSOR_H
