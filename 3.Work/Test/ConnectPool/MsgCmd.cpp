// MsgCmd.cpp: implementation of the CMsgCmd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MsgCmd.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CMsgCmd::CMsgCmd()
{
    InitCMD(m_szBuf, &m_CSInfo);
}

CMsgCmd::~CMsgCmd()
{
    if(m_CSInfo.m_Deal.m_pszData != NULL)
    {
        delete m_CSInfo.m_Deal.m_pszData;
        m_CSInfo.m_Deal.m_pszData = NULL;
    }
    
}

MYBOOL CMsgCmd::CmdExecute()
{
    if(!CHandleSqlReply::ExecSql(m_szBuf, m_RS))
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CMsgCmd::CmdExecute: 执行Sql语句失败"));
        return FALSE;
    }
    
    if(!CHandleSqlReply::ConvertReplyStr(m_RS, m_CSInfo))
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CMsgCmd::CmdExecute: 获取SQL返回信息：错误"));
        return FALSE;
    }

    

    CHandleSqlReply::OutputErrorMsg(TEXT("获取S返回信息，成功！"));
    char szBuf[MAXBYTE] = {0};
    wsprintf(szBuf, TEXT("长度：%d"), m_CSInfo.m_Deal.m_dwDataSize);
    CHandleSqlReply::OutputErrorMsg(szBuf);
    CHandleSqlReply::OutputErrorMsg(m_CSInfo.m_Deal.m_pszData);


    return TRUE;
}

MYBOOL CMsgCmd::SetCMD(MYLPVOID pData)
{
    strcpy(m_szBuf, ("SELECT * FROM t_Student"));
    return TRUE;
}