#pragma once



#include<Windows.h>
#include"Default.h"

class MYLIBRARY_API SynReaderWriterComponent
{
public:
	SynReaderWriterComponent();
	~SynReaderWriterComponent();

	void ReadStart();
	void ReadEnd();

	void WriteStart();
	void WriteEnd();

private:

	HANDLE mCommonMutex;
	HANDLE mReadCountMutex;
	HANDLE mReadEvent;

	unsigned long mReaderCount;

};

