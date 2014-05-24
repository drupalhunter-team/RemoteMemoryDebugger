#ifndef UDP_CONNECTION_H
#define UDP_CONNECTION_H

#include "OSTypes.h"

#define ADDR LPVOID

class UdpConnection
{
public:

	virtual ~UdpConnection() {}

	virtual BOOL Create() = 0;
	virtual BOOL Bind(const unsigned short port) = 0;
	virtual BOOL Connect(const char * ip, const unsigned short port) = 0;
	virtual BOOL Read(char * buffer, int in_size, int * out_size) = 0;
	virtual BOOL ReadFrom(char * buffer, int in_size, int * out_size, class RemoteAddress ** addr) = 0;
	virtual BOOL Write(const char * buffer, int in_size, int * out_size) = 0;
	virtual BOOL WriteTo(const char * buffer, int in_size, int * out_size, class RemoteAddress * addr) = 0;
	virtual BOOL Close() = 0;

};

#endif//UDP_CONNECTION_H