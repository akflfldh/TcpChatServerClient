#include "StringPacket.h"
#include"PacketHelper.h"
#include<assert.h>

StringPacket::StringPacket()
{
}

StringPacket::~StringPacket()
{
}

void StringPacket::SetData(const std::string& data)
{
	mDataSize = 1 + UserIDMaxLength +data.size();
	mData = data;
	CreateSeriBuffer(mDataSize);
}

const std::string& StringPacket::GetData() const
{
	return mData;
	// TODO: 여기에 return 문을 삽입합니다.
}


void StringPacket::Serialize()
{
	unsigned long nextOffset=PacketHelper::Serialize(mSeriData, mDataSize, GetPacketID(), GetUserID(), true);
	
	memcpy(&mSeriData[nextOffset], mData.data(), mData.size());

	nextOffset += mData.size();

	memcpy(&mSeriData[nextOffset],&gEndMark, sizeof(gEndMark));

}

void StringPacket::DeSerialize()
{
	memcpy(&mDataSize, mSeriData, sizeof(unsigned long));

	UserID userID;
	unsigned long nextOffset = PacketHelper::DeSerializeUserID(mSeriData, userID);
	SetUserID(userID);

	//mData.assign(&mSeriData[nextOffset],&mSeriData[nextOffset+ (mDataSize - (1 + UserIDMaxLength))]);
	mData.assign(&mSeriData[nextOffset]);
	unsigned long size = mDataSize - (1 + UserIDMaxLength);
	mData.resize(size);
}

unsigned long StringPacket::GetDataSize() const
{
	return mDataSize;
}
