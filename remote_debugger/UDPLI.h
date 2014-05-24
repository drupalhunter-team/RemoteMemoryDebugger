#ifndef UDP_LI_H
#define UDP_LI_H

#include "CLI.h"

class UDPLi : public CLI
{
protected:
	class UdpConnection * udpConnection;
public:

	UDPLi(class RemoteDebugger * dbg, const unsigned short port);
	virtual ~UDPLi();

protected:

	virtual void GetInput(char * buffer, size_t buffer_len, size_t * len, RemoteAddress ** addr);
	virtual void PrintOut(RemoteAddress * addr, const char *);
	
};

#endif//UDP_LI_H