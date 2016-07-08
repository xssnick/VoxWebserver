//
// Created by root on 13.06.16.
//

#ifndef VOXWEBSERVER_LIN64_CONNECTMANAGER_H
#define VOXWEBSERVER_LIN64_CONNECTMANAGER_H

#include "settings.h"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <list>
#include <time.h>

#define WORK_THREADS_NUM 8
//#define SERVER_PORT 80
#define SERVER_HOST "0.0.0.0"
#define EPOLL_RUN_TIMEOUT 300
#define EPOLL_SIZE 16000


class ConnectManager
{
public:
	static void StartServer(int port);

	static bool Started;
};


#endif //VOXWEBSERVER_LIN64_CONNECTMANAGER_H
