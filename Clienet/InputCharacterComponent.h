#pragma once

#include"Type.h"
#include"PositionPacket.h"
#include<Windows.h>

class Client;

class InputCharacterComponent
{
public:
	InputCharacterComponent() = default;
	~InputCharacterComponent() = default;


	bool Initialize(Client * instance, const UserID& userID);
	void Update(Character* character, unsigned long versionNum, unsigned long& preVersionNum);

private:
	Client* mClientInstance;
	Character* mCharacterState;
	UserID mUserID;
	unsigned long mPreUpdateVersionNum;
};

