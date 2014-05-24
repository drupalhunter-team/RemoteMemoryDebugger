#ifndef CLI_H
#define CLI_H

#include "CommandInterface.h"
#include "Process.h"

class CLI : public CommandInterface
{
public:
	CLI(class RemoteDebugger * dbg);
	virtual ~CLI();

	virtual void ProcessInput();
	virtual void ProcessInput(class RemoteAddress * addr, const char * input);

protected:

	virtual void GetInput(
		char * buffer, 
		size_t buffer_len,
		size_t * len,
		class RemoteAddress ** addr);

	virtual void PrintOut(class RemoteAddress * addr, const char *);
	virtual void PrintOut(class RemoteAddress * addr, const WCHAR *);

	virtual void PrintHeader(class RemoteAddress * addr);
	virtual void PrintHelp(class RemoteAddress * addr);

	virtual void FindProcesses(class RemoteAddress * addr);
	virtual void ListProcesses(class RemoteAddress * addr, BOOL list_names_only, BOOL details);
	virtual void ListSearches(class RemoteAddress * addr);
	virtual void ListSearch(class RemoteAddress * addr, DWORD id);
	virtual void MonitorSearch(class RemoteAddress * addr, DWORD id);

	virtual void ProcessInformation(class RemoteAddress * addr, ProcessId procId);

	virtual void ParseSearchMemory(class RemoteAddress * addr, char* cmd);
	virtual void ParseRefineSearch(class RemoteAddress * addr, char* cmd);
	virtual void ParseModifyMemory(class RemoteAddress * addr, char* cmd);

	virtual void DebugProcess(class RemoteAddress * addr, ProcessId procId);
	virtual void StopDebugging(class RemoteAddress * addr, ProcessId procId);

	virtual void DumpMemory(class RemoteAddress * addr, ProcessId procId);

	class RemoteDebugger * debugger;
	BOOL active;
};

#endif//CLI_H