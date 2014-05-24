#ifndef SEARCH_TRANSACTION_H
#define SEARCH_TRANSACTION_H

#include "OSTypes.h"
#include <map>

class SearchTransaction
{
protected:
	class Process * process;
	DWORD randomTransactionId;
	BYTE width;
	DWORD value;
	std::map<QWORD,QWORD> * searchResults;
public:
	SearchTransaction(class Process * proc, BYTE width);
	virtual ~SearchTransaction();

	virtual void GetProcess(class Process ** proc);
	virtual DWORD GetTransactionId();
	virtual BYTE GetWidth();
	virtual DWORD GetValue();

	virtual void Search(DWORD value);
	virtual void Refine(DWORD value);

	virtual size_t GetNumberOfResults();
	virtual std::map<QWORD, QWORD>::iterator GetIterator();
	virtual std::map<QWORD, QWORD>::iterator GetEndIterator();

};

#endif//SEARCH_TRANSACTION_H