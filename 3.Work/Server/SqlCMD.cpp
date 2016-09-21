// SqlCMD.cpp: implementation of the CSqlCMD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SqlCMD.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSqlCMD::CSqlCMD()
{
    memset(&m_CSInfo, 0, sizeof(m_szBuf)); 
    memset(m_szBuf, 0, sizeof(m_szBuf));
    m_CSInfo.m_Deal.m_pszData = NULL;
}

CSqlCMD::~CSqlCMD()
{
    if(m_CSInfo.m_Deal.m_pszData != NULL)
    {
        delete m_CSInfo.m_Deal.m_pszData;
        m_CSInfo.m_Deal.m_pszData = NULL;
    }
}

MYBOOL CSqlCMD::CmdExecute()
{
    tstring strErrMsg = TEXT("");
    //执行Sql语法
    if(!CHandleSqlReply::ExecSql(m_szBuf, m_RS, strErrMsg))
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CSqlCMD::CmdExecute: 执行Sql语句失败"));
        SendInfo(strErrMsg);
        return FALSE;
    }
    
    //转换回执信息
    if(!CHandleSqlReply::ConvertReplyStr(m_RS, m_CSInfo))
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CSqlCMD::CmdExecute: 获取SQL返回信息：错误"));
        return FALSE;
    }
    
    CHandleSqlReply::OutputErrorMsg(TEXT("CSqlCMD::CmdExecute获取Sql返回信息，成功！"));
    CHandleSqlReply::OutputErrorMsg(m_CSInfo.m_Deal.m_pszData);
    
    SendInfo(strErrMsg);

    CHandleSqlReply::OutputErrorMsg(TEXT("CSqlCMD::CmdExecute回执信息成功"));
    
    return TRUE;
}

MYBOOL CSqlCMD::SendInfo(tstring& strErrMsg)
{
    if(!strErrMsg.empty())
    {
        //处理完毕转换字符串
        LPTSTR pszBuf = new TCHAR[strErrMsg.length() + 1];
        if(pszBuf == NULL)
        {
            CHandleSqlReply::OutputErrorMsg(TEXT("CHandleSqlReply::ConvertReplyStr: 申请缓冲区失败！"));
            return FALSE;
        }
    
        memcpy(pszBuf, strErrMsg.c_str(), strErrMsg.length() + 1);
    
        m_CSInfo.m_Deal.m_dwDataSize = strErrMsg.length() + 1;
        m_CSInfo.m_Deal.m_pszData = pszBuf;
    
        CHandleSqlReply::OutputErrorMsg(pszBuf);
        CHandleSqlReply::ConvertErrorFlags(m_CSInfo);
    }

    //发送头部
    CHandleSqlReply::m_Sock.SendAll(m_CSInfo.m_MidSock, 
                                    (LPCTSTR)&m_CSInfo, 
                                    TagCSInfoSize);
    
    
    //发送后续
    CHandleSqlReply::m_Sock.SendAll(m_CSInfo.m_MidSock, 
                                    m_CSInfo.m_Deal.m_pszData, 
                                    m_CSInfo.m_Deal.m_dwDataSize);

    return TRUE;
}

MYBOOL CSqlCMD::SetCMD(MYLPVOID pData)
{
    //转换数据
    MyTagCSInfo& Data = *(PMyTagCSInfo)pData; 
    
    //拷贝数据,缓存到类中
    memcpy(&m_CSInfo, &Data, TagCSInfoSize);
    memcpy(m_szBuf, Data.m_Deal.m_pszData, m_CSInfo.m_Deal.m_dwDataSize + 1);
    CHandleSqlReply::OutputErrorMsg(m_szBuf);
    return TRUE;
}