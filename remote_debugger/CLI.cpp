#include "OSTypes.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "CLI.h"
#include "Version.h"
#include "RemoteDebugger.h"
#include "SearchTransaction.h"
#include "CLIAddress.h"

using std::string;
using std::cin;
using std::cout;

#define PROC_NAME_LEN 255u
#define MAX_LINE_LENGTH 255u
#define DUMP_BYTE_SIZE 1//1024 * 4
#define LINE_LENGTH 10 + 2 + (3 * 16) + 16 + 1

CLI::CLI(RemoteDebugger * dbg)
{
	this->debugger = dbg;
	active = TRUE;
}

CLI::~CLI()
{
	this->debugger = NULL;
}

static char* ExtractNextParameter(char ** buffer)
{
	size_t size = strlen(*buffer);
	size_t p = 0;
	char * text = (*buffer);

	for ( ; p < size ; p++ )
	{
		if ( (*buffer)[p] == ',' )
		{
			(*buffer)[p] = 0;
			*buffer = &(*buffer)[p + 1];
			break;
		}
		if ( p == (size - 1) )
		{
			*buffer = &(*buffer)[p + 1];
			break;
		}
	}

	return text;
}

void CLI::ProcessInput()
{
	this->PrintHelp(NULL);

	char buffer[255];
	
	while(active)
	{
		RemoteAddress  * addr = NULL;
		memset(buffer,0,255);
		this->GetInput(buffer, 255, NULL, &addr);
		this->ProcessInput(addr, buffer);
		if(addr)
		{
			delete addr;
			addr = NULL;
		}
	}
}

void CLI::ProcessInput(RemoteAddress * addr, const char * input)
{
	if (strncmp(input,"exit",4)==0)
	{
		active = FALSE;
	}
	else if (strncmp(input,"help",4)==0)
	{
		this->PrintHelp(addr);
	}
	else if (strncmp(input,"lpd",3)==0)
	{
		this->ListProcesses(addr, FALSE, FALSE);
	}
	else if (strncmp(input,"fp",2)==0)
	{
		this->FindProcesses(addr);
	}
	else if (strncmp(input,"lp",2)==0)
	{
		this->ListProcesses(addr, TRUE, FALSE);
	}
	else if (strncmp(input,"dld",4)==0)
	{
		this->ListProcesses(addr, FALSE, TRUE);
	}
	else if (strncmp(input,"dl",2)==0)
	{
		this->ListProcesses(addr, TRUE, TRUE);
	}
	else if (strncmp(input,"di",2)==0)
	{
		char * c = (char*)(input + 2);
		int pid = -1;
		pid = atoi(c);
		this->ProcessInformation(addr, pid);
	}
	else if (strncmp(input,"lsr",3)==0)
	{
		char * c = (char*)(input + 3);
		int pid = -1;
		pid = atoi(c);
		this->ListSearch(addr, pid);
	}
	else if (strncmp(input,"ls",2)==0)
	{
		this->ListSearches(addr);
	}
	else if (strncmp(input,"ms",2)==0)
	{
		char * c = (char*)(input + 2);
		int pid = -1;
		pid = atoi(c);
		this->MonitorSearch(addr, pid);
	}
	else if (strncmp(input,"mm",2)==0)
	{
		char * c = (char*)(input + 2);
		this->ParseModifyMemory(addr, c);
	}
	else if (strncmp(input,"dp",2)==0)
	{
		char * c = (char*)(input + 2);
		int pid = -1;
		pid = atoi(c);
		this->DebugProcess(addr, pid);
	}
	else if (strncmp(input,"sd",2)==0)
	{
		char * c = (char*)(input + 2);
		int pid = -1;
		pid = atoi(c);
		this->StopDebugging(addr, pid);
	}
	else if (strncmp(input,"dm",2)==0)
	{
		char * c = (char*)(input + 2);
		int pid = -1;
		pid = atoi(c);
		this->DumpMemory(addr, pid);
	}
	else if (strncmp(input,"sm",2)==0)
	{
		char * c = (char*)(input + 2);
		this->ParseSearchMemory(addr, c);
	}
	else if (strncmp(input,"rs",2)==0)
	{
		char * c = (char*)(input + 2);
		this->ParseRefineSearch(addr, c);
	}
	else if (strncmp(input,"cs",2)==0)
	{
		char * c = (char*)(input + 2);
		DWORD txId = 0;
		txId = (DWORD)_atoi64(c);
		BOOL ret = this->debugger->CloseSearch(txId);
		
		char msg[MAX_LINE_LENGTH] = {0};
		sprintf_s(msg,MAX_LINE_LENGTH, "Search %d closed: %d\n", txId, ret);
		this->PrintOut(addr, msg);
	}
}

