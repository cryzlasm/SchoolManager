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
    CEventServer Server;
    string str = "";

    try
    {
        cout << "开始监听..." << endl;
        Server.StartServer(8080);
    
        
        
        while(TRUE)
        {
            //询问线程是否有数据
            if(!Server.IsCanHandleData())
            {
                break;
            }

            MyTagCSInfo* pInfo = Server.GetRetData();

            ASSERT(pInfo != NULL);

            //处理数据
            MyTagCSInfo& CSInfo = *pInfo;
        
            MyTagCDataInfo& Info = CSInfo.m_Deal.m_BufInfo;
        
            cout << "总长度："   << CSInfo.m_Deal.m_dwDataSize << endl
                 << "SidLen：" << (int)Info.cSidLen     << endl
                 << "SNameLen：" << (int)Info.cSNameLen   << endl
                 << "SgenderLen：" << (int)Info.cSgenderLen << endl
                 << "CouIDLen：" << (int)Info.cCouIDLen   << endl
                 << "CouNameLen：" << (int)Info.cCouNameLen << endl
                 << "CidLen:" << (int)Info.cCidLen     << endl
                 << "cCnameLen:" << (int)Info.cCnameLen   << endl;

        
            str = CSInfo.m_Deal.m_pszData;
            cout << str << endl;

            //通知线程，处理结束
            Server.HandleOver(CSInfo);
        }
    }
    catch(int nErr)
    {
        if(RET_ERROR == nErr)
        {
            cout << "服务端开启失败" << endl;   
        }
        
    }

    //closesocket(sNew);

    system("pause")  ;  
    
    return 0;
}
