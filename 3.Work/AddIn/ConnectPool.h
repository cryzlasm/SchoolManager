// ThreadPool.h: interface for the CThreadPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICOMMAND_H__B5E2F512_83E1_498F_B8EE_028916495C03__INCLUDED_)
#define AFX_ICOMMAND_H__B5E2F512_83E1_498F_B8EE_028916495C03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//***************************************************************************
//虚基类，必须派生一个类，类中有CmdExecute() 方法, 为执行的功能
//有SetCMD方法，用于设置CMD信息
//***************************************************************************
#define MYLPVOID void *
#define MYBOOL  long

class ICommand  
{
public:
    virtual ~ICommand(){}

    //设置CMD命令内容
    virtual MYBOOL SetCMD(MYLPVOID pData) = 0;

    //执行Cmd
    virtual MYBOOL CmdExecute() = 0;
};

//初始化CMD数据成员
#define InitCMD(x, y) \
do\
{\
    memset(&x, 0, sizeof(x));\
    memset(y, 0, sizeof(y));\
}while(0)

//转换参数类型
#define ConvertTagCSInfo(x, y) \
do\
{\
    MyTagCSInfo& x = *(PMyTagCSInfo)y;\
}while(0)

//把传入参数和类进行绑定
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
//使用此模版类，必须创建一个类，类中有CmdExecute() 方法
//如需使用多态，则创建一个带有CmdExecute() 方法的虚基类
//***************************************************************************
class CConnectPoolManager  
{
public:
    CConnectPoolManager();
    virtual ~CConnectPoolManager();
    
    //插入一个命令
    BOOL Insert(ICommand* Cmd);
    
    //删除所有命令节点
    BOOL RemoveAll();
    
    //执行命令
    BOOL Execute();
    
protected:
    //临界区
    CCriticalSection  m_CsLock; 
    
    //命令队列
    CList<ICommand*, ICommand*&> m_CmdList;
};
//===========================================================================

//此时只有两个
//1. 通知线程退出的锁
//2. 信号量来的通知
#define HANDLE_COUNT    2
#define OBJ_FAILED       -1

class CMyConnectPool  
{
public:

    //实例对象，默认为5个线程的线程池，
    //可随用户输入创建线程池的线程

	CMyConnectPool();

	virtual ~CMyConnectPool();

    //处理命令,用于在命令队列加入命令
    BOOL Handle(ICommand* pCmd);

    //获取错误信息
    static LPCTSTR GetErrorMsg(); 
    
    //销毁线程池
    BOOL Destroy();

    //设置服务器信息，并开启连接池
    //ServerInfo 为连接SQL服务器的信息
    //dwThreadCount 连接池的线程数目，默认为5
    BOOL StartPool(LPCTSTR pszServerInfo, DWORD dwThreadCount = 5);

    //获取错误信息
    LPCTSTR GetErrStr() const;

    //连接池命令数据保护锁
    CCriticalSection m_CsLockCMDExec;

    CWinThread** m_ppWinThreadAry;

protected:

    //创建一个线程池，（初始化）默认为5个线程
    BOOL Create(INT nCount = 5);

    //SQL服务器的连接信息
    tstring m_strServerInfo;

    //错误信息
    tstring m_strErrStr;
    
    //线程回调函数
    static DWORD AFX_CDECL ThreadFun(LPVOID lParam);
    
    //是否为第一次进入Destory函数
    BOOL        m_bIsFirstIntoDestroy;

    //信号量
    HANDLE      m_hSemaphore;

    //线程句柄数组
    HANDLE*     m_pThreadAry;
    DWORD       m_dwThreadCount;
    
    //用于退出的Event事件
    HANDLE      m_hExitEvent;
    
    //等待用的句柄数组
    //等待退出事件m_hExitEvent
    //等待信号量，线程退出事件
    HANDLE      m_hWaitHandle[HANDLE_COUNT];

    //命令管理
    CConnectPoolManager   m_Manager;
};

#pragma warning( disable : 4146 )
#import "msado15.dll" no_namespace rename("EOF", "adoEOF")



//===============================
//用于处理Sql 返回的数据
#include "MySocket.h"
class CHandleSqlReply
{
public:
    CHandleSqlReply();
    ~CHandleSqlReply();

    //提供SendAll 服务
    static CMySocket m_Sock;
    
    //提供转换SqlReply数据,RS数据，ppsz空指针
    static BOOL ConvertReplyStr(_RecordsetPtr& pRs, _OUT_ MyTagCSInfo& pCSInfo);
    
    //初始化连接对象
    static BOOL InitSqlConnect(LPCTSTR strConnect);

    //关闭Sql连接
    static BOOL CloseSQLConnect();
    
    //执行Sql语句
    static BOOL ExecSql(LPCTSTR pszSqlMsg, _RecordsetPtr& Rs, tstring& strErrMsg);
    
    //连接对象
    static _ConnectionPtr m_Conn; 
    
    //输出错误信息
    static void OutputErrorMsg(LPCTSTR strError);

    static LPCTSTR GetErrorMsg(DWORD dwErrID);  

    //转换标志位
    static BOOL ConvertFlags( _OUT_ MyTagCSInfo& pCSInfo);

    //转换错误标志位
    static BOOL ConvertErrorFlags( _OUT_ MyTagCSInfo& pCSInfo);
};
//===========================================================================

#endif // !defined(AFX_THREADPOOL_H__B5E2F512_83E1_498F_B8EE_028916495C03__INCLUDED_)
