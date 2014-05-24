#ifndef COMMAND_INTERFACE_H
#define COMMAND_INTERFACE_H

class CommandInterface
{
public:

	virtual ~CommandInterface() {};
	virtual void ProcessInput() = 0;
	virtual void ProcessInput(class RemoteAddress * addr, const char * input) = 0;

protected:

	virtual void PrintOut(class RemoteAddress * addr, const char *) = 0;
};

#endif//COMMAND_INTERFACE_H