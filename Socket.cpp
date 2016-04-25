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
#include "Socket.h"
#include <vector>
#include <iostream>

Socket::Socket(const std::string &ip, const std::string &port){
#ifdef _WIN32
	//----------------------
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		//wprintf(L"WSAStartup failed with error: %d\n", iResult);
	}

	//----------------------
	// Create a SOCKET for connecting to server
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) {
		//wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
	}

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(ip.c_str());
	clientService.sin_port = htons(stoi(port));
	// Connect to server.
	iResult = connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService));
	if (iResult == SOCKET_ERROR) {
		//wprintf(L"connect failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		std::cout << "Connection failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Connected to " << ip << ":" << port << std::endl;
#else
	portno = atoi(port.c_str());
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		printf("FEHLER");
	server = gethostbyname(ip.c_str());
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
	}
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		(char *)&serv_addr.sin_addr.s_addr,
		server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		std::cout << "Connection failed" << std::endl;
		exit(EXIT_FAILURE);
	}
#endif
}

Socket::~Socket(){
#ifdef _WIN32
	iResult = closesocket(ConnectSocket);
	if (iResult == SOCKET_ERROR) {
		//wprintf(L"close failed with error: %d\n", WSAGetLastError());
		WSACleanup();
	}
	WSACleanup();
#else
	close(sockfd);
#endif
}

//Sendet String an Server und gibt Anzahl der gesendeten bytes zurück
int Socket::sendT(const std::string &buff){
#ifdef _WIN32
	iResult = send(ConnectSocket, buff.c_str(), buff.size(), 0);
	if (iResult == SOCKET_ERROR) {
		//wprintf(L"send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		std::cout << "Send failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	return iResult;
#else
	n = write(sockfd, buff.c_str(), buff.size());
	if (n < 0) {
		std::cout << "Send failed" << std::endl;
		exit(EXIT_FAILURE);
	}
		
#endif
}

std::string Socket::receiveT(){
#ifdef _WIN32
	std::string result;
	char firstPart[5];
	iResult = recv(ConnectSocket, firstPart, 5, MSG_WAITALL);
	if (iResult > 0){
		//wprintf(L"Bytes received: %d\n", iResult);
		result.append(firstPart, 5);

		u_long length = messageSize(result);
		char* secondPart = (char*)malloc(length);
		iResult = recv(ConnectSocket, secondPart, length, MSG_WAITALL);
		if (iResult > 0){
			//wprintf(L"Bytes received: %d\n", iResult);
			result.append(secondPart, length);
		}
		free(secondPart);
	}
	if(iResult == SOCKET_ERROR){
		std::cout << "Receive failed" << std::endl;
		exit(EXIT_FAILURE);	
	}
	return result;
#else
	std::string result;
	char firstPart[5];
	n = read(sockfd,firstPart,5);
	if(n > 0) {
		result.append(firstPart,5);
		u_long length = messageSize(result);
		char* secondPart = (char*)malloc(length);
		n = read(sockfd, secondPart, length);
		if (n > 0){
			result.append(secondPart, length);
		}
		free(secondPart);
	}
	if (n < 0){
		std::cout << "Receive failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	return result;
#endif
}

//Teilt den String in die Einzelnen Nachrichten
std::vector<std::string> splitString(std::string &s){
	std::vector<std::string> result;
	std::string part;
	while (s.size() >= 5){
		part = s.substr(0, messageSize(s) + 5);
		result.push_back(part);
		s = s.substr(messageSize(s) + 5, s.size());
	}
	return result;
}

//Gibt die Länge der Nachricht zurück, String wird mit Typ übergeben, also mindestens 5 Bytes
u_long messageSize(const std::string &message){
	u_long result = 0;
	unsigned char bytes[4] = { (unsigned char)message[1], (unsigned char)message[2], (unsigned char)message[3], (unsigned char)message[4] };
	result = (u_long)bytes[3] | ((u_long)bytes[2] << 8) | ((u_long)bytes[1] << 16) | ((u_long)bytes[0] << 24);
	return result;
}
