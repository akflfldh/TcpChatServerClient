#pragma once

#include"Singleton.h"
#include<unordered_map>

#include<WinSock2.h>

#include"Type.h"
#include"SynReaderWriterComponent.h"

struct ClientInfo
{
	SOCKET mSocket;
	Character mCharacter;
};


class SocketIDTable:public Singleton<SocketIDTable>,public SynReaderWriterComponent
{
public:
	SocketIDTable() = default;
	~SocketIDTable() = default;

	bool AddID(const UserID &id, const ClientInfo & client);
	ClientInfo* GetClientInfo(const UserID& id) const;
	bool DeleteID(const UserID& id);

	const std::unordered_map<UserID, ClientInfo*>& GetTable() const;

private: 
	std::unordered_map<UserID, ClientInfo * > mIDTable;

};

