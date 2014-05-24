#ifndef WINDOWS_UDP_CONNECTION_H
#define WINDOWS_UDP_CONNECTION_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#include "UdpConnection.h"

#pragma comment(lib, "Ws2_32.lib")

#define SOCKADDR struct sockaddr
#define SOCKADDR_IN struct sockaddr_in 

class WindowsUdpConnection : public UdpConnection
{
protected:
	SOCKET mySocket;
	SOCKADDR_IN address;
	static BOOL initialised;
	static WSADATA wsaData;
public:

	WindowsUdpConnection();
	virtual ~WindowsUdpConnection();

	virtual BOOL Create();
	virtual BOOL Bind(const unsigned short port);
	virtual BOOL Connect(const char * ip, const unsigned short port);
	virtual BOOL Read(char * buffer, int in_size, int * out_size);
	virtual BOOL ReadFrom(char * buffer, int in_size, int * out_size, class RemoteAddress ** addr);
	virtual BOOL Write(const char * buffer, int in_size, int * out_size);
	virtual BOOL WriteTo(const char * buffer, int in_size, int * out_size, class RemoteAddress * addr);
	virtual BOOL Close();
};

#endif//WINDOWS_UDP_CONNECTION_H