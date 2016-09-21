// QueryCMD.cpp: implementation of the CQueryCMD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Mid.h"
#include "QueryCMD.h"

#include "MidDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQueryCMD::CQueryCMD()
{
    memset(&m_CSInfo, 0, sizeof(m_CSInfo));
    memset(m_szBuf, 0, sizeof(m_szBuf));
}

CQueryCMD::~CQueryCMD()
{

}

BOOL CQueryCMD::SetCMD(MyTagCSInfo& pData)
{
    //�����������
    memcpy(&m_CSInfo, &pData, TagCSInfoSize);
    if(pData.m_Deal.m_pszData != NULL)
    {
        memcpy(m_szBuf, pData.m_Deal.m_pszData, pData.m_Deal.m_dwDataSize + 1);
    }
    
    
    return TRUE;
}


/*
CHAR    cSidLen;        //���ȣ�ѧ��ID      //0
CHAR    cSNameLen;      //���ȣ�ѧ����      //1
CHAR    cSgenderLen;    //���ȣ�ѧ���Ա�    //2
CHAR    cCouIDLen;      //���ȣ��γ�ID      //3
CHAR    cCouNameLen;    //���ȣ��γ���      //4
CHAR    cCidLen;        //���ȣ��༶ID      //5
CHAR    cCnameLen;      //���ȣ��༶��      //6
CHAR    cUnUse;         //δ��              //7
*/

BOOL CQueryCMD::CmdExecute()
{

    MyTagCDataInfo& Info = m_CSInfo.m_Deal.m_BufInfo;

    CString strSelect = TEXT("");

    DWORD dwPos = 0;

    CString strSrcBuf = m_szBuf;

    //����
    //CString strWhere = TEXT("WHERE 1 = 1");
    
/*
    CHAR    cSidLen;        //���ȣ�ѧ��ID      //0
    CHAR    cSNameLen;      //���ȣ�ѧ����      //1
    CHAR    cSgenderLen;    //���ȣ�ѧ���Ա�    //2
*/
    //ѧ��
    if(Info.bStu)
    {
        if(!m_CSInfo.m_Deal.m_dwDataSize)
        {
            strSelect = strSelect + SELECTALL + t_Student;
        }
        else
        {
            //SELECT * FROM t_Student WHERE 1 = 1
            strSelect = strSelect + SELECTALL + t_Student + SQL_WHERE + SQL_ONE;
        

            //ƴ���ַ���
            if(Info.cSidLen != 0)
            {
                Concatenation(strSelect, STUID, strSrcBuf, Info.cSidLen, dwPos);
            }

            if(Info.cSNameLen != 0)
            {
                Concatenation(strSelect, STUNAME, strSrcBuf, Info.cSNameLen, dwPos);
            }

            if(Info.cSgenderLen != 0)
            {
                Concatenation(strSelect, STUGENDER, strSrcBuf, Info.cSgenderLen, dwPos);
            }

            //ƴ���ַ���
            if(Info.cCidLen != 0)
            {
                Concatenation(strSelect, CLSID, strSrcBuf, Info.cCidLen, dwPos);
            }
        }
    }

/*
    CHAR    cCouIDLen;      //���ȣ��γ�ID      //3
    CHAR    cCouNameLen;    //���ȣ��γ���      //4
*/
    //�γ�
    if(Info.bCourse)
    {        
        if(!m_CSInfo.m_Deal.m_dwDataSize)
        {
            strSelect = strSelect + SELECTALL + t_Course;
        }
        else
        {
            //SELECT * FROM t_Course  WHERE 1 = 1
            strSelect = strSelect + SELECTALL + t_Course + SQL_WHERE + SQL_ONE;
        

            //ƴ���ַ���
            if(Info.cCouIDLen != 0)
            {
                Concatenation(strSelect, COUID, strSrcBuf, Info.cCouIDLen, dwPos);
            }

            if(Info.cCouNameLen != 0)
            {
                Concatenation(strSelect, COUNAME, strSrcBuf, Info.cCouNameLen, dwPos);
            }
        }
    }
/*
    CHAR    cCidLen;        //���ȣ��༶ID      //5
    CHAR    cCnameLen;      //���ȣ��༶��      //6
    CHAR    cUnUse;         //δ��              //7
*/
    //�༶
    if(Info.bClass)
    {        
        if(!m_CSInfo.m_Deal.m_dwDataSize)
        {
            strSelect = strSelect + SELECTALL + t_Class;
        }
        else
        {
            //SELECT * FROM t_Class  WHERE 1 = 1
            strSelect = strSelect + SELECTALL + t_Class + SQL_WHERE + SQL_ONE;
    

            //ƴ���ַ���
            if(Info.cCidLen != 0)
            {
                Concatenation(strSelect, CLSID, strSrcBuf, Info.cCidLen, dwPos);
            }
        
            if(Info.cCnameLen != 0)
            {
                Concatenation(strSelect, CLSNAME, strSrcBuf, Info.cCnameLen, dwPos);
            }
        }
    }

    //ѡ��
//     if(Info.cCidLen != 0 || Info.cCnameLen != 0)
//     {
//         
//         strTable += t_Sel_Cou;
// 
//         bIsFirstTable = FALSE;
//     }

    //����ִ������
    //CString strExec = TEXT("");

    CMySocket::OutputErrorMsg(strSelect);

    //����Client Socket ��������ͨ��Deal
    MYSOCKET sClient = m_CSInfo.m_Socket;
    PACKET_FLAG  dwFlag = m_CSInfo.m_Deal.m_flag;
    memset(&m_CSInfo, 0, sizeof(m_CSInfo));
    m_CSInfo.m_Socket = sClient;
    m_CSInfo.m_Deal.m_flag = dwFlag;

    //��û�����
    m_CSInfo.m_Deal.m_dwDataSize = strSelect.GetLength() + 1;
    m_CSInfo.m_Deal.m_pszData = strSelect.GetBuffer(0);
    

    //��������
    CDialog* pDlg = ((CMidApp*)AfxGetApp())->m_pThis;
    CMidDlg& Mid = *(CMidDlg*)pDlg;

    //����ͷ��
    Mid.m_Sock.SendAll(Mid.m_Sock.GetMainSock(), (LPTSTR)&m_CSInfo, TagCSInfoSize);

    //���ͺ���
    Mid.m_Sock.SendAll(Mid.m_Sock.GetMainSock(), m_CSInfo.m_Deal.m_pszData, m_CSInfo.m_Deal.m_dwDataSize);

    //AfxMessageBox(strSelect);
    return TRUE;
}


BOOL CQueryCMD::Concatenation(CString& strDst, CString strColumn, CONST CString& strSrcBuf, DWORD dwLen, DWORD& dwPos)
{
    //��ȡ�û��ַ���
    CString strTmp = TEXT("");

    if(dwLen <= 1)
    {
        TCHAR ch = strSrcBuf.GetAt(dwPos);
        strTmp = ch;
    }
    else
    {
        strTmp = strSrcBuf.Mid(dwPos, dwLen);
    }

    dwPos += dwLen;
    
    //ƴ���ַ���
    //SELECT * FROM Table WHERE 1 = 1 AND strColumn =  LIKE '%str%\'
    strDst = strDst + TEXT(" AND ") +  strColumn + LIKEBEGIN + strTmp + LIKEEND;

    return TRUE;
}