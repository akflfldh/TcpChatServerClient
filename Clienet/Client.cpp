#include "Client.h"
#include"ReqConPacket.h"
#include"AckConPacket.h"
#include"PositionPacket.h"
#include"StringPacket.h"




Client::Client()
{

}

bool Client::Initialize()
{
    //�����ϴºκ��� �Ҹ��ڿ��� ���ǵ����� �����ؾ��ϴ°Ŵ�.
    mConnectServerEvent = CreateEvent(nullptr, false, false, L"");
    if (mConnectServerEvent == NULL)
    {
        return false;
    }

    WSAData wsaData;

    WORD requestedVersion = MAKEWORD(2, 2);
    if (WSAStartup(requestedVersion, &wsaData) != 0)
    {
        std::cout << "�ʱ�ȭ ����(WSAStartup)" << std::endl;
        return false;
    }

    mSocket= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mSocket == INVALID_SOCKET)
    {
        std::cout << "�ʱ�ȭ����(socket)" << std::endl;
        WSACleanup();
        return false;
    }

}

void Client::Run()
{

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORTNUM);
    serverAddr.sin_addr.S_un.S_addr = inet_addr(ServerIp);//       htonl(INADDR_ANY);

    if (connect(mSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cout << "�������ӽ���(connect)" << std::endl;
       std::cout<< WSAGetLastError()<<std::endl;
        closesocket(mSocket);
        WSACleanup();
        return;
    }


    HANDLE recvThreadHandle = CreateThread(nullptr, 0, RecvProc, nullptr, 0, nullptr);
    if (recvThreadHandle == NULL)
    {
        std::cout << " recv ������ ��������" << std::endl;
        closesocket(mSocket);
        WSACleanup();
        return;

    }
    CloseHandle(recvThreadHandle);


    HANDLE inputThreadHandle = CreateThread(nullptr, 0, InputThread, nullptr, 0, nullptr);
    if (inputThreadHandle == NULL)
    {
        std::cout << " input ������ ��������" << std::endl;
        closesocket(mSocket);
        mRecvLoopFlag = false;
        WSACleanup();
        return;

    }
    CloseHandle(inputThreadHandle);

    while (mMainLoopFlag)
    {
     
    }

}

DWORD Client::RecvProc(LPVOID lpParam)
{
    auto instance = GetInstance();
    SOCKET clientSocket = instance->mSocket;
    std::unique_ptr<Packet> packet = nullptr;
    while (instance->mRecvLoopFlag)
    {
        try {
            packet = RecvPacket(clientSocket);
        }
        catch (int result)
        {
            switch (result)
            {
            case -1:
                std::cout << GetLastError() << std::endl;
                std::cout << "����recv����" << std::endl;
                closesocket(instance->mSocket);
                break;
            case 0:
                closesocket(instance->mSocket);
                break;
            }
            break;
        }

        packet->DeSerialize();
        instance->HandlePacket(packet.get());
        packet.reset(nullptr);
    }

    return 0;
}
bool IsConsoleFocused(HWND hwnd) {

    return hwnd==GetForegroundWindow();
}

DWORD Client::InputThread(LPVOID lpParam)
{
    auto instance = GetInstance();

    UserID userID;
    std::cout << "id�Է�:";
    std::cin >> userID;
    instance->mUserID = userID;

    ReqConPacket* reqConPacket = new ReqConPacket;
    reqConPacket->SetPacketID(EPACKETID::req_con);
    reqConPacket->SetUserID(userID);

    instance->HandlePacket(reqConPacket);
    delete reqConPacket;
    reqConPacket = nullptr;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) {
        // ���� �ڵ带 ����մϴ�.
        DWORD dwError = GetLastError();
        printf("Failed to get standard output handle. Error code: %lu\n", dwError);
        return 0;
    }
    instance->mConsoleHandle = hConsole;

    WaitForSingleObject(instance->mConnectServerEvent, INFINITE);

    InputLoop();
    return 0;
}

void Client::HandlePacket(Packet* packet)
{

    EPACKETID packetID = packet->GetPacketID();

    switch (packetID)
    {
    case EPACKETID::req_con:
        HandleReqCon(packet);
        break;
    case EPACKETID::ack_con:
        HandleAckCon(packet);
        break;
    case EPACKETID::req_move:
        HandleReqMove(packet);
        break;
    case EPACKETID::ack_move:
        HandleAckMove(packet);
        break;
    case EPACKETID::req_chat_string:
        HandleReqChatString(packet);
        break;

    case EPACKETID::ack_chat_string:
        HandleAckChatString(packet);
        break;
    case EPACKETID::req_discon:
        HandleReqDisCon(packet);
        break;
    case EPACKETID::ack_discon:
        HandleAckDisCon(packet);
        break;
    }

}

