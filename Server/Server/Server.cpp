#include "Server.h"

bool Server::Initialize()
{

	WSADATA wsaData;
	WORD requestedVersion = MAKEWORD(2, 2);
	if (WSAStartup(requestedVersion, &wsaData) != 0)
	{
		std::cout << "초기화실패" << std::endl;
		return false;
	}

	mListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mListenSocket == INVALID_SOCKET)
	{
		std::cout << "대기 소켓 생성실패(socket)" << std::endl;
		WSACleanup();
		return false;
	}


	sockaddr_in listenSocketAddr;
	listenSocketAddr.sin_family = AF_INET;
	listenSocketAddr.sin_port = htons(PORT_NUM);
	listenSocketAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(mListenSocket, (sockaddr*)&listenSocketAddr, sizeof(listenSocketAddr)) == SOCKET_ERROR)
	{
		std::cout << "대기소켓 주소 바인딩실패(bind)" << std::endl;
		closesocket(mListenSocket);
		WSACleanup();

		return false;
	}

	if (listen(mListenSocket, LISTENSOCKET_QUEUESIZE) == SOCKET_ERROR)
	{
		std::cout << "대기소켓 등록실패(listen)" << std::endl;
		closesocket(mListenSocket);
		WSACleanup();
		return false;
	}


	return true;
}

void Server::Run() 
{

	HANDLE acceptThreadHandle = CreateThread(nullptr, 0, AcceptClientProc, nullptr, 0, nullptr);
	if (acceptThreadHandle == NULL)
	{
		std::cout << "Accept스레드 생성실패" << std::endl;
		closesocket(mListenSocket);
		WSACleanup();
	}
	CloseHandle(acceptThreadHandle);





	while (1)
	{



	}


}

DWORD Server::AcceptClientProc(LPVOID lpParam)
{
	auto instance= GetInstance();
	SOCKET listenSocket= instance->mListenSocket;


	while (1)
	{
		sockaddr_in clientSocketAddr;
		int addrSize = sizeof(clientSocketAddr);
		SOCKET clientSocket = accept(listenSocket, (sockaddr*)&clientSocketAddr, &addrSize);
		if (clientSocket == INVALID_SOCKET)
		{
			std::cout << "소켓 연결 실패" << std::endl;
			continue;
		}

		HANDLE comClientThreadHandle= CreateThread(nullptr, 0, RecvClientProc, (LPVOID)clientSocket, 0, nullptr);
		if (comClientThreadHandle == NULL)
		{
			std::cout << "클라이언트 통신 스레드 생성실패" << std::endl;
			closesocket(clientSocket);
			continue;
		}
		CloseHandle(comClientThreadHandle);

	}


}

DWORD Server::RecvClientProc(LPVOID lpParam)
{
	SOCKET clientSocket = (SOCKET)lpParam;
	std::unique_ptr<Packet> packet = nullptr;
	while (1)
	{
		try {
			 packet= RecvPacket(clientSocket);
		}
		catch (int result)
		{
			switch (result)
			{
			case -1:
				std::cout<<GetLastError()<<std::endl;
				std::cout << "소켓recv에러" << std::endl;
				break;
			case 0:

				break;
			}
			break;
		}
		packet->DeSerialize();
		HandleThread(packet.get());
		packet.reset(nullptr);
	}

	closesocket(clientSocket);
	return 0;
}

void Server::HandleThread(Packet* packet)
{
	auto instance = GetInstance();
	switch (packet->GetPacketID())
	{
	case EPACKETID::req_con:
		instance->HandleReqCon(packet);
		break;
	case EPACKETID::ack_con:


		break;
	case EPACKETID::req_move:
		instance->HandleReqMove(packet);

		break;
	case EPACKETID::ack_move:
		

		break;
	case EPACKETID::req_chat_string:
		instance->HandleChatMessage(packet);


		break;
	case EPACKETID::ack_chat_string:
		
	
		break;
	case EPACKETID::req_discon:
		instance->HandleReqDisCon(packet);
		break;

	case EPACKETID::ack_discon:


		break;
	}

}

