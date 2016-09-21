// ServerTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../AddIn/MySocket.h"
#include "../../AddIn/SocketDeal.h"


#include <IOSTREAM>
#include <STRING>
using namespace std;

int main(int argc, char* argv[])
{
    CMySocket Server;
    Server.StartServer(8080);
    
    cout << "开始监听..." << endl;

    if(!Server.Accept())
        return 0;
    
    SOCKET sNew = Server.GetNewSock();
    SOCKADDR_IN addrNew = Server.GetNewAddr();

    cout << "有客户端连接: " 
         << inet_ntoa(addrNew.sin_addr)
         << " : "
         << addrNew.sin_port
         << endl;
    
    
    char szBuf[MAX_RECV_BYTES] = {0};

	while(1)
    {
        if(Server.Recv(sNew, szBuf, TagCMDealSize))
        {
            MyTagCMDeal& CMdeal = *(MyTagCMDeal *)szBuf;
            
            MyTagCDataInfo& Info = CMdeal.m_BufInfo;

            cout << "总长度："   << Info.dwSize      << endl
                 << "SidLen：" << (int)Info.cSidLen     << endl
                 << "SNameLen：" << (int)Info.cSNameLen   << endl
                 << "SgenderLen：" << (int)Info.cSgenderLen << endl
                 << "CouIDLen：" << (int)Info.cCouIDLen   << endl
                 << "CouNameLen：" << (int)Info.cCouNameLen << endl
                 << "CidLen:" << (int)Info.cCidLen     << endl
                 << "cCnameLen:" << (int)Info.cCnameLen   << endl;
            
            if(Server.Recv(sNew, szBuf, CMdeal.m_BufInfo.dwSize))
            {
                CMySocket::OutputErrorMsg(szBuf);
                string str = szBuf;
                cout << str << endl;
            }
        }
        else
        {
            cout << "主机退出" << endl;
            CMySocket::OutputErrorMsg(Server.GetErrorMsg());
            break;
        }
    }

    closesocket(sNew);

    system("pause")  ;  
    
    return 0;
}