void Client::HandleReqCon(Packet* packet)
{
    ReqConPacket* reqConPacket = (ReqConPacket*)packet;
    reqConPacket->Serialize();

    send(mSocket, reqConPacket->GetSeriDataBuffer(), reqConPacket->GetSeriDataSize(), 0);

}

void Client::HandleAckCon(Packet* packet)
{

    AckConPacket* ackConPacket = (AckConPacket*)packet;
    const UserID & userID = ackConPacket->GetUserID();
    

    mClientCharacterTable.WriteStart();
    
    mClientCharacterTable.AddID(userID);

    mClientCharacterTable.WriteEnd();


    const SYSTEMTIME & systemTime = ackConPacket->GetData();

    std::ostringstream oStringStream;
    oStringStream << systemTime.wYear << "�� " << systemTime.wMonth << "�� " << systemTime.wDay << "�� "
        << systemTime.wHour << "�� " << systemTime.wMinute << "�� " << systemTime.wSecond << "�ʿ�" << userID <<
        "�� �����߽��ϴ�.";
    OutputNewChat(oStringStream.str());

    if (mUserID == userID)
    {
        SetEvent(mConnectServerEvent);
    }
}

void Client::HandleReqMove(Packet* packet)
{
    PositionPacket* positionPacket = (PositionPacket*)packet;
    positionPacket->Serialize();
    
    send(mSocket, positionPacket->GetSeriDataBuffer(), positionPacket->GetSeriDataSize(), 0);
}

void Client::HandleAckMove(Packet* packet)
{
    PositionPacket* positionPacket = (PositionPacket*)packet;

    const UserID & userID=  positionPacket->GetUserID();
    
    const Position& position = positionPacket->GetData();

    mClientCharacterTable.WriteStart();

    Character* character = mClientCharacterTable.GetCharacter(userID);
    if (character == nullptr)
    {
        //�������������� ���� �������ִ� user���� ������ �޴´�.

        
        mClientCharacterTable.AddID(userID);
        character = mClientCharacterTable.GetCharacter(userID);
        character->mPos = position;

        mClientCharacterTable.WriteEnd();
    }
    else
    {

        character->mPos = position;

        mClientCharacterTable.WriteEnd();

        std::ostringstream oStringStream;
        oStringStream << "[" << userID << "] �� ��ġ�̵�" << "(" << position.mPosX << "," << position.mPosY << "," << position.mPosZ << ")";
        OutputNewChat(oStringStream.str());
        //���������Ҽ��ִ»��¸鰻�����ϴµ�(�� ���� pc���ݿ��̵ȴ� (�ᱹ�ִ��ѹ�))
        //�״�����������Ʈ�� ���� ������Ŷ�� ���� �����Ŀ� �����ϴ�.(������Ʈ�Ұ���)(�������� �����Ѵ�))
        //�ƴϸ� ȭ�鿡�� �������ʰ�? �����ǰ��� �����µȴ����� ���ο���� ȭ�鿡 �����������ʴ´�.
        if (userID == mUserID)  //���⵵ �����Ϸ���,cpu������ ��ɾ� ��迭�� ������ �ٲ� �������ɼ��ִ�->����ȭ�� �ʿ��Ҽ���
        {
            mUpdateVersionNum++;
        }
    }
}


void Client::HandleReqChatString(Packet* packet)
{
    StringPacket* stringPacket = (StringPacket*)packet;
    stringPacket->Serialize();
    send(mSocket, stringPacket->GetSeriDataBuffer(), stringPacket->GetSeriDataSize(), 0);

}

void Client::HandleAckChatString(Packet* packet)
{

    StringPacket* stringPacket = (StringPacket*)packet;
    const UserID & userID=stringPacket->GetUserID();

    std::ostringstream oStringStream;
    oStringStream << "[" << userID << "] : " << stringPacket->GetData();
    OutputNewChat(oStringStream.str());
}


void Client::HandleReqDisCon(Packet* packet)
{
    ReqConPacket* reqConPacket = (ReqConPacket*)packet;
    reqConPacket->Serialize();


    send(mSocket, reqConPacket->GetSeriDataBuffer(), reqConPacket->GetSeriDataSize(), 0);
    shutdown(mSocket, SD_BOTH);
    closesocket(mSocket);
    mRecvLoopFlag = false;
}

