// AlterCMD.h: interface for the CAlterCMD class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALTERCMD_H__79044C6E_363F_41D0_87E4_BFC723DA32BA__INCLUDED_)
#define AFX_ALTERCMD_H__79044C6E_363F_41D0_87E4_BFC723DA32BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\ADDIN\ThreadPool.h"
#include "../AddIn/SQLDeal.h"

class CAlterCMD : public ICommand  
{
public:
	CAlterCMD();
	virtual ~CAlterCMD();
    
    virtual BOOL SetCMD(MyTagCSInfo& pData);
    virtual BOOL CmdExecute();
    
    //ƴ�Ӻ��ַ���
    BOOL ConcatenationAfter(CString& strDst, CString strColumn, CONST CString& strSrcBuf, DWORD dwLen, DWORD& dwPos);

    //ƴ��ǰ���ַ���
    BOOL ConcatenationBefore(CString& strDst, CString strColumn, CONST CString& strSrcBuf, DWORD dwLen, DWORD& dwPos);
    
protected:
    
    //���洫������
    MyTagCSInfo  m_CSInfo;
    TCHAR m_szBuf[MAXSHORT];
};

#endif // !defined(AFX_ALTERCMD_H__79044C6E_363F_41D0_87E4_BFC723DA32BA__INCLUDED_)
