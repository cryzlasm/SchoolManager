// SqlServer.h: interface for the SqlServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SQLSERVER_H__24DDFCA9_435D_475A_A296_4819D5C3AF11__INCLUDED_)
#define AFX_SQLSERVER_H__24DDFCA9_435D_475A_A296_4819D5C3AF11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../AddIn/MySocket.h"
#include "../AddIn/ConnectPool.h"

class CSqlServer  
{
public:
	CSqlServer();
	virtual ~CSqlServer();

    //���������,   _IN_
    //�˿ڣ�        _IN_
    //SQL������Ϣ   (����UDL��������)�� 
    //���ӳ�����    (Ĭ��Ϊ5)
    //Socketģʽ��  (Ĭ����ʽ)
    //�����IP��    (Ĭ��Ϊ����)
    BOOL StartServer(UINT nSocketPort, 
                     LPCTSTR pszServerInfo, 
                     DWORD dwThreadCount = 5,
                     INT nSocketType = SOCK_STREAM,
                     LPCTSTR lpszSocketAddress = NULL);


    //�رշ�����
    BOOL EndServer();


    //�ж����ݰ������ĸ�����
    BOOL JudgeDeal(MyTagCSInfo& pData);

    //�߳��˳���־
    BOOL m_bIsExitConvertThread;
    BOOL GetExitInfo() CONST;

    //Eventģ��Server
    CEventServer    m_Server;

    //���ӳ�
    CMyConnectPool  m_Pool;

protected:
    //���ڴ���Eventģ�ʹ���������
    static DWORD ConvertClientMsgThread(LPVOID lParam);

    //���ڼ���Eventģ�Ͷ���������
    CWinThread*     m_pThreadConvert;

    //�˳�ConvertThread
    BOOL EndConvertThread();

};

#endif // !defined(AFX_SQLSERVER_H__24DDFCA9_435D_475A_A296_4819D5C3AF11__INCLUDED_)
