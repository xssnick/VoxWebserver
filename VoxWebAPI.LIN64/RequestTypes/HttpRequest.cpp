//
// Created by root on 14.06.16.
//
#include "VoxWebAPI.h"

HttpRequest::HttpRequest(Connection *packet)
{
	Connect = packet;
}

void HttpRequest::SendResult(string data, string status, string type)
{
	if (Connect == 0 || !Connect->Connected)
	{
		return;
	}

	string snd = "HTTP/1.1 " + status + "\r\nServer: " SERVER_NAME
			"\r\nContent-Length: " + to_string(data.length()) + "\r\nContent-Type: " + type;
	for (PrmItmS it : SetCookiePrm)
	{
		snd += "\r\nSet-Cookie: " + it.Name + "=" + it.Value + "; domain=." + Host + "; Path=/";
	}
	snd += "\r\nConnection: keep-alive\r\n\r\n";
	snd += data;
	Connect->SendPacket(snd.c_str(), snd.length());
}

//TODO: dangerous use! NEED TO FIX THIS
void HttpRequest::StaticSendResult(int sock, string data, string status, string type)
{

	string snd = "HTTP/1.1 " + status;
	snd += "\r\nServer: " SERVER_NAME;
	snd += "\r\nContent-Length: " + to_string(data.length());
	snd += "\r\nContent-Type: " + type;
	snd += "\r\nConnection: close";
	snd += "\r\n\r\n";
	snd += data;
	send(sock, snd.c_str(), snd.length(), 0);
}

/*
void HttpRequest::SendFile(const char* path, string status, string type)
{
	if (Connect == 0 || !Connect->Connected)
	{
		return;
	}
	int fd = open(path, O_RDONLY);

	struct stat stat_buf;
	fstat(fd, &stat_buf);

	string snd = "HTTP/1.1 " + status;
	snd += "\r\nServer: VoxWeb"
				   "\r\nContent-Length: " + to_string(stat_buf.st_size)
		   + "\r\nContent-Type: " + type;
	for (PrmItmS it : SetCookiePrm)
	{
		snd += "\r\nSet-Cookie: " + it.Name + "=" + it.Value + "; domain=." + Host + "; Path=/";
	}
	snd += "\r\nConnection: keep-alive\r\n\r\n";

	Connect->SendPacket(snd.c_str(), snd.size());
	sendfile(Connect->Socket, fd, 0, stat_buf.st_size);

	close(fd);
}*/

void HttpRequest::SendData(char *data, unsigned long size, bool del_data_after_send, string status, string type)
{
	if (Connect == 0 || !Connect->Connected)
	{
		return;
	}

	string snd = "HTTP/1.1 " + status;
	snd += "\r\nServer: " SERVER_NAME
				   "\r\nContent-Length: " + to_string(size) + "\r\nContent-Type: " + type;
	for (PrmItmS it : SetCookiePrm)
	{
		snd += "\r\nSet-Cookie: " + it.Name + "=" + it.Value + "; domain=." + Host + "; Path=/";
	}
	snd += "\r\nConnection: keep-alive\r\n\r\n";
	Connect->SendPacket(snd.c_str(), snd.size());
	Connect->SendPacketWithDataControl(data, size, del_data_after_send);
}

void HttpRequest::Redirect(string url)
{
	if (Connect == 0 || !Connect->Connected)
	{
		return;
	}

	string cont = "<meta http-equiv=\"refresh\" content=\"0; url=\"" + url + "\">";

	string snd = "HTTP/1.1 301 Moved Permanently"
						 "\nServer: " SERVER_NAME
						 "\nContent-Length: " + to_string(cont.length());
	snd += "\nContent-Type: text/html; charset=UTF-8";
	"\nCache-Control: no-store, no-cache, must-revalidate, post-check=0, pre-check=0"
			"\nLocation: " + url;
	for (PrmItmS it : SetCookiePrm)
	{
		snd += "\nSet-Cookie: " + it.Name + "=" + it.Value + "; domain=." + Host + "; Path=/";
	}
	snd += "\nConnection: keep-alive";
	snd += "\n\n";
	snd += cont;
	Connect->SendPacket(snd.c_str(), snd.length());
}

void HttpRequest::SendBadRequest()
{
	if (!Connect->ConnectedTo || !Connect->ConnectedTo->Get400())
		SendResult("400 Bad Request!", "400 Bad Request");
	else
		Connect->ConnectedTo->Get400()->OnRequest(this);

	//Connect->Processing = false;
	Connect->Drop();
}

void HttpRequest::SendNotFound()
{
	if (!Connect->ConnectedTo || !Connect->ConnectedTo->Get404())
		SendResult("404 Not Found!", "404 Not Found");
	else
		Connect->ConnectedTo->Get404()->OnRequest(this);
}

string HttpRequest::Cookie(string name)
{
	for (PrmItm it : CookiePrm)
	{
		if (strcmp(name.c_str(), it.Name) == 0)
		{
			return voxstring::decode_web(it.Value);
		}
	}
	return string();
}

string HttpRequest::PostValue(string name)
{
	for (PrmItm it : PostPrm)
	{
		if (strcmp(it.Name, name.c_str()) == 0)
		{
			return voxstring::decode_web(it.Value);
		}
	}
	return string();
}

string HttpRequest::GetValue(string name)
{
	for (PrmItm it : GetPrm)
	{
		if (strcmp(it.Name, name.c_str()) == 0)
		{
			return voxstring::decode_web(it.Value);
		}
	}
	return string();
}

UserFile HttpRequest::File(string name)
{
	for (PrmItmF it : FilePrm)
	{
		if (strcmp(it.Name, name.c_str()) == 0)
		{
			return it.Value;
		}
	}
	return UserFile();
}

void HttpRequest::push_cookie(char *name, char *val)
{
	PrmItm it;
	it.Name = name;
	it.Value = val;
	CookiePrm.push_back(it);
}

void HttpRequest::push_post(char *name, char *val)
{
	PrmItm it;
	it.Name = name;
	it.Value = val;
	PostPrm.push_back(it);
}

void HttpRequest::push_get(char *name, char *val)
{
	PrmItm it;
	it.Name = name;
	it.Value = val;
	GetPrm.push_back(it);
}

void HttpRequest::push_file(char *name, UserFile file)
{
	PrmItmF it;
	it.Name = name;
	it.Value = file;
	FilePrm.push_back(it);
}

Connection *HttpRequest::GetConnection()
{
	return Connect;
}

void HttpRequest::Close()
{
	if (PData)
		delete[] PData;
	delete this;
}

void HttpRequest::SetCookie(string name, string value)
{
	PrmItmS it;
	it.Name = name;
	it.Value = value;
	SetCookiePrm.push_back(it);
}

HttpRequest::~HttpRequest()
{
}