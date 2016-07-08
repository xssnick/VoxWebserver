//
// Created by root on 16.06.16.
//

#ifndef VOXWEBSERVER_LIN64_NETWORKHELPER_H
#define VOXWEBSERVER_LIN64_NETWORKHELPER_H


class NetworkHelper
{
public:
	static int sendall(int socket, const char *buf, unsigned long len, unsigned long &sent);
};


#endif //VOXWEBSERVER_LIN64_NETWORKHELPER_H
