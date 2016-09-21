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

    //开启服务端,   _IN_
    //端口，        _IN_
    //SQL连接信息   (可用UDL快速设置)， 
    //连接池数量    (默认为5)
    //Socket模式，  (默认流式)
    //服务端IP，    (默认为本机)
    BOOL StartServer(UINT nSocketPort, 
                     LPCTSTR pszServerInfo, 
                     DWORD dwThreadCount = 5,
                     INT nSocketType = SOCK_STREAM,
                     LPCTSTR lpszSocketAddress = NULL);


    //关闭服务器
    BOOL EndServer();


    //判断数据包属于哪个命令
    BOOL JudgeDeal(MyTagCSInfo& pData);

    //线程退出标志
    BOOL m_bIsExitConvertThread;
    BOOL GetExitInfo() CONST;

    //Event模型Server
    CEventServer    m_Server;

    //连接池
    CMyConnectPool  m_Pool;

protected:
    //用于处理Event模型传出的数据
    static DWORD ConvertClientMsgThread(LPVOID lParam);

    //用于监听Event模型丢出的数据
    CWinThread*     m_pThreadConvert;

    //退出ConvertThread
    BOOL EndConvertThread();

};

#endif // !defined(AFX_SQLSERVER_H__24DDFCA9_435D_475A_A296_4819D5C3AF11__INCLUDED_)
