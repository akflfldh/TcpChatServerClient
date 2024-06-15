#include "ReqConPacket.h"
#include"PacketHelper.h"

char* ReqConPacket::GetSeriDataBuffer()
{
	return mSeriData;
}

void ReqConPacket::Serialize()
{
	unsigned long nextOffset=	PacketHelper::Serialize(mSeriData, GetDataSize(), GetPacketID(), GetUserID(), true);
	memcpy(&mSeriData[nextOffset], &gEndMark, sizeof(gEndMark));
}

void ReqConPacket::DeSerialize()
{
	UserID userID;
	PacketHelper::DeSerializeUserID(mSeriData, userID);
	SetUserID(std::move(userID));
}