void CLI::FindProcesses(class RemoteAddress * addr)
{
	this->debugger->FindProcesses();
	this->PrintOut(addr, "Done\n");
}

void CLI::ListProcesses(RemoteAddress * addr, BOOL list_names_only, BOOL details)
{
	Collection<ProcessId, Process*> * processes = NULL;
	debugger->GetProcessCollection(&processes);
	Collection<ProcessId, Process*>::Iterator pit = processes->GetIterator();

	this->PrintOut(addr, ">> Process List\n");

	while ( !pit.IsEmpty() )
	{
		Process* itp = pit.Get();
		WCHAR name[PROC_NAME_LEN] = {0};
		WCHAR filename[PROC_NAME_LEN] = {0};
		ProcessInfo info;

		itp->GetProcessImageName(name,PROC_NAME_LEN);
		itp->GetProcessFilename(filename,PROC_NAME_LEN);
		itp->GetProcessInformation(&info);

		if ( wcslen(name) > 0 || !list_names_only)
		{
			WCHAR msg[MAX_LINE_LENGTH] = {0};
			wsprintf(msg, L"Process %d, %s, %s\n", itp->GetProcessId(), name, filename);
			this->PrintOut(addr, msg);
			if ( details == TRUE )
			{
				char msg[MAX_LINE_LENGTH] = {0};
				sprintf_s(msg,MAX_LINE_LENGTH,"\tSizeOfImage:\t\t%08X\n\0", info.GetSizeOfImage());
				this->PrintOut(addr, msg);
				sprintf_s(msg,MAX_LINE_LENGTH,"\tEntryPoint:\t\t%08X\n\0", (QWORD)info.GetEntryPoint());
				this->PrintOut(addr, msg);
				sprintf_s(msg,MAX_LINE_LENGTH,"\tBaseAddressOfDLL:\t%08X\n\0", (QWORD)info.GetBaseAddress());
				this->PrintOut(addr, msg);
				sprintf_s(msg,MAX_LINE_LENGTH,"\tWorking Memory:\t\t%08X\n\0", info.GetWorkingSize());
				this->PrintOut(addr, msg);
				sprintf_s(msg,MAX_LINE_LENGTH,"\tPage File Usage:\t%08X\n\0", info.GetPageFileUsage());
				this->PrintOut(addr, msg);
			}
		}
		pit.Next();
	}
}

void CLI::ListSearches(RemoteAddress * addr)
{
	Collection<DWORD, SearchTransaction*> * searches = NULL;
	debugger->GetSearchCollection(&searches);
	Collection<DWORD, SearchTransaction*>::Iterator sit = searches->GetIterator();

	this->PrintOut(addr, ">> Search List\n");
	this->PrintOut(addr, "Search ID\tProcess\t\tWidth\t\tValue\t\tResults\n");
	while ( !sit.IsEmpty() )
	{
		SearchTransaction* st = sit.Get();
		if ( st )
		{
			Process * proc = NULL;
			st->GetProcess(&proc);
			if ( proc )
			{
				WCHAR name[PROC_NAME_LEN] = {0};
				proc->GetProcessImageName(name,PROC_NAME_LEN);

				WCHAR msg[MAX_LINE_LENGTH] = {0};
				wsprintf(msg,L"%d\t\t%s\t%d bytes\t\t%d\t\t%d results\n",
					st->GetTransactionId(),
					name,
					st->GetWidth(),
					st->GetValue(),
					st->GetNumberOfResults());
				this->PrintOut(addr, msg);
			}
		}
		sit.Next();
	}
}

