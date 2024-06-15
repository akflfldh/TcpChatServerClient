#include "SynReaderWriterComponent.h"

SynReaderWriterComponent::SynReaderWriterComponent():mReaderCount(0)
{
	mCommonMutex=CreateMutex(nullptr, false, L"");
	mReadCountMutex = CreateMutex(nullptr, false, L"");
	mReadEvent= CreateEvent(nullptr, true, true, L"");
}

SynReaderWriterComponent::~SynReaderWriterComponent()
{
	CloseHandle(mCommonMutex);
	CloseHandle(mReadCountMutex);
	CloseHandle(mReadEvent);
}

void SynReaderWriterComponent::ReadStart()
{
	WaitForSingleObject(mCommonMutex, INFINITE);
	
	WaitForSingleObject(mReadCountMutex,INFINITE);

	mReaderCount++;
	if (mReaderCount == 1)
	{
		ResetEvent(mReadEvent);
	}

	ReleaseMutex(mReadCountMutex);

	ReleaseMutex(mCommonMutex);

}

void SynReaderWriterComponent::ReadEnd()
{
	WaitForSingleObject(mReadCountMutex,INFINITE);
	
	mReaderCount--;

	if (mReaderCount == 0)
	{
		SetEvent(mReadEvent);
	}

	ReleaseMutex(mReadCountMutex);

}

void SynReaderWriterComponent::WriteStart()
{
	WaitForSingleObject(mCommonMutex, INFINITE);
	WaitForSingleObject(mReadEvent,INFINITE);

}

void SynReaderWriterComponent::WriteEnd()
{
	ReleaseMutex(mCommonMutex);
}
