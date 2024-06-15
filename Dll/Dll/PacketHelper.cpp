#include "PacketHelper.h"

unsigned long  PacketHelper::Serialize(char* seriDataBuffer, unsigned long dataLength, EPACKETID ePacketID, const UserID& userID, bool userIDFlag)
{

	unsigned long nextOffset = 0;

	dataLength = htonl(dataLength);
	memcpy(&seriDataBuffer[nextOffset], &dataLength, sizeof(dataLength));


	nextOffset += sizeof(dataLength);
	unsigned long packetID = htonl((unsigned long)ePacketID);
	memcpy(&seriDataBuffer[nextOffset], &packetID, sizeof(packetID));
	nextOffset += sizeof(packetID);

	if (userIDFlag)
	{
		unsigned char userIDLength = userID.size();
		memcpy(&seriDataBuffer[nextOffset], &userIDLength, sizeof(userIDLength));

		nextOffset += sizeof(userIDLength);
		memcpy(&seriDataBuffer[nextOffset], userID.data(), userID.size());

		nextOffset += UserIDMaxLength;
	}

	return nextOffset;
}

unsigned long PacketHelper::DeSerializeUserID(char* seriDataBuffer, UserID& oUserID)
{
	unsigned char userIDLength = 0;
	unsigned long nextOffset = sizeof(unsigned long) * 2;

	memcpy(&userIDLength, &seriDataBuffer[nextOffset], sizeof(userIDLength));

	nextOffset += sizeof(userIDLength);
	UserID userID(&seriDataBuffer[nextOffset], userIDLength);
	oUserID =std::move(userID);

	nextOffset += UserIDMaxLength;
	return nextOffset;

}
