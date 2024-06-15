#include "VariableLengthPacket.h"
#include<assert.h>

VariableLengthPacket::VariableLengthPacket():mSeriData(nullptr)
{

}

VariableLengthPacket::~VariableLengthPacket()
{
	delete[] mSeriData;
}

char* VariableLengthPacket::GetSeriDataBuffer()
{
	return mSeriData;
}

unsigned long VariableLengthPacket::GetSeriDataSize() const
{
	return mSerialDataSize;
}

void VariableLengthPacket::CreateSeriBuffer(unsigned long dataLength)
{
	assert(!mSeriData);
	mSerialDataSize = sizeof(unsigned long) * 2 + dataLength + sizeof(gEndMark);
	mSeriData = new char[mSerialDataSize];
	
}
