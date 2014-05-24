#include "OSTypes.h"

#include "RemoteDebugger.h"
#include "UDPLi.h"
#include "CLI.h"

int main(int argc, char * argv[])
{
#ifdef _DEBUG
	//_CrtSetBreakAlloc(782);
#endif

	RemoteDebugger * debugger = new RemoteDebugger();
	debugger->FindProcesses();

	CommandInterface * cli = NULL;

	for ( int i = 1; i < argc ; i++ )
	{
		if ( strncmp(argv[i],"-n", 2) == 0 )
		{
			cli = new UDPLi(debugger, 4444);
		}
	}

	if ( cli == NULL )
	{
		cli = new CLI(debugger);
	}

	cli->ProcessInput();
	delete cli;
	cli = NULL;

	delete debugger;
	debugger = NULL;

#ifdef _DEBUG
	if(_CrtDumpMemoryLeaks())
	{
		printf("Memory leaks detected\n");
		_ASSERTE( _CrtCheckMemory( ) );
		printf("Press any key to continue...\n");
		getchar();
	}
#endif

	return 0;
}