// AlterCMD.cpp: implementation of the CAlterCMD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Mid.h"
#include "AlterCMD.h"

#include "MidDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAlterCMD::CAlterCMD()
{
    memset(&m_CSInfo, 0, sizeof(m_CSInfo));
    memset(m_szBuf, 0, sizeof(m_szBuf));
}

CAlterCMD::~CAlterCMD()
{

}

BOOL CAlterCMD::SetCMD(MyTagCSInfo& pData)
{
    //�����������
    memcpy(&m_CSInfo, &pData, TagCSInfoSize);
    memcpy(m_szBuf, pData.m_Deal.m_pszData, pData.m_Deal.m_dwDataSize + 1);

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

BOOL CAlterCMD::CmdExecute()
{
MyTagCDataInfo& Info = m_CSInfo.m_Deal.m_BufInfo;

    CString strAlter = TEXT("");
    CString strBefore = TEXT("");
    CString strAfter = TEXT("");

    DWORD dwPos = 0;

    CString strSrcBuf = m_szBuf;

    //����
    //CString strWhere = TEXT("WHERE 1 = 1");
    
/*
    CHAR    cSidLen;        //���ȣ�ѧ��ID      //0
    CHAR    cSNameLen;      //���ȣ�ѧ����      //1
    CHAR    cSgenderLen;    //���ȣ�ѧ���Ա�    //2
*/

    /*
    update t_Class set cID = 'cr05', cName = '����5��' 
    where cID like '%cr04%' and cName like '%����4��%'
    */
    //ѧ��
    if(Info.bStu)
    {
        //update t_Student set
        strAlter = strAlter + UPDATESQL + t_Student + SETSQL;
    
        //ƴ���ַ���
        if(Info.nChangeOne != 0)
        {
            ConcatenationAfter(strAfter, STUID, strSrcBuf, Info.cSidLen, dwPos);
            ConcatenationBefore(strBefore, STUID, strSrcBuf, Info.nChangeOne, dwPos);
        }

        if(Info.nChangeTwo != 0)
        {
            ConcatenationAfter(strAfter, STUNAME, strSrcBuf, Info.cSNameLen, dwPos);
            ConcatenationBefore(strBefore, STUNAME, strSrcBuf, Info.nChangeTwo, dwPos);
        }

        if(Info.nChangeThree != 0)
        {
            ConcatenationAfter(strAfter, STUGENDER, strSrcBuf, Info.cSgenderLen, dwPos);
            ConcatenationBefore(strBefore, STUGENDER, strSrcBuf, Info.nChangeThree, dwPos);
        }

        if(Info.nChangeFour != 0)
        {
            ConcatenationAfter(strAfter, CLSID, strSrcBuf, Info.cCidLen, dwPos);
            ConcatenationBefore(strBefore, CLSID, strSrcBuf, Info.nChangeFour, dwPos);
        }
    }

/*
    CHAR    cCouIDLen;      //���ȣ��γ�ID      //3
    CHAR    cCouNameLen;    //���ȣ��γ���      //4
*/
    //�γ�
    if(Info.bCourse)
    {        
        //update t_Course set
        strAlter = strAlter + UPDATESQL + t_Course + SETSQL;

        //ƴ���ַ���
        if(Info.nChangeOne != 0)
        {
            ConcatenationAfter(strAfter, COUID, strSrcBuf, Info.cCouIDLen, dwPos);
            ConcatenationBefore(strBefore, COUID, strSrcBuf, Info.nChangeOne, dwPos);
        }

        if(Info.nChangeTwo != 0)
        {
            ConcatenationAfter(strAfter, COUNAME, strSrcBuf, Info.cCouNameLen, dwPos);
            ConcatenationBefore(strBefore, COUNAME, strSrcBuf, Info.nChangeTwo, dwPos);
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
        //update t_Class set
        strAlter = strAlter + UPDATESQL + t_Class + SETSQL;

        //ƴ���ַ���
        if(Info.nChangeOne != 0)
        {
            ConcatenationAfter(strAfter, CLSID, strSrcBuf, Info.cCidLen, dwPos);
            ConcatenationBefore(strBefore, CLSID, strSrcBuf, Info.nChangeOne, dwPos);
        }
    
        if(Info.nChangeTwo != 0)
        {
            ConcatenationAfter(strAfter, CLSNAME, strSrcBuf, Info.cCnameLen, dwPos);
            ConcatenationBefore(strBefore, CLSNAME, strSrcBuf, Info.nChangeTwo, dwPos);
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

    //ȥ��ͷǰ׺�Ķ�����ո�
    strBefore = strBefore.Mid(2, strBefore.GetLength());
    strAlter = strAlter + strBefore + SQL_WHERE + SQL_ONE + strAfter;

    CMySocket::OutputErrorMsg(strAlter);

    //����Client Socket ��������ͨ��Deal
    MYSOCKET sClient = m_CSInfo.m_Socket;
    PACKET_FLAG  dwFlag = m_CSInfo.m_Deal.m_flag;
    memset(&m_CSInfo, 0, sizeof(m_CSInfo));
    m_CSInfo.m_Socket = sClient;
    m_CSInfo.m_Deal.m_flag = dwFlag;

    //��û�����
    m_CSInfo.m_Deal.m_dwDataSize = strAlter.GetLength() + 1;
    m_CSInfo.m_Deal.m_pszData = strAlter.GetBuffer(0);
    

    //��������
    CDialog* pDlg = ((CMidApp*)AfxGetApp())->m_pThis;
    CMidDlg& Mid = *(CMidDlg*)pDlg;

    //����ͷ��
    Mid.m_Sock.SendAll(Mid.m_Sock.GetMainSock(), (LPTSTR)&m_CSInfo, TagCSInfoSize);

    //���ͺ���
    Mid.m_Sock.SendAll(Mid.m_Sock.GetMainSock(), m_CSInfo.m_Deal.m_pszData, m_CSInfo.m_Deal.m_dwDataSize);

    //AfxMessageBox(strAlter);
    return TRUE;
}

BOOL CAlterCMD::ConcatenationAfter(CString& strDst, CString strColumn, CONST CString& strSrcBuf, DWORD dwLen, DWORD& dwPos)
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
    //And sId like '%tmp%'
    strDst = strDst + TEXT(" AND ") +  strColumn + LIKEBEGIN + strTmp + LIKEEND;

    return TRUE;
}

BOOL CAlterCMD::ConcatenationBefore(CString& strDst, CString strColumn, CONST CString& strSrcBuf, DWORD dwLen, DWORD& dwPos)
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
    //, sId = 'tmp'
    strDst = strDst + TEXT(", ") + strColumn + TEXT(" = '") + strTmp + TEXT("'");
    
    return TRUE;
}