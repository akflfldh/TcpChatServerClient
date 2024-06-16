#include "InputCharacterComponent.h"
#include"Client.h"

bool InputCharacterComponent::Initialize(Client * instance,const UserID & userID)
{
    mClientInstance = instance;
    mUserID = userID;
	return true;
}

void InputCharacterComponent::Update(Character * character , unsigned long versionNum, unsigned long & preVersionNum)
{
    bool posChange = false;

    if (versionNum > preVersionNum)
    {
        Position characterPos = character->mPos;

        if (GetAsyncKeyState('A') & 0x8000)
        {
            //characterPos = characterState->mPos;
            characterPos.mPosX -= 1.0f;
            posChange = true;
        }

        if (GetAsyncKeyState('D') & 0x8000)
        {
            // characterPos = characterState->mPos;
            characterPos.mPosX += 1.0f;
            posChange = true;

        }

        if (GetAsyncKeyState('W') & 0x8000)
        {
            //characterPos = characterState->mPos;
            characterPos.mPosY += 1.0f;
            posChange = true;

        }

        if (GetAsyncKeyState('S') & 0x8000)
        {
            //characterPos = characterState->mPos;
            characterPos.mPosY -= 1.0f;
            posChange = true;
        }

        if (posChange)
        {
            preVersionNum = versionNum;
            PositionPacket positionPacket;
            positionPacket.SetData(characterPos);
            positionPacket.SetPacketID(EPACKETID::req_move);
            positionPacket.SetUserID(mUserID);
            mClientInstance->HandlePacket(&positionPacket);
            posChange = false;
            Sleep(100);
        }
    }

}