void Server::HandleReqCon(Packet* packet)
{
	//table id추가
	//먼저 id가중복되지않는지 점검해야한다.


	ReqConPacket* connectPacket =(ReqConPacket*)packet;

	ClientInfo clientInfo;
	clientInfo.mSocket = connectPacket->GetSocket();
	clientInfo.mCharacter.mUserID = connectPacket->GetUserID();
	clientInfo.mCharacter.mPos = { 0,0,0 };

	mSocketIDTable.ReadStart();

	const auto & table = mSocketIDTable.GetTable();
	PositionPacket positionPacket;
	positionPacket.SetPacketID(EPACKETID::ack_move);
	for (auto it = table.cbegin(); it != table.cend(); ++it)
	{
		positionPacket.SetUserID(it->second->mCharacter.mUserID);
		positionPacket.SetData(it->second->mCharacter.mPos);
		positionPacket.Serialize();
		send(clientInfo.mSocket, positionPacket.GetSeriDataBuffer(), positionPacket.GetSeriDataSize(), 0);
	}

	mSocketIDTable.ReadEnd();

	mSocketIDTable.WriteStart();

	bool result = mSocketIDTable.AddID(clientInfo.mCharacter.mUserID, clientInfo);

	mSocketIDTable.WriteEnd();

	if (result)
	{
		//ack_con
		AckConPacket ackPacket;
		SYSTEMTIME systemTime;
		GetSystemTime(&systemTime);

		ackPacket.SetUserID(connectPacket->GetUserID());
		ackPacket.SetPacketID(EPACKETID::ack_con);
		ackPacket.SetData(systemTime);
		ackPacket.Serialize();

		//모든 클라이언트들에게 패킷 전송

		mSocketIDTable.ReadStart();

		const auto & idTable= mSocketIDTable.GetTable();
		for(auto it=idTable.cbegin(); it!=idTable.cend(); ++it)
		{			
			send(it->second->mSocket, ackPacket.GetSeriDataBuffer(), ackPacket.GetSeriDataSize(), 0);
		}

		mSocketIDTable.ReadEnd();

		std::cout << systemTime.wYear << "년 " << systemTime.wMonth << "월 " << systemTime.wDay << "일 "
			<< systemTime.wHour << "시 " << systemTime.wMinute << "분 " << systemTime.wSecond << "초에" << connectPacket->GetUserID() <<
			"가 접속했습니다."<<std::endl;

	}else
	{
		//실패
		//이미 이 아이디로 누군가접속했다.
	}


}



void Server::HandleReqMove(Packet* packet)
{
	
	PositionPacket* positionPacket = (PositionPacket*)packet;
	const Position& position = positionPacket->GetData();
	const UserID& userID = positionPacket->GetUserID();

	mSocketIDTable.WriteStart();

	ClientInfo * clientInfo= mSocketIDTable.GetClientInfo(userID);
	clientInfo->mCharacter.mPos = position;

	mSocketIDTable.WriteEnd();

	positionPacket->SetPacketID(EPACKETID::ack_move);
	positionPacket->Serialize();

	mSocketIDTable.ReadStart();

	const auto& table = mSocketIDTable.GetTable();
	for (auto it = table.cbegin(); it != table.cend(); ++it)
	{
		send(it->second->mSocket, positionPacket->GetSeriDataBuffer(), positionPacket->GetSeriDataSize(), 0);
	}

	mSocketIDTable.ReadEnd();


	std::cout << "[" << userID << "]가 (" << position.mPosX << ", " << position.mPosY << ", " << position.mPosZ << ")로 이동" << std::endl;

}

void Server::HandleChatMessage(Packet* packet)
{
	StringPacket* stringPacket = (StringPacket*)packet;

	const UserID& userID = stringPacket->GetUserID();

	std::cout << "[" << userID << "] :" << stringPacket->GetData() << std::endl;



	stringPacket->SetPacketID(EPACKETID::ack_chat_string);
	stringPacket->Serialize();
	
	mSocketIDTable.ReadStart();

	const auto& table=	mSocketIDTable.GetTable();
	for (auto it = table.cbegin(); it != table.cend(); ++it)
	{
		send(it->second->mSocket, stringPacket->GetSeriDataBuffer(), stringPacket->GetSeriDataSize(), 0);
	}

	mSocketIDTable.ReadEnd();


}

void Server::HandleReqDisCon(Packet* packet)
{
	ReqConPacket* reqConPacket = (ReqConPacket*)packet;

	const UserID& userID = reqConPacket->GetUserID();


	mSocketIDTable.WriteStart();
	mSocketIDTable.DeleteID(userID);
	mSocketIDTable.WriteEnd();


	AckConPacket ackConPacket;
	SYSTEMTIME systemTime;
	GetSystemTime(&systemTime);


	ackConPacket.SetData(systemTime);
	ackConPacket.SetUserID(userID);
	ackConPacket.SetPacketID(EPACKETID::ack_discon);
	ackConPacket.Serialize();


	mSocketIDTable.ReadStart();
	const auto& table = mSocketIDTable.GetTable();
	for (auto& element : table)
	{
		send(element.second->mSocket, ackConPacket.GetSeriDataBuffer(), ackConPacket.GetSeriDataSize(), 0);

	}
	mSocketIDTable.ReadEnd();


	std::cout << systemTime.wYear << "년 " << systemTime.wMonth << "월 " << systemTime.wDay << "일 "
		<< systemTime.wHour << "시 " << systemTime.wMinute << "분 " << systemTime.wSecond << "초에" << userID <<
		"가 접속해제했습니다." << std::endl;

}
