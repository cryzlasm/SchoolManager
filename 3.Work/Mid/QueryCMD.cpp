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
    //缓存命令到本地
    memcpy(&m_CSInfo, &pData, TagCSInfoSize);
    if(pData.m_Deal.m_pszData != NULL)
    {
        memcpy(m_szBuf, pData.m_Deal.m_pszData, pData.m_Deal.m_dwDataSize + 1);
    }
    
    
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

BOOL CQueryCMD::CmdExecute()
{

    MyTagCDataInfo& Info = m_CSInfo.m_Deal.m_BufInfo;

    CString strSelect = TEXT("");

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
        if(!m_CSInfo.m_Deal.m_dwDataSize)
        {
            strSelect = strSelect + SELECTALL + t_Student;
        }
        else
        {
            //SELECT * FROM t_Student WHERE 1 = 1
            strSelect = strSelect + SELECTALL + t_Student + SQL_WHERE + SQL_ONE;
        

            //拼接字符串
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

            //拼接字符串
            if(Info.cCidLen != 0)
            {
                Concatenation(strSelect, CLSID, strSrcBuf, Info.cCidLen, dwPos);
            }
        }
    }

/*
    CHAR    cCouIDLen;      //长度：课程ID      //3
    CHAR    cCouNameLen;    //长度：课程名      //4
*/
    //课程
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
        

            //拼接字符串
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
    CHAR    cCidLen;        //长度：班级ID      //5
    CHAR    cCnameLen;      //长度：班级名      //6
    CHAR    cUnUse;         //未用              //7
*/
    //班级
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
    

            //拼接字符串
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

    //选课
//     if(Info.cCidLen != 0 || Info.cCnameLen != 0)
//     {
//         
//         strTable += t_Sel_Cou;
// 
//         bIsFirstTable = FALSE;
//     }

    //处理执行命令
    //CString strExec = TEXT("");

    CMySocket::OutputErrorMsg(strSelect);

    //保留Client Socket ，并重置通信Deal
    MYSOCKET sClient = m_CSInfo.m_Socket;
    PACKET_FLAG  dwFlag = m_CSInfo.m_Deal.m_flag;
    memset(&m_CSInfo, 0, sizeof(m_CSInfo));
    m_CSInfo.m_Socket = sClient;
    m_CSInfo.m_Deal.m_flag = dwFlag;

    //获得缓冲区
    m_CSInfo.m_Deal.m_dwDataSize = strSelect.GetLength() + 1;
    m_CSInfo.m_Deal.m_pszData = strSelect.GetBuffer(0);
    

    //发送数据
    CDialog* pDlg = ((CMidApp*)AfxGetApp())->m_pThis;
    CMidDlg& Mid = *(CMidDlg*)pDlg;

    //发送头部
    Mid.m_Sock.SendAll(Mid.m_Sock.GetMainSock(), (LPTSTR)&m_CSInfo, TagCSInfoSize);

    //发送后续
    Mid.m_Sock.SendAll(Mid.m_Sock.GetMainSock(), m_CSInfo.m_Deal.m_pszData, m_CSInfo.m_Deal.m_dwDataSize);

    //AfxMessageBox(strSelect);
    return TRUE;
}


BOOL CQueryCMD::Concatenation(CString& strDst, CString strColumn, CONST CString& strSrcBuf, DWORD dwLen, DWORD& dwPos)
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
    //SELECT * FROM Table WHERE 1 = 1 AND strColumn =  LIKE '%str%\'
    strDst = strDst + TEXT(" AND ") +  strColumn + LIKEBEGIN + strTmp + LIKEEND;

    return TRUE;
}