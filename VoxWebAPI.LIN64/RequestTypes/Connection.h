//
// Created by root on 14.06.16.
//

#ifndef VOXWEBSERVER_LIN64_CONNECTION_H
#define VOXWEBSERVER_LIN64_CONNECTION_H

#include "VoxWebAPI.h"
#include <sys/epoll.h>

struct Data_Part
{
	unsigned long size;
	char *Data = 0;

	void Destroy()
	{
		if (Data)
			delete[] Data;
	}
};


struct Data_Part_Snd
{
	unsigned long size;
	unsigned long offset = 0;
	char *data = 0;
	bool iskill;

	void Destroy()
	{
		if (data)
			delete[] data;
	}
};

class Connection
{
public:
	Connection(Webserver *server);

	~Connection();

	void WaitForPacket();

	void WaitForRequest();

	//void PushRecvData(char* data, unsigned int sz);
	void ChangePollState(unsigned int epoll_state);

	void Drop();

	void Disconnect();

	void DestroyData();

	void SendPacketWithDataControl(char *data, unsigned long size, bool delete_data);

	void SendPacket(const char *data, unsigned long size);

	void SwitchToWebSocket(const unsigned char *hash);

	char *BuildData(int &size);

	bool Connected = false;
	bool Processing = false;
	voxchain<Data_Part> RevcData = voxchain<Data_Part>(1);
	queue<Data_Part_Snd *> SendQueue = queue<Data_Part_Snd *>();
	//char State = 0;
	unsigned int HeadEnd = 0;
	unsigned int ContentLen = 0;
	int GotLen = 0;
	enum Protocol Protocol = HTTP;
	void *Additional = 0;
	Site *ConnectedTo = 0;
	Webserver *Server;
	bool GotData = false;

	int Socket;
	struct epoll_event epoll_data;
};


#endif //VOXWEBSERVER_LIN64_CONNECTION_H
