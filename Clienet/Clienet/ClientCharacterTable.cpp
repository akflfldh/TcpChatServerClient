#include "ClientCharacterTable.h"

ClientCharacterTable::ClientCharacterTable()
{
  
}

ClientCharacterTable::~ClientCharacterTable()
{
    for (auto& element : mClientCharacterTable) {
        delete element.second;
    }
}

void ClientCharacterTable::AddID(const UserID& id)
{

    //id�������ϴ� �����κ��� ��Ŷ�����������߰������� �ߺ��Ǵ����̾���.
    Character* character = new Character;
    character->mPos = { 0,0,0 };
    character->mUserID = id;


    mClientCharacterTable.insert({ character->mUserID, character });


    return;
}

void ClientCharacterTable::AddID(UserID&& id)
{

    Character* character = new Character;
    character->mPos = { 0,0,0 };
    character->mUserID = std::move(id);


    mClientCharacterTable.insert({ character->mUserID, character });

}

void ClientCharacterTable::DeleteID(const UserID& id)
{
    
    auto it=  mClientCharacterTable.find(id);
    if (it != mClientCharacterTable.end())
    {
        delete it->second;
        mClientCharacterTable.erase(id);
    }
}

Character* ClientCharacterTable::GetCharacter(const UserID& id)
{
    auto it = mClientCharacterTable.find(id);

    if (it == mClientCharacterTable.end())
    {
        return nullptr;
    }
    return it->second;

}

unsigned long ClientCharacterTable::GetChLatestUpdateState() const
{
    
    return mChLatestUpadateState;
}

void ClientCharacterTable::SetChLatestUpdateState(unsigned long state)
{
    mChUpdateSynObject.WriteStart();
    mChLatestUpadateState = state;
    mChUpdateSynObject.WriteEnd();
}
