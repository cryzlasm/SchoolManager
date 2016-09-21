// LoginCmd.h: interface for the CLoginCmd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGINCMD_H__F8876B2E_DE40_4D98_AA56_46DA1347ED10__INCLUDED_)
#define AFX_LOGINCMD_H__F8876B2E_DE40_4D98_AA56_46DA1347ED10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../AddIn/ThreadPool.h"

class CLoginCmd : public ICommand  
{
public:
	CLoginCmd();
	virtual ~CLoginCmd();

    //Ö´ÐÐCmd
    virtual BOOL CmdExecute();

    virtual BOOL SetCMD(MyTagCSInfo& pData){return TRUE;}
};

#endif // !defined(AFX_LOGINCMD_H__F8876B2E_DE40_4D98_AA56_46DA1347ED10__INCLUDED_)
