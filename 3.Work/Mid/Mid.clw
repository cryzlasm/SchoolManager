; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMidDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Mid.h"

ClassCount=3
Class1=CMidApp
Class2=CMidDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_MID_DIALOG

[CLS:CMidApp]
Type=0
HeaderFile=Mid.h
ImplementationFile=Mid.cpp
Filter=N

[CLS:CMidDlg]
Type=0
HeaderFile=MidDlg.h
ImplementationFile=MidDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_BTN_CLIENT_SERVER

[CLS:CAboutDlg]
Type=0
HeaderFile=MidDlg.h
ImplementationFile=MidDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_MID_DIALOG]
Type=1
Class=CMidDlg
ControlCount=8
Control1=IDC_BTN_SERVER_CONNECT,button,1342373888
Control2=IDC_BTN_CLIENT_SERVER,button,1342373888
Control3=IDC_BTN_CLIENT_OVER,button,1476591616
Control4=IDC_BTN_SERVER_BREAK,button,1476591616
Control5=IDC_STATIC,button,1342309127
Control6=IDC_STATIC,button,1342309127
Control7=IDC_STATIC_CLIENTINFO,static,1342312961
Control8=IDC_STATIC_SERVERINFO,static,1342312961

