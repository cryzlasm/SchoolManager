// MyServer.h: interface for the CMyServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYSERVER_H__E196A12C_61D4_468F_87AD_5CBF908FDE28__INCLUDED_)
#define AFX_MYSERVER_H__E196A12C_61D4_468F_87AD_5CBF908FDE28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../AddIn/MySocket.h"
#include "../AddIn/SocketDeal.h"

 class CMyServer  
{
public:
	CMyServer();
	virtual ~CMyServer();
    
    //Server Sock
    CMySocket       m_Sock;
};

#endif // !defined(AFX_MYSERVER_H__E196A12C_61D4_468F_87AD_5CBF908FDE28__INCLUDED_)
