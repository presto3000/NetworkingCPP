// Networking_Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <string>

using namespace std;

class TCPClient {
private:
	SOCKET clientSocket;
	int port;
	string serverIP;

public:
	TCPClient(const string& ip, int port) : serverIP(ip), port(port), clientSocket(INVALID_SOCKET) {}

	// Initialize Winsock
	bool initializeWinsock() 
	{
		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD(2, 2);
		int wsaerr = WSAStartup(wVersionRequested, &wsaData);
		if (wsaerr != 0) {
			cout << "WSAStartup failed: " << wsaerr << endl;
			return false;
		}
		return true;
	}

	// Create a socket for the client
	bool createClientSocket() 
	{
		clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (clientSocket == INVALID_SOCKET) {
			cout << "Error creating socket: " << WSAGetLastError() << endl;
			return false;
		}
		return true;
	}

	// Connect to the server
	bool connectToServer() 
	{
		std::string serverIP = "127.0.0.1"; // The server's IP address as a string
		std::wstring wideServerIP(serverIP.begin(), serverIP.end());

		sockaddr_in clientService;
		clientService.sin_family = AF_INET;
		InetPton(AF_INET, wideServerIP.c_str(), &clientService.sin_addr.s_addr);
		clientService.sin_port = htons(port);

		if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) 
		{
			cout << "Connect failed: " << WSAGetLastError() << endl;
			return false;
		}

		cout << "Connected to server at " << serverIP << ":" << port << endl;
		return true;
	}

	// Send message to the server
	bool sendMessage(const string& message) 
	{
		int byteCount = send(clientSocket, message.c_str(), message.length(), 0);
		if (byteCount == SOCKET_ERROR) {
			cout << "Send failed: " << WSAGetLastError() << endl;
			return false;
		}
		cout << "Message sent: " << message << endl;
		return true;
	}

	// Receive message from the server
	bool receiveMessage() 
	{
		char buffer[200];
		int byteCount = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (byteCount > 0) 
		{
			buffer[byteCount] = '\0'; // Null-terminate the received string
			cout << "Message received from server: " << buffer << endl;
			return true;
		}
		else if (byteCount == 0) {
			cout << "Connection closed by server" << endl;
			return false;
		}
		else {
			cout << "Receive failed: " << WSAGetLastError() << endl;
			return false;
		}
	}

	// Close the client socket
	void closeConnection() 
	{
		closesocket(clientSocket);
		WSACleanup();
	}
};

int main()
{
	string serverIP = "127.0.0.1";  // Server IP address
	int port = 55555;  // Server port

	// Create a TCPClient object
	TCPClient client(serverIP, port);

	// Initialize Winsock
	if (!client.initializeWinsock()) 
	{
		cout << "Failed to initialize Winsock" << endl;
		return 1;
	}

	// Create the client socket
	if (!client.createClientSocket()) 
	{
		cout << "Failed to create client socket" << endl;
		client.closeConnection();
		return 1;
	}

	// Connect to the server
	if (!client.connectToServer()) 
	{
		cout << "Failed to connect to the server" << endl;
		client.closeConnection();
		return 1;
	}

	// Chat with the server
	string message;
	cout << "Please enter a message to send to the server: ";
	std::getline(std::cin, message);
	if (!client.sendMessage(message)) 
	{
		cout << "Failed to send the message" << endl;
		client.closeConnection();
		return 1;
	}

	// Receive response from the server
	if (!client.receiveMessage()) 
	{
		cout << "Failed to receive a response from the server" << endl;
		client.closeConnection();
		return 1;
	}

	// Close the connection
	client.closeConnection();
	return 0;

}
