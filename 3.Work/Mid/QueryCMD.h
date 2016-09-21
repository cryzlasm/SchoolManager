// QueryCMD.h: interface for the CQueryCMD class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUERYCMD_H__608D95DB_0E12_4AF2_89F1_18A681CEB041__INCLUDED_)
#define AFX_QUERYCMD_H__608D95DB_0E12_4AF2_89F1_18A681CEB041__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\ADDIN\ThreadPool.h"
#include "../AddIn/SQLDeal.h"

class CQueryCMD : public ICommand  
{
public:
	CQueryCMD();
	virtual ~CQueryCMD();
    
    virtual BOOL SetCMD(MyTagCSInfo& pData);
    virtual BOOL CmdExecute();
    
    //目标数据，字段名 ，源数据，长度，当前位置字符串
    BOOL Concatenation(CString& strDst, CString strColumn, CONST CString& strSrcBuf, DWORD dwLen, DWORD& dwPos);
    
protected:
    
    //保存传入命令
    MyTagCSInfo  m_CSInfo;
    TCHAR m_szBuf[MAXSHORT];
};

#endif // !defined(AFX_QUERYCMD_H__608D95DB_0E12_4AF2_89F1_18A681CEB041__INCLUDED_)
