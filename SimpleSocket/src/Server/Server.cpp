// Server.cpp

#include "pch.h"
#include <WinSock2.h>
#include <iostream>
#include<WS2tcpip.h>
using namespace std;

#pragma comment (lib,"ws2_32.lib")

int main()
{

	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		WSACleanup();
		return -1;
	}

	SOCKET serverSocket;
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {
		cout << "ERROR:" + WSAGetLastError() << endl;
		WSACleanup();
		return -2;
	}

	SOCKADDR_IN server;
	server.sin_family = AF_INET;
	server.sin_port = htons(6863);
	inet_pton(AF_INET, "127.0.0.1", (void*)&server.sin_addr.S_un.S_addr);

	if (bind(serverSocket, (struct sockaddr *) &server, sizeof(server)) == SOCKET_ERROR) {
		cout << "ERROR:" << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return -3;
	}

	if (listen(serverSocket, 2) == SOCKET_ERROR) {
		cout << "ERROR:" << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return -4;
	}

	SOCKET clientSocket;
	SOCKADDR_IN client;
	int addrSize = sizeof(SOCKADDR_IN);
	int const CLIENT_MSG_SIZE = 128;
	char inMSG[CLIENT_MSG_SIZE];
	char outMSG[CLIENT_MSG_SIZE];
	char wx[] = "Invalid Message";
	int size;
	while (true) {
		cout << "\nWaiting for Client Connecting..." << endl;

		clientSocket = accept(serverSocket, (struct sockaddr *) &client, &addrSize);
		if (clientSocket == INVALID_SOCKET) {
			cout << "Client accept failed，ERROR：" << WSAGetLastError() << endl;
			closesocket(serverSocket);
			WSACleanup();
			return -5;
		}
		else {
			char sendBuf[20] = { '\0' };
			cout << "Client\n"
				<< inet_ntop(AF_INET, (void*)&server.sin_addr, sendBuf, 16)
				<< "\nVia Port：\n"
				<< ntohs(client.sin_port)
				<< "\nConnection Succeed." << endl;

			while (true) {
				memset(inMSG, 0, CLIENT_MSG_SIZE);
				size = recv(clientSocket, inMSG, CLIENT_MSG_SIZE, 0);
				if (size == SOCKET_ERROR) {
					cout << "Connection Interrupted, ERROR：" << WSAGetLastError() << endl;
					closesocket(clientSocket);
					break;
				}
				cout << "Clinet Message：\n" << inMSG << endl;
				if (strcmp(inMSG, "$help") == 0) {
					sprintf_s(outMSG, " $time\t\tShow server current time.\n $help\t\tShow all commands.\n $quit\t\tTerminate the Connection.");
					send(clientSocket, outMSG, CLIENT_MSG_SIZE, 0);
					memset(outMSG, 0, CLIENT_MSG_SIZE);
				}
				else if (strcmp(inMSG, "$time") == 0) {
					SYSTEMTIME systime = { 0 };
					GetLocalTime(&systime);
					sprintf_s(outMSG, "%d-%02d-%02d %02d:%02d:%02d",
						systime.wYear, systime.wMonth, systime.wDay,
						systime.wHour, systime.wMinute, systime.wSecond);
					send(clientSocket, outMSG, CLIENT_MSG_SIZE, 0);
					memset(outMSG, 0, CLIENT_MSG_SIZE);
				}
				else if (strcmp(inMSG, "$quit") == 0) {
					closesocket(clientSocket);
					cout << "Client quit successfully." << endl;
					break;
				}
				else {
					send(clientSocket, wx, sizeof(wx), 0);
				}
			}

			closesocket(clientSocket);
		}
	}

	closesocket(serverSocket);
	WSACleanup();

	return 0;

}
