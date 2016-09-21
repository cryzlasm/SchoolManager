// InsertCMD.h: interface for the CInsertCMD class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSERTCMD_H__D0FADF27_4995_46B8_BE64_2EFFCAACF0D1__INCLUDED_)
#define AFX_INSERTCMD_H__D0FADF27_4995_46B8_BE64_2EFFCAACF0D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\ADDIN\ThreadPool.h"
#include "../AddIn/SQLDeal.h"

class CInsertCMD : public ICommand  
{
public:
	CInsertCMD();
	virtual ~CInsertCMD();

    virtual BOOL SetCMD(MyTagCSInfo& pData);
    virtual BOOL CmdExecute();

    //拼接字符串
    BOOL Concatenation(CString& strStu, CONST CString& strSrcBuf, DWORD dwLen, DWORD& dwPos);

protected:

    //保存传入命令
    MyTagCSInfo  m_CSInfo;
    TCHAR m_szBuf[MAXSHORT];
};

#endif // !defined(AFX_INSERTCMD_H__D0FADF27_4995_46B8_BE64_2EFFCAACF0D1__INCLUDED_)