void CLI::ListSearch(RemoteAddress * addr, DWORD id)
{
	Collection<DWORD, SearchTransaction*> * searches = NULL;
	debugger->GetSearchCollection(&searches);

	if ( searches->Contains(id) )
	{
		SearchTransaction * st = NULL;
		st = searches->GetItem(id);

		if ( st != NULL )
		{
			char msg[MAX_LINE_LENGTH] = {0};
			sprintf_s(msg, MAX_LINE_LENGTH, "Search %d results\n", id);
			this->PrintOut(addr, msg);
			std::map<QWORD,QWORD>::const_iterator it;

			for (it = st->GetIterator() ; it != st->GetEndIterator() ; ++it )
			{
				QWORD address = (*it).second;
				sprintf_s(msg, MAX_LINE_LENGTH, "\t%08X\n", address);
				this->PrintOut(addr, msg);
			}
		}
	}
}

void CLI::MonitorSearch(class RemoteAddress * addr, DWORD id)
{
	Collection<DWORD, SearchTransaction*> * searches = NULL;
	debugger->GetSearchCollection(&searches);

	if ( searches->Contains(id) )
	{
		SearchTransaction * st = NULL;
		st = searches->GetItem(id);

		if ( st != NULL )
		{
			BOOL active = TRUE;
			while ( active )
			{
				Process * proc = NULL;
				std::map<QWORD,QWORD>::const_iterator it;

				st->GetProcess(&proc);

				if ( proc )
				{
					for ( it = st->GetIterator() ; it != st->GetEndIterator() ; ++it )
					{
						QWORD address = (*it).second;
						DWORD value = 0;
						SIZE_T bytesRead = 0;

						BOOL result = proc->ReadMemory(
							(LPCVOID)address,
							&value,
							sizeof(DWORD),
							&bytesRead);

						if ( result == TRUE )
						{
							char msg[255] = {0};
							sprintf_s(msg, 255, "\t0x%08X = 0x%08X (%d)\n", (DWORD)address, (DWORD)value, value);
							this->PrintOut(addr, msg);
						}
					}
				}
				char in[2] = {0,0};
				this->PrintOut(addr,"x to exit, others to contine\n");
				this->GetInput(in,2,NULL,&addr);
				if ( in[0] == 'x' )
				{
					active = FALSE;
				}
			}
			this->PrintOut(addr, "Monitor terminated\n");
		}
	}
	else
	{
		this->PrintOut(addr, "Unknown search transaction\n");
	}
}

void CLI::ProcessInformation(RemoteAddress * addr, ProcessId procId)
{
	Collection<ProcessId, Process*> * processes = NULL;
	Process * proc = NULL;

	debugger->GetProcessCollection(&processes);
	proc = processes->GetItem(procId);

	if ( proc != NULL )
	{
		WCHAR name[PROC_NAME_LEN] = {0};
		WCHAR filename[PROC_NAME_LEN] = {0};
		ProcessInfo info;

		proc->GetProcessImageName(name,PROC_NAME_LEN);
		proc->GetProcessFilename(filename,PROC_NAME_LEN);
		proc->GetProcessInformation(&info);

		WCHAR msg[MAX_LINE_LENGTH] = {0};
		wsprintf(msg, L"Process %d, %s, %s\n", proc->GetProcessId(), name, filename);
		this->PrintOut(addr, msg);
	
		wsprintf(msg,L"\tSizeOfImage:\t\t%08X\n\0", info.GetSizeOfImage());
		this->PrintOut(addr, msg);
		wsprintf(msg,L"\tEntryPoint:\t\t%08X\n\0", (QWORD)info.GetEntryPoint());
		this->PrintOut(addr, msg);
		wsprintf(msg,L"\tBaseAddressOfDLL:\t%08X\n\0", (QWORD)info.GetBaseAddress());
		this->PrintOut(addr, msg);
		wsprintf(msg,L"\tWorking Memory:\t\t%08X\n\0", info.GetWorkingSize());
		this->PrintOut(addr, msg);
		wsprintf(msg,L"\tPage File Usage:\t%08X\n\0", info.GetPageFileUsage());
		this->PrintOut(addr, msg);
	}
}

