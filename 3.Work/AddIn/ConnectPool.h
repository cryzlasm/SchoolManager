// ThreadPool.h: interface for the CThreadPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICOMMAND_H__B5E2F512_83E1_498F_B8EE_028916495C03__INCLUDED_)
#define AFX_ICOMMAND_H__B5E2F512_83E1_498F_B8EE_028916495C03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//***************************************************************************
//����࣬��������һ���࣬������CmdExecute() ����, Ϊִ�еĹ���
//��SetCMD��������������CMD��Ϣ
//***************************************************************************
#define MYLPVOID void *
#define MYBOOL  long

class ICommand  
{
public:
    virtual ~ICommand(){}

    //����CMD��������
    virtual MYBOOL SetCMD(MYLPVOID pData) = 0;

    //ִ��Cmd
    virtual MYBOOL CmdExecute() = 0;
};

//��ʼ��CMD���ݳ�Ա
#define InitCMD(x, y) \
do\
{\
    memset(&x, 0, sizeof(x));\
    memset(y, 0, sizeof(y));\
}while(0)

//ת����������
#define ConvertTagCSInfo(x, y) \
do\
{\
    MyTagCSInfo& x = *(PMyTagCSInfo)y;\
}while(0)

//�Ѵ������������а�
// #define CopyCSInfoToThis(x) \
// do\
// {\
//     memcpy(&m_CSInfo, &(x), TagCSInfoSize);\
//     memcpy(m_szBuf, (x), m_CSInfo.m_Deal.m_dwDataSize + 1);\  
// }while(0)

#endif
//===========================================================================


#if !defined(AFX_THREADPOOL_H__B5E2F512_83E1_498F_B8EE_028916495C03__INCLUDED_)
#define AFX_THREADPOOL_H__B5E2F512_83E1_498F_B8EE_028916495C03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <AFXTEMPL.H>
#include <AFXMT.H>

#include <STRING>

//===============================

#ifdef _UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif
//===============================

//***************************************************************************
//ʹ�ô�ģ���࣬���봴��һ���࣬������CmdExecute() ����
//����ʹ�ö�̬���򴴽�һ������CmdExecute() �����������
//***************************************************************************
class CConnectPoolManager  
{
public:
    CConnectPoolManager();
    virtual ~CConnectPoolManager();
    
    //����һ������
    BOOL Insert(ICommand* Cmd);
    
    //ɾ����������ڵ�
    BOOL RemoveAll();
    
    //ִ������
    BOOL Execute();
    
protected:
    //�ٽ���
    CCriticalSection  m_CsLock; 
    
    //�������
    CList<ICommand*, ICommand*&> m_CmdList;
};
//===========================================================================

//��ʱֻ������
//1. ֪ͨ�߳��˳�����
//2. �ź�������֪ͨ
#define HANDLE_COUNT    2
#define OBJ_FAILED       -1

class CMyConnectPool  
{
public:

    //ʵ������Ĭ��Ϊ5���̵߳��̳߳أ�
    //�����û����봴���̳߳ص��߳�

	CMyConnectPool();

	virtual ~CMyConnectPool();

    //��������,������������м�������
    BOOL Handle(ICommand* pCmd);

    //��ȡ������Ϣ
    static LPCTSTR GetErrorMsg(); 
    
    //�����̳߳�
    BOOL Destroy();

    //���÷�������Ϣ�����������ӳ�
    //ServerInfo Ϊ����SQL����������Ϣ
    //dwThreadCount ���ӳص��߳���Ŀ��Ĭ��Ϊ5
    BOOL StartPool(LPCTSTR pszServerInfo, DWORD dwThreadCount = 5);

    //��ȡ������Ϣ
    LPCTSTR GetErrStr() const;

    //���ӳ��������ݱ�����
    CCriticalSection m_CsLockCMDExec;

    CWinThread** m_ppWinThreadAry;

protected:

    //����һ���̳߳أ�����ʼ����Ĭ��Ϊ5���߳�
    BOOL Create(INT nCount = 5);

    //SQL��������������Ϣ
    tstring m_strServerInfo;

    //������Ϣ
    tstring m_strErrStr;
    
    //�̻߳ص�����
    static DWORD AFX_CDECL ThreadFun(LPVOID lParam);
    
    //�Ƿ�Ϊ��һ�ν���Destory����
    BOOL        m_bIsFirstIntoDestroy;

    //�ź���
    HANDLE      m_hSemaphore;

    //�߳̾������
    HANDLE*     m_pThreadAry;
    DWORD       m_dwThreadCount;
    
    //�����˳���Event�¼�
    HANDLE      m_hExitEvent;
    
    //�ȴ��õľ������
    //�ȴ��˳��¼�m_hExitEvent
    //�ȴ��ź������߳��˳��¼�
    HANDLE      m_hWaitHandle[HANDLE_COUNT];

    //�������
    CConnectPoolManager   m_Manager;
};

#pragma warning( disable : 4146 )
#import "msado15.dll" no_namespace rename("EOF", "adoEOF")



//===============================
//���ڴ���Sql ���ص�����
#include "MySocket.h"
class CHandleSqlReply
{
public:
    CHandleSqlReply();
    ~CHandleSqlReply();

    //�ṩSendAll ����
    static CMySocket m_Sock;
    
    //�ṩת��SqlReply����,RS���ݣ�ppsz��ָ��
    static BOOL ConvertReplyStr(_RecordsetPtr& pRs, _OUT_ MyTagCSInfo& pCSInfo);
    
    //��ʼ�����Ӷ���
    static BOOL InitSqlConnect(LPCTSTR strConnect);

    //�ر�Sql����
    static BOOL CloseSQLConnect();
    
    //ִ��Sql���
    static BOOL ExecSql(LPCTSTR pszSqlMsg, _RecordsetPtr& Rs, tstring& strErrMsg);
    
    //���Ӷ���
    static _ConnectionPtr m_Conn; 
    
    //���������Ϣ
    static void OutputErrorMsg(LPCTSTR strError);

    static LPCTSTR GetErrorMsg(DWORD dwErrID);  

    //ת����־λ
    static BOOL ConvertFlags( _OUT_ MyTagCSInfo& pCSInfo);

    //ת�������־λ
    static BOOL ConvertErrorFlags( _OUT_ MyTagCSInfo& pCSInfo);
};
//===========================================================================

#endif // !defined(AFX_THREADPOOL_H__B5E2F512_83E1_498F_B8EE_028916495C03__INCLUDED_)
