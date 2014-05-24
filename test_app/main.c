#include <Windows.h>
#include <stdio.h>

static unsigned int something = 0;
static unsigned int * pSomething = &something;

int main(int argc, char * argv[])
{
	while(TRUE)
	{
		printf("%d\n", something++);
		Sleep(10000);
	}
}