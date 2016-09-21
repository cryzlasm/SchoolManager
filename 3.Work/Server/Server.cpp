// Server.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"

#include "SqlServer.h"

#include <STDIO.H>
#include <WINDOWS.H>

//用于初始化COM调用的类
CHandleSqlReply g_Handle;


#define  SERVER_CONNECT_STRING \
TEXT("Provider=SQLOLEDB.1;Persist Security Info=False;User ID=sa;Initial Catalog=School;Data Source=192.168.1.166")

int main(int argc, char* argv[])
{

    {
        CSqlServer sql;

        if(!sql.StartServer(8088, SERVER_CONNECT_STRING))
        {
            g_Handle.OutputErrorMsg(TEXT("main: 开启服务器失败"));
            return 0;
        }

        printf(TEXT("服务器启动...\r\n"));

        char ch = 0;
        while(ch = getchar())
        {
            if(ch == 'q' || ch == 'Q')
            {
                break;
            }
        }
        
//         while(TRUE)
//         {
//             ;
//         }
    }

    system("pause");
	return 0;
}
