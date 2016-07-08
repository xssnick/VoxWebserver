//
// Created by root on 14.06.16.
//

#include "Connection.h"


WebSocket::WebSocket(Connection *connect)
{
	Connect = connect;
}

WebSocket::~WebSocket()
{
	if (Data)
		delete[] Data;
}

Connection *WebSocket::GetConnection()
{
	return Connect;
}

char *WebSocket::GetData()
{
	return Data;
}

string WebSocket::GetTextData()
{
	if (Data)
		return string(Data);
	return string();
}

void WebSocket::Send(const char *Data, unsigned long long sz)
{
	SendStatic(Connect, Data, sz);
}

void WebSocket::Send(string Data)
{
	SendStatic(Connect, Data.c_str(), Data.size());
}

void WebSocket::SendStatic(Connection *connect, const char *Data, unsigned long long sz)
{
	if (!connect || connect->Protocol != WEBSOCKET)
		return;

	int base_sz = 2;

	if (sz > 125 && sz <= UINT16_MAX)
		base_sz += 2;
	else if (sz > UINT16_MAX)
		base_sz += 8;

	char *snd = new char[base_sz + sz];
	snd[0] = (char) 0x81;

	if (base_sz == 2)
		snd[1] = (char) sz;
	else if (base_sz == 4)
	{
		snd[1] = 126;
		memcpy(&snd[2], &sz, 2);
		reverse(&snd[2], &snd[4]);
	} else if (base_sz == 10)
	{
		snd[1] = 127;
		memcpy(&snd[2], &sz, 8);
		reverse(&snd[2], &snd[10]);
	}

	memcpy(snd + base_sz, Data, sz);
	send(connect->Socket, snd, base_sz + sz, 0);
	delete[] snd;
}

void WebSocket::SendStatic(Connection *connect, string Data)
{
	SendStatic(connect, Data.c_str(), Data.size());
}

WSPacketType WebSocket::Decode(unsigned char *data, int sz)
{
	if (sz < 7)
		return WS_BAD_PACKET;

	bool isfull = data[0] & 128;
	if (!isfull)
	{
#ifdef _DEBUG
		printf("Got not full WS packet!\n");
#endif
		return WS_BAD_PACKET;
	}

	char opcode = (char) (data[0] & ~240);
	int sz_v = data[1] - 128;

	if (sz_v < 0)
		return WS_BAD_PACKET;

	char key[4];
	unsigned long long packet_size = 0;

	int key_offset = 2;
	if (sz_v <= 125)
	{
		packet_size = sz_v;
	} else if (sz_v == 126)
	{
		packet_size = *(unsigned short *) &data[2];
		key_offset = 4;
	} else if (sz_v == 127)
	{
		packet_size = *(unsigned long long *) &data[2];
		key_offset = 10;
	}

	if (!packet_size)
	{
#ifdef _DEBUG
		printf("Got empty WS data packet!\n");
#endif
		return WS_BAD_PACKET;
	}

	if (packet_size > MAX_WS_PACKET)
		return WS_BAD_PACKET;

	Connect->ContentLen = (unsigned int) min((unsigned long) packet_size, (unsigned long) UINT32_MAX);
	Connect->GotLen = sz - key_offset - 4;
	if (Connect->ContentLen == Connect->GotLen)
	{
		memcpy(key, data + key_offset, 4);
		key_offset += 4;

		DataSz = packet_size;
		Data = new char[packet_size + 1];
		Data[packet_size] = 0;

		for (int i = 0; i < packet_size; i++)
		{
			Data[i] = data[key_offset + i] ^ key[i % 4];
		}
		return WS_DATA_PACKET;
	}
	return WS_NOTFULL_PACKET;
}