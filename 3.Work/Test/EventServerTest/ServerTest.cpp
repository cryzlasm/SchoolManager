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
        cout << "��ʼ����..." << endl;
        Server.StartServer(8080);
    
        
        
        while(TRUE)
        {
            //ѯ���߳��Ƿ�������
            if(!Server.IsCanHandleData())
            {
                break;
            }

            MyTagCSInfo* pInfo = Server.GetRetData();

            ASSERT(pInfo != NULL);

            //��������
            MyTagCSInfo& CSInfo = *pInfo;
        
            MyTagCDataInfo& Info = CSInfo.m_Deal.m_BufInfo;
        
            cout << "�ܳ��ȣ�"   << CSInfo.m_Deal.m_dwDataSize << endl
                 << "SidLen��" << (int)Info.cSidLen     << endl
                 << "SNameLen��" << (int)Info.cSNameLen   << endl
                 << "SgenderLen��" << (int)Info.cSgenderLen << endl
                 << "CouIDLen��" << (int)Info.cCouIDLen   << endl
                 << "CouNameLen��" << (int)Info.cCouNameLen << endl
                 << "CidLen:" << (int)Info.cCidLen     << endl
                 << "cCnameLen:" << (int)Info.cCnameLen   << endl;

        
            str = CSInfo.m_Deal.m_pszData;
            cout << str << endl;

            //֪ͨ�̣߳��������
            Server.HandleOver(CSInfo);
        }
    }
    catch(int nErr)
    {
        if(RET_ERROR == nErr)
        {
            cout << "����˿���ʧ��" << endl;   
        }
        
    }

    //closesocket(sNew);

    system("pause")  ;  
    
    return 0;
}
