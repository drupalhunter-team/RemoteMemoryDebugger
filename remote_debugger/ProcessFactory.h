#ifndef PROCESS_FACTORY_H
#define PROCESS_FACTORY_H

#include "WindowsProcess.h"

class ProcessFactory
{
public:

	static ProcessFactory* GetInstance();
	static void FindProcesses(Collection<ProcessId,Process*> * col);
	virtual Process * CreateNewProcess(ProcessId id);
};

static ProcessFactory instance;

ProcessFactory* ProcessFactory::GetInstance()
{
	return &instance;
}

void ProcessFactory::FindProcesses(Collection<ProcessId,Process*> * col)
{
#ifdef WIN32
	WindowsProcess::FindProcesses(col);
#endif
}

Process * ProcessFactory::CreateNewProcess(ProcessId id)
{
#ifdef WIN32
	Process * p = new WindowsProcess();
#endif
	p->Create(id);
	return p;
}

#endif//PROCESS_FACTORY_H