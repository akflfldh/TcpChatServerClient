#pragma once
#include"Packet.h"
class MYLIBRARY_API ReqConPacket: public Packet,public UserIDComponent
{
public:
	ReqConPacket() = default;
	~ReqConPacket() = default;



	char* GetSeriDataBuffer() override;

	constexpr unsigned long GetSeriDataSize() { return mSerialDataSize; };
	constexpr unsigned long GetDataSize() { return mDataSize; };
	
	void Serialize() override;
	void DeSerialize() override;

private:
	static constexpr unsigned long mDataSize = 1 + UserIDMaxLength;
	static constexpr unsigned long mSerialDataSize = 2 * sizeof(unsigned long) + mDataSize + sizeof(gEndMark);
	char mSeriData[mSerialDataSize];
	

};

