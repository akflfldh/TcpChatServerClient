#pragma once

#include"SynReaderWriterComponent.h"
#include"Type.h"
#include<unordered_map>

class ClientCharacterTable:public SynReaderWriterComponent
{
public:


	ClientCharacterTable();
	~ClientCharacterTable();

	void AddID(const UserID& id);
	void AddID(UserID&& id);
	void DeleteID(const UserID& id);
	Character* GetCharacter(const UserID& Id);

	unsigned long GetChLatestUpdateState()const;
	void SetChLatestUpdateState(unsigned long state);

private:
	std::unordered_map<UserID, Character * > mClientCharacterTable;
	unsigned long mChLatestUpadateState=0;
	SynReaderWriterComponent mChUpdateSynObject;

};

