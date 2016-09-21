// DelCMD.cpp: implementation of the CDelCMD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Mid.h"
#include "DelCMD.h"

#include "MidDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDelCMD::CDelCMD()
{
    memset(&m_CSInfo, 0, sizeof(m_CSInfo));
    memset(m_szBuf, 0, sizeof(m_szBuf));
}

CDelCMD::~CDelCMD()
{

}

BOOL CDelCMD::SetCMD(MyTagCSInfo& pData)
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

BOOL CDelCMD::CmdExecute()
{

    MyTagCDataInfo& Info = m_CSInfo.m_Deal.m_BufInfo;

    CString strDel = TEXT("");

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
    strBuf = strBuf + m_strSid
        + m_strSname
        + m_strSgender
        + m_strCouID
        + m_strCouName
        + m_strCid
        + m_strCname;
*/
    //ѧ��
    if(Info.bStu)
    {
        //Delete From t_Student WHERE 1 = 1 
        strDel = strDel + DELSQL + t_Student + SQL_WHERE + SQL_ONE;

        //ƴ���ַ���
        if(Info.cSidLen != 0)
        {
            Concatenation(strDel, STUID, strSrcBuf, Info.cSidLen, dwPos);
        }

        if(Info.cSNameLen != 0)
        {
            Concatenation(strDel, STUNAME, strSrcBuf, Info.cSNameLen, dwPos);
        }

        if(Info.cSgenderLen != 0)
        {
            Concatenation(strDel, STUGENDER, strSrcBuf, Info.cSgenderLen, dwPos);
        }

        if(Info.cCidLen != 0)
        {
            Concatenation(strDel, CLSID, strSrcBuf, Info.cCidLen, dwPos);
        }
    }

/*
    CHAR    cCouIDLen;      //���ȣ��γ�ID      //3
    CHAR    cCouNameLen;    //���ȣ��γ���      //4
*/
    //�γ�
    if(Info.bCourse)
    {

        //Delete From t_Course WHERE 1 = 1 
        strDel = strDel + DELSQL + t_Course + SQL_WHERE + SQL_ONE;
        
        //ƴ���ַ���
        if(Info.cCouIDLen != 0)
        {
            Concatenation(strDel, COUID, strSrcBuf, Info.cCouIDLen, dwPos);
        }

        if(Info.cCouNameLen != 0)
        {
            Concatenation(strDel, COUNAME, strSrcBuf, Info.cCouNameLen, dwPos);
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
        

        //Delete From t_Class WHERE 1 = 1 
        strDel = strDel + DELSQL + t_Class + SQL_WHERE + SQL_ONE;

        //ƴ���ַ���
        if(Info.cCidLen != 0)
        {
            Concatenation(strDel, CLSID, strSrcBuf, Info.cCidLen, dwPos);

            DWORD dwStart =  strDel.Find(TEXT("cID"));
            DWORD dwEnd = strDel.GetLength();

            //sID Like '%sss%'
            CString strTmp = strDel.Mid(dwStart, dwEnd - dwStart);

            //ɾ���༶֮ǰ����ɾ����������ı�  t_Student
            //Delete From t_Student Where cID = 
            strTmp = TEXT("Delete From t_Student Where ") + strTmp;

            //��ɾ��ѧ�������ã���ɾ���༶��
            strDel = strTmp + TEXT("\r\n") + strDel;
        }
        
        if(Info.cCnameLen != 0)
        {
            Concatenation(strDel, CLSNAME, strSrcBuf, Info.cCnameLen, dwPos);
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
    //AfxMessageBox(strDel);

    CMySocket::OutputErrorMsg(strDel);

    //����Client Socket ��������ͨ��Deal
    MYSOCKET sClient = m_CSInfo.m_Socket;
    PACKET_FLAG  dwFlag = m_CSInfo.m_Deal.m_flag;
    memset(&m_CSInfo, 0, sizeof(m_CSInfo));
    m_CSInfo.m_Socket = sClient;
    m_CSInfo.m_Deal.m_flag = dwFlag;

    //��û�����
    m_CSInfo.m_Deal.m_dwDataSize = strDel.GetLength() + 1;
    m_CSInfo.m_Deal.m_pszData = strDel.GetBuffer(0);
    

    //��������
    CDialog* pDlg = ((CMidApp*)AfxGetApp())->m_pThis;
    CMidDlg& Mid = *(CMidDlg*)pDlg;

    //����ͷ��
    Mid.m_Sock.SendAll(Mid.m_Sock.GetMainSock(), (LPTSTR)&m_CSInfo, TagCSInfoSize);

    //���ͺ���
    Mid.m_Sock.SendAll(Mid.m_Sock.GetMainSock(), m_CSInfo.m_Deal.m_pszData, m_CSInfo.m_Deal.m_dwDataSize);

    return TRUE;
}


BOOL CDelCMD::Concatenation(CString& strDst, CString strColumn, CONST CString& strSrcBuf, DWORD dwLen, DWORD& dwPos)
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