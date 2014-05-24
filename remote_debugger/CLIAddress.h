#ifndef CLI_ADDRESS_H
#define CLI_ADDRESS_H

#include "RemoteAddress.h"

class CLIAddress : public RemoteAddress
{
public:
	virtual void GetName(char * buffer, size_t buffer_len)
	{
		if ( buffer )
		{
			sprintf_s(buffer, buffer_len, "CLI\0");
		}
	}
};

#endif//CLI_ADDRESS_H