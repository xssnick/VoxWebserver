//
// Created by root on 14.06.16.
//

#ifndef VOXWEBSERVER_LIN64_HTTPREQUEST_H
#define VOXWEBSERVER_LIN64_HTTPREQUEST_H

#include "../VoxWebAPI.h"

enum RequestType
{
	GET, POST
};

enum FileType
{
	FILETYPE_JPEG, FILETYPE_PNG, FILETYPE_OTHER
};

struct UserFile
{
	unsigned long long Size;
	string Name;
	char *Data;
};

class HttpRequest
{
public:
	HttpRequest(Connection *packet);

	void SendResult(string data, string status = "200 OK", string type = "text/html; charset=UTF-8");

	static void StaticSendResult(int sock, string data, string status = "200 OK",
								 string type = "text/html; charset=UTF-8");

	//void SendFile(const char* path, string status = "200 OK", string type = "application/octet-stream");
	void SendData(char *data, unsigned long size, bool del_data_after_send, string status = "200 OK",
				  string type = "application/octet-stream");

	void Redirect(string url);

	void SendBadRequest();

	void SendNotFound();

	string Cookie(string name);

	string PostValue(string name);

	string GetValue(string name);

	UserFile File(string name);

	void push_cookie(char *name, char *val);

	void push_post(char *name, char *val);

	void push_get(char *name, char *val);

	void push_file(char *name, UserFile file);

	Connection *GetConnection();

	void Close();

	void SetCookie(string name, string value);

	~HttpRequest();

	enum RequestType RequestType = GET;
	string Host;
	string URL;
	string Referer;
	string UserAgent;
	string ContentType;
	string IP;
	Page *CurrentPage;
	char *PData = 0;
private:
	struct PrmItm
	{
		char *Name;
		char *Value;
	};

	struct PrmItmS
	{
		string Name;
		string Value;
	};

	struct PrmItmF
	{
		char *Name;
		UserFile Value;
	};

	Connection *Connect;
	vector<PrmItmF> FilePrm;
	vector<PrmItm> GetPrm;
	vector<PrmItm> PostPrm;
	vector<PrmItm> CookiePrm;
	vector<PrmItmS> SetCookiePrm;
};


#endif //VOXWEBSERVER_LIN64_HTTPREQUEST_H
