// MySocket.h: interface for the CMySocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYSOCKET_H__D9605F44_2F0B_4976_9B11_CD65507EA11E__INCLUDED_)
#define AFX_MYSOCKET_H__D9605F44_2F0B_4976_9B11_CD65507EA11E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AFXMT.H>
#include <AFXWIN.H>

#include <Winsock2.h>
#include <STRING>

#pragma comment(lib, "Ws2_32.lib")


//#include "../AddIn/MyEvent.h"
#include "../AddIn/SocketDeal.h"

#define MAX_CLIENT_CONNECT (64)
#define MAX_RECV_BYTES      (MAXSHORT)

#define MAX_HTTP_HEAD_SIZE      (1024 * 2)  //一般Http头不超过2K

#define _IN_        //输入参数
#define _OUT_       //输出参数
#define _IN_OUT_    //输入输出参数

#define RET_ERROR   EOF //用于抛出函数执行异常，

// ===============================
// 
// #ifdef _UNICODE
//     #define tstring wstring
// #else
//     #define tstring string
// #endif
// ===============================

//=============================CMySocket======================================
class CMySocket  
{
public:

	CMySocket();
	virtual ~CMySocket();

    LPCTSTR GetErrorMsg();      //获取SocketAPI 错误信息

    static INT m_nErrorCode;                              //错误码

    //LPCTSTR strError = "Anythings"  || LPCTSTR strError = GetErrorMsg()
    static  void    OutputErrorMsg(LPCTSTR strError = NULL);      //输出调试信息
    
    BOOL CloseSocket(SOCKET& sCur);     //关闭指定套接字

    //创建Client， nSocketPort(服务器端口)  lpszSocketAddress(服务器地址) nSocketType(UDP || TCP)
    BOOL StartClient(UINT nSocketPort, LPCTSTR lpszSocketAddress, INT nSocketType = SOCK_STREAM);

    //创建Server， nSocketPort(服务器端口)  lpszSocketAddress(服务器地址) nSocketType(UDP || TCP)
	BOOL StartServer(UINT nSocketPort, LPCTSTR lpszSocketAddress = NULL, INT nSocketType = SOCK_STREAM);

    //等待连接 ,如果有连接，则返回True, 
    //调用GetNewSock()获取客户端的Socket,调用GetNewAddr获取客户端的Addr
    BOOL Accept(/*_OUT_ SOCKET& sNewScok, _OUT_ sockaddr_in& NewAddr*/);              

    //Send,  失败返回SOCKET_ERROR，可调用GetErrMSG， 成功则返回成功发送字节数
    BOOL Send(SOCKET sDstScok, _IN_ LPCTSTR pszBuf, _IN_ DWORD dwSize);     
    
    //Recv，失败返回SOCKET_ERROR，可调用GetErrMSG，成功则返回成功读取的字节数，返回0则没有数据可读
    BOOL Recv(SOCKET sDstScok, _IN_OUT_ LPTSTR pszBuf, _IN_ DWORD dwSize);       

    //发送数据，一次性发送pszBuf所有的数据
    BOOL SendAll(SOCKET sDstScok, _IN_ LPCTSTR pszBuf, DWORD dwSize);    
    
    //接收数据，一次性接收strBuf所有的数据
    BOOL RecvAll(SOCKET sDstScok, _IN_OUT_ LPTSTR pszBuf, DWORD dwSize);    


    //获取Socket
    SOCKET GetMainSock() CONST;
    SOCKET GetNewSock() CONST;
    
    //获取Addr
    sockaddr_in GetMainAddr() CONST;
    sockaddr_in GetNewAddr() CONST;

    //使用完成，提供销毁Socket功能
    BOOL SocketOver();


protected:
    //创建一个Socket    端口：  模式：TCP   地址：NULL
    BOOL Create(UINT nSocketPort, LPCTSTR lpszSocketAddress = NULL, int nSocketType = SOCK_STREAM);

    BOOL CreateSocket(int type = SOCK_STREAM, int af = AF_INET, int protocol = 0);    //创建套接字

    //设置地址
    //sockaddr_in& Addr = m_MainAddr, LPCTSTR pIP = TEXT("127.0.0.1"), WORD wPort
    BOOL SetAddr(/*sockaddr_in& Addr = m_MainAddr, */LPCTSTR pIP /*= TEXT("127.0.0.1")*/, WORD wPort);

    //设置地址
    //sockaddr_in& Addr = m_MainAddr, DWORD dwIP = INADDR_ANY, WORD wPort
    BOOL SetAddr(/*sockaddr_in& Addr = m_MainAddr, */DWORD dwIP /*= INADDR_ANY*/, WORD wPort);

    //绑定端口SOCKET sScok = m_MainSock, sockaddr_in& Addr = m_MainAddr
    BOOL Bind(/*SOCKET sScok = m_MainSock, sockaddr_in& Addr = m_MainAddr*/);   

    //监听 SOCKET sScok = m_MainSock, int backlog = SOMAXCONN
    BOOL Listen(/*SOCKET sScok = m_MainSock, */int backlog = SOMAXCONN);         

    BOOL Connect(/*SOCKET sScok = m_MainSock, sockaddr_in& Addr = m_MainAddr*/);             //连接 

