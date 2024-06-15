#pragma once

#include<WinSock2.h>
#include<Windows.h>

#include<iostream>

#include"Packet.h"
#include<memory>



MYLIBRARY_API std::unique_ptr<Packet> RecvPacket(SOCKET socket);




MYLIBRARY_API std::unique_ptr<Packet> CreatePacket(unsigned long length, EPACKETID packetID);
