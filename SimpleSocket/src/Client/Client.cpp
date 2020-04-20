// Client.cpp

#include "pch.h"
#include <WinSock2.h>
#include<WS2tcpip.h>
#include <iostream>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

int main()
{
    
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		WSACleanup();
		return -1;
	}

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		cout << "ERROR:" << WSAGetLastError() << endl;
		WSACleanup();
		return -2;
	}

	SOCKADDR_IN client;
	client.sin_family = AF_INET;
	client.sin_port = htons(6863);
	inet_pton(AF_INET, "127.0.0.1", (void*)&client.sin_addr.S_un.S_addr);

	int const SERVER_MSG_SIZE = 128;
	char inMSG[SERVER_MSG_SIZE] = { 0 };
	char outMSG[SERVER_MSG_SIZE];

	if (connect(clientSocket, (struct sockaddr*) &client, sizeof(client)) < 0) {
		cout << "ERROR:" << WSAGetLastError() << endl;
		closesocket(clientSocket);
		WSACleanup();
		return -3;
	}
	else {
		cout << "Connect to the Server Successfully!\n" << endl;
		while (true) {
			memset(outMSG, 0, SERVER_MSG_SIZE);
			cout << "Please input the request(input \"$help\" for command direction)：" << endl;
			cin >> outMSG;
			send(clientSocket, outMSG, SERVER_MSG_SIZE, 0);
			if (strcmp(outMSG, "$quit") == 0) {
				break;
			}
			int size = recv(clientSocket, inMSG, SERVER_MSG_SIZE, 0);
			cout << "Server Response：\n" << inMSG << endl;
			memset(inMSG, 0, SERVER_MSG_SIZE);
		}
	}

	closesocket(clientSocket);
	WSACleanup();

	system("pause");
	return 0;
}
