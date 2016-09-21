// DelCMD.h: interface for the CDelCMD class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DELCMD_H__CC3A89D2_9FEE_4557_9637_0FFB6A317A7F__INCLUDED_)
#define AFX_DELCMD_H__CC3A89D2_9FEE_4557_9637_0FFB6A317A7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\ADDIN\ThreadPool.h"
#include "../AddIn/SQLDeal.h"

class CDelCMD : public ICommand  
{
public:
	CDelCMD();
	virtual ~CDelCMD();
    
    virtual BOOL SetCMD(MyTagCSInfo& pData);
    virtual BOOL CmdExecute();
    
    //拼接字符串
    BOOL CDelCMD::Concatenation(CString& strDst, CString strColumn, CONST CString& strSrcBuf, DWORD dwLen, DWORD& dwPos);
    
protected:
    
    //保存传入命令
    MyTagCSInfo  m_CSInfo;
    TCHAR m_szBuf[MAXSHORT];
};

#endif // !defined(AFX_DELCMD_H__CC3A89D2_9FEE_4557_9637_0FFB6A317A7F__INCLUDED_)
