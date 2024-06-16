#pragma once

#include<iostream>
#include<WIndows.h>
#include<string>
#include"Type.h"
class Client;


class InputChatComponent 
{
public:
	InputChatComponent() = default;
	~InputChatComponent() = default;

	bool Initialize(Client* instance, HANDLE consoleHandle, const UserID& userID);

	void Update();

private:
	Client* mClientInstance;
	HANDLE mConsoleHandle;
	COORD mChatPos;
	bool mLowerCase = true;
	UserID mUserID;
	std::string mStr;


	CONSOLE_SCREEN_BUFFER_INFO mConsoleScreenBufferInfo;


};
 