void CLI::DebugProcess(RemoteAddress * addr, ProcessId procId)
{
	BOOL result = FALSE;
	result = this->debugger->DebugProcess(procId);
	char msg[MAX_LINE_LENGTH] = {0};
	sprintf_s(msg,MAX_LINE_LENGTH,">> Debugging process %d returned: %d\n", procId, result);
	this->PrintOut(addr, msg);
}

void CLI::StopDebugging(RemoteAddress * addr, ProcessId procId)
{
	BOOL result = FALSE;
	result = this->debugger->StopDebugging(procId);
	char msg[MAX_LINE_LENGTH] = {0};
	sprintf_s(msg,MAX_LINE_LENGTH,">> Stopping debugging process %d returned: %d\n", procId, result);
	this->PrintOut(addr, msg);
}

void CLI::DumpMemory(RemoteAddress * addr,ProcessId procId)
{
	BOOL result = FALSE;

	Collection<ProcessId, Process*> * processes = NULL;
	debugger->GetProcessCollection(&processes);

	if ( processes->Contains(procId) == TRUE )
	{
		Process * proc = NULL;
		proc = processes->GetItem(procId);
		
		if ( proc != NULL && proc->IsDebugging() )
		{
			SIZE_T bytesRead = DUMP_BYTE_SIZE;
			unsigned char buffer[DUMP_BYTE_SIZE];
			QWORD address = 0x0;
			ProcessInfo procInfo;

			proc->GetProcessInformation(&procInfo);

			memset(buffer,0,DUMP_BYTE_SIZE);
			result = TRUE;
			SIZE_T position = 0;

			unsigned char dataBuffer[16];
			unsigned char dataPosition = 0;
			char lineBuffer[LINE_LENGTH];
			memset(lineBuffer,0,LINE_LENGTH);
			_snprintf_s(lineBuffer, LINE_LENGTH, "0x%08X: ", address);

			while( address < procInfo.GetWorkingSize() )
			{
				result = proc->ReadMemory(
					(LPCVOID)address,
					buffer,
					DUMP_BYTE_SIZE,
					&bytesRead);

				for ( SIZE_T d = 0 ; d < DUMP_BYTE_SIZE ; d++ )
				{
					dataBuffer[dataPosition++] = buffer[d];
					if ( result == TRUE)
					{
						char buf[4] = {0,0,0,0};
						_snprintf_s(buf,4,"%02X ", buffer[d]);
						strncat_s(lineBuffer, buf,3);
					}
					else
					{
						strncat_s(lineBuffer, "?? ",3);
					}
					if (position != 0 && (position % 16 == 0) )
					{
						this->PrintOut(addr, lineBuffer);
						for(int i = 0; i < 16; i++)
						{
							if ( dataBuffer[i] >= 32 && dataBuffer[i] <= 126)
							{
								char msg[3] = {0};
								sprintf_s(msg, 3, "%c", dataBuffer[i]);
								this->PrintOut(addr, msg);
							}
							else
							{
								this->PrintOut(addr, ".");
							}
						}
						dataPosition = 0;
						this->PrintOut(addr, "\n");
						memset(lineBuffer, 0, LINE_LENGTH);
						_snprintf_s(lineBuffer, LINE_LENGTH, "0x%08X: ", address);
					}
					position++;
				}

				(address+=DUMP_BYTE_SIZE);

				result = TRUE;
			}
			this->PrintOut(addr, "\n");
		}
	}

	char msg[MAX_LINE_LENGTH] = {0};
	sprintf_s(msg,MAX_LINE_LENGTH,">> Memory dump for process %d returned: %d\n", procId, result);
	this->PrintOut(addr,msg);
}

