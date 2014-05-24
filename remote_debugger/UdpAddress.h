#ifndef UDP_ADDRESS_H
#define UDP_ADDRESS_H

#include "RemoteAddress.h"

class UdpAddress : public RemoteAddress
{
public:
	virtual ~UdpAddress() {}
	virtual void GetName(char * buffer, size_t buffer_len) = 0;
};

#endif//UDP_ADDRESS_H