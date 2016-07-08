//
// Created by root on 14.06.16.
//

#ifndef VOXWEBSERVER_LIN64_VOXWEBAPI_H_H
#define VOXWEBSERVER_LIN64_VOXWEBAPI_H_H

//#define WS_NO_QUEUE
#define SEND_PACKET_SZ 8192UL
#define PACKET_SIZE 4096
#define MAX_CONTENT_LEN 1*1024*1024 // 12 MB
#define MAX_WS_PACKET 8192
#define MAX_PARTS 1600
#define PRE_ALLOC_PARTS 4
#define SERVER_NAME "VoxWeb-LX"
#define PARAM_MAP map<string, void*>
#define NO_PARAM_MAP map<string, void*>()
#define SITE_EXPORT extern "C"  __attribute__((visibility("default")))
#define SPLIT_SYM "\b\v"

#include <stdio.h>
#include <cstring>
#include <sys/socket.h>
#include <stdlib.h>
#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <map>

using namespace std;

class Site;

class Page;

class Connection;

class HttpRequest;

class WebSocket;

class Webserver;

enum Protocol
{
	HTTP = 1, HTTP_TLS = 2, WEBSOCKET = 4, WEBSOCKET_TLS = 8
};

#include "voxchain.h"
#include "voxcrypt.h"
#include "voxstring.h"

#ifndef _VX_INSIDE_API__

#include "Postgres.h"
#include "Page.h"
#include "Connection.h"
#include "HttpRequest.h"
#include "WebSocket.h"
#include "Webserver.h"
#include "DataFileStorage.h"
#include "ProcessPage.h"
#include "SimplePage.h"
#include "Site.h"

#endif

#endif //VOXWEBSERVER_LIN64_VOXWEBAPI_H_H
