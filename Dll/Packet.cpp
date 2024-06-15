#include"Packet.h"



Packet::Packet()
{

}

Packet::~Packet()
{

}

void Packet::SetPacketID(EPACKETID packetID)
{
	mPacketID = packetID;
}

EPACKETID Packet::GetPacketID() const
{
	return mPacketID;
}

SOCKET Packet::GetSocket() const
{
	return mSocket;
}

void Packet::SetSocket(SOCKET socket)
{
	mSocket = socket;
}

void UserIDComponent::SetUserID(const UserID& id)
{
	mUserID = id;
}

void UserIDComponent::SetUserID(UserID&& id)
{
	mUserID = id;
}

const UserID& UserIDComponent::GetUserID() const
{
	return mUserID;
	// TODO: 여기에 return 문을 삽입합니다.
}
