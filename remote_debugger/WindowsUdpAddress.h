#ifndef WINDOWS_UDP_ADDRESS
#define WINDOWS_UDP_ADDRESS

#include "UdpAddress.h"

#include "OSTypes.h"
#include <winsock2.h>

#define SOCKADDR struct sockaddr
#define SOCKADDR_IN struct sockaddr_in 

class WindowsUdpAddress : public UdpAddress
{
protected:
	SOCKADDR_IN address;
public:
	WindowsUdpAddress()
	{
		memset(&address,0, sizeof(SOCKADDR_IN));
	}
	SOCKADDR_IN * GetAddress()
	{
		return &this->address;
	}
	void SetAddress(SOCKADDR_IN addr)
	{
		this->address = addr;
	}
	virtual void GetName(char * buffer, size_t buffer_len)
	{
		if ( buffer )
		{
			char ip[4] = {0,0,0,0};
			unsigned short port = 0;

			port = address.sin_port;
			ip[3] = (char)((address.sin_addr.s_addr >> 24) & 0xFF);
			ip[2] = (char)((address.sin_addr.s_addr >> 16) & 0xFF);
			ip[1] = (char)((address.sin_addr.s_addr >> 8) & 0xFF);
			ip[0] = (char)((address.sin_addr.s_addr) & 0xFF);

			sprintf_s(buffer, buffer_len, "%d.%d.%d.%d:%d\0",
				ip[0],ip[1],ip[2],ip[3],
				port);

		}
	}
};

#endif//WINDOWS_UDP_ADDRESS