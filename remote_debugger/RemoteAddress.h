#ifndef REMOTE_ADDRESS_H
#define REMOTE_ADDRESS_H

class RemoteAddress
{
public:
	virtual void GetName(char * buffer, size_t buffer_len) = 0;
};

#endif//REMOTE_ADDRESS_H