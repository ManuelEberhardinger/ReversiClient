#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <vector>
#ifdef _WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
#else
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#endif

class Socket
{
public:
	Socket(const std::string &ip, const std::string &port);
	~Socket();
	int sendT(const std::string &buff);
	std::string receiveT();
private:
#ifdef _WIN32
	int iResult;
	WSADATA wsaData;

	SOCKET ConnectSocket = INVALID_SOCKET;
	struct sockaddr_in clientService;
#else
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
#endif
};

std::vector<std::string> splitString(std::string &s);
u_long messageSize(const std::string &);

#endif