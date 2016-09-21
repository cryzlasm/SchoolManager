// MsgCmd.h: interface for the CMsgCmd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGCMD_H__A916694A_0DF1_4796_B834_CE1F20D74959__INCLUDED_)
#define AFX_MSGCMD_H__A916694A_0DF1_4796_B834_CE1F20D74959__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../AddIn/ThreadPool.h"

class CMsgCmd : public ICommand  
{
public:
	CMsgCmd();
	virtual ~CMsgCmd();

    //Ö´ÐÐCmd
    virtual BOOL CmdExecute();
    virtual BOOL SetCMD(MyTagCSInfo& pData){return TRUE;}
};

#endif // !defined(AFX_MSGCMD_H__A916694A_0DF1_4796_B834_CE1F20D74959__INCLUDED_)
