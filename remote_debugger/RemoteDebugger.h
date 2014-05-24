#ifndef REMOTE_DEBUGGER_H
#define REMOTE_DEBUGGER_H

#include "OSTypes.h"
#include "Collection.h"
#include "Process.h"

class RemoteDebugger
{
public:

	RemoteDebugger();
	virtual ~RemoteDebugger();

	virtual void GetProcessCollection(Collection<ProcessId, class Process*> ** col);
	virtual void GetSearchCollection(Collection<DWORD, class SearchTransaction*> ** col);

	virtual void FindProcesses(void);

	virtual DWORD SearchMemory(ProcessId procId, BYTE width, DWORD value);
	virtual DWORD RefineSearch(DWORD transaction, DWORD value);
	virtual BOOL GetSearch(DWORD transaction, SearchTransaction ** st);
	virtual BOOL CloseSearch(DWORD transaction);

	virtual BOOL DebugProcess(ProcessId procId);
	virtual BOOL StopDebugging(ProcessId procId);

	virtual BOOL ModifyMemory(
		ProcessId procId, 
		LPVOID address, 
		DWORD width, 
		DWORD value);

protected:

	virtual void ClearProcessList();

	class Collection<ProcessId,class Process*> * processes;
	class Collection<DWORD, class SearchTransaction*> * searches;

};

#endif//REMOTE_DEBUGGER_H