
#include"Packet.h"
#include"Client.h"
int main()
{
	Client clinet;
	
	if (clinet.Initialize())
	{

		clinet.Run();
	}


}