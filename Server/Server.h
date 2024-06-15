#pragma once




#pragma comment(lib,"ws2_32.lib")

#include<WinSock2.h>
#include<Windows.h>
#include<iostream>
#include"GlobalFunction.h"
#include"Singleton.h"
#include"SocketIDTable.h"
#include"ReqConPacket.h"
#include"AckConPacket.h"
#include"PositionPacket.h"
#include"StringPacket.h"

#define PORT_NUM 40002
#define LISTENSOCKET_QUEUESIZE 10

class Server:public Singleton<Server>
{
public:
	Server() = default;
	~Server() = default;


	bool Initialize();
	void Run();
	
	static DWORD AcceptClientProc(LPVOID lpParam); 
	static DWORD RecvClientProc(LPVOID lpParam);
	static void HandleThread(Packet* packet);

	void HandleReqCon(Packet* packet);
	
	void HandleReqMove(Packet* packet);

	void HandleChatMessage(Packet* packet);

	void HandleReqDisCon(Packet* packet);
	//void HandleAckDisCon(Packet* packet);


private:
	SOCKET mListenSocket;
	SocketIDTable mSocketIDTable;

};

