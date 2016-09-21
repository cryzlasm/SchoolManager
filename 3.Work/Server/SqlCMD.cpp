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
    //ִ��Sql�﷨
    if(!CHandleSqlReply::ExecSql(m_szBuf, m_RS, strErrMsg))
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CSqlCMD::CmdExecute: ִ��Sql���ʧ��"));
        SendInfo(strErrMsg);
        return FALSE;
    }
    
    //ת����ִ��Ϣ
    if(!CHandleSqlReply::ConvertReplyStr(m_RS, m_CSInfo))
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CSqlCMD::CmdExecute: ��ȡSQL������Ϣ������"));
        return FALSE;
    }
    
    CHandleSqlReply::OutputErrorMsg(TEXT("CSqlCMD::CmdExecute��ȡSql������Ϣ���ɹ���"));
    CHandleSqlReply::OutputErrorMsg(m_CSInfo.m_Deal.m_pszData);
    
    SendInfo(strErrMsg);

    CHandleSqlReply::OutputErrorMsg(TEXT("CSqlCMD::CmdExecute��ִ��Ϣ�ɹ�"));
    
    return TRUE;
}

MYBOOL CSqlCMD::SendInfo(tstring& strErrMsg)
{
    if(!strErrMsg.empty())
    {
        //�������ת���ַ���
        LPTSTR pszBuf = new TCHAR[strErrMsg.length() + 1];
        if(pszBuf == NULL)
        {
            CHandleSqlReply::OutputErrorMsg(TEXT("CHandleSqlReply::ConvertReplyStr: ���뻺����ʧ�ܣ�"));
            return FALSE;
        }
    
        memcpy(pszBuf, strErrMsg.c_str(), strErrMsg.length() + 1);
    
        m_CSInfo.m_Deal.m_dwDataSize = strErrMsg.length() + 1;
        m_CSInfo.m_Deal.m_pszData = pszBuf;
    
        CHandleSqlReply::OutputErrorMsg(pszBuf);
        CHandleSqlReply::ConvertErrorFlags(m_CSInfo);
    }

    //����ͷ��
    CHandleSqlReply::m_Sock.SendAll(m_CSInfo.m_MidSock, 
                                    (LPCTSTR)&m_CSInfo, 
                                    TagCSInfoSize);
    
    
    //���ͺ���
    CHandleSqlReply::m_Sock.SendAll(m_CSInfo.m_MidSock, 
                                    m_CSInfo.m_Deal.m_pszData, 
                                    m_CSInfo.m_Deal.m_dwDataSize);

    return TRUE;
}

MYBOOL CSqlCMD::SetCMD(MYLPVOID pData)
{
    //ת������
    MyTagCSInfo& Data = *(PMyTagCSInfo)pData; 
    
    //��������,���浽����
    memcpy(&m_CSInfo, &Data, TagCSInfoSize);
    memcpy(m_szBuf, Data.m_Deal.m_pszData, m_CSInfo.m_Deal.m_dwDataSize + 1);
    CHandleSqlReply::OutputErrorMsg(m_szBuf);
    return TRUE;
}