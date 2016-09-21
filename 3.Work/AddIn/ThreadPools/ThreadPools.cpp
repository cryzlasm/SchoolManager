// ThreadPool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../AddIn/ThreadPool.h"
#include "LoginCmd.h"
#include "MsgCmd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
// 		CString strHello;
// 		strHello.LoadString(IDS_HELLO);
// 		cout << (LPCTSTR)strHello << endl;
        
        try
        {
            CMyThreadPool<ICommand> Pool;

            Pool.Create();

            system("pause");
            for (int i = 0; i < 100; i++)
            {
                ICommand* pCmd = new CLoginCmd;
                Pool.Handle(pCmd);
                
                pCmd = new CMsgCmd;
                Pool.Handle(pCmd);
            }
            
            system("pause");
        }
        catch(CString str)
        {
            printf(str);
        }

	}

	return nRetCode;
}


