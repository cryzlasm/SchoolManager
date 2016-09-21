// InsertCMD.cpp: implementation of the CInsertCMD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Mid.h"
#include "InsertCMD.h"

#include "MidDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInsertCMD::CInsertCMD()
{
    memset(&m_CSInfo, 0, sizeof(m_CSInfo));
    memset(m_szBuf, 0, sizeof(m_szBuf));
}

CInsertCMD::~CInsertCMD()
{

}

BOOL CInsertCMD::SetCMD(MyTagCSInfo& pData)
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

BOOL CInsertCMD::CmdExecute()
{

    MyTagCDataInfo& Info = m_CSInfo.m_Deal.m_BufInfo;

    CString strInsert = TEXT("");
    CString strTmp = TEXT("");
    CString strColumn = TEXT("");

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
        //INSERT INTO t_Student VALUES
        strInsert = strInsert + INSERTALL + t_Student;

        //ƴ���ַ���
        if(Info.cSidLen != 0)
        {
            strColumn = strColumn + SPLITSQL + STUID;
            Concatenation(strTmp, strSrcBuf, Info.cSidLen, dwPos);
        }

        if(Info.cSNameLen != 0)
        {
            strColumn = strColumn + SPLITSQL + STUNAME;
            Concatenation(strTmp, strSrcBuf, Info.cSNameLen, dwPos);
        }

        if(Info.cSgenderLen != 0)
        {
            strColumn = strColumn + SPLITSQL + STUGENDER;
            Concatenation(strTmp, strSrcBuf, Info.cSgenderLen, dwPos);
        }

        //ƴ���ַ���
        if(Info.cCidLen != 0)
        {
            strColumn = strColumn + SPLITSQL + CLSID;
            Concatenation(strTmp, strSrcBuf, Info.cCidLen, dwPos);
        }
    }

/*
    CHAR    cCouIDLen;      //���ȣ��γ�ID      //3
    CHAR    cCouNameLen;    //���ȣ��γ���      //4
*/
    //�γ�
    if(Info.bCourse)
    {
        //INSERT INTO t_Course VALUES
        strInsert = strInsert + INSERTALL + t_Course;

        //ƴ���ַ���
        if(Info.cCouIDLen != 0)
        {
            strColumn = strColumn + SPLITSQL + COUID;
            Concatenation(strTmp, strSrcBuf, Info.cCouIDLen, dwPos);
        }

        if(Info.cCouNameLen != 0)
        {
            strColumn = strColumn + SPLITSQL + COUNAME;
            Concatenation(strTmp, strSrcBuf, Info.cCouNameLen, dwPos);
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
        //INSERT INTO t_Class VALUES
        strInsert = strInsert + INSERTALL + t_Class;

        //ƴ���ַ���
        if(Info.cCidLen != 0)
        {
            strColumn = strColumn + SPLITSQL + CLSID;
            Concatenation(strTmp, strSrcBuf, Info.cCidLen, dwPos);
        }
        
        if(Info.cCnameLen != 0)
        {
            strColumn = strColumn + SPLITSQL + CLSNAME;
            Concatenation(strTmp, strSrcBuf, Info.cCnameLen, dwPos);
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

    //ȥ������Ķ���
    strTmp = strTmp.Right(strTmp.GetLength() - 2);
    strColumn = strColumn.Right(strColumn.GetLength() - 2);

    //����ִ������,�ϳ�SQL�﷨
    strInsert = strInsert + TEXT("(") + strColumn + TEXT(")") 
                          + INSERTVALUE + TEXT("(") + strTmp + TEXT(")");

    //AfxMessageBox(strInsert);

    CMySocket::OutputErrorMsg(strInsert);

    //����Client Socket ��������ͨ��Deal
    MYSOCKET sClient = m_CSInfo.m_Socket;
    PACKET_FLAG  dwFlag = m_CSInfo.m_Deal.m_flag;
    memset(&m_CSInfo, 0, sizeof(m_CSInfo));
    m_CSInfo.m_Socket = sClient;
    m_CSInfo.m_Deal.m_flag = dwFlag;

    //��û�����
    m_CSInfo.m_Deal.m_dwDataSize = strInsert.GetLength() + 1;
    m_CSInfo.m_Deal.m_pszData = strInsert.GetBuffer(0);
    

    //��������
    CDialog* pDlg = ((CMidApp*)AfxGetApp())->m_pThis;
    CMidDlg& Mid = *(CMidDlg*)pDlg;

    //����ͷ��
    Mid.m_Sock.SendAll(Mid.m_Sock.GetMainSock(), (LPTSTR)&m_CSInfo, TagCSInfoSize);

    //���ͺ���
    Mid.m_Sock.SendAll(Mid.m_Sock.GetMainSock(), m_CSInfo.m_Deal.m_pszData, m_CSInfo.m_Deal.m_dwDataSize);


    return TRUE;
}

BOOL CInsertCMD::Concatenation(CString& strStu, CONST CString& strSrcBuf, DWORD dwLen, DWORD& dwPos)
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
    strStu = strStu + SPLITSQL + TEXT("'") + strTmp + TEXT("'");

    return TRUE;
}