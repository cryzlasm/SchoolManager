// SqlServer.cpp: implementation of the SqlServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SqlServer.h"

#include "SqlCMD.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSqlServer::CSqlServer()
{
    m_pThreadConvert = NULL;
    
    m_bIsExitConvertThread = FALSE;
}

CSqlServer::~CSqlServer()
{
    CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::~CSqlServer: 结束服务器"));
    EndServer();
}

BOOL CSqlServer::StartServer(UINT nSocketPort, 
                             LPCTSTR pszServerInfo, 
                             DWORD dwThreadCount,
                             INT nSocketType,
                             LPCTSTR lpszSocketAddress)
{
    //启动Event服务器
    try
    {
        m_Server.StartServer(nSocketPort, lpszSocketAddress, nSocketType);
        CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::StartServer: 启动Event服务器成功"));
    }
    catch (int e)
    {
        if(e == RET_ERROR)
        {
            CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::StartServer: 启动Event服务器失败"));
            return FALSE;
        }
    }
    
    //启动连接池
    if(!m_Pool.StartPool(pszServerInfo, dwThreadCount))
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::StartServer: 启动连接池失败"));
        return FALSE;
    }

    //开启线程，监听Event模型传出通知
    m_pThreadConvert = AfxBeginThread((AFX_THREADPROC)ConvertClientMsgThread, (LPVOID)this);

    return TRUE;
}

DWORD CSqlServer::ConvertClientMsgThread(LPVOID lParam)
{
    //得到窗口数据
    CSqlServer& This = *(CSqlServer*)lParam;
    
    //得到Server
    CEventServer& Server = This.m_Server;
    
    //处理数据, 拷贝一份数据
    MyTagCSInfo CSInfo = {0};
    
    TCHAR szBuf[MAXSHORT] = {0};
    
    while(TRUE)
    {
        //询问线程是否有数据
        if(!Server.IsCanHandleData())
        {
            break;
        }
        
        //判断Event模型线程退出标志
        if(!Server.GetExitInfo())
        {
            CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::ConvertClientMsgThread：等待Dlg通知退出"));
            
            //暂停800毫秒，看线程是否需要退出
            Sleep(3000);
            
            //检查Dlg退出标志
            if(This.GetExitInfo())
            {
                break;
            }
        }
        
        //初始化缓冲区
        memset(&CSInfo, 0, sizeof(CSInfo));
        memset(&szBuf, 0, sizeof(szBuf));
        CSInfo.m_Deal.m_pszData = szBuf;
        
        MyTagCSInfo* pInfo = Server.GetRetData();
        
        ASSERT(pInfo != NULL);
        
        //处理数据, 拷贝一份数据
        //拷贝协议头
        memcpy(&CSInfo, pInfo, TagCSInfoSize);
        
        //如果有数据则申请空间
        if(CSInfo.m_Deal.m_dwDataSize > 0)
        {
            //拷贝数据
            memcpy(szBuf, 
                pInfo->m_Deal.m_pszData, 
                pInfo->m_Deal.m_dwDataSize + 1);
        }
        else
        {
            CSInfo.m_Deal.m_pszData = NULL;
        }
        
        //转换对应的命令，给线程池执行命令
        This.JudgeDeal(CSInfo);
        
        //通知线程，处理结束
        Server.HandleOver(*pInfo);
        
        //如果线程退出, 本次则解锁
        if(!Server.GetExitInfo())
        {
            Server.m_DataLock.UnLock();
        }
        
    }//End while(TRUE)
    
    CMySocket::OutputErrorMsg(TEXT("CSqlServer::ConvertClientMsgThread: 线程退出成功"));
    AfxEndThread(This.GetExitInfo());
    
    return 0;
}

BOOL CSqlServer::EndConvertThread()
{
    //是否为第一次进入
    static BOOL bIsFirstInto = TRUE;
    
    if(bIsFirstInto)
    {
        //30秒
        #define HALF_MINUTE 30000
        
        //置标志位
        m_bIsExitConvertThread = TRUE;
        
        //如果线程已经实例，则处理线程
        if(m_pThreadConvert != NULL && m_pThreadConvert->m_hThread != INVALID_HANDLE_VALUE)
        {
            //等待线程退出,等30秒
            DWORD dwRet = WaitForSingleObject(m_pThreadConvert->m_hThread, HALF_MINUTE);
            if(dwRet == WAIT_TIMEOUT)
            {
                CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::EndServer：等待线程退出超时！"));
                
                //终止线程，
                if(!TerminateThread(m_pThreadConvert->m_hThread, 1))
                {
                    CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::EndServer：终止线程失败，可能权限不够"));
                    
                    //如果线程无法终止,则暂停掉
                    if(m_pThreadConvert->SuspendThread() != -1)
                    {
                        CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::EndServer：暂停线程，防止操作野指针"));
                    }
                }//End if(!TerminateThread(m_pThreadConvert->m_hThread, 1))
                
                
                //关闭线程句柄 AfxBeginThread 会自动关闭
                //::CloseHandle(m_pThreadConvert->m_hThread);
                //m_pThreadConvert->ExitInstance();
                //m_pThreadConvert->m_hThread = INVALID_HANDLE_VALUE;
                
            }//End if(dwRet == WAIT_TIMEOUT)
        }//End if(m_pThreadConvert != NULL && m_pThreadConvert->m_hThread != INVALID_HANDLE_VALUE)
        
        bIsFirstInto = FALSE;
        
    }//End if(bIsFirstInto)
    
    CMySocket::OutputErrorMsg(TEXT("CSqlServer::EndServer：线程退出成功"));
    
    return TRUE;  
}

BOOL CSqlServer::JudgeDeal(MyTagCSInfo& pData)
{
    //生成对应的CMD命令
    ICommand* pCMD = new CSqlCMD;
    if(pCMD == NULL)
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::JudgeDeal: 生成CInsertCMD 错误"));
        return FALSE;
    }
    
    CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::JudgeDeal: 生成命令成功"));
    
    //设置CMD命令
    if(!pCMD->SetCMD(&pData))
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::JudgeDeal: 设置命令失败"));
        return FALSE;
    }
    
    //线程池执行一个命令
    return m_Pool.Handle(pCMD);
}

BOOL CSqlServer::GetExitInfo() CONST
{
    return m_bIsExitConvertThread;
}

//关闭服务器
BOOL CSqlServer::EndServer()
{
    //关闭服务器
    m_Server.EndServer();
    
    //退出线程
    EndConvertThread();
    
    //销毁线程池
    m_Pool.Destroy();
    return TRUE;
}