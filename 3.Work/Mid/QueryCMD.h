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
    
    //Ŀ�����ݣ��ֶ��� ��Դ���ݣ����ȣ���ǰλ���ַ���
    BOOL Concatenation(CString& strDst, CString strColumn, CONST CString& strSrcBuf, DWORD dwLen, DWORD& dwPos);
    
protected:
    
    //���洫������
    MyTagCSInfo  m_CSInfo;
    TCHAR m_szBuf[MAXSHORT];
};

#endif // !defined(AFX_QUERYCMD_H__608D95DB_0E12_4AF2_89F1_18A681CEB041__INCLUDED_)
