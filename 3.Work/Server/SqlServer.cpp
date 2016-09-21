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
    CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::~CSqlServer: ����������"));
    EndServer();
}

BOOL CSqlServer::StartServer(UINT nSocketPort, 
                             LPCTSTR pszServerInfo, 
                             DWORD dwThreadCount,
                             INT nSocketType,
                             LPCTSTR lpszSocketAddress)
{
    //����Event������
    try
    {
        m_Server.StartServer(nSocketPort, lpszSocketAddress, nSocketType);
        CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::StartServer: ����Event�������ɹ�"));
    }
    catch (int e)
    {
        if(e == RET_ERROR)
        {
            CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::StartServer: ����Event������ʧ��"));
            return FALSE;
        }
    }
    
    //�������ӳ�
    if(!m_Pool.StartPool(pszServerInfo, dwThreadCount))
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::StartServer: �������ӳ�ʧ��"));
        return FALSE;
    }

    //�����̣߳�����Eventģ�ʹ���֪ͨ
    m_pThreadConvert = AfxBeginThread((AFX_THREADPROC)ConvertClientMsgThread, (LPVOID)this);

    return TRUE;
}

DWORD CSqlServer::ConvertClientMsgThread(LPVOID lParam)
{
    //�õ���������
    CSqlServer& This = *(CSqlServer*)lParam;
    
    //�õ�Server
    CEventServer& Server = This.m_Server;
    
    //��������, ����һ������
    MyTagCSInfo CSInfo = {0};
    
    TCHAR szBuf[MAXSHORT] = {0};
    
    while(TRUE)
    {
        //ѯ���߳��Ƿ�������
        if(!Server.IsCanHandleData())
        {
            break;
        }
        
        //�ж�Eventģ���߳��˳���־
        if(!Server.GetExitInfo())
        {
            CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::ConvertClientMsgThread���ȴ�Dlg֪ͨ�˳�"));
            
            //��ͣ800���룬���߳��Ƿ���Ҫ�˳�
            Sleep(3000);
            
            //���Dlg�˳���־
            if(This.GetExitInfo())
            {
                break;
            }
        }
        
        //��ʼ��������
        memset(&CSInfo, 0, sizeof(CSInfo));
        memset(&szBuf, 0, sizeof(szBuf));
        CSInfo.m_Deal.m_pszData = szBuf;
        
        MyTagCSInfo* pInfo = Server.GetRetData();
        
        ASSERT(pInfo != NULL);
        
        //��������, ����һ������
        //����Э��ͷ
        memcpy(&CSInfo, pInfo, TagCSInfoSize);
        
        //���������������ռ�
        if(CSInfo.m_Deal.m_dwDataSize > 0)
        {
            //��������
            memcpy(szBuf, 
                pInfo->m_Deal.m_pszData, 
                pInfo->m_Deal.m_dwDataSize + 1);
        }
        else
        {
            CSInfo.m_Deal.m_pszData = NULL;
        }
        
        //ת����Ӧ��������̳߳�ִ������
        This.JudgeDeal(CSInfo);
        
        //֪ͨ�̣߳��������
        Server.HandleOver(*pInfo);
        
        //����߳��˳�, ���������
        if(!Server.GetExitInfo())
        {
            Server.m_DataLock.UnLock();
        }
        
    }//End while(TRUE)
    
    CMySocket::OutputErrorMsg(TEXT("CSqlServer::ConvertClientMsgThread: �߳��˳��ɹ�"));
    AfxEndThread(This.GetExitInfo());
    
    return 0;
}

BOOL CSqlServer::EndConvertThread()
{
    //�Ƿ�Ϊ��һ�ν���
    static BOOL bIsFirstInto = TRUE;
    
    if(bIsFirstInto)
    {
        //30��
        #define HALF_MINUTE 30000
        
        //�ñ�־λ
        m_bIsExitConvertThread = TRUE;
        
        //����߳��Ѿ�ʵ���������߳�
        if(m_pThreadConvert != NULL && m_pThreadConvert->m_hThread != INVALID_HANDLE_VALUE)
        {
            //�ȴ��߳��˳�,��30��
            DWORD dwRet = WaitForSingleObject(m_pThreadConvert->m_hThread, HALF_MINUTE);
            if(dwRet == WAIT_TIMEOUT)
            {
                CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::EndServer���ȴ��߳��˳���ʱ��"));
                
                //��ֹ�̣߳�
                if(!TerminateThread(m_pThreadConvert->m_hThread, 1))
                {
                    CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::EndServer����ֹ�߳�ʧ�ܣ�����Ȩ�޲���"));
                    
                    //����߳��޷���ֹ,����ͣ��
                    if(m_pThreadConvert->SuspendThread() != -1)
                    {
                        CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::EndServer����ͣ�̣߳���ֹ����Ұָ��"));
                    }
                }//End if(!TerminateThread(m_pThreadConvert->m_hThread, 1))
                
                
                //�ر��߳̾�� AfxBeginThread ���Զ��ر�
                //::CloseHandle(m_pThreadConvert->m_hThread);
                //m_pThreadConvert->ExitInstance();
                //m_pThreadConvert->m_hThread = INVALID_HANDLE_VALUE;
                
            }//End if(dwRet == WAIT_TIMEOUT)
        }//End if(m_pThreadConvert != NULL && m_pThreadConvert->m_hThread != INVALID_HANDLE_VALUE)
        
        bIsFirstInto = FALSE;
        
    }//End if(bIsFirstInto)
    
    CMySocket::OutputErrorMsg(TEXT("CSqlServer::EndServer���߳��˳��ɹ�"));
    
    return TRUE;  
}

BOOL CSqlServer::JudgeDeal(MyTagCSInfo& pData)
{
    //���ɶ�Ӧ��CMD����
    ICommand* pCMD = new CSqlCMD;
    if(pCMD == NULL)
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::JudgeDeal: ����CInsertCMD ����"));
        return FALSE;
    }
    
    CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::JudgeDeal: ��������ɹ�"));
    
    //����CMD����
    if(!pCMD->SetCMD(&pData))
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CSqlServer::JudgeDeal: ��������ʧ��"));
        return FALSE;
    }
    
    //�̳߳�ִ��һ������
    return m_Pool.Handle(pCMD);
}

BOOL CSqlServer::GetExitInfo() CONST
{
    return m_bIsExitConvertThread;
}

//�رշ�����
BOOL CSqlServer::EndServer()
{
    //�رշ�����
    m_Server.EndServer();
    
    //�˳��߳�
    EndConvertThread();
    
    //�����̳߳�
    m_Pool.Destroy();
    return TRUE;
}