#pragma once

#include"VariableLengthPacket.h"
#include<iostream>
#include<string>




class MYLIBRARY_API StringPacket :public VariableLengthPacket, public UserIDComponent
{
public:
	StringPacket();
	~StringPacket();


	void SetData(const std::string& data);
	const std::string& GetData()const;

	void Serialize() override;
	void DeSerialize() override;

	unsigned long GetDataSize()const;

private:
	std::string mData;
	unsigned long mDataSize = 0;
};



