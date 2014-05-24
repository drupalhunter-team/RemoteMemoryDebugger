#include "UDPLI.h"
#include "RemoteDebugger.h"
#include "UdpConnectionFactory.h"
#include "RemoteAddress.h"

UDPLi::UDPLi( RemoteDebugger * dbg, const unsigned short port)
	:CLI(dbg)
{
	this->udpConnection = UdpConnectionFactory::GetUdpConnection();

	if ( this->udpConnection )
	{
		if ( this->udpConnection->Create() == TRUE )
		{
			if ( this->udpConnection->Bind(port) == TRUE )
			{
				printf("Server socket ready\n");
			}
		}
	}
}

UDPLi::~UDPLi()
{
	if ( this->udpConnection )
	{
		this->udpConnection->Close();
		delete this->udpConnection;
		this->udpConnection = NULL;
	}
}

void UDPLi::GetInput(char * buffer, size_t buffer_len, size_t * len, RemoteAddress ** addr)
{
	if ( this->udpConnection )
	{
		if ( this->udpConnection->ReadFrom(
			buffer, 
			(int)buffer_len,
			(int*)len,
			addr) == FALSE)
		{
			printf("Failed to get UDP data\n");
		}
	}
	else
	{
		printf("Connection not present\n");
	}
}

void UDPLi::PrintOut(RemoteAddress * addr, const char * buffer)
{
	if ( this->udpConnection && addr && buffer )
	{
		char address[255] = {0};
		addr->GetName(address,255);
		printf("%s: %s", address, buffer);
		if ( this->udpConnection->WriteTo(buffer, (int)strlen(buffer), NULL, addr) == FALSE)
		{
			printf("NO WRITE: %s", buffer);
		}
	}
	else
	{
		printf("NO CONN: %s", buffer);
	}
}
