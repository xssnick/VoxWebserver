//
// Created by root on 15.06.16.
//

#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
#include "FileManager.h"

const char *GetFileTypeByExtAlt(const char *name)
{
	int ln = (int) strlen(name);
	char f = 0;
	char s = 0;
	for (int i = ln - 1; i > 0; i--)
	{
		if (name[i] == '.')
		{
			if (i + 1 < ln - 1)
			{
				f = name[i + 1];
			}

			if (i + 2 < ln - 1)
			{
				s = name[i + 2];
			}
			break;
		}
	}

	switch (f)
	{
		case 'a':
			return "video/x-msvideo";
		case 'c':
			return "text/css";
		case 'd':
			return "application/msword";
		case 'g':
			return "image/gif";
		case 'h':
			return "text/html";
		case 'j':
		{
			if (s)
			{
				if (s == 'p')
					return "image/jpeg";
				if (s == 's')
					return "application/x-javascript";
			}
			return "application/octet-stream";
		}
			//  case "js": return "application/x-javascript";
		case 'm':
			return "audio/mpeg";
		case 'p':
			return "image/png";
		case 'i':
			return "image/x-icon";
			//case 'pd': return "application/pdf";
		case 'z':
			return "application/zip";
		case 't':
			return "text/plain";
		default:
			return "application/octet-stream";
	}
}

bool FileIsExist(std::string filePath)
{
	bool isExist = false;
	std::ifstream fin(filePath.c_str());

	if (fin.is_open())
		isExist = true;

	fin.close();
	return isExist;
}

bool FileManager::ProcessFile(Site *site, HttpRequest *request)
{
	bool okay = false;
	string fname;

	if (request->URL.find("..") != std::string::npos || request->URL[0] == '.')
	{
		request->SendBadRequest();
		return false;
	}

	fname = string("./content/" + site->Name + "/" + request->URL);
	//FILE* fl = fopen(fname.c_str(),"rb");

	int fd = open(fname.c_str(), O_RDONLY | O_NONBLOCK);

	if (fd != -1)
	{
		struct stat stat_buf;
		fstat(fd, &stat_buf);

		long sz = stat_buf.st_size;
		if (sz > 0)
		{
			char *Buf = new char[sz];
			if (read(fd, Buf, sz) == sz)
			{
				string directory;
				const size_t last_slash_idx = fname.rfind('/');
				if (std::string::npos != last_slash_idx)
				{
					directory = fname.substr(0, last_slash_idx);
				}

				//\nCache-Control: public, max-age=60000
				string hed = GetFileTypeByExtAlt(request->URL.c_str());
				string cac = string("cached_");
				if ((request->URL.size() > cac.size() && request->URL.substr(0, cac.size()) == cac) ||
					FileIsExist(directory + "/cached_dir.txt"))
				{
					hed += "\nCache-Control: public, max-age=604800";
				}
				request->SendData(Buf, (unsigned long) sz, true, "200 OK", hed);
				okay = true;
			} else
			{
				delete[] Buf;
				printf("ERR read file (mgr)%d\n", errno);
			}
		} else
		{
			printf("ERR size file (mgr)%d\n", errno);
		}
		close(fd);
	}

	return okay;
}

/*
bool FileManager::ProcessFile(Site * site, HttpRequest * request)
{
	if (request->URL.find("..") != std::string::npos || request->URL[0] == '.')
	{
		request->SendBadRequest();
		return false;
	}

	string fname = string("./content/" + site->Name + "/" + request->URL);
	if(FileIsExist(fname))
	{
		request->SendFile(fname.c_str(),"200 OK",GetFileTypeByExtAlt(request->URL.c_str()));
		return true;
	}

	/*FILE* fl = fopen(fname.c_str(),"rb");

	if (fl)
	{
		fseek(fl, 0L, SEEK_END);
		long sz = ftell(fl);
		if(sz > 0)
		{
			rewind(fl);
			char *Buf = new char[sz];
			if (fread(Buf, 1, sz, fl) == sz)
			{
				string directory;
				const size_t last_slash_idx = fname.rfind('/');
				if (std::string::npos != last_slash_idx)
				{
					directory = fname.substr(0, last_slash_idx);
				}

				string hed = GetFileTypeByExtAlt(request->URL.c_str());
				string cac = string("cached_");
				if ((request->URL.size() > cac.size() && request->URL.substr(0, cac.size()) == cac) ||
					FileIsExist(directory + "/cached_dir.txt"))
				{
					hed += "\nCache-Control: public, max-age=604800";
				}
				request->SendFile(Buf, (unsigned long)sz, "200 OK", hed);
				okay = true;
			}
			else
			{
				printf("ERR read file (mgr)%d\n", errno);
			}
			delete[] Buf;
		}
		else
		{
			printf("ERR size file (mgr)%d\n", errno);
		}
		fclose(fl);
	}

return false;
}*/