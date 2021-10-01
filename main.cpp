#define _WIN32_WINNT_WIN10 0x0A00 // Windows 10

#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main()
{
	
	//string ipAddress = "0.0.0.0";			// IP Address of the server
	int port = 502;						// Listening port # on the server

	// Initialize WinSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cerr << "Can't start Winsock, Err #" << wsResult << endl;
		return 0;
	}

	// Create socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create socket, Err #" << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}

	// Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; //inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsick the socket is for listening 
	listen(listening, SOMAXCONN);

	// Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	if (clientSize == INVALID_SOCKET) 
	{
		cerr << "Can't create client socket, Err #" << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}

	char host[NI_MAXHOST];		// Client's remote name
	char service[NI_MAXHOST];	// Service (i.e. port) the client is connected on

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port " << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " <<
			ntohs(client.sin_port) << endl;
	}

	// Close listening socket
	closesocket(listening); // Can remove later

	// While loop: accept and echo message back to client
	char buffer[4096];
	string message;
	int command;

	while (true)
	{
		ZeroMemory(buffer, 4096);

		// Wait for client to send data
		int bytesReceived = recv(clientSocket, buffer, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			cerr << "Error in recv(). Quitting..." << endl;
		}

		else if (bytesReceived == 0)
		{
			cout << "Client disconnected " << endl;
			break;
		}

		// Echo message back to client
		//send(clientSocket, buffer, bytesReceived + 1, 0);
		
		if ((int)buffer[0] != 13)
		{
			message = buffer;
			cout << "Message recieved: " << message << endl;
			command = buffer[6] - '0';
			cout << command << endl;
			switch (command)
			{
			case 1:
				cout << "Homing Shuttle" << endl;
				break;
			case 2:
				cout << "Homing Flipper" << endl;
				break;
			default:
				break;
			}
		}
		
	}

	// Close the socket
	closesocket(clientSocket);

	// Cleanup winsock
	WSACleanup();

	return 0;
	
}
