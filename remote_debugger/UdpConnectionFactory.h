#ifndef UDP_CONNECTION_FACTORY_H
#define UDP_CONNECTION_FACTORY_H

#ifdef WIN32
#include "WindowsUdpConnection.h"
#endif

class UdpConnectionFactory
{
public:
	static class UdpConnection * GetUdpConnection()
	{
#ifdef WIN32
		return new WindowsUdpConnection();
#endif
	}
};

#endif//UDP_CONNECTION_FACTORY_H