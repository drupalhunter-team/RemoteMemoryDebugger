#ifndef OS_FUNCTIONS_H
#define OS_FUNCTIONS_H

#ifdef WIN32
#include <stdlib.h>
#endif

class OSFunctions
{
public:
	static DWORD GenerateRandomNumber();
};

#ifdef WIN32
DWORD OSFunctions::GenerateRandomNumber()
{
	unsigned int value = 0;
	rand_s(&value);
	return value;
}
#endif

#endif//OS_FUNCTIONS_H