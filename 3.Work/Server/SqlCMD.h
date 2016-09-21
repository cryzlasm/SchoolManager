// SqlCMD.h: interface for the CSqlCMD class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SQLCMD_H__8D8ACFC3_1219_4BB8_9D1C_3F76531A5569__INCLUDED_)
#define AFX_SQLCMD_H__8D8ACFC3_1219_4BB8_9D1C_3F76531A5569__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\ADDIN\ConnectPool.h"

class CSqlCMD : public ICommand  
{
public:
	CSqlCMD();
	virtual ~CSqlCMD();
    //执行Cmd
    virtual MYBOOL CmdExecute();
    virtual MYBOOL SetCMD(MYLPVOID pData);

    //用于发送数据
    MYBOOL SendInfo(tstring& strErrMsg);
    
protected:
    _RecordsetPtr   m_RS;
    MyTagCSInfo     m_CSInfo;
    TCHAR           m_szBuf[MAXSHORT];
};

#endif // !defined(AFX_SQLCMD_H__8D8ACFC3_1219_4BB8_9D1C_3F76531A5569__INCLUDED_)
