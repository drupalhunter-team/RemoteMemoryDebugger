#include "WindowsProcess.h"
#include "OSTypes.h"
#include <psapi.h>

BOOL EnablePrivilage(WCHAR * name)
{
	BOOL ret = FALSE;

	HANDLE tokenHandle = NULL;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &tokenHandle);
	if ( tokenHandle != NULL )
	{
		LUID security;
		if ( LookupPrivilegeValue(NULL, name, &security) == TRUE )
		{
			TOKEN_PRIVILEGES newPriv;
			newPriv.PrivilegeCount = 1;
			newPriv.Privileges[0].Luid = security;
			newPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if ( AdjustTokenPrivileges(tokenHandle, FALSE, &newPriv, sizeof(newPriv), NULL, NULL) == TRUE )
			{
				ret = TRUE;
			}
		}
		CloseHandle(tokenHandle);
	}
	
	return ret;
}

WindowsProcess::WindowsProcess()
{
	this->myProcessId = 0;
	this->processHandle = 0;
}

WindowsProcess::~WindowsProcess()
{
	this->myProcessId = 0;
	this->processHandle = 0;
}

void WindowsProcess::Create(ProcessId id) 
{
	this->myProcessId = id;
}

ProcessId WindowsProcess::GetProcessId()
{
	return this->myProcessId;
}

void WindowsProcess::GetProcessImageName(WCHAR * name, int len)
{
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ | PROCESS_VM_WRITE,
								   FALSE, 
								   this->myProcessId );

	if (NULL != hProcess )
    {
        HMODULE hMod;
        DWORD cbNeeded;

        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
        {
            GetModuleBaseName( hProcess, hMod, name, len );
        }

		CloseHandle( hProcess );
    }
}

void WindowsProcess::GetProcessFilename(WCHAR * name, int len)
{
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ,
								   FALSE, 
								   this->myProcessId );

	if (NULL != hProcess )
    {
        HMODULE hMod = NULL;
        DWORD cbNeeded = 0;

        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
        {
			GetModuleFileName(hMod, name, len);
        }

		CloseHandle( hProcess );
    }
}

void WindowsProcess::GetProcessInformation(class ProcessInfo * p_info)
{
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ,
								   FALSE, 
								   this->myProcessId );

	if (NULL != hProcess )
    {
        HMODULE hMod;
        DWORD cbNeeded;

        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
        {
			if ( p_info )
			{
				MODULEINFO info;

				GetModuleInformation(
					hProcess,
					hMod,
					&info,
					sizeof(info));

				p_info->SetEntryPoint(info.EntryPoint);
				p_info->SetSizeOfImage(info.SizeOfImage);
				p_info->SetBaseAddress(info.lpBaseOfDll);

				PROCESS_MEMORY_COUNTERS counters;
				GetProcessMemoryInfo(
					hProcess,
					&counters,
					sizeof(counters));

				p_info->SetWorkingSize(counters.WorkingSetSize);
				p_info->SetPageFileUsage(counters.PagefileUsage);

			}
        }

		CloseHandle( hProcess );
    }
}

BOOL WindowsProcess::DebugProcess()
{
	HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS,
								   FALSE, 
								   this->myProcessId );
	BOOL ret = FALSE;

	if (NULL != hProcess )
    {
		ret = TRUE;
        this->processHandle = hProcess;
    }
	else
	{
		this->processHandle = 0;
	}

	if ( ret == TRUE )
	{
		ret = EnablePrivilage(SE_DEBUG_NAME);
		if ( ret == TRUE )
		{
			ret = EnablePrivilage(SE_SECURITY_NAME);
		}
	}

	return ret;
}

BOOL WindowsProcess::IsDebugging()
{
	return this->processHandle != 0;
}

BOOL WindowsProcess::StopDebugging()
{
	BOOL ret = FALSE;

	if ( this->processHandle != 0 )
	{
		CloseHandle( this->processHandle );
		this->processHandle = 0;
		ret = TRUE;
	}

	return ret;
}

BOOL WindowsProcess::ReadMemory(LPCVOID address, LPVOID buffer, SIZE_T size, SIZE_T * size_read)
{
	BOOL ret = FALSE;

	if ( this->processHandle != 0 )
	{
		ret = ReadProcessMemory(
			this->processHandle,
			address,
			buffer,
			size,
			size_read);
	}

	return ret;
}

BOOL WindowsProcess::WriteMemory(LPVOID address, LPCVOID buffer, SIZE_T size, SIZE_T * size_written)
{
	BOOL ret = FALSE;

	if ( this->processHandle != 0 )
	{
		ret = WriteProcessMemory(
			this->processHandle,
			address,
			buffer,
			size,
			size_written);

		if ( ret == FALSE )
		{
			DWORD err = GetLastError();
			LPVOID lpMsgBuf;

			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				err,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR) &lpMsgBuf,
				0, NULL );

			wprintf(L"Memory Modification error for Process %d: %s\n", this->GetProcessId(), lpMsgBuf);

			LocalFree(lpMsgBuf);
		}
	}

	return ret;
}

void WindowsProcess::FindProcesses(Collection<ProcessId,Process*> * col)
{
	DWORD aProcesses[1024], cbNeeded = 0, cProcesses = 0;
	memset(&aProcesses,0,sizeof(DWORD)*1024);
	if ( !EnumProcesses(
		aProcesses,
		sizeof(aProcesses),
		&cbNeeded))
	{
		return;
	}
	cProcesses = cbNeeded / sizeof(DWORD);

	DWORD p = 0;
	for ( ; p < cProcesses ; p++ )
	{
		DWORD procId = aProcesses[p];
		Process * proc = new WindowsProcess();
		proc->Create(procId);
		col->AddItem(procId, proc);
	}
}