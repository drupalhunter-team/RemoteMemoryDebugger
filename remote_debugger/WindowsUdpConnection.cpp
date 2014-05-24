#include "WindowsUdpConnection.h"
#include "WindowsUdpAddress.h"

BOOL WindowsUdpConnection::initialised = FALSE;
WSADATA WindowsUdpConnection::wsaData;

WindowsUdpConnection::WindowsUdpConnection()
{
	this->mySocket= 0;
	memset((void*)&this->address, 0, sizeof(SOCKADDR_IN));
}

WindowsUdpConnection::~WindowsUdpConnection()
{
	if (this->mySocket> 0)
	{
		this->Close();
	}
}

BOOL WindowsUdpConnection::Create()
{
	BOOLEAN ret = TRUE;

	if ( initialised == FALSE )
	{
		int iResult;

		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (iResult != 0) {
			ret = FALSE;
		}
		else
		{
			initialised = TRUE;
		}
	}

	if ( ret == TRUE )
	{
		if ( this->mySocket > 0 )
		{
			this->Close();
		}
		this->mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if ( this->mySocket <= 0 )
		{
			ret = FALSE;
		}
	}

	return ret;
}

BOOL WindowsUdpConnection::Bind(const unsigned short port)
{
	BOOL ret = FALSE;

	if ( this->mySocket > 0 )
	{
		this->address.sin_family = AF_INET;
		this->address.sin_port = htons(port);
		this->address.sin_addr.s_addr = htonl(INADDR_ANY);
		int b = bind(this->mySocket, (SOCKADDR*)&this->address, sizeof(SOCKADDR_IN));

		if ( b == 0 )
		{
			ret = TRUE;
		}
	}

	return ret;
}

BOOL WindowsUdpConnection::Connect(const char * ip, const unsigned short port)
{
	BOOL ret = FALSE;

	if ( this->mySocket > 0 )
	{
		this->address.sin_family = AF_INET;
		this->address.sin_port = htons(port);
		this->address.sin_addr.s_addr = inet_addr(ip);

		if ( connect(this->mySocket, (SOCKADDR*)&this->address,sizeof(SOCKADDR_IN)) == 0 )
		{
			ret = TRUE;
		}
	}

	return ret;
}

BOOL WindowsUdpConnection::Read(char * buffer, int in_size, int * out_size)
{
	BOOL ret = FALSE;

	if ( this->mySocket > 0 )
	{
		int got = recv(this->mySocket, buffer, in_size, 0);

		if ( out_size )
		{
			*out_size = got;
		}

		if ( got > 0 )
		{
			ret = TRUE;
		}
	}

	return ret;
}

BOOL WindowsUdpConnection::Write(const char * buffer, int in_size, int * out_size)
{
	BOOL ret = FALSE;
	
	if ( this->mySocket > 0 )
	{
		int put = send(this->mySocket, buffer, in_size, 0);

		if ( out_size )
		{
			*out_size = put;
		}

		if ( put > 0 )
		{
			ret = TRUE;
		}
	}

	return ret;
}

BOOL WindowsUdpConnection::Close()
{
	BOOL ret = FALSE;

	if ( this->mySocket > 0 )
	{
		closesocket(this->mySocket);
		this->mySocket = 0;
		ret = TRUE;
	}

	return ret;
}

BOOL WindowsUdpConnection::ReadFrom(char * buffer, int in_size, int * out_size, RemoteAddress ** addr)
{
	BOOL ret = FALSE;

	if ( this->mySocket > 0 )
	{
		SOCKADDR_IN sadr;
		int sadrSize = sizeof(sadr);

		memset(&sadr, 0, sizeof(SOCKADDR_IN));
		int got = recvfrom(this->mySocket, buffer, in_size, 0, (SOCKADDR*)&sadr, &sadrSize);

		if ( out_size )
		{
			*out_size = got;
		}

		if ( got > 0 )
		{
			buffer[got-1] = '\0';
			*addr = new WindowsUdpAddress();
			((WindowsUdpAddress*)*addr)->SetAddress(sadr);
			ret = TRUE;
		}
	}

	return ret;
}

BOOL WindowsUdpConnection::WriteTo(const char * buffer, int in_size, int * out_size, RemoteAddress * addr)
{
	BOOL ret = FALSE;
	
	if ( this->mySocket > 0 )
	{
		WindowsUdpAddress *adr = (WindowsUdpAddress*)addr;
		int put = sendto(this->mySocket, buffer, in_size, 0,(SOCKADDR*)adr->GetAddress(), sizeof(SOCKADDR_IN));

		if ( out_size )
		{
			*out_size = put;
		}

		if ( put > 0 )
		{
			ret = TRUE;
		}
	}

	return ret;
}