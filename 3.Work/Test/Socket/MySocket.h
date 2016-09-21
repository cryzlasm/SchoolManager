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

#define MAX_CLIENT_CONNECT (64)
#define MAX_RECV_BYTES      (MAXSHORT)

#define MAX_HTTP_HEAD_SIZE      (1024 * 2)  //一般Http头不超过2K

#define _IN_        //输入参数
#define _OUT_       //输出参数
#define _IN_OUT_    //输入输出参数

//===============================

#ifdef _UNICODE
    #define tstring wstring
#else
    #define tstring string
#endif
//===============================

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
    BOOL StartClient(UINT nSocketPort, LPCTSTR lpszSocketAddress, int nSocketType = SOCK_STREAM);

    //创建Server， nSocketPort(服务器端口)  lpszSocketAddress(服务器地址) nSocketType(UDP || TCP)
	BOOL StartServer(UINT nSocketPort, LPCTSTR lpszSocketAddress = NULL, int nSocketType = SOCK_STREAM);

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
    
    static UINT AFX_CDECL RecvThread(LPVOID lParam);                     //线程接收数据

    
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

#endif // !defined(AFX_MYSOCKET_H__D9605F44_2F0B_4976_9B11_CD65507EA11E__INCLUDED_)
