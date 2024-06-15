#include "AckConPacket.h"

#include"PacketHelper.h"

void AckConPacket::SetData(const SYSTEMTIME& data)
{
    mData = data;
}

const SYSTEMTIME& AckConPacket::GetData() const
{

    return mData;
    // TODO: 여기에 return 문을 삽입합니다.
}

char* AckConPacket::GetSeriDataBuffer() 
{
    return mSeriData;
}

void AckConPacket::Serialize()
{
	
	unsigned long nextOffset = PacketHelper::Serialize(mSeriData, GetDataSize(), GetPacketID(), GetUserID(), true);
	
	//systemTime
	WORD time = htons(mData.wYear);
	memcpy(&mSeriData[nextOffset], &time, sizeof(time));


	nextOffset += sizeof(time);
	time = htons(mData.wMonth);
	memcpy(&mSeriData[nextOffset], &time, sizeof(time));

	
	nextOffset += sizeof(time);
	time = htons(mData.wDayOfWeek);
	memcpy(&mSeriData[nextOffset], &time, sizeof(time));

	nextOffset += sizeof(time);
	time = htons(mData.wDay);
	memcpy(&mSeriData[nextOffset], &time, sizeof(time));


	nextOffset += sizeof(time);
	time = htons(mData.wHour);
	memcpy(&mSeriData[nextOffset], &time, sizeof(time));

	nextOffset += sizeof(time);
	time = htons(mData.wMinute);
	memcpy(&mSeriData[nextOffset], &time, sizeof(time));

	nextOffset += sizeof(time);
	time = htons(mData.wSecond);
	memcpy(&mSeriData[nextOffset], &time, sizeof(time));

	nextOffset += sizeof(time);
	time = htons(mData.wMilliseconds);
	memcpy(&mSeriData[nextOffset], &time, sizeof(time));


	nextOffset += sizeof(time);
	memcpy(&mSeriData[nextOffset], &gEndMark, sizeof(gEndMark));

}

void AckConPacket::DeSerialize()
{
	UserID userID;
	unsigned long nextOffset=PacketHelper::DeSerializeUserID(mSeriData, userID);
	SetUserID(std::move(userID));

	WORD time;
	memcpy(&time, &mSeriData[nextOffset], sizeof(time));
	mData.wYear = ntohs(time);

	nextOffset += sizeof(time);
	memcpy(&time, &mSeriData[nextOffset], sizeof(time));
	mData.wMonth = ntohs(time);

	nextOffset += sizeof(time);
	memcpy(&time, &mSeriData[nextOffset], sizeof(time));
	mData.wDayOfWeek = ntohs(time);

	nextOffset += sizeof(time);
	memcpy(&time, &mSeriData[nextOffset], sizeof(time));
	mData.wDay = ntohs(time);

	nextOffset += sizeof(time);
	memcpy(&time, &mSeriData[nextOffset], sizeof(time));
	mData.wHour = ntohs(time);


	nextOffset += sizeof(time);
	memcpy(&time, &mSeriData[nextOffset], sizeof(time));
	mData.wMinute = ntohs(time);

	nextOffset += sizeof(time);
	memcpy(&time, &mSeriData[nextOffset], sizeof(time));
	mData.wSecond = ntohs(time);

	nextOffset += sizeof(time);
	memcpy(&time, &mSeriData[nextOffset], sizeof(time));
	mData.wMilliseconds = ntohs(time);


}

