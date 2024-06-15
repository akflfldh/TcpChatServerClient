#pragma once

#pragma comment(lib,"ws2_32.lib")

#include"Type.h"
#include"Default.h"
#include<WinSock2.h>




MYLIBRARY_API enum class EPACKETID :unsigned long
{
	req_con = 0,
	ack_con,
	req_move,
	ack_move,
	req_chat_string,
	ack_chat_string,
	req_discon,
	ack_discon
};

class MYLIBRARY_API Packet
{
public:
	Packet();
	virtual ~Packet();

	void SetPacketID(EPACKETID packetID);
	EPACKETID GetPacketID()const;
	
	SOCKET GetSocket() const;
	void SetSocket(SOCKET socket);
	
	virtual char* GetSeriDataBuffer() = 0;

	virtual void Serialize() = 0;
	virtual void DeSerialize()=0;

	
protected:
	EPACKETID mPacketID;
	SOCKET mSocket;


};


class MYLIBRARY_API UserIDComponent
{
public:
	UserIDComponent() = default;
	~UserIDComponent() = default;


	void SetUserID(const UserID& id);
	void SetUserID(UserID&& id);

	const UserID& GetUserID() const;

private:
	UserID mUserID;

};



