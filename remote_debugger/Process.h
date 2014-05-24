#ifndef PROCESS_H
#define PROCESS_H

#include "OSTypes.h"

typedef DWORD ProcessId;

class Process
{
public:

	virtual void Create(ProcessId id) = 0;
	virtual ~Process() {}

	virtual ProcessId GetProcessId() = 0;
	virtual void GetProcessImageName(WCHAR * name, int len) = 0;
	virtual void GetProcessFilename(WCHAR * name, int len) = 0;
	virtual void GetProcessInformation(class ProcessInfo * info) = 0;

	virtual BOOL DebugProcess() = 0;
	virtual BOOL IsDebugging() = 0;
	virtual BOOL StopDebugging() = 0;
	
	virtual BOOL ReadMemory(LPCVOID address, LPVOID buffer, SIZE_T size, SIZE_T * size_read) = 0;
	virtual BOOL WriteMemory(LPVOID address, LPCVOID buffer, SIZE_T size, SIZE_T * size_written) = 0;
};

class ProcessInfo
{
protected:
	LPVOID baseAddressOfDll;
    DWORD  sizeOfImage;
    LPVOID entryPoint;
	SIZE_T workingSize;
	SIZE_T pageFileUsage;
public:
	ProcessInfo()
	{
		this->baseAddressOfDll = 0;
		this->sizeOfImage = 0;
		this->entryPoint = 0;
		this->workingSize = 0;
		this->pageFileUsage = 0;
	}

	ProcessInfo(
		LPVOID base, 
		DWORD size, 
		LPVOID entry,
		SIZE_T working_size,
		SIZE_T page_file_usage)
	{
		this->baseAddressOfDll = base;
		this->sizeOfImage = size;
		this->entryPoint = entry;
		this->workingSize = working_size;
		this->pageFileUsage = page_file_usage;
	}

	LPVOID GetBaseAddress()
	{
		return this->baseAddressOfDll;
	}

	void SetBaseAddress(LPVOID base)
	{
		this->baseAddressOfDll = base;
	}

	DWORD GetSizeOfImage()
	{
		return this->sizeOfImage;
	}

	void SetSizeOfImage(DWORD size)
	{
		this->sizeOfImage = size;
	}

	LPVOID GetEntryPoint()
	{
		return this->entryPoint;
	}

	void SetEntryPoint(LPVOID entry)
	{
		this->entryPoint = entry;
	}

	SIZE_T GetWorkingSize()
	{
		return this->workingSize;
	}

	void SetWorkingSize(SIZE_T wksize)
	{
		this->workingSize = wksize;
	}

	SIZE_T GetPageFileUsage()
	{
		return this->pageFileUsage;
	}

	void SetPageFileUsage(SIZE_T pkfu)
	{
		this->pageFileUsage = pkfu;
	}
};

#endif//PROCESS_H