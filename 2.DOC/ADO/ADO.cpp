// ADO.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#pragma warning( disable : 4146 )
#import "C:\Program Files (x86)\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","adoEOF")
#define CONN_STRING "Provider=SQLOLEDB.1;Integrated Security=SSPI;Persist Security Info=False;Initial Catalog=CR23;Data Source=."

class CComInit
{
public:
  CComInit()
  {
    CoInitialize(NULL);
  }
  ~CComInit()
  {
    CoUninitialize();
  }
}g_Init;


void ShowRecordSet(_RecordsetPtr rs)
{
  //��ʾ�ֶ�
  int nCount = rs->Fields->GetCount();
  for (long i = 0; i < nCount; i++)
  {
    printf("%-15s", (char*)rs->Fields->Item[i]->Name);
  }
  printf("\r\n");
  
  //�������
  while (!rs->adoEOF)
  {
    for (long i = 0; i < nCount; i++)
    {
      printf("%-15s", (char*)(_bstr_t)rs->Fields->Item[i]->Value);
      //printf("%-15s", (char*)(_bstr_t)rs->Fields->GetItem(i)->GetValue());
    }
    printf("\r\n");
    rs->MoveNext();
  } 
}

void Connect()
{
  _ConnectionPtr Conn; //���Ӷ���
  _RecordsetPtr  rs;   //��¼������
  try {
    Conn.CreateInstance("ADODB.Connection");
    
    //�������ݿ������
    Conn->Open(CONN_STRING, "", "", adConnectUnspecified);
    
    //ִ��SQL���
    rs = Conn->Execute("select * from tbl_test", NULL, adCmdText);
    ShowRecordSet(rs);
    
    Conn->Close();
  }
  catch (_com_error e) {
    puts(e.ErrorMessage());
    try  {
      Conn->Close();
    } 
    catch (...) {
    }
  }
}

void RecordSet()
{
  _ConnectionPtr Conn; //���Ӷ���
  _RecordsetPtr  rs;   //��¼������
  
  try {
    rs.CreateInstance("ADODB.Recordset");
    Conn.CreateInstance("ADODB.Connection");
    
    //�������ݿ������
    Conn->Open(CONN_STRING, "", "", adConnectUnspecified);
    
    //ִ��SQL���
    rs->Open("select * from tbl_student", (IDispatch*)Conn, adOpenStatic, adLockOptimistic, adCmdText);
    ShowRecordSet(rs);
    
    Conn->Close();
  }
  catch (_com_error e) {
    puts(e.ErrorMessage());
    try  {
      Conn->Close();
    } 
    catch (...) {
    }
  }
}

void Command()
{
  _CommandPtr Command;
  _ConnectionPtr Conn; //���Ӷ���
  _RecordsetPtr  rs;   //��¼������
  
  try {
    Command.CreateInstance("ADODB.Command");
    Conn.CreateInstance("ADODB.Connection");
    
    //�������ݿ������
    Conn->Open(CONN_STRING, "", "", adConnectUnspecified);
    
    
    _variant_t RecordsAffected;
    Command->ActiveConnection = Conn;
    Command->CommandText = "select * from tbl_student";
    rs = Command->Execute(&RecordsAffected, NULL, adCmdText);
    Command->CommandText = "insert into tbl_student values('s1000', 's1000', 'cr02')";
    rs = Command->Execute(&RecordsAffected, NULL, adCmdText);
    printf("RecordsAffected:%d\r\n", (long)RecordsAffected);
    ShowRecordSet(rs);
    
    Conn->Close();
  }
  catch (_com_error e) {
    puts(e.ErrorMessage());
    try  {
      Conn->Close();
    } 
    catch (...) {
    }
  }
}

int main(int argc, char* argv[])
{
  //����
  _ConnectionPtr Conn; //���Ӷ���
  _RecordsetPtr  rs;   //��¼������
  try {
    Conn.CreateInstance("ADODB.Connection");
    
    //�������ݿ������
    Conn->Open(CONN_STRING, "", "", adConnectUnspecified);
    
    //ִ��SQL���
    Conn->BeginTrans();
    Conn->Execute("exec fun1", NULL, adCmdText);
    Conn->Execute("insert into tbl_student values('s1003', 's1000', 'cr02')", NULL, adCmdText);
    Conn->Execute("insert into tbl_student values('s1004', 's1000', 'cr02')", NULL, adCmdText);
    Conn->CommitTrans();

    Conn->Close();
  }
  catch (_com_error e) {
    Conn->RollbackTrans();
    puts(e.ErrorMessage());
    try  {
      Conn->Close();
    } 
    catch (...) {
    }
  }
	return 0;
}

