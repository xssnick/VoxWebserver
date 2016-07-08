//
// Created by root on 14.06.16.
//

#ifndef VOXWEBSERVER_LIN64_WEBSOCKET_H
#define VOXWEBSERVER_LIN64_WEBSOCKET_H

#include "VoxWebAPI.h"

enum WSPacketType
{
	WS_BAD_PACKET, WS_DATA_PACKET, WS_NOTFULL_PACKET, WS_SYS_PACKET
};

class WebSocket
{
public:
	WebSocket(Connection *connect);

	~WebSocket();

	Connection *GetConnection();

	char *GetData();

	string GetTextData();

	void Send(const char *Data, unsigned long long sz);

	void Send(string Data);

	static void SendStatic(Connection *connect, const char *Data, unsigned long long sz);

	static void SendStatic(Connection *connect, string Data);

	WSPacketType Decode(unsigned char *data, int sz);

private:
	Connection *Connect;
	char *Data = 0;
	unsigned long long DataSz = 0;

protected:
};


#endif //VOXWEBSERVER_LIN64_WEBSOCKET_H
