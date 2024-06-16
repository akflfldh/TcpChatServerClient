
#include"StringPacket.h"
#include"Client.h"
#include "InputChatComponent.h"
bool InputChatComponent::Initialize( Client * instance, HANDLE consoleHandle,const UserID & userID)
{
    mClientInstance = instance;
    mConsoleHandle = consoleHandle;
    mChatPos = { 0,10 };
    mUserID = userID;
    return true;
}



void InputChatComponent::Update()
{

    DWORD written;


    if (GetAsyncKeyState(VK_CAPITAL) & 0x8000)
    {
        if (mLowerCase)
        {
            mLowerCase = false;
        }
        else
        {
            mLowerCase = true;
        }
        Sleep(200);
    }

    if (GetAsyncKeyState(VK_BACK) & 0x8000)
    {
        GetConsoleScreenBufferInfo(mConsoleHandle, &mConsoleScreenBufferInfo);
        if (mChatPos.X != 0)
            mChatPos.X -= 1;
        FillConsoleOutputCharacter(mConsoleHandle, ' ', 1, mChatPos, &written);

        // 커서를 해당 라인의 처음으로 이동시킵니다.
        //coord.X = 0;
        SetConsoleCursorPosition(mConsoleHandle, mChatPos);

        if (mStr.size() != 0)
            mStr.resize(mStr.size() - 1);

        Sleep(100);
    }
    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
        // std::system("cls");
        mChatPos.X++;
        std::cout << " ";
        mStr += ' ';
        Sleep(180);
    }


    for (int i = 48; i <= 57; i++)
    {
        //  std::system("cls");
        if (GetAsyncKeyState(i) & 0x8000)
        {
            char c = (char)i;
            mChatPos.X++;
            std::cout << c;
            mStr += c;
            Sleep(150);
        }
    }

    for (int i = 58; i <= 90; i++)
    {
        if (GetAsyncKeyState(i) & 0x8000)
        {
            // std::system("cls");
            char c = (char)i;
            if (mLowerCase)
            {
                c += 32;
            }
            mChatPos.X++;
            std::cout << c;
            mStr += c;
            Sleep(150);
        }
    }


    if ((GetAsyncKeyState(VK_RETURN) & 0x8000) && mStr != "")
    {
        //std::system("cls");
        GetConsoleScreenBufferInfo(mConsoleHandle, &mConsoleScreenBufferInfo);
        FillConsoleOutputCharacter(mConsoleHandle, ' ', mChatPos.X, { 0,10 }, &written);

        //// 커서를 해당 라인의 처음으로 이동시킵니다.
        mChatPos.X = 0;
        SetConsoleCursorPosition(mConsoleHandle, mChatPos);

        StringPacket stringPacket;
        stringPacket.SetPacketID(EPACKETID::req_chat_string);
        stringPacket.SetUserID(mUserID);
        stringPacket.SetData(mStr);

        mClientInstance->HandlePacket(&stringPacket);

        mStr.clear();
        Sleep(200);
    }



}
