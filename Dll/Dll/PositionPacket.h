#pragma once


#include"Packet.h"




class MYLIBRARY_API PositionPacket:public Packet,public UserIDComponent
{
public:
	PositionPacket();
	~PositionPacket();


	void SetData(const Position& data);
	const Position & GetData() const;
	char* GetSeriDataBuffer()  override;

	void Serialize() override;
	void DeSerialize() override;
	constexpr unsigned long GetSeriDataSize() { return mSerialDataSize; };
	constexpr unsigned long GetDataSize() { return mDataSize; };
private:
	static constexpr unsigned long mDataSize = 1 + UserIDMaxLength + sizeof(float) * 3;
	static constexpr unsigned long mSerialDataSize = 2 * sizeof(unsigned long) + mDataSize + sizeof(gEndMark);
	char mSeriData[mSerialDataSize];
	Position mData;
};

