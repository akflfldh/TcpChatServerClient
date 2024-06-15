#include "PositionPacket.h"
#include"PacketHelper.h"

PositionPacket::PositionPacket()
{
}

PositionPacket::~PositionPacket()
{
}


void PositionPacket::SetData(const Position& data)
{
    mData = data;

}

const Position& PositionPacket::GetData() const
{
    return mData;
}

char* PositionPacket::GetSeriDataBuffer() 
{
    return mSeriData;
}

void PositionPacket::Serialize()
{
   
    unsigned long nextOffset=PacketHelper::Serialize(mSeriData,GetDataSize(),GetPacketID(),GetUserID(),true);

    float pos = htonf(mData.mPosX);
    memcpy(&mSeriData[nextOffset], &pos, sizeof(pos));

    nextOffset += sizeof(pos);
    pos = htonf(mData.mPosY);
    memcpy(&mSeriData[nextOffset], &pos, sizeof(pos));

    nextOffset += sizeof(pos);
    pos = htonf(mData.mPosZ);
    memcpy(&mSeriData[nextOffset], &pos, sizeof(pos));

    nextOffset += sizeof(pos);
    memcpy(&mSeriData[nextOffset], &gEndMark,sizeof(gEndMark));

}


void PositionPacket::DeSerialize()
{
  
    UserID userID;
    unsigned long nextOffset = PacketHelper::DeSerializeUserID(mSeriData, userID);
    SetUserID(std::move(userID));

    float pos = 0.0f;
    memcpy(&pos, &mSeriData[nextOffset], sizeof(pos));
    mData.mPosX = ntohf(pos);
   

    nextOffset += sizeof(pos);
    memcpy(&pos, &mSeriData[nextOffset], sizeof(pos));
    mData.mPosY = ntohf(pos);

    nextOffset += sizeof(pos);
    memcpy(&pos, &mSeriData[nextOffset], sizeof(pos));
    mData.mPosZ = ntohf(pos);

}

