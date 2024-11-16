// Networking_Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <vector>

using namespace std;

#include <thread>

class TCPServer {
private:
    SOCKET serverSocket;
    vector<SOCKET> clientSockets;
    int port;

public:
    TCPServer(int port) : port(port), serverSocket(INVALID_SOCKET) {}

    bool initializeWinsock() 
    {
        WSADATA wsaData;
        WORD wVersionRequested = MAKEWORD(2, 2);
        int wsaerr = WSAStartup(wVersionRequested, &wsaData);
        if (wsaerr != 0) 
        {
            return false;
        }
        return true;
    }

    SOCKET createServerSocket() 
    {
        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        return serverSocket;
    }

    bool bindServerSocket() 
    {
        sockaddr_in service;
        service.sin_family = AF_INET;
        InetPton(AF_INET, L"127.0.0.1", &service.sin_addr.s_addr);
        service.sin_port = htons(port);

        if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) 
        {
            return false;
        }
        return true;
    }

    bool listenForClients() 
    {
        if (listen(serverSocket, 5) == SOCKET_ERROR) 
        {
            return false;
        }
        return true;
    }

    void handleClientConnections() 
    {
        fd_set readfds;
        SOCKET maxSocket = serverSocket;

        cout << "Server listening on port " << port << "..." << endl;

        while (true) 
        {
            // Reset the readfds set
            FD_ZERO(&readfds);
            FD_SET(serverSocket, &readfds);  // Add server socket to the set

            // Add all client sockets to the set
            for (auto& client : clientSockets) 
            {
                FD_SET(client, &readfds);
            }

            // Set a timeout (e.g., 5 seconds)
            timeval timeout;
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;

            // Wait for activity on any of the sockets (server or client)
            int activity = select(0, &readfds, nullptr, nullptr, &timeout);

            if (activity == SOCKET_ERROR) 
            {
                cout << "select() error: " << WSAGetLastError() << endl;
                break;
            }

            // Check if there is activity on the server socket (incoming client connection)
            if (FD_ISSET(serverSocket, &readfds)) 
            {
                SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
                if (clientSocket != INVALID_SOCKET) 
                {
                    cout << "New client connected!" << endl;
                    clientSockets.push_back(clientSocket); // Add to client list
                }
                else 
                {
                    cout << "accept() failed: " << WSAGetLastError() << endl;
                }
            }

            // Handle I/O for each client socket
            for (size_t i = 0; i < clientSockets.size(); ++i) 
            {
                SOCKET clientSocket = clientSockets[i];

                if (FD_ISSET(clientSocket, &readfds)) 
                {
                    handleClientCommunication(clientSocket);
                }
            }
        }
    }

    void handleClientCommunication(SOCKET clientSocket) 
    {
        char buffer[200];
        int byteCount = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (byteCount > 0) 
        {
            buffer[byteCount] = '\0';  // Null-terminate the received data
            cout << "Message received from client: " << buffer << endl;
            // Send a response
            const char* confirmation = "Message Received";
            send(clientSocket, confirmation, strlen(confirmation), 0);
        }
        else if (byteCount == 0) 
        {
            // Client disconnected
            cout << "Client disconnected!" << endl;
            closesocket(clientSocket);
        }
        else 
        {
            cout << "recv failed: " << WSAGetLastError() << endl;
        }
    }

    void cleanUp() 
    {
        closesocket(serverSocket);
        WSACleanup();
    }

    SOCKET getServerSocket() const { return serverSocket; }
};


int main()
{
	cout << "// ----------------------------- SERVER ---------------------------------------------// " "\n";
    int port = 55555;
    TCPServer server(port);

    // Initialize Winsock
    if (!server.initializeWinsock()) 
    {
        cout << "Failed to initialize Winsock" << endl;
        return 0;
    }

    // Create the server socket
    if (server.createServerSocket() == INVALID_SOCKET) 
    {
        cout << "Failed to create server socket" << endl;
        server.cleanUp();
        return 0;
    }

    // Bind the server socket
    if (!server.bindServerSocket()) 
    {
        cout << "Failed to bind server socket" << endl;
        server.cleanUp();
        return 0;
    }

    // Start listening for clients
    if (!server.listenForClients()) 
    {
        cout << "Failed to listen on server socket" << endl;
        server.cleanUp();
        return 0;
    }

    // Handle incoming connections and communication with clients
    server.handleClientConnections();

    // Clean up before exiting
    server.cleanUp();
    return 0;
}

