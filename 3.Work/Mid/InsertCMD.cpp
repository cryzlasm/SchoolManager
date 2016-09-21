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

BOOL CInsertCMD::CmdExecute()
{

    MyTagCDataInfo& Info = m_CSInfo.m_Deal.m_BufInfo;

    CString strInsert = TEXT("");
    CString strTmp = TEXT("");
    CString strColumn = TEXT("");

    DWORD dwPos = 0;

    CString strSrcBuf = m_szBuf;

    //条件
    //CString strWhere = TEXT("WHERE 1 = 1");
    
/*
    CHAR    cSidLen;        //长度：学生ID      //0
    CHAR    cSNameLen;      //长度：学生名      //1
    CHAR    cSgenderLen;    //长度：学生性别    //2
*/
    //学生
    if(Info.bStu)
    {
        //INSERT INTO t_Student VALUES
        strInsert = strInsert + INSERTALL + t_Student;

        //拼接字符串
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

        //拼接字符串
        if(Info.cCidLen != 0)
        {
            strColumn = strColumn + SPLITSQL + CLSID;
            Concatenation(strTmp, strSrcBuf, Info.cCidLen, dwPos);
        }
    }

/*
    CHAR    cCouIDLen;      //长度：课程ID      //3
    CHAR    cCouNameLen;    //长度：课程名      //4
*/
    //课程
    if(Info.bCourse)
    {
        //INSERT INTO t_Course VALUES
        strInsert = strInsert + INSERTALL + t_Course;

        //拼接字符串
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
    CHAR    cCidLen;        //长度：班级ID      //5
    CHAR    cCnameLen;      //长度：班级名      //6
    CHAR    cUnUse;         //未用              //7
*/
    //班级
    if(Info.bClass)
    {
        //INSERT INTO t_Class VALUES
        strInsert = strInsert + INSERTALL + t_Class;

        //拼接字符串
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

    //选课
//     if(Info.cCidLen != 0 || Info.cCnameLen != 0)
//     {
//         
//         strTable += t_Sel_Cou;
// 
//         bIsFirstTable = FALSE;
//     }

    //去除多余的逗号
    strTmp = strTmp.Right(strTmp.GetLength() - 2);
    strColumn = strColumn.Right(strColumn.GetLength() - 2);

    //处理执行命令,合成SQL语法
    strInsert = strInsert + TEXT("(") + strColumn + TEXT(")") 
                          + INSERTVALUE + TEXT("(") + strTmp + TEXT(")");

    //AfxMessageBox(strInsert);

    CMySocket::OutputErrorMsg(strInsert);

    //保留Client Socket ，并重置通信Deal
    MYSOCKET sClient = m_CSInfo.m_Socket;
    PACKET_FLAG  dwFlag = m_CSInfo.m_Deal.m_flag;
    memset(&m_CSInfo, 0, sizeof(m_CSInfo));
    m_CSInfo.m_Socket = sClient;
    m_CSInfo.m_Deal.m_flag = dwFlag;

    //获得缓冲区
    m_CSInfo.m_Deal.m_dwDataSize = strInsert.GetLength() + 1;
    m_CSInfo.m_Deal.m_pszData = strInsert.GetBuffer(0);
    

    //发送数据
    CDialog* pDlg = ((CMidApp*)AfxGetApp())->m_pThis;
    CMidDlg& Mid = *(CMidDlg*)pDlg;

    //发送头部
    Mid.m_Sock.SendAll(Mid.m_Sock.GetMainSock(), (LPTSTR)&m_CSInfo, TagCSInfoSize);

    //发送后续
    Mid.m_Sock.SendAll(Mid.m_Sock.GetMainSock(), m_CSInfo.m_Deal.m_pszData, m_CSInfo.m_Deal.m_dwDataSize);


    return TRUE;
}

BOOL CInsertCMD::Concatenation(CString& strStu, CONST CString& strSrcBuf, DWORD dwLen, DWORD& dwPos)
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
    strStu = strStu + SPLITSQL + TEXT("'") + strTmp + TEXT("'");

    return TRUE;
}