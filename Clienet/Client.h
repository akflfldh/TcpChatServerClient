#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#include<WinSock2.h>
#include<iostream>
#include<sstream>

#include"GlobalFunction.h"
#include"Singleton.h"

#include"Packet.h"
#include"ClientCharacterTable.h"



#define ServerIp "192.168.0.102"





class Client:public Singleton<Client>
{
public:
	Client();
	~Client() = default;

	bool Initialize();
	void Run();
	
	static DWORD RecvProc(LPVOID lpParam);
	static DWORD InputThread(LPVOID lpParam);
	void HandlePacket(Packet * packet);

	void HandleReqCon(Packet* packet);
	void HandleAckCon(Packet* packet);
	void HandleReqMove(Packet* packet);
	void HandleAckMove(Packet* packet);
	void HandleReqChatString(Packet* packet);
	void HandleAckChatString(Packet* packet);
	void HandleReqDisCon(Packet* packet);
	void HandleAckDisCon(Packet* packet);


private:

	void AddNewChat(const std::string& str);
	void AddNewChat(std::string&& str);


private:
	SOCKET mSocket;
	UserID mUserID;
	ClientCharacterTable mClientCharacterTable;


	bool mRecvLoopFlag = true;
	bool mConnectServer = false;
	bool mMainLoopFlag = true;
	bool mInputLoopFlag = true;
	bool mNewChatFlag = true;

	HWND foregroundWindow;
	HANDLE mStringOutputMutex;
	
	std::string mStringArray[10];
	std::string mStringBlank[10];

};

