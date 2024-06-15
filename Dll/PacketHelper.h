#pragma once


#include"Packet.h"


class MYLIBRARY_API PacketHelper
{
public:
	PacketHelper() = delete;
	~PacketHelper() = delete;


	static unsigned long  Serialize(char* seriDataBuffer, unsigned long dataLength, EPACKETID ePacketID, const UserID& userID, bool userIDFlag);
	static unsigned long  DeSerializeUserID(char* seriDataBuffer, UserID& oUserID);




private:


	

};

