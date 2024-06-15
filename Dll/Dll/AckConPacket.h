#pragma once

#include"Packet.h"

class MYLIBRARY_API AckConPacket:public Packet, public UserIDComponent
{
public:
	AckConPacket()=default;
	~AckConPacket() = default;

	void SetData(const SYSTEMTIME& data);
	const SYSTEMTIME& GetData() const;

	char* GetSeriDataBuffer() override;

	void Serialize() override;
	void DeSerialize() override;
	
	constexpr unsigned long GetSeriDataSize() { return mSerialDataSize; };
	constexpr unsigned long GetDataSize() { return mDataSize; };


private:
	constexpr static unsigned long mDataSize = sizeof(unsigned char) + UserIDMaxLength + 8 * sizeof(WORD);
	constexpr static unsigned long mSerialDataSize = sizeof(unsigned long)*2 + mDataSize + sizeof(gEndMark);
	SYSTEMTIME mData;
	char mSeriData[mSerialDataSize];
};

