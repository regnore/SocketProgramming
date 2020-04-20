// Server.cpp

#include "pch.h"
#include<WinSock2.h>
#include<WS2tcpip.h>
#include <iostream>

using namespace std;

#pragma comment(lib,"ws2_32.lib")

DWORD threadID = 0;
int currentClientNum = 0;

DWORD WINAPI clientChildThread(LPVOID ipParameter) {
	SOCKET clientSocket = (SOCKET)ipParameter;
	int ID = ++threadID;
	char IDString[10];
	int const CLIENT_MSG_SIZE = 128;
	char inMSG[CLIENT_MSG_SIZE];
	char outMSG[CLIENT_MSG_SIZE];
	char wx[] = "Invalid Command.";
	int size;
	memset(inMSG, 0, CLIENT_MSG_SIZE);
	sprintf_s(outMSG, "%03d",ID);
	send(clientSocket, outMSG, CLIENT_MSG_SIZE, 0);
	sprintf_s(IDString, "%03d", ID);
	currentClientNum++;
	cout << "Thread "<<IDString<<" starts to serve." <<"("<<currentClientNum<<")"<< endl;
	while (true) {
		memset(inMSG, 0, CLIENT_MSG_SIZE);
		size = recv(clientSocket, inMSG, CLIENT_MSG_SIZE, 0);
		if (size == SOCKET_ERROR) {
			cout << "Connection Interruptted，ERROR：" << WSAGetLastError() << endl;
			closesocket(clientSocket);
			break;
		}
		cout << "Client "<<IDString<<" :" << inMSG << endl;
		if (strcmp(inMSG, "$help") == 0) {
			sprintf_s(outMSG, " $help\t\tShow Command Direction.\n $time\t\tShow Server Current Time.\n $quit\t\tTerminate the Connection.");
			send(clientSocket, outMSG, CLIENT_MSG_SIZE, 0);
		}
		else if (strcmp(inMSG, "$time") == 0) {
			SYSTEMTIME systime = { 0 };
			GetLocalTime(&systime);
			sprintf_s(outMSG, "%d-%02d-%02d %02d:%02d:%02d",
				systime.wYear, systime.wMonth, systime.wDay,
				systime.wHour, systime.wMinute, systime.wSecond);
			send(clientSocket, outMSG, CLIENT_MSG_SIZE, 0);
		}
		else if (strcmp(inMSG, "$quit") == 0) {
			closesocket(clientSocket);
			currentClientNum--;
			cout << IDString << " Connection Terminated Successfully.(" << currentClientNum << ")" << endl;
			if (currentClientNum == 0)
				cout << "\nWaiting for the Client Connection.(0)" << endl;
			break;
		}
		else {
			send(clientSocket, wx, sizeof(wx), 0);
		}
	}

	return 0;

}


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
		cout << "ERROR:" << WSAGetLastError() << endl;
		WSACleanup();
		return -2;
	}

	SOCKADDR_IN server;
	server.sin_family = AF_INET;
	server.sin_port = htons(6863);
	server.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(serverSocket, (struct sockaddr*) &server, sizeof(server)) == SOCKET_ERROR) {
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

	cout << "\nWaiting for the Client Connection.(0)" << endl;

	SOCKET clientSocket;
	SOCKADDR_IN client;
	int addrsize = sizeof(SOCKADDR_IN);
	HANDLE pThread;
	while (true) {

		clientSocket = accept(serverSocket, (struct sockaddr*) &client, &addrsize);
		if (clientSocket == INVALID_SOCKET) {
			cout << "Client Accept Failed，ERROR：" << WSAGetLastError() << endl;
			closesocket(serverSocket);
			WSACleanup();
			return -5;
		}
		else {
			char sendBuf[20] = { '\0' };
			cout << "Client\n"
				<< inet_ntop(AF_INET, (void*) &client.sin_addr, sendBuf, 16)
				<< "\nVia Port：\n"
				<< ntohs(client.sin_port)
				<< "\nConnection Succeed." << endl;
			pThread = CreateThread(NULL, 0, clientChildThread, (LPVOID)clientSocket, 0, NULL);
			if (pThread == NULL) {
				cout << "Failed to Create the Thread." << endl;
				break;
			}
			CloseHandle(pThread);

		}
	}

	closesocket(serverSocket);
	closesocket(clientSocket);
	WSACleanup();

	return 0;
}
