#if !defined(SOCKET_DEAL_H__D9605F44_2F0B_4976_9B11_CD65507EA11E__INCLUDED_)
#define SOCKET_DEAL_H__D9605F44_2F0B_4976_9B11_CD65507EA11E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//������Ϣ��
enum PACKET_FLAG
{
    NONE,

    //client -> server 
    CLIENT_LOGIN,               //��½����
    CLIENT_MSG_INSERT,          //�����������������Ϣ
    CLIENT_MSG_QUERY,           //����������Ͳ�����Ϣ
    CLIENT_MSG_DEL,             //�����������ɾ����Ϣ
    CLIENT_MSG_ALTER,           //������������޸���Ϣ
    CLIENT_HEART_BEAT,          //�����������һ��������
    
    //server -> client
    SERVER_LOGINREPLY,          //��������½�ظ�
    SERVER_REPLY_INSERT,        //����������������Ϣ�ظ�
    SERVER_REPLY_QUERY,         //���������ղ�����Ϣ�ظ�
    SERVER_REPLY_DEL,           //����������ɾ����Ϣ�ظ�
    SERVER_REPLY_ALTER,         //�����������޸���Ϣ�ظ�
    SERVER_HEART_BEAT_REPLY,    //�������ظ�һ��������
    SERVER_ALTER,               //����������һ�����ݱ��޸ĵ�״̬��

    SERVER_ERROR_INSERT,        //���ӳ���
    SERVER_ERROR_DEL,           //ɾ������
    SERVER_ERROR_ALTER          //�޸ĳ���

};

#pragma pack(push)
#pragma pack(1)

/*
m_strSname = _T("");
m_strSid = _T("");
m_strSgender = _T("");
m_strCouID = _T("");
m_strCouName = _T("");
m_strCname = _T("");
m_strCID = _T("");
m_strTips = _T("");
*/
//----------------------------------------------------------------------------
//�ͻ���������Ϣ����ʾ�ͻ��˷��͵�������ν���
typedef struct _TagClientDataLength
{
    CHAR    cSidLen;        //���ȣ�ѧ��ID      //0
    CHAR    cSNameLen;      //���ȣ�ѧ����      //1
    CHAR    cSgenderLen;    //���ȣ�ѧ���Ա�    //2
    CHAR    cCouIDLen;      //���ȣ��γ�ID      //3
    CHAR    cCouNameLen;    //���ȣ��γ���      //4
    CHAR    cCidLen;        //���ȣ��༶ID      //5
    CHAR    cCnameLen;      //���ȣ��༶��      //6
    CHAR    bStu:1;         //Stu��ѡ��         //7
    CHAR    bCourse:1;      //Course��ѡ��      //7
    CHAR    bClass:1;       //Class��ѡ��       //7
    CHAR    nChange:5;      //nChange,��������  //7
    CHAR    nChangeOne;     //��һ�����ĵĳ���  //8
    CHAR    nChangeTwo;     //�ڶ������ĵĳ���  //9
    CHAR    nChangeThree;   //���������ĵĳ���  //10
    CHAR    nChangeFour;    //���ĸ����ĵĳ���  //11
}MyTagCDataInfo, *PMyTagCDataInfo;  //Size: 12

//----------------------------------------------------------------------------
//Server---Client  �շ�Э��
typedef struct  _TagClientMidDeal
{
    DWORD           m_dwDataSize; //���ݳ���    //0
    PACKET_FLAG     m_flag;       //������Ϣ��  //4
    
    //Client �� Mid ͨ��ʱ ʹ�ã� Mid ��Serverͨ��ʱ����ʹ��
    MyTagCDataInfo  m_BufInfo;    //������ν���//8     
    LPTSTR          m_pszData;    //����        //20
}MyTagDeal, *PMyTagDeal;    //Size:24

//�����-�ͻ��� ��Э��ͷ��С
#define TagDealSize (sizeof(MyTagDeal) - sizeof(LPTSTR))

//----------------------------------------------------------------------------
#define MYSOCKET unsigned int
//�����Eventģ���շ�����ʱʹ�õ����ݽṹ
typedef struct _TagServerRecvInfo
{
    DWORD       m_dwRecvedSize;     //�ѽ��ܵ�����  //0
    MYSOCKET    m_Socket;           //�ͻ���Socket  //4
    MYSOCKET    m_MidSock;          //�м���׽���  //8
    MyTagDeal   m_Deal;             //C S Э��      //12
}MyTagCSInfo, *PMyTagCSInfo;    //Size:36

#define TagCSInfoSize (sizeof(MyTagCSInfo) - sizeof(LPTSTR))
//----------------------------------------------------------------------------

#pragma pack(pop)


#endif // !defined(SOCKET_DEAL_H__D9605F44_2F0B_4976_9B11_CD65507EA11E__INCLUDED_)