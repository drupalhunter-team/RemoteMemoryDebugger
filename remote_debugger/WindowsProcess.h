#ifndef WINDOWS_PROCESS_H
#define WINDOWS_PROCESS_H

#include "OSTypes.h"
#include "Process.h"
#include "Collection.h"

typedef DWORD ProcessId;

class WindowsProcess : public Process
{
public:

	WindowsProcess();
	virtual ~WindowsProcess();

	virtual void Create(ProcessId id);

	static void FindProcesses(Collection<ProcessId,Process*> * col);

	virtual ProcessId GetProcessId();
	virtual void GetProcessImageName(WCHAR * name, int len);
	virtual void GetProcessFilename(WCHAR * name, int len);
	virtual void GetProcessInformation(class ProcessInfo * info);

	virtual BOOL DebugProcess();
	virtual BOOL IsDebugging();
	virtual BOOL StopDebugging();
	
	virtual BOOL ReadMemory(LPCVOID address, LPVOID buffer, SIZE_T size, SIZE_T * size_read);
	virtual BOOL WriteMemory(LPVOID address, LPCVOID buffer, SIZE_T size, SIZE_T * size_written);

protected:

	ProcessId myProcessId;
	HANDLE processHandle;
};

#endif//PROCESS_H