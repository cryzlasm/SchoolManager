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
    //缓存命令到本地
    memcpy(&m_CSInfo, &pData, TagCSInfoSize);
    memcpy(m_szBuf, pData.m_Deal.m_pszData, pData.m_Deal.m_dwDataSize + 1);

    return TRUE;
}


/*
CHAR    cSidLen;        //长度：学生ID      //0
CHAR    cSNameLen;      //长度：学生名      //1
CHAR    cSgenderLen;    //长度：学生性别    //2
CHAR    cCouIDLen;      //长度：课程ID      //3
CHAR    cCouNameLen;    //长度：课程名      //4
CHAR    cCidLen;        //长度：班级ID      //5
CHAR    cCnameLen;      //长度：班级名      //6
CHAR    cUnUse;         //未用              //7
*/

BOOL CAlterCMD::CmdExecute()
{
MyTagCDataInfo& Info = m_CSInfo.m_Deal.m_BufInfo;

    CString strAlter = TEXT("");
    CString strBefore = TEXT("");
    CString strAfter = TEXT("");

    DWORD dwPos = 0;

    CString strSrcBuf = m_szBuf;

    //条件
    //CString strWhere = TEXT("WHERE 1 = 1");
    
/*
    CHAR    cSidLen;        //长度：学生ID      //0
    CHAR    cSNameLen;      //长度：学生名      //1
    CHAR    cSgenderLen;    //长度：学生性别    //2
*/

    /*
    update t_Class set cID = 'cr05', cName = '科瑞5班' 
    where cID like '%cr04%' and cName like '%科瑞4班%'
    */
    //学生
    if(Info.bStu)
    {
        //update t_Student set
        strAlter = strAlter + UPDATESQL + t_Student + SETSQL;
    
        //拼接字符串
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
    CHAR    cCouIDLen;      //长度：课程ID      //3
    CHAR    cCouNameLen;    //长度：课程名      //4
*/
    //课程
    if(Info.bCourse)
    {        
        //update t_Course set
        strAlter = strAlter + UPDATESQL + t_Course + SETSQL;

        //拼接字符串
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
    CHAR    cCidLen;        //长度：班级ID      //5
    CHAR    cCnameLen;      //长度：班级名      //6
    CHAR    cUnUse;         //未用              //7
*/
    //班级
    if(Info.bClass)
    {        
        //update t_Class set
        strAlter = strAlter + UPDATESQL + t_Class + SETSQL;

        //拼接字符串
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

    //选课
//     if(Info.cCidLen != 0 || Info.cCnameLen != 0)
//     {
//         
//         strTable += t_Sel_Cou;
// 
//         bIsFirstTable = FALSE;
//     }

    //处理执行命令

    //去除头前缀的逗号与空格
    strBefore = strBefore.Mid(2, strBefore.GetLength());
    strAlter = strAlter + strBefore + SQL_WHERE + SQL_ONE + strAfter;

    CMySocket::OutputErrorMsg(strAlter);

    //保留Client Socket ，并重置通信Deal
    MYSOCKET sClient = m_CSInfo.m_Socket;
    PACKET_FLAG  dwFlag = m_CSInfo.m_Deal.m_flag;
    memset(&m_CSInfo, 0, sizeof(m_CSInfo));
    m_CSInfo.m_Socket = sClient;
    m_CSInfo.m_Deal.m_flag = dwFlag;

    //获得缓冲区
    m_CSInfo.m_Deal.m_dwDataSize = strAlter.GetLength() + 1;
    m_CSInfo.m_Deal.m_pszData = strAlter.GetBuffer(0);
    

    //发送数据
    CDialog* pDlg = ((CMidApp*)AfxGetApp())->m_pThis;
    CMidDlg& Mid = *(CMidDlg*)pDlg;

    //发送头部
    Mid.m_Sock.SendAll(Mid.m_Sock.GetMainSock(), (LPTSTR)&m_CSInfo, TagCSInfoSize);

    //发送后续
    Mid.m_Sock.SendAll(Mid.m_Sock.GetMainSock(), m_CSInfo.m_Deal.m_pszData, m_CSInfo.m_Deal.m_dwDataSize);

    //AfxMessageBox(strAlter);
    return TRUE;
}

BOOL CAlterCMD::ConcatenationAfter(CString& strDst, CString strColumn, CONST CString& strSrcBuf, DWORD dwLen, DWORD& dwPos)
{
    //获取用户字符串
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
    
    //拼接字符串
    //And sId like '%tmp%'
    strDst = strDst + TEXT(" AND ") +  strColumn + LIKEBEGIN + strTmp + LIKEEND;

    return TRUE;
}

BOOL CAlterCMD::ConcatenationBefore(CString& strDst, CString strColumn, CONST CString& strSrcBuf, DWORD dwLen, DWORD& dwPos)
{
    //获取用户字符串
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
    
    //拼接字符串
    //, sId = 'tmp'
    strDst = strDst + TEXT(", ") + strColumn + TEXT(" = '") + strTmp + TEXT("'");
    
    return TRUE;
}