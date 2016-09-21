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

#define MAX_HTTP_HEAD_SIZE      (1024 * 2)  //һ��Httpͷ������2K

#define _IN_        //�������
#define _OUT_       //�������
#define _IN_OUT_    //�����������

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

    LPCTSTR GetErrorMsg();      //��ȡSocketAPI ������Ϣ

    static INT m_nErrorCode;                              //������

    //LPCTSTR strError = "Anythings"  || LPCTSTR strError = GetErrorMsg()
    static  void    OutputErrorMsg(LPCTSTR strError = NULL);      //���������Ϣ
    
    BOOL CloseSocket(SOCKET& sCur);     //�ر�ָ���׽���

    //����Client�� nSocketPort(�������˿�)  lpszSocketAddress(��������ַ) nSocketType(UDP || TCP)
    BOOL StartClient(UINT nSocketPort, LPCTSTR lpszSocketAddress, int nSocketType = SOCK_STREAM);

    //����Server�� nSocketPort(�������˿�)  lpszSocketAddress(��������ַ) nSocketType(UDP || TCP)
	BOOL StartServer(UINT nSocketPort, LPCTSTR lpszSocketAddress = NULL, int nSocketType = SOCK_STREAM);

    //�ȴ����� ,��������ӣ��򷵻�True, 
    //����GetNewSock()��ȡ�ͻ��˵�Socket,����GetNewAddr��ȡ�ͻ��˵�Addr
    BOOL Accept(/*_OUT_ SOCKET& sNewScok, _OUT_ sockaddr_in& NewAddr*/);              

    //Send,  ʧ�ܷ���SOCKET_ERROR���ɵ���GetErrMSG�� �ɹ��򷵻سɹ������ֽ���
    BOOL Send(SOCKET sDstScok, _IN_ LPCTSTR pszBuf, _IN_ DWORD dwSize);     
    
    //Recv��ʧ�ܷ���SOCKET_ERROR���ɵ���GetErrMSG���ɹ��򷵻سɹ���ȡ���ֽ���������0��û�����ݿɶ�
    BOOL Recv(SOCKET sDstScok, _IN_OUT_ LPTSTR pszBuf, _IN_ DWORD dwSize);       

    //�������ݣ�һ���Է���pszBuf���е�����
    BOOL SendAll(SOCKET sDstScok, _IN_ LPCTSTR pszBuf, DWORD dwSize);    
    
    //�������ݣ�һ���Խ���strBuf���е�����
    BOOL RecvAll(SOCKET sDstScok, _IN_OUT_ LPTSTR pszBuf, DWORD dwSize);    


    //��ȡSocket
    SOCKET GetMainSock() CONST;
    SOCKET GetNewSock() CONST;
    
    //��ȡAddr
    sockaddr_in GetMainAddr() CONST;
    sockaddr_in GetNewAddr() CONST;


protected:
    //����һ��Socket    �˿ڣ�  ģʽ��TCP   ��ַ��NULL
    BOOL Create(UINT nSocketPort, LPCTSTR lpszSocketAddress = NULL, int nSocketType = SOCK_STREAM);

    BOOL CreateSocket(int type = SOCK_STREAM, int af = AF_INET, int protocol = 0);    //�����׽���

    //���õ�ַ
    //sockaddr_in& Addr = m_MainAddr, LPCTSTR pIP = TEXT("127.0.0.1"), WORD wPort
    BOOL SetAddr(/*sockaddr_in& Addr = m_MainAddr, */LPCTSTR pIP /*= TEXT("127.0.0.1")*/, WORD wPort);

    //���õ�ַ
    //sockaddr_in& Addr = m_MainAddr, DWORD dwIP = INADDR_ANY, WORD wPort
    BOOL SetAddr(/*sockaddr_in& Addr = m_MainAddr, */DWORD dwIP /*= INADDR_ANY*/, WORD wPort);

    //�󶨶˿�SOCKET sScok = m_MainSock, sockaddr_in& Addr = m_MainAddr
    BOOL Bind(/*SOCKET sScok = m_MainSock, sockaddr_in& Addr = m_MainAddr*/);   

    //���� SOCKET sScok = m_MainSock, int backlog = SOMAXCONN
    BOOL Listen(/*SOCKET sScok = m_MainSock, */int backlog = SOMAXCONN);         

    BOOL Connect(/*SOCKET sScok = m_MainSock, sockaddr_in& Addr = m_MainAddr*/);             //���� 
    
    static UINT AFX_CDECL RecvThread(LPVOID lParam);                     //�߳̽�������

    
    sockaddr_in m_MainAddr;                           //Addr,��Ҫ��Addr
    SOCKET      m_sMainSock;                           //Socket,�����ģʽ��ΪServer���ͻ���ģʽΪ����

    sockaddr_in m_NewAddr;                           //Addr,��Ҫ��Addr
    SOCKET      m_sNewSock;                           //Socket,�����ģʽ��ΪServer���ͻ���ģʽΪ����
    
    CEvent      m_Event;                            //�¼���

    BOOL    m_bIsServer;                    //�Ƿ�Ϊ�����

    //WSAStartup    Begin
    WORD        m_wVersionRequested;        //Socket Ver
    WSADATA     m_wsaData;
    INT         m_err;
    //WSAStartup    End

    BOOL    InitSocketWSA();       //��ʼ���׽��ֻ���
    BOOL    DestroySocketWSA();    //�ر��׽��ֻ���

    LPCTSTR ConvertErrorMsg(int nErrorCode);       //ת��������

};

#endif // !defined(AFX_MYSOCKET_H__D9605F44_2F0B_4976_9B11_CD65507EA11E__INCLUDED_)
