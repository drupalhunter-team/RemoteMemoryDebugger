#include "SearchTransaction.h"
#include "OSFunctions.h"
#include "Process.h"

#include <list>

SearchTransaction::SearchTransaction(Process * proc, BYTE width)
{
	this->process = proc;
	// limit searches to 16bits
	this->randomTransactionId = (OSFunctions::GenerateRandomNumber() % 0xFFFF);
	this->width = width;
	this->value = 0;
	this->searchResults = new std::map<QWORD,QWORD>;
}

SearchTransaction::~SearchTransaction()
{
	this->process = NULL;
	if ( this->searchResults )
	{
		this->searchResults->clear();
		delete this->searchResults;
		this->searchResults = NULL;
	}
}

void SearchTransaction::GetProcess(class Process ** proc)
{
	if ( proc )
	{
		*proc = this->process;
	}
}

DWORD SearchTransaction::GetTransactionId()
{
	return this->randomTransactionId;
}

BYTE SearchTransaction::GetWidth()
{
	return this->width;
}

DWORD SearchTransaction::GetValue()
{
	return this->value;
}

void SearchTransaction::Search(DWORD value)
{
	ProcessInfo info;

	this->searchResults->clear();

	this->value = value;
	
	if ( this->process )
	{
		this->process->GetProcessInformation(&info);

		SIZE_T bytesRead = 0;
		DWORD buffer = 0;
		QWORD address = 0x0;
		BOOL result = TRUE;

		while( address < info.GetWorkingSize() )
		{
			buffer = 0;
			result = this->process->ReadMemory(
				(LPCVOID)address,
				&buffer,
				this->width,
				&bytesRead);

			for ( SIZE_T d = 0 ; d < bytesRead ; d++ )
			{
				if ( value == buffer 
					&& this->searchResults->find(address) == this->searchResults->end() )
				{
					this->searchResults->insert( std::pair<QWORD,QWORD>(address,address) );
				}
			}

			address+=this->width;
		}
	}
}

void SearchTransaction::Refine(DWORD value)
{
	if ( this->process )
	{
		SIZE_T bytesRead = 0;
		DWORD buffer = 0;
		QWORD address = 0x0;
		BOOL result = TRUE;

		this->value = value;

		std::list<QWORD> remove;
		std::list<QWORD>::iterator lit;
		std::map<QWORD,QWORD>::iterator mit;
		for ( mit = this->searchResults->begin() ; mit != this->searchResults->end() ; ++mit )
		{
			address = (*mit).second;

			result = this->process->ReadMemory(
				(LPCVOID)address,
				&buffer,
				this->width,
				&bytesRead);

			if ( result == TRUE && bytesRead == this->width)
			{
				if ( buffer != value )
				{
					remove.push_back(address);
				}
			}
		}
		for ( lit = remove.begin() ; lit != remove.end() ; ++lit )
		{
			this->searchResults->erase(*lit);
		}
		remove.clear();
	}
}

size_t SearchTransaction::GetNumberOfResults()
{
	return this->searchResults->size();
}

std::map<QWORD,QWORD>::iterator SearchTransaction::GetIterator()
{
	return this->searchResults->begin();	
}

std::map<QWORD,QWORD>::iterator SearchTransaction::GetEndIterator()
{
	return this->searchResults->end();	
}