void Client::HandleAckDisCon(Packet* packet)
{
    AckConPacket* ackConPacket = (AckConPacket*)packet;
    const UserID & userID  = ackConPacket->GetUserID();
    
    const SYSTEMTIME & systemTime = ackConPacket->GetData();
  
    mClientCharacterTable.WriteStart();
    mClientCharacterTable.DeleteID(userID);
    mClientCharacterTable.WriteEnd();


    /*std::cout << systemTime.wYear << "�� " << systemTime.wMonth << "�� " << systemTime.wDay << "�� "
        << systemTime.wHour << "�� " << systemTime.wMinute << "�� " << systemTime.wSecond << "�ʿ�" << userID <<
        "�� ���������߽��ϴ�."<<std::endl;*/

    std::ostringstream oStringStream;
    oStringStream << systemTime.wYear << "�� " << systemTime.wMonth << "�� " << systemTime.wDay << "�� "
        << systemTime.wHour << "�� " << systemTime.wMinute << "�� " << systemTime.wSecond << "�ʿ�" << userID <<
        "�� ���������߽��ϴ�.";
    OutputNewChat(oStringStream.str());
    
}

void Client::OutputNewChat(const std::string& str)
{
   // WaitForSingleObject(mStringOutputMutex, INFINITE);

    for (int i = 0; i < 9; ++i)
    {
        mStringBlank[i] = std::move(mStringBlank[i + 1]);
    }
    mStringBlank[9].resize(mStringArray[9].size(), ' ');


    for (int i = 0; i < 9; ++i)
    {

        mStringArray[i] = std::move(mStringArray[i + 1]);
    }
    mStringArray[9] = str;

    SetConsoleCursorPosition(mConsoleHandle, { 0,0 });

    for (int i = 0; i < 10; ++i)
    {
        std::cout << mStringBlank[i] << std::endl;
    }
 //   mStringBlank[9].resize(mStringArray[9].size(),' ');
    SetConsoleCursorPosition(mConsoleHandle, { 0,0 });
    for (int i = 0; i < 10; ++i)
    {
        std::cout << mStringArray[i] << std::endl;
    }

    //mNewChatFlag = true;



    //ReleaseMutex(mStringOutputMutex);
}

void Client::OutputNewChat(std::string&& str)
{
    //WaitForSingleObject(mStringOutputMutex, INFINITE);
    
    for (int i = 0; i < 9; ++i)
    {
        mStringBlank[i] = std::move(mStringBlank[i + 1]);
    }
    mStringBlank[9].resize(mStringArray[9].size(), ' ');

    for (int i = 0; i < 9; ++i)
    {
        mStringArray[i] = std::move(mStringArray[i + 1]);
    }
    mStringArray[9] = std::move(str);

    SetConsoleCursorPosition(mConsoleHandle, { 0,0 });

    for (int i = 0; i < 10; ++i)
    {
        std::cout << mStringBlank[i] << std::endl;
    }
    SetConsoleCursorPosition(mConsoleHandle, { 0,0 });
    for (int i = 0; i < 10; ++i)
    {
        std::cout << mStringArray[i] << std::endl;
    }

    //mNewChatFlag = true;
    //ReleaseMutex(mStringOutputMutex);
}

void Client::InputLoop()
{
    auto instance = GetInstance();

    instance->mClientCharacterTable.ReadStart();
    Character* characterState = instance->mClientCharacterTable.GetCharacter(instance->mUserID);
    instance->mClientCharacterTable.ReadEnd();



    std::string str;
    bool chatFlag = true;
    unsigned long preUpdateVersionNum = 0;

    instance->mUpdateVersionNum = 1;
    instance->foregroundWindow = GetForegroundWindow();
    SetConsoleCursorPosition(instance->mConsoleHandle, { 0,10 });
    instance->mInputChatComponent.Initialize(instance, instance->mConsoleHandle, instance->mUserID);
    instance->mInputCharacterComponent.Initialize(instance, instance->mUserID);

    while (instance->mInputLoopFlag)
    {

        if (IsConsoleFocused(instance->foregroundWindow))
        {
            if (GetAsyncKeyState(VK_TAB) & 0x8000)
            {
                if (chatFlag)
                {
                    chatFlag = false;
                }
                else
                {
                    chatFlag = true;
                }
                Sleep(200);
            }

            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
            {

                ReqConPacket reqConPacket;
                reqConPacket.SetPacketID(EPACKETID::req_discon);
                reqConPacket.SetUserID(instance->mUserID);
                instance->HandlePacket(&reqConPacket);
                instance->mInputLoopFlag = false;
                instance->mMainLoopFlag = false;
                break;
            }


            if (chatFlag)
            {
                instance->mInputChatComponent.Update();
            }
            else
            {
                instance->mInputCharacterComponent.Update(characterState, instance->mUpdateVersionNum, preUpdateVersionNum);
            }
        }

    }
}

