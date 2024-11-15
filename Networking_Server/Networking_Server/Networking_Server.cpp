// Networking_Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>

using namespace std;

int main()
{
	cout << "// ----------------------------- SERVER ---------------------------------------------// " "\n";
	cout << "// ----------------------------- Step 1 - Set up DLL ------------------------------- // " "\n";
	SOCKET serverSocket, acceptSocket;
	int port = 55555;
	WSADATA wsaData;
	int wsaerr;
	WORD wVersionRequested = MAKEWORD(2, 2);
	wsaerr = WSAStartup(wVersionRequested, &wsaData);

	if (wsaerr != 0)
	{
		cout << "The Winsock dll not found!" << endl;
		return 0;
	}
	else
	{
		cout << "The winsock dll found!" << endl;
		cout << "The status: " << wsaData.szSystemStatus << endl;
	}

	cout << "// ----------------------------- Step 2 - Set up Server Sockets ------------------------------- // " "\n";
	serverSocket = INVALID_SOCKET;
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET)
	{
		cout << "Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}
	else
	{
		cout << "socket() is OK" << endl;
	}
	cout << "// ----------------------------- Step 3 - Bind Socket ------------------------------- // " "\n";
	sockaddr_in service;
	service.sin_family = AF_INET;
	InetPton(AF_INET, L"127.0.0.1", &service.sin_addr.s_addr);
	service.sin_port = htons(port);
	if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		cout << "bind() failed: " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	}
	else
	{
		cout << "bind() is OK!" << endl;
	}
	cout << "// ----------------------------- Step 4 - Initiate Listen ------------------------------- // " "\n";
	if (listen(serverSocket, 1) == SOCKET_ERROR)
	{
		cout << "listen(): Error listening on socket" << WSAGetLastError() << endl;
	}
	else
	{
		cout << "listen() is OK, I'm waiting for connections..." << endl;
	}
	cout << "// ----------------------------- Step 5 - Accept connection from Client ------------------------------- // " "\n";
	acceptSocket = accept(serverSocket, NULL, NULL);
	if (acceptSocket == INVALID_SOCKET)
	{
		cout << "accept failed: " << WSAGetLastError() << endl;
		WSACleanup();
		return -1;
	}
	cout << "Accepted connection " << endl;
	system("pause");
	WSACleanup();


	return 0;
}

