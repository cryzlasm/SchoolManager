// Socket.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MySocket.h"

int main(int argc, char* argv[])
{
    CMySocket Sock;
    CMySocket Sock2;

    Sock.StartServer(8080);
    system("pause");

    Sock2.StartClient(8080, TEXT("127.0.0.1"));
    system("pause");
	return 0;
}
