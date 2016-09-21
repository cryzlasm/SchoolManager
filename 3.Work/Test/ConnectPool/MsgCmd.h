// MsgCmd.h: interface for the CMsgCmd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGCMD_H__A916694A_0DF1_4796_B834_CE1F20D74959__INCLUDED_)
#define AFX_MSGCMD_H__A916694A_0DF1_4796_B834_CE1F20D74959__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../AddIn/ConnectPool.h"


class CMsgCmd : public ICommand  
{
public:
	CMsgCmd();
	virtual ~CMsgCmd();

    //Ö´ÐÐCmd
    virtual MYBOOL CmdExecute();
    virtual MYBOOL SetCMD(MYLPVOID pData);

protected:
    _RecordsetPtr   m_RS;
    MyTagCSInfo     m_CSInfo;
    TCHAR           m_szBuf[MAXBYTE * 2];
};

#endif // !defined(AFX_MSGCMD_H__A916694A_0DF1_4796_B834_CE1F20D74959__INCLUDED_)