void CLI::PrintHeader(RemoteAddress * addr)
{
	this->PrintOut(addr, ">> Remote Debugger\n");
	char msg[255] = {0};
	sprintf_s(msg,255,"Version: %s\n", RD_VERSION);
	this->PrintOut(addr, msg);
	this->PrintOut(addr, "\n");
	this->PrintOut(addr, ">> Command Line Interface\n");
}

void CLI::PrintHelp(RemoteAddress * addr)
{
	this->PrintOut(addr, "\n");
	this->PrintHeader(addr);
	this->PrintOut(addr,"\n");
	this->PrintOut(addr,"help\t\tShow this dialog\n");
	this->PrintOut(addr,"\n");
	this->PrintOut(addr,">> Information\n");
	this->PrintOut(addr,"fp\t\tFind Processes, builds the list up\n");
	this->PrintOut(addr,"lp\t\tList Processes\n");
	this->PrintOut(addr,"lpd\t\tList Processes (including unknown ones)\n");
	this->PrintOut(addr,"dl\t\tDetailed list of Processes\n");
	this->PrintOut(addr,"dld\t\tDetailed list of Processes (inc. unknown)\n");
	this->PrintOut(addr,"di<n>\t\tDetailed information for Process <n>\n");

	this->PrintOut(addr,"\n");
	this->PrintOut(addr,">> Debugging commands\n");
	this->PrintOut(addr,"dp<n>\t\tDebug Process Id <n>\n");
	this->PrintOut(addr,"sd<n>\t\tStop debugging Process Id <n>\n");
	this->PrintOut(addr,"dm<n>\t\tDump all the memory for Process Id <n>\n");

	this->PrintOut(addr,"\n");
	this->PrintOut(addr,">> Search commands\n");
	this->PrintOut(addr,"ls\t\tList searches\n");
	this->PrintOut(addr,"lsr<s>\t\tList search results for search <s>\n");
	this->PrintOut(addr,"sm<p>,<w>,<v>\tSearch memory of Process <p> with width <w> for value <v>\n");
	this->PrintOut(addr,"rs<t>,<v>\tRefine search transaction <t> for value <v>\n");
	this->PrintOut(addr,"ms<s>\t\tMonitor a search results values\n");
	this->PrintOut(addr,"cs\t\tClose search\n");

	this->PrintOut(addr,"mm<p>,<a>,<w>,<v>\tModify memory of process <p>, address <a> of width <w> with value <v>\n");

	this->PrintOut(addr,"\n");
}

void CLI::ParseSearchMemory(RemoteAddress * addr, char* cmd)
{
	DWORD txId = 0;
	DWORD procId = 0;
	BYTE width = 0;
	DWORD value = 0;

	if ( cmd ) 
	{
		size_t size = strlen(cmd);
		size_t p = 0;
		for ( ; p < size ; p++ )
		{
			if ( cmd[p] == ',' )
			{
				cmd[p] = 0;
				procId = atoi(cmd);
				p++;
				cmd = cmd + p;
				break;
			}
		}
		size = strlen(cmd);
		for ( p = 0; p < size ; p++ )
		{
			if ( cmd[p] == ',' )
			{
				cmd[p] = 0;
				width = (BYTE)atoi(cmd);
				p++;
				cmd = cmd + p;
				break;
			}
		}
		size = strlen(cmd);
		for ( p = 0 ; p < size ; p++ )
		{
			if ( cmd[p] == ',' || p == (size-1) )
			{
				value = atoi(cmd);
				break;
			}
		}
	}

	txId = debugger->SearchMemory(procId, width, value);

	char msg[MAX_LINE_LENGTH] = {0};
	sprintf_s(msg, MAX_LINE_LENGTH, "Search TxId: %d\n", txId);
	this->PrintOut(addr, msg);

	if ( txId > 0 )
	{
		SearchTransaction * st = NULL;
		if ( debugger->GetSearch(txId, &st) == TRUE )
		{
			sprintf_s(msg, MAX_LINE_LENGTH, "Search has %d results\n", st->GetNumberOfResults());
			this->PrintOut(addr, msg);
		}
	}
}