    sockaddr_in m_MainAddr;                           //Addr,主要的Addr
    SOCKET      m_sMainSock;                           //Socket,服务端模式，为Server，客户端模式为本地

    sockaddr_in m_NewAddr;                           //Addr,主要的Addr
    SOCKET      m_sNewSock;                           //Socket,服务端模式，为Server，客户端模式为本地
    
    CEvent      m_Event;                            //事件锁

    BOOL    m_bIsServer;                    //是否为服务端

    //WSAStartup    Begin
    WORD        m_wVersionRequested;        //Socket Ver
    WSADATA     m_wsaData;
    INT         m_err;
    //WSAStartup    End

    BOOL    InitSocketWSA();       //初始化套接字环境
    BOOL    DestroySocketWSA();    //关闭套接字环境

    LPCTSTR ConvertErrorMsg(int nErrorCode);       //转换错误码

};
//=============================CMySocket======================================

//=============================CMyEvent=======================================

//#include <Windows.h>

#pragma warning (disable:4290)      //忽略警告，警告因为构造抛异常
class CMyEvent  
{
public:
    
    //Throw BOOL . FALSE 表示创建Event失败
    CMyEvent(BOOL bInitiallyOwn = FALSE,    //初始是否为解锁状态，FALSE 为上锁   
        BOOL bManualReset = FALSE,     //是否为自动锁，FALSE为自动锁
        LPCTSTR lpszNAme = NULL,       //锁的名字
        LPSECURITY_ATTRIBUTES lpsaAttribute = NULL     //安全属性
        ) throw(BOOL);   
    
    virtual ~CMyEvent();
    
    //上锁
    BOOL Lock(DWORD dwTimeout = INFINITE);
    
    //解锁
    BOOL UnLock();
    
    //检查状态
    BOOL IsLock() CONST;
    
    BOOL ResetEvent();
    BOOL PulseEvent();
    BOOL SetEvent();
    
    operator HANDLE() CONST;    //返回Evenet句柄
    HANDLE  m_hObject;          //Event的句柄
    
protected:
    BOOL m_bIsAutoLock;         //是否为自动锁
    BOOL m_bIsLock;             //是否已经上锁
    
};
//=============================CMyEvent=======================================

//=============================EventServer====================================
//Event Select模型，只负责接收客户端的连接和接收数据
class CEventServer
{
public:
    CEventServer();
    ~CEventServer();

    //创建Server， nSocketPort(服务器端口)  lpszSocketAddress(服务器地址) nSocketType(UDP || TCP),
    //throw(int), int为 RET_ERROR 则创建Event模型失败
    BOOL StartServer(UINT nSocketPort, LPCTSTR lpszSocketAddress = NULL, INT nSocketType = SOCK_STREAM) throw(int);  

    //服务端退出
    BOOL EndServer();

    //数据锁
    CMyEvent        m_DataLock;     //在操作数据时，解锁
    CMyEvent        m_ThreadLock;   //在操作数据时，上锁

    //是否可以操作数据,会阻塞当前线程，
    //在数据完成获取之后, 线程通知解锁
    //调用之后，必须调用HandleOver()返回
    BOOL IsCanHandleData();

    //通知Event处理数据完成
    //操作结束，通知线程解锁，通知外部加锁
    //配合IsCanHandleData()使用
    BOOL HandleOver(MyTagCSInfo& CSInfo);

    //对外的接口数据
    PMyTagCSInfo GetRetData();

    //获取m_bIsNoExit状态
    BOOL GetExitInfo() CONST;
protected:

    //退出标志
    BOOL  m_bIsNoExit;

    //对外的数据接口
    //LPTSTR          m_pszData;
    PMyTagCSInfo    m_pInfoRet;

    //用于检查IsCanHandleData()是否配合HandleOver()使用
    BOOL            m_bDataIsLock;

    //每个客户端对应一个用于接收数据的缓存
    MyTagCSInfo     m_SRInfo[WSA_MAXIMUM_WAIT_EVENTS];          

    //Event模型线程
    static DWORD ThreadFunc(LPVOID lparam);

    //用于线程中获取连接数据
    BOOL EventAcceptOfThread(int nIndex);
    
    //用于线程中读数据
    BOOL EventReadOfThread(int nIndex);   

    //用于线程中结束连接
    BOOL EventCloseOfThread(int nIndex);

    //结束连接时，需要保护下边的数组
    CCriticalSection    m_ThreadCS;


    CMySocket       m_Sock;     //Socket实例

    CWinThread*     m_pEventThread;     //Event事件线程句柄


    WSAEVENT        m_hEventAry[WSA_MAXIMUM_WAIT_EVENTS];       //Event模型
    SOCKET          m_hSocketAry[WSA_MAXIMUM_WAIT_EVENTS];      //Socket数组
    sockaddr_in     m_addrClient[WSA_MAXIMUM_WAIT_EVENTS];      //Client Addr
    DWORD           m_dwCount;                                  //已连接数目
};
//=============================EventServer====================================

#endif // !defined(AFX_MYSOCKET_H__D9605F44_2F0B_4976_9B11_CD65507EA11E__INCLUDED_)
