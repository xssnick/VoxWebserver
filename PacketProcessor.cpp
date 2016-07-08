//
// Created by root on 14.06.16.
//

#include <cstring>
#include "PacketProcessor.h"
#include "FileManager.h"
#include <openssl/sha.h>

Webserver *PacketProcessor::Web = 0;

void PacketProcessor::OnConnect(Connection *client)
{
	bool wait_req = true;
	if (client->Protocol == HTTP)
	{
		int fign = 0;
		char *raw = client->BuildData(fign);
		if (client->ContentLen == 0)
		{
			char *contlenpos = strstr(raw, "Content-Length: ");
			if (contlenpos)
			{
				unsigned long pos = contlenpos - raw;
				if (pos + 18 < fign)
				{
					if (contlenpos > 0 && isdigit(contlenpos[16]))
					{
						for (unsigned long i = pos + 16; i < fign; i++)
						{
							if (!isdigit(raw[i]))
							{
								raw[i] = 0;

								client->ContentLen = (unsigned int) max(0, atoi(&contlenpos[16]));
								if (client->ContentLen > MAX_CONTENT_LEN)
								{
									//HttpRequest::StaticSendResult(client->Socket, "Too big request!","403");

									delete[] raw;
									client->Processing = false;
									client->Disconnect();
									return;
								}
								raw[i] = ';';
								break;
							}
						}
					}
				}
			}
		}

		if (!client->HeadEnd)
		{
			const char *bound = "\r\n\r\n";
			int oks = 0;
			for (unsigned int i = 0; i < fign; i++)
			{
				if (bound[oks] == raw[i])
				{
					if (++oks == 4 && i < fign - 1)
					{
						client->HeadEnd = i + 1;
						break;
					}
				} else
					oks = 0;
			}
		}

		if (client->ContentLen && client->HeadEnd)
		{
			client->GotLen = fign - client->HeadEnd - 1;
			if (client->GotLen == client->ContentLen)
			{
				delete[] raw;
				goto okaysi;
			} else if (client->GotLen > client->ContentLen)
			{
				//HttpRequest::StaticSendResult(client->Socket, "More than excepted!", "403");
#ifdef _DEBUG
				printf("Bad client! More data than excepted (Content length %d, got %d)\n", client->ContentLen, client->GotLen);
#endif
				delete[] raw;
				client->Processing = false;
				client->Disconnect();
				return;
			}
		} else if (client->HeadEnd)
		{
			delete[] raw;
			goto okaysi;
		}
		delete[] raw;

		if (client->Connected)
		{
			client->WaitForPacket();
		} else
		{
			client->Processing = false;
			client->Disconnect();
		}

		return;
		okaysi:;

		HttpRequest *hclient = new HttpRequest(client);
		bool result = ParseHeader(hclient);
		if (result && hclient->Host.length() > 0 && hclient->URL.length() > 0 && hclient->URL.length() < 512)
		{

			Site *site = FindSite(hclient->Host);
			if (client->ConnectedTo && client->ConnectedTo != site)
			{
				hclient->SendResult("403 Cross domain connection is not supported!", "403 Access Denied");
				hclient->Close();
				client->Processing = false;
				client->Disconnect();
				return;
			} else
				client->ConnectedTo = site;

			if (site)
			{
				if (client->Protocol == HTTP)
				{
					for (Page *page : site->GetPages())
					{
						if (page->CheckURL(hclient->URL))
						{
							hclient->CurrentPage = page;
							if (site->PreInit(hclient, page)) if (page->OnRequest(hclient))
								site->AfterInit(hclient, page);

							goto ender;
						}
					}

					if (!FileManager::ProcessFile(site, hclient))
						hclient->SendResult("404!!! Not Found!", "404 Not Found");
				} else if (client->Protocol == WEBSOCKET && site->IsAcceptProtocol(WEBSOCKET) && client->Additional)
				{
					for (Page *page : site->GetPages())
					{
						if (page->CheckURL(hclient->URL))
						{
							char *dt = strcat((char *) client->Additional, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
							const unsigned char *ibuf = (const unsigned char *) dt;
							unsigned char obuf[20];

							SHA1(ibuf, strlen(dt), obuf);

							client->SwitchToWebSocket(obuf);
							client->Additional = page;
							goto ender;
						}
					}
					hclient->SendBadRequest();
				} else
				{
					hclient->SendBadRequest();
				}
			} else
			{
				hclient->SendNotFound();
			}
		} else
		{
			hclient->SendBadRequest();
		}

		ender:;
		hclient->Close();
	} else if (client->Protocol == WEBSOCKET)
	{
		WebSocket *web = new WebSocket(client);
		int sz = 0;
		unsigned char *res = (unsigned char *) client->BuildData(sz);
		WSPacketType ptype = web->Decode(res, sz);
		if (ptype == WS_DATA_PACKET)
		{

#ifdef WS_NO_QUEUE
			wait_req = false;
			if (client->Connected)
				client->WaitForRequest();
#endif

			((Page *) client->Additional)->OnWebSocket(web);
		} else if (ptype == WS_NOTFULL_PACKET)
		{
			wait_req = false;

			if (client->Connected)
				client->WaitForPacket();
		}
		delete web;
	} else
	{
		printf("Unknown request! WTF!? O.O\n");
	}


	if (wait_req)
	{
		if (client->Connected)
		{
			client->WaitForRequest();
			//if(client->GotData && client->Connected)

		} else
			delete client;
	} else if (!client->Connected)
	{
		delete client;
		return;
	}

	return;
}

bool PacketProcessor::ParseHeader(HttpRequest *req)
{
	int fign;
	char *builded = req->GetConnection()->BuildData(fign);
	req->PData = builded;
	char *cur_str = builded;
	bool content = false;
	bool end;
	for (int i = 0; i < fign; i++)
	{
		end = i > fign - 1;

		if (!content)
		{
			if ((i > 0 && builded[i - 1] == '\r' && builded[i] == '\n') || end)
			{
				if (!end)
				{
					builded[i - 1] = 0;
					builded[i] = 0;
				}
			} else continue;

			if (strlen(cur_str) == 0)
			{
				content = true;
				continue;
			}

			vector<char *> lstr = voxstring::split(cur_str, " ");
			if (cur_str == builded)
			{
				if (lstr.size() > 2)
				{
					if (strcmp(lstr[0], "POST") == 0)
					{
						req->RequestType = RequestType::POST;
					}

					char *furl = lstr[1];
					vector<char *> urs = voxstring::split(furl, "?");
					req->URL = string(furl);
					if (urs.size() > 1)
					{
						vector<char *> ursdet = voxstring::split(urs[1], "&");
						for (char *ddet : ursdet)
						{
							vector<char *> gprm = voxstring::split(ddet, "=");
							if (gprm.size() == 2 && strlen(gprm[1]) > 0)
								req->push_get(gprm[0], gprm[1]);
						}
					}
				} else
				{
					return false;
				}
			} else if (lstr.size() > 1)
			{
				if (voxstring::icompare(lstr[0], "Host:"))
				{
					req->Host = voxstring::split(string(lstr[1]), ":")[0];
				} else if (voxstring::icompare(lstr[0], "Upgrade:"))
				{
					if (voxstring::icompare(lstr[1], "websocket"))
					{
						req->GetConnection()->Protocol = WEBSOCKET;
					}
				} else if (voxstring::icompare(lstr[0], "Sec-WebSocket-Key:"))
				{
					req->GetConnection()->Additional = lstr[1];
				} else if (voxstring::icompare(lstr[0], "Content-Type:"))
				{
					for (int h = 1; h < lstr.size(); h++)
					{
						req->ContentType += lstr[h];
					}
				} else if (voxstring::icompare(lstr[0], "Cookie:"))
				{
					lstr[0] = &(lstr[0][8]);
					for (char *ns : lstr)
					{
						vector<char *> ursdet = voxstring::split(ns, ";");
						if (ursdet.size() > 0)
						{
							for (char *ddet: ursdet)
							{
								vector<char *> gprm = voxstring::split(ddet, "=");
								if (gprm.size() == 2)
									req->push_cookie(gprm[0], gprm[1]);
							}
						}
					}
				}
			}

			if (!end)
			{
				cur_str = &builded[i + 1];
			}
		} else if (req->RequestType == RequestType::POST) //content
		{
			vector<string> bndr = voxstring::split(req->ContentType, "boundary=");
			if (bndr.size() < 2)
			{
				vector<char *> ursdet = voxstring::split(&builded[i], "&");
				for (char *ddet : ursdet)
				{
					vector<char *> gprm = voxstring::split(ddet, "=");
					if (gprm.size() == 2 && strlen(gprm[1]) > 0)
					{
						req->push_post(gprm[0], gprm[1]);
					}
				}
				break;
			} else //MIME
			{
				//string bound = "--" + bndr[1].substr(0, bndr[1].length() - 1);
				unsigned long lena = bndr[1].length();
				char *bound = new char[2 + lena];
				bound[0] = '-';
				bound[1] = '-';
				memcpy(&bound[2], bndr[1].c_str(), lena);
				//bound[lena + 1] = 0;
				int oks = 0;
				bool ye = false;
				//char* spos = &builded[i + 1];
				for (int x = i; x < fign; x++)
				{
					if (!ye)
					{
						if (bound[oks] == builded[x])
						{
							if (oks++ == lena + 1)
							{
								ye = true;
								oks = 0;
							}
						} else
							oks = 0;
					} else
					{
						if (x >= fign - 3)
							return false;

						int strt = x;
						char *hed = &builded[strt];
						for (; x < fign; x++)
						{
							if (x >= strt + 3 && builded[x - 3] == '\r' && builded[x - 2] == '\n' &&
								builded[x - 1] == '\r' && builded[x] == '\n')
							{
								builded[x - 3] = 0;
								builded[x - 2] = 0;
								builded[x - 1] = 0;
								builded[x] = 0;
								bool isfile = false;
								char *prname = 0;
								char *fname = 0;
								string conttyp;

								if (x >= fign - 1)
									return false;

								int st_r = x + 1;
								char *mim_data = &builded[st_r];
								vector<char *> hed_data = voxstring::split(hed, "\r\n");
								for (char *hdt : hed_data)
								{
									vector<char *> ldt = voxstring::split(hdt, " ");
									if (ldt.size() > 1 && strcmp(ldt[0], "Content-Disposition:") == 0)
									{
										for (int p = 1; p < ldt.size(); p++)
										{
											if (strncmp(ldt[p], "name=\"", 6) == 0)
											{
												//size_t ln = strlen(&ldt[p][6]);

												prname = voxstring::split(&ldt[p][6], "\"")[0];
											} else if (strncmp(ldt[p], "filename=\"", 10) == 0)
											{
												//size_t ln = strlen(&ldt[p][10]);
												//ldt[p][10 + ln - 1] = 0;
												fname = voxstring::split(&ldt[p][10], "\"")[0];
												isfile = true;
											}
										}
									} else if (ldt.size() > 1 && strcmp(ldt[0], "Content-Type:") == 0 && ldt[1][0] != 0)
									{
										conttyp = ldt[1];
									}
								}

								if (!isfile && prname)
								{
									for (; x < fign; x++)
									{
										if (bound[oks] == builded[x])
										{
											if (oks++ == lena)
											{
												x -= lena;
												builded[x - 1] = 0;
												builded[x - 2] = 0;
												//char* ndat = &builded[x];
												x--;
												oks = 0;
												break;
											}
										} else
											oks = 0;
									}
									req->push_post(prname, mim_data);
								} else if (prname && fname)
								{
									UserFile fl;
									fl.Name = fname;
									fl.Data = mim_data;
									for (; x < fign; x++)
									{
										if (bound[oks] == builded[x])
										{
											if (oks++ == lena)
											{
												fl.Size = x - st_r - lena;
												x -= lena;
												builded[x - 1] = 0;
												builded[x - 2] = 0;
												x--;
												oks = 0;
												break;
											}
										} else
											oks = 0;
									}

									req->push_file(prname, fl);
								}

								break;
							}
						}

						ye = false;
						//if (builded[i - 1] == '\r' && builded[x] == '\n')
						//{

						//}
					}
				}
				delete[] bound;
				break;
			}
		} else
		{
			break;
		}
	}

	return !req->URL.empty() && !req->Host.empty();
}

void PacketProcessor::OnDisonnect(Connection *client)
{
	//printf("Dude disconnected!\n");
	if (client->ConnectedTo)
		client->ConnectedTo->OnDisconnect(client);
}

Site *PacketProcessor::FindSite(string domain)
{
	for (Site *site : Web->GetSitesList())
	{
		for (string dom : site->GetDomains())
		{
			if (dom == domain)
				return site;
		}
	}
	return 0;
}