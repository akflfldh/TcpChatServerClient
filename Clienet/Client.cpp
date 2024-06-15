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

    mStringOutputMutex = CreateMutex(nullptr, false, L"");
    if (mStringOutputMutex == NULL)
    {
        return false;
    }

    WSAData wsaData;

    WORD requestedVersion = MAKEWORD(2, 2);
    if (WSAStartup(requestedVersion, &wsaData) != 0)
    {
        std::cout << "초기화 실패(WSAStartup)" << std::endl;
        return false;
    }

    mSocket= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mSocket == INVALID_SOCKET)
    {
        std::cout << "초기화실패(socket)" << std::endl;
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
        std::cout << "서버접속실패(connect)" << std::endl;
       std::cout<< WSAGetLastError()<<std::endl;
        closesocket(mSocket);
        WSACleanup();
        return;
    }


    HANDLE recvThreadHandle = CreateThread(nullptr, 0, RecvProc, nullptr, 0, nullptr);
    if (recvThreadHandle == NULL)
    {
        std::cout << " recv 스레드 생성실패" << std::endl;
        closesocket(mSocket);
        WSACleanup();
        return;

    }
    CloseHandle(recvThreadHandle);


    HANDLE inputThreadHandle = CreateThread(nullptr, 0, InputThread, nullptr, 0, nullptr);
    if (inputThreadHandle == NULL)
    {
        std::cout << " input 스레드 생성실패" << std::endl;
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
                std::cout << "소켓recv에러" << std::endl;
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
    std::cout << "id입력:";
    std::cin >> userID;
    instance->mUserID = userID;



    ReqConPacket* reqConPacket = new ReqConPacket;
    reqConPacket->SetPacketID(EPACKETID::req_con);
    reqConPacket->SetUserID(userID);

    instance->HandlePacket(reqConPacket);
    delete reqConPacket;
    reqConPacket = nullptr;

    while (!instance->mConnectServer)
    {
        ;
    }

    instance->mClientCharacterTable.ReadStart();
    Character* characterState = instance->mClientCharacterTable.GetCharacter(userID);
    instance->mClientCharacterTable.ReadEnd();


    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) {
        // 오류 코드를 출력합니다.
        DWORD dwError = GetLastError();
        printf("Failed to get standard output handle. Error code: %lu\n", dwError);
        return 1;
    }
    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    COORD chatCoord = { 0,10 };

    std::string str;
    DWORD written;
    bool chatFlag = true;
    bool posChange = false;
    bool lowercase = true;
    unsigned long preUpdateState = instance->mClientCharacterTable.GetChLatestUpdateState();
    instance->mClientCharacterTable.SetChLatestUpdateState(preUpdateState + 1);

    Position characterPos;
    instance->foregroundWindow = GetForegroundWindow();
    SetConsoleCursorPosition(hConsole, chatCoord);
    while (instance->mInputLoopFlag)
    {
        
        if (IsConsoleFocused(instance->foregroundWindow))
        {
            if (GetAsyncKeyState(VK_TAB) & 0x8000)
            {
                if (chatFlag)
                {
                    //GetConsoleScreenBufferInfo(hConsole, &screenBufferInfo);
                    //chatCoord.X = 0;
                    //chatCoord.Y = screenBufferInfo.dwCursorPosition.Y;
                    //FillConsoleOutputCharacter(hConsole, ' ', screenBufferInfo.dwSize.X, chatCoord, &written);

                    //// 커서를 해당 라인의 처음으로 이동시킵니다.
                    //chatCoord.X = 0;
                    //SetConsoleCursorPosition(hConsole, chatCoord);
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

            if (GetAsyncKeyState(VK_CAPITAL) & 0x8000)
            {
                if (lowercase)
                {
                    lowercase = false;
                }
                else
                {
                    lowercase = true;
                }
                Sleep(200);
            }


            if (chatFlag)
            {
                

                if (GetAsyncKeyState(VK_BACK) & 0x8000)
                {
                    GetConsoleScreenBufferInfo(hConsole, &screenBufferInfo);
                    chatCoord.X -= 1;
                    FillConsoleOutputCharacter(hConsole, ' ',1, chatCoord, &written);

                    // 커서를 해당 라인의 처음으로 이동시킵니다.
                    //coord.X = 0;
                    SetConsoleCursorPosition(hConsole, chatCoord);  
                    str.resize(str.size() - 1);
                    Sleep(100);
                }
                if (GetAsyncKeyState(VK_SPACE) & 0x8000)
                {
                   // std::system("cls");
                    chatCoord.X++;
                     std::cout << " ";
                    str += ' ';
                    Sleep(180);
                }


                for (int i = 48; i <= 57; i++)
                {
                  //  std::system("cls");
                    if (GetAsyncKeyState(i) & 0x8000)
                    {
                        char c = (char)i;
                        chatCoord.X++;
                        std::cout << c;
                        str += c;
                        Sleep(150);
                    }
                }

                for (int i = 58; i <= 90; i++)
                {
                    if (GetAsyncKeyState(i) & 0x8000)
                    {
                       // std::system("cls");
                        char c = (char)i;
                        if (lowercase)
                        {
                            c += 32;
                        }
                        chatCoord.X++;
                        std::cout << c;
                        str += c;
                        Sleep(150);
                    }
                }


                if ((GetAsyncKeyState(VK_RETURN) & 0x8000) && str != "")
                {
                    //std::system("cls");
                    GetConsoleScreenBufferInfo(hConsole, &screenBufferInfo);
                    FillConsoleOutputCharacter(hConsole, ' ', chatCoord.X, { 0,10 }, &written);

                    //// 커서를 해당 라인의 처음으로 이동시킵니다.
                    chatCoord.X = 0;
                    SetConsoleCursorPosition(hConsole, chatCoord);

                    StringPacket stringPacket;
                    stringPacket.SetPacketID(EPACKETID::req_chat_string);
                    stringPacket.SetUserID(userID);
                    stringPacket.SetData(str);

                    instance->HandlePacket(&stringPacket);

                    str.clear();
                    Sleep(200);
                }

            }
            else
            {

                //x,y조작

                if (instance->mClientCharacterTable.GetChLatestUpdateState() > preUpdateState)
                {
                    if (GetAsyncKeyState('A') & 0x8000)
                    {
                        characterPos = characterState->mPos;
                        characterPos.mPosX -= 1.0f;
                        posChange = true;
                    }
                    else if (GetAsyncKeyState('D') & 0x8000)
                    {
                        characterPos = characterState->mPos;
                        characterPos.mPosX += 1.0f;
                        posChange = true;

                    }
                    else if (GetAsyncKeyState('W') & 0x8000)
                    {
                        characterPos = characterState->mPos;
                        characterPos.mPosY += 1.0f;
                        posChange = true;

                    }
                    else if (GetAsyncKeyState('S') & 0x8000)
                    {
                        characterPos = characterState->mPos;
                        characterPos.mPosY -= 1.0f;
                        posChange = true;
                    }

                    if (posChange)
                    {
                        preUpdateState = instance->mClientCharacterTable.GetChLatestUpdateState();
                        PositionPacket positionPacket;
                        positionPacket.SetData(characterPos);
                        positionPacket.SetPacketID(EPACKETID::req_move);
                        positionPacket.SetUserID(userID);
                        instance->HandlePacket(&positionPacket);
                        posChange = false;
                    }

                    Sleep(100);
                }
            }





        }



         if (instance->mNewChatFlag)
        {
           
        SetConsoleCursorPosition(hConsole, { 0,0 });
        WaitForSingleObject(instance->mStringOutputMutex, INFINITE);

        for (int i = 0; i < 10; ++i)
        {
            std::cout << instance->mStringBlank[i] << std::endl;
        }
        SetConsoleCursorPosition(hConsole, { 0,0 });
        for (int i = 0; i < 10; ++i)
        {
            std::cout << instance->mStringArray[i] << std::endl;
        }
        instance->mNewChatFlag = false;
        ReleaseMutex(instance->mStringOutputMutex);
        //SetConsoleCursorPosition(hConsole, { 0,10 });
        }
      
    }


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
    oStringStream << systemTime.wYear << "년 " << systemTime.wMonth << "월 " << systemTime.wDay << "일 "
        << systemTime.wHour << "시 " << systemTime.wMinute << "분 " << systemTime.wSecond << "초에" << userID <<
        "가 접속했습니다.";
    AddNewChat(oStringStream.str());

    mConnectServer = true;
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
        //새로접속했을때 기존 서버에있던 user들의 정보를 받는다.

        
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
        oStringStream << "[" << userID << "] 의 위치이동" << "(" << position.mPosX << "," << position.mPosY << "," << position.mPosZ << ")";
        AddNewChat(oStringStream.str());
        //내가갱신할수있는상태면갱신을하는데(즉 나의 pc에반영이된다 (결국최대한번))
        //그다음번업데이트는 내가 보낸패킷을 내가 받은후에 가능하다.(업데이트불가능)(버전으로 관리한다))
        //아니면 화면에는 보이지않게? 기존의값만 계속출력된느거지 새로운것이 화면에 렌더링되지않는다.
        if (userID == mUserID)
        {
            mClientCharacterTable.SetChLatestUpdateState(mClientCharacterTable.GetChLatestUpdateState() + 1);
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
    AddNewChat(oStringStream.str());
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


    /*std::cout << systemTime.wYear << "년 " << systemTime.wMonth << "월 " << systemTime.wDay << "일 "
        << systemTime.wHour << "시 " << systemTime.wMinute << "분 " << systemTime.wSecond << "초에" << userID <<
        "가 접속해제했습니다."<<std::endl;*/

    std::ostringstream oStringStream;
    oStringStream << systemTime.wYear << "년 " << systemTime.wMonth << "월 " << systemTime.wDay << "일 "
        << systemTime.wHour << "시 " << systemTime.wMinute << "분 " << systemTime.wSecond << "초에" << userID <<
        "가 접속해제했습니다.";
    AddNewChat(oStringStream.str());
    
}

void Client::AddNewChat(const std::string& str)
{
    WaitForSingleObject(mStringOutputMutex, INFINITE);

    for (int i = 0; i < 9; ++i)
    {
        mStringBlank[i] = std::move(mStringBlank[i + 1]);
    }
    mStringBlank[9].resize(mStringArray[9].size(), ' ');


    for (int i = 0; i < 9; ++i)
    {
        
        mStringArray[i]=   std::move(mStringArray[i + 1]);
    }
    mStringArray[9] = str;
 //   mStringBlank[9].resize(mStringArray[9].size(),' ');
   

    mNewChatFlag = true;



    ReleaseMutex(mStringOutputMutex);
}

void Client::AddNewChat(std::string&& str)
{
    WaitForSingleObject(mStringOutputMutex, INFINITE);
    
    for (int i = 0; i < 9; ++i)
    {
        mStringBlank[i] = std::move(mStringBlank[i + 1]);
    }
    mStringBlank[9].resize(mStringArray[9].size(), ' ');

    for (int i = 0; i < 9; ++i)
    {
        mStringArray[i] = std::move(mStringArray[i+1]);
    }
    mStringArray[9] = std::move(str);
   
    mNewChatFlag = true;
    ReleaseMutex(mStringOutputMutex);
}

