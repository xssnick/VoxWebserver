//
// Created by root on 16.06.16.
//

#include <sys/socket.h>
#include <errno.h>
#include "NetworkHelper.h"


int NetworkHelper::sendall(int socket, const char *buf, unsigned long len, unsigned long &sent)
{
	sent = 0;
	unsigned long bytesleft = len; // how many we have left to send
	int n;

	while (sent < len)
	{
		n = send(socket, buf + sent, bytesleft, 0);
		if (n == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				//buffer is full, will push to send q
				return 1;
			}
			// err
			return -1;
		}
		sent += n;
		bytesleft -= n;
	}

	return 0;
}