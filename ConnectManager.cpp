//
// Created by root on 13.06.16.
//
#include "thpool.h"
#include "settings.h"
#include "ConnectManager.h"
#include "PacketProcessor.h"
#include "VoxWebAPI.LIN64/NetworkHelper.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

int setnonblocking(int sockfd)
{
	fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
	return 0;
}

bool ConnectManager::Started = false;

void ConnectManager::StartServer(int port)
{
	int listener; //server socket
	struct sockaddr_in addr, their_addr;
	addr.sin_family = PF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(SERVER_HOST);

	socklen_t socklen;
	socklen = sizeof(struct sockaddr_in);

	static struct epoll_event ev, events[EPOLL_SIZE];
	//     watch just incoming(EPOLLIN)
	ev.events = EPOLLIN | EPOLLRDHUP;

	//     epoll descriptor to watch events

	int client, epoll_events_count;

	listener = socket(PF_INET, SOCK_STREAM, 0);

	setnonblocking(listener);

	//    bind listener to address(addr)
	if (bind(listener, (struct sockaddr *) &addr, sizeof(addr)) != 0)
	{
		printf("Cant get access to that port\n");
		exit(-1);
	}

	listen(listener, 1);

	PacketProcessor::Web->MainEPoll = epoll_create(EPOLL_SIZE);

	ev.data.fd = listener;

	epoll_ctl(PacketProcessor::Web->MainEPoll, EPOLL_CTL_ADD, listener, &ev);
	Started = true;
	threadpool thpool = thpool_init(WORK_THREADS_NUM);

	printf("Started! epool:%d\n", PacketProcessor::Web->MainEPoll);

	//TODO: we have big memory leak, possibly when sending files
	while (Started)
	{
		epoll_events_count = epoll_wait(PacketProcessor::Web->MainEPoll, events, EPOLL_SIZE, EPOLL_RUN_TIMEOUT);

		for (int i = 0; i < epoll_events_count; i++)
		{
			if (events[i].events & EPOLLIN)
			{
				if (events[i].data.fd == listener)//connected
				{
					client = accept(listener, (struct sockaddr *) &their_addr, &socklen);

					setnonblocking(client);

					Connection *con = new Connection(PacketProcessor::Web);
					ev.data.fd = client;
					ev.data.ptr = con;
					con->epoll_data = ev;
					con->Socket = client;
					con->Connected = true;
					epoll_ctl(PacketProcessor::Web->MainEPoll, EPOLL_CTL_ADD, client, &ev);
				} else//request
				{
					Connection *con = (Connection *) events[i].data.ptr;
					if (con->Processing)
					{
						//printf("W");
						continue;
					}
					ssize_t count = 0;
					bool fail = false;
					while (1)
					{
						char *buf = new char[PACKET_SIZE]();
						count = read(con->Socket, buf, PACKET_SIZE);
						if (count == -1)//err
						{
							if (errno != EAGAIN && errno != EWOULDBLOCK)
							{
								//printf("ER %d |%s\n",errno,strerror(errno));
								fail = true;
							}
							delete[] buf;
							break;
						} else if (count == 0) //closed by client
						{
							fail = true;
							delete[] buf;
							break;
						} else if (count > 0)//ok
						{
							if (con->RevcData.GetSize() > MAX_PARTS)
							{
								fail = true;
								delete[] buf;
								break;
							}
							con->GotData = true;
							Data_Part part;
							part.size = (unsigned long) count;
							part.Data = buf;
							con->RevcData.Add(part);
							//con->PushRecvData(buf, (unsigned int) count);
						}
					}

					if (fail)
					{
						con->Disconnect();
					} else if (!con->Processing)
					{
						//TODO: loosing data if comes too fast ????
						//while(con->GotData && con->Connected)
						//{
						con->Processing = true;
						thpool_add_work(thpool, (void *(*)(void *)) PacketProcessor::OnConnect,
										con);// PacketProcessor::OnConnect(con);
						//PacketProcessor::OnConnect(con);
						//}
					} else
					{
						//printf("R");
					}
				}
			} else if (events[i].events & EPOLLOUT)
			{
				Connection *con = (Connection *) events[i].data.ptr;
				while (con->SendQueue.size() > 0)
				{
					Data_Part_Snd *dps = con->SendQueue.front();
					unsigned long sent;
					int res = NetworkHelper::sendall(con->Socket, &dps->data[dps->offset], dps->size - dps->offset,
													 sent);
					if (res == 1)//buffer full
					{
						dps->offset += sent;
						goto unluck;
					} else if (res == -1)//err
					{
						con->Processing = false;
						con->Disconnect();
						goto unluck;
					} else//ok
					{
						con->SendQueue.pop();
						if (dps->iskill)
							dps->Destroy();

						delete dps;
					}
				}
				con->Processing = false;
				con->ChangePollState(EPOLLIN);
				unluck:;
			} else
			{
				printf("WTF %d!\n", events[i].events);
				((Connection *) events[i].data.ptr)->Disconnect();
			}
		}

		// printf("Statistics: %d events handled at: %.5lf second(s)\n",
		//       epoll_events_count,
		//       (double)(clock() - tStart)/CLOCKS_PER_SEC);
	}

	thpool_destroy(thpool);
	close(listener);
	close(PacketProcessor::Web->MainEPoll);
	printf("Webserver stopped successfully!\n");
}

#pragma clang diagnostic pop