void CLI::ParseModifyMemory(RemoteAddress * addr, char* cmd)
{
	BOOL ret = FALSE;
	DWORD procId = 0;
	LPVOID address = 0;
	DWORD width = 0;
	DWORD value = 0;

	char* tProc = ExtractNextParameter(&cmd);
	char* tAddr = ExtractNextParameter(&cmd);
	char* tWidth = ExtractNextParameter(&cmd);
	char* tValue = ExtractNextParameter(&cmd);

	procId = (DWORD)_atoi64(tProc);
	address = (LPVOID)_atoi64(tAddr);
	width = (DWORD)_atoi64(tWidth);
	value = (DWORD)_atoi64(tValue);

	ret = debugger->ModifyMemory(procId,address,width,value);

	if ( ret == TRUE )
	{
		char msg[MAX_LINE_LENGTH] = {0};
		sprintf_s(msg, MAX_LINE_LENGTH, "Memory modified OK: 0x%08X to 0x%08X\n", (DWORD)_atoi64(tAddr), value);
		this->PrintOut(addr, msg);
	}
	else
	{
		this->PrintOut(addr, "Unable to modify memory\n");
	}
}

void CLI::ParseRefineSearch(RemoteAddress * addr, char* cmd)
{
	DWORD txId = 0;
	DWORD value = 0;

	if ( cmd ) 
	{
		size_t size = strlen(cmd);
		size_t p = 0;
		for ( ; p < size ; p++ )
		{
			if ( cmd[p] == ',' )
			{
				cmd[p] = 0;
				txId = (DWORD)_atoi64(cmd);
				p++;
				cmd = cmd + p;
				break;
			}
		}
		size = strlen(cmd);
		for ( p = 0 ; p < size ; p++ )
		{
			if ( cmd[p] == ',' || p == (size-1) )
			{
				value = atoi(cmd);
				break;
			}
		}
	}

	txId = debugger->RefineSearch(txId, value);

	if ( txId > 0 )
	{
		SearchTransaction * st = NULL;
		if ( debugger->GetSearch(txId, &st) == TRUE )
		{
			char msg[MAX_LINE_LENGTH] = {0};
			sprintf_s(msg, MAX_LINE_LENGTH, "Search has %d results\n", st->GetNumberOfResults());
			this->PrintOut(addr, msg);
		}
	}
}

void CLI::PrintOut(RemoteAddress * addr, const char * details)
{
	char name[255] = {0};
	if ( addr ) 
	{
		addr->GetName(name, 255);
	}
	else
	{
		sprintf_s(name, 255, "?");
	}

	if ( details )
	{
		cout << name << ": " << details;
	}
}

void CLI::PrintOut(RemoteAddress * addr, const WCHAR * details)
{
	char cstr[MAX_LINE_LENGTH] = {0};
	size_t length = 0;
	wcstombs_s(&length,cstr,MAX_LINE_LENGTH,details,sizeof(WCHAR)*wcslen(details));
	this->PrintOut(addr, cstr);
}

void CLI::GetInput(char * buffer, size_t buffer_len, size_t * len, RemoteAddress ** addr)
{
	if ( buffer )
	{
		string line;
		getline(cin,line);
		
		strncpy_s(buffer,buffer_len,line.c_str(),line.length());

		if ( len )
		{
			*len = strlen(buffer);
		}
	}

	if ( addr )
	{
		*addr = new CLIAddress();
	}
}