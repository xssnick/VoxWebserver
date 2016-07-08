//
// Created by root on 14.06.16.
//

#ifndef VOXWEBSERVER_LIN64_SETTINGS_H
#define VOXWEBSERVER_LIN64_SETTINGS_H

#include <string>
#include <vector>
#include "PageTypes/Site.h"
#include "VoxWebAPI.LIN64/PageTypes/Page.h"
#include "VoxWebAPI.LIN64/RequestTypes/Connection.h"
#include "VoxWebAPI.LIN64/RequestTypes/WebSocket.h"
#include "VoxWebAPI.LIN64/RequestTypes/HttpRequest.h"
#include "VoxWebAPI.LIN64/Network/Webserver.h"

#define WORK_THREADS_NUM 8
//#define SERVER_PORT 80
#define SERVER_HOST "0.0.0.0"
#define EPOLL_RUN_TIMEOUT 300
#define EPOLL_SIZE 16000

using namespace std;
#endif //VOXWEBSERVER_LIN64_SETTINGS_H
