#include "RemoteDebugger.h"
#include "ProcessFactory.h"
#include "SearchTransaction.h"

RemoteDebugger::RemoteDebugger()
{
	processes = new Collection<ProcessId,Process*>;
	searches = new Collection<DWORD, SearchTransaction*>;
}

RemoteDebugger::~RemoteDebugger()
{
	this->ClearProcessList();

	Collection<DWORD, SearchTransaction*>::Iterator sit = this->searches->GetIterator();

	while ( !sit.IsEmpty() )
	{
		SearchTransaction * s = sit.Get();
		if ( s )
		{
			delete s;
		}
		sit.Next();
	}

	delete processes;
	processes = NULL;
	delete searches;
	searches = NULL;
}

void RemoteDebugger::ClearProcessList()
{
	Collection<ProcessId, Process*>::Iterator pit = this->processes->GetIterator();

	while ( !pit.IsEmpty() )
	{
		Process * p = pit.Get();
		if ( p )
		{
			delete p;
		}
		pit.Next();
	}
}

void RemoteDebugger::GetProcessCollection(Collection<ProcessId, Process*> ** col)
{
	if ( col )
	{
		*col = this->processes;
	}
}

void RemoteDebugger::GetSearchCollection(Collection<DWORD, SearchTransaction*> ** col)
{
	if ( col )
	{
		*col = this->searches;
	}
}

void RemoteDebugger::FindProcesses(void)
{
	this->ClearProcessList();
	ProcessFactory::FindProcesses(this->processes);
}

BOOL RemoteDebugger::DebugProcess(ProcessId procId)
{
	BOOL ret = 0;

	if ( this->processes->Contains(procId) == TRUE )
	{
		Process * proc = NULL;
		proc = this->processes->GetItem(procId);
		ret = proc->DebugProcess();
	}

	return ret;
}

BOOL RemoteDebugger::StopDebugging(ProcessId procId)
{
	BOOL ret = 0;

	if ( this->processes->Contains(procId) == TRUE )
	{
		Process * proc = NULL;
		proc = this->processes->GetItem(procId);
		ret = proc->StopDebugging();
	}

	return ret;
}

DWORD RemoteDebugger::SearchMemory(ProcessId procId, BYTE width, DWORD value)
{
	DWORD txId = 0;

	if ( this->processes->Contains(procId) == TRUE )
	{
		Process * proc = NULL;
		proc = this->processes->GetItem(procId);
		
		SearchTransaction * st = new SearchTransaction(proc, width);
		st->Search(value);
		txId = st->GetTransactionId();
		searches->AddItem(txId, st);
	}

	return txId;
}

DWORD RemoteDebugger::RefineSearch(DWORD transaction, DWORD value)
{
	DWORD txId = 0;

	if ( this->searches->Contains(transaction) )
	{
		SearchTransaction * st = NULL;
		st = this->searches->GetItem(transaction);
		if ( st )
		{
			st->Refine(value);
		}
		txId = transaction;
	}

	return txId;
}

BOOL RemoteDebugger::GetSearch(DWORD transaction, SearchTransaction ** st)
{
	BOOL ret = FALSE;

	if ( st && this->searches->Contains(transaction) )
	{
		*st = this->searches->GetItem(transaction);

		if ( *st )
		{
			ret = TRUE;
		}
	}

	return ret;
}

BOOL RemoteDebugger::CloseSearch(DWORD transaction)
{
	BOOL ret = FALSE;

	if ( this->searches->Contains(transaction) )
	{
		SearchTransaction * st = NULL;
		st = this->searches->GetItem(transaction);
		if ( st )
		{
			if ( this->searches->Remove(transaction) )
			{
				ret = TRUE;
				delete st;
				st = NULL;
			}
		}
	}

	return ret;
}

BOOL RemoteDebugger::ModifyMemory(
		ProcessId procId, 
		LPVOID address, 
		DWORD width, 
		DWORD value)
{
	BOOL result = FALSE;

	if ( this->processes->Contains(procId) == TRUE )
	{
		Process * proc = NULL;
		proc = this->processes->GetItem(procId);
		
		if ( proc != NULL && proc->IsDebugging() )
		{
			SIZE_T written = 0;
			result = proc->WriteMemory(
				address,
				&value,
				width,
				&written);
		}
	}

	return result;
}
