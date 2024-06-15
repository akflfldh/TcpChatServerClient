#pragma once


#include<string>
#include"Default.h"

#define	UserIDMaxLength 10

constexpr char  gEndMark = (char)0xff;
 using UserID = MYLIBRARY_API std::string;

struct MYLIBRARY_API Position
{
	float mPosX;
	float mPosY;
	float mPosZ;
};


struct MYLIBRARY_API Character
{
	UserID mUserID;
	Position mPos;
};