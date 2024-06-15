#pragma once

#include"Packet.h"

class MYLIBRARY_API VariableLengthPacket:public Packet
{

public:
	VariableLengthPacket();
	virtual ~VariableLengthPacket();

	void CreateSeriBuffer(unsigned long dataLength);


	char* GetSeriDataBuffer()  override;
	unsigned long GetSeriDataSize() const;


	virtual void Serialize() = 0;
	virtual void DeSerialize() = 0;


protected:
	char* mSeriData;
	unsigned long mSerialDataSize;

};

