#include <iostream>
#include <csignal>
#include <execinfo.h>
#include "ConnectManager.h"
#include "SiteLoader.h"
#include "PacketProcessor.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
using namespace std;

void handler(int sig)
{
	void *array[10];
	int size;

	//10 last
	size = backtrace(array, 10);

	fprintf(stderr, "Error: signal %d:\n", sig);
	backtrace_symbols_fd(array, size, STDERR_FILENO);
	//so hard
	exit(322);
}

int main()
{
	//for crash info
	signal(SIGSEGV, handler);
	//ignoring sigpipe, cuz we not like him
	signal(SIGPIPE, SIG_IGN);
	PacketProcessor::Web = new Webserver(ServerMode(WEBSERVER_MODE | CORESERVER_MODE | DATASERVER_MODE));
	SiteLoader::LoadAll();
	pthread_t trd;
	pthread_create(&trd, NULL, (void *(*)(void *)) ConnectManager::StartServer, (void *) 80);
	while (true)
	{
		string inp;
		cin >> inp;
		if (inp == "stop")
		{
			ConnectManager::Started = false;
		}
	}
}

#pragma clang diagnostic pop