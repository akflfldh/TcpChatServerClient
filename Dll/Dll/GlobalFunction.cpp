#include"GlobalFunction.h"

#include"ReqConPacket.h"
#include"AckConPacket.h"
#include"PositionPacket.h"
#include"PacketHelper.h"
#include"StringPacket.h"/

std::unique_ptr<Packet> RecvPacket(SOCKET socket)
{
	unsigned long length = 0;
	unsigned long packetID = 0;
	unsigned long nextOffset = 0;
	uint32_t remainingLength = sizeof(length);
	unsigned long bufOffset = 0;
	
	// data length
	while (remainingLength)
	{

		int result = recv(socket,(char *)&length + nextOffset, remainingLength, 0);
		if (result == -1)
		{
			throw - 1;
			
		}
		else if (result == 0)
		{
			throw 0;
		}

		remainingLength -= result;
		nextOffset += result;
	}
	length = ntohl(length);

	//packet id
	
	remainingLength = sizeof(packetID);
	nextOffset = 0;
	while (remainingLength)
	{
		int result = recv(socket,(char*)&packetID+ nextOffset, remainingLength, 0);
		if (result == -1)
		{
			throw - 1;
		}
		else if (result == 0)
		{
			throw 0;
		}

		remainingLength -= result;
		nextOffset += result;
	}
	packetID= ntohl(packetID);

	//length ½á¾ßÁö
	std::unique_ptr<Packet> packet= CreatePacket(length, (EPACKETID)packetID);

	char * seriBuffer= packet->GetSeriDataBuffer();
	
	memcpy(seriBuffer, &length, sizeof(length));
	memcpy(seriBuffer + sizeof(length), &packetID, sizeof(packetID));

	packet->SetPacketID((EPACKETID)packetID);



	//data
	bufOffset = sizeof(length) + sizeof(packetID);
	remainingLength = length;
	while (remainingLength)
	{
		int result = recv(socket, seriBuffer + bufOffset, remainingLength, 0);
		if (result == -1)
		{
			throw -1;
		}
		else if (result == 0)
		{
			throw 0;
		}

		remainingLength -= result;
		bufOffset += result;
	}


	//end mark
	
	remainingLength = sizeof(char);

	while (remainingLength)
	{
		int result = recv(socket, seriBuffer + bufOffset, remainingLength, 0);
		if (result == -1)
		{
			throw -1;
		}
		else if (result == 0)
		{
			throw 0;
		}

		remainingLength -= result;
		bufOffset += result;
	}

	packet->SetSocket(socket);
	return packet;
}

std::unique_ptr<Packet> CreatePacket(unsigned long length, EPACKETID packetID)
{
	//std::unique_ptr<Packet> packet = nullptr;

	switch (packetID)
	{
	case  EPACKETID::req_con:
		return std::make_unique<ReqConPacket>();
	case EPACKETID::ack_con:
		return std::make_unique<AckConPacket>();
	case EPACKETID::req_move:
		return std::make_unique<PositionPacket>();
	case EPACKETID::ack_move:
		return std::make_unique<PositionPacket>();
	case EPACKETID::req_chat_string:
	{
		auto packet = std::make_unique<StringPacket>();
		packet->CreateSeriBuffer(length);
		return packet;
	}
	case EPACKETID::ack_chat_string:
	{
		auto packet = std::make_unique<StringPacket>();
		packet->CreateSeriBuffer(length);
		return packet;
	}
	case EPACKETID::req_discon:
		return std::make_unique<ReqConPacket>();
	case EPACKETID::ack_discon:
		return std::make_unique<AckConPacket>();
	}

	return nullptr;
}
