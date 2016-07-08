//
// Created by root on 14.06.16.
//

#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <sys/epoll.h>

#include "../PageTypes/Site.h"
#include "../Network/NetworkHelper.h"
#include "../../VoxWebserver/Processors/PacketProcessor.h"

Connection::Connection(Webserver *server)
{
	Server = server;
}

void Connection::WaitForPacket()
{
	if (SendQueue.size() == 0)
		Processing = false;
}

void Connection::WaitForRequest()
{
	GotLen = 0;
	ContentLen = 0;
	HeadEnd = 0;
	//State = 0;
	DestroyData();
	RevcData = voxchain<Data_Part>(1);
	WaitForPacket();
}

void Connection::Drop()
{
	Connected = false;
	if (Socket)
	{
		close(Socket);
		Socket = 0;
	}
}

//TODO: if 2 or more requests continiously, we have drop all them data when first request processed!
void Connection::DestroyData()
{
	voxchain<Data_Part>::chain_item *citem = RevcData.GetFirst();
	while (citem)
	{
		voxchain<Data_Part>::chain_item *cur = citem;
		citem = citem->Next;
		cur->Data->Destroy();
	}
	RevcData.Destroy();
}

//TODO: sends only 2 first lines, why?
void Connection::SwitchToWebSocket(const unsigned char *hash)
{
	if (!Connected)
		return;

	string key = voxcrypt::Base64Encode(hash, 20);
	string snd = "HTTP/1.1 101 Switching Protocols"
						 "\r\nServer: " SERVER_NAME
						 "\r\nUpgrade: websocket"
						 "\r\nConnection: Upgrade"
						 "\r\nSec-WebSocket-Accept: " + key;
	snd += "\r\n\r\n";
	send(Socket, snd.c_str(), snd.length(), 0);
}

char *Connection::BuildData(int &size)
{
	this->GotData = false;
	int sz = 0;
	int num = 0;
	voxchain<Data_Part>::chain_item *citem = RevcData.GetFirst();
	while (citem)
	{
		sz += citem->Data->size;
		citem = citem->Next;
		num++;
	}

	char *dat = new char[sz + 1];
	dat[sz] = 0;
	int off = 0;

	voxchain<Data_Part>::chain_item *sitem = RevcData.GetFirst();
	while (sitem)
	{
		memcpy(&dat[off], sitem->Data->Data, sitem->Data->size);
		off += sitem->Data->size;
		sitem = sitem->Next;
	}

	//printf(dat);
	size = sz + 1;
	return dat;
}


Connection::~Connection()
{
	if (Socket)
	{
		close(Socket);
	}

	while (SendQueue.size() > 0)
	{
		Data_Part_Snd *dps = SendQueue.front();
		SendQueue.pop();

		if (dps->iskill)
			dps->Destroy();

		delete dps;
	}

	DestroyData();
}
/*
void Connection::PushRecvData(char* data, unsigned int sz)
{
	this->GotData = true;
	Data_Part part;
	part.size = sz;
	part.Data = data;
	RevcData.Add(part);
}*/

//allowed only in server, not in site!
void Connection::Disconnect()
{
	PacketProcessor::OnDisonnect(this);
	if (!this->Processing)
	{
		delete this;
	} else
	{
		this->Connected = false;
		if (Socket)
		{
			close(Socket);
			Socket = 0;
		}
	}
}


void Connection::SendPacket(const char *data, unsigned long size)
{
	if (!Connected)
		return;

	unsigned long sent = 0;
	int res = 0;
	if (SendQueue.size() == 0)
		res = NetworkHelper::sendall(Socket, data, size, sent);
	else
		res = 1;

	if (res == 1)
	{
		Data_Part_Snd *dp = new Data_Part_Snd;
		dp->size = size;
		dp->offset = sent;
		dp->data = new char[size];
		memcpy(dp->data, data, size);
		dp->iskill = true;
		SendQueue.push(dp);
		ChangePollState(EPOLLOUT);
		return;
	} else if (res == -1)
	{
		Drop();
	}
}


void Connection::SendPacketWithDataControl(char *data, unsigned long size, bool delete_data)
{
	if (!Connected)
		return;

	unsigned long sent = 0;
	int res = 0;
	if (SendQueue.size() == 0)
		res = NetworkHelper::sendall(Socket, data, size, sent);
	else
		res = 1;

	if (res == 1)
	{
		Data_Part_Snd *dp = new Data_Part_Snd;
		dp->size = size;
		dp->offset = sent;
		dp->data = data;
		dp->iskill = delete_data;
		SendQueue.push(dp);
		ChangePollState(EPOLLOUT);
		return;
	} else if (res == -1)
	{
		Drop();
	}

	if (delete_data)
	{
		delete[] data;
	}
}

void Connection::ChangePollState(unsigned int epoll_state)
{
	epoll_data.events = epoll_state | EPOLLRDHUP;
	epoll_ctl(Server->MainEPoll, EPOLL_CTL_MOD, Socket, &epoll_data);
}







