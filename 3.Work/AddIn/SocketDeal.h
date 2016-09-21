#if !defined(SOCKET_DEAL_H__D9605F44_2F0B_4976_9B11_CD65507EA11E__INCLUDED_)
#define SOCKET_DEAL_H__D9605F44_2F0B_4976_9B11_CD65507EA11E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//定义消息码
enum PACKET_FLAG
{
    NONE,

    //client -> server 
    CLIENT_LOGIN,               //登陆请求
    CLIENT_MSG_INSERT,          //向服务器发送增加消息
    CLIENT_MSG_QUERY,           //向服务器发送查找消息
    CLIENT_MSG_DEL,             //向服务器发送删除消息
    CLIENT_MSG_ALTER,           //向服务器发送修改消息
    CLIENT_HEART_BEAT,          //向服务器发送一个心跳包
    
    //server -> client
    SERVER_LOGINREPLY,          //服务器登陆回复
    SERVER_REPLY_INSERT,        //服务器接收增加信息回复
    SERVER_REPLY_QUERY,         //服务器接收查找信息回复
    SERVER_REPLY_DEL,           //服务器接收删除信息回复
    SERVER_REPLY_ALTER,         //服务器接收修改信息回复
    SERVER_HEART_BEAT_REPLY,    //服务器回复一个心跳包
    SERVER_ALTER,               //服务器发送一个数据被修改的状态码

    SERVER_ERROR_INSERT,        //增加出错
    SERVER_ERROR_DEL,           //删除出错
    SERVER_ERROR_ALTER          //修改出错

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
//客户端数据信息，表示客户端发送的数据如何解析
typedef struct _TagClientDataLength
{
    CHAR    cSidLen;        //长度：学生ID      //0
    CHAR    cSNameLen;      //长度：学生名      //1
    CHAR    cSgenderLen;    //长度：学生性别    //2
    CHAR    cCouIDLen;      //长度：课程ID      //3
    CHAR    cCouNameLen;    //长度：课程名      //4
    CHAR    cCidLen;        //长度：班级ID      //5
    CHAR    cCnameLen;      //长度：班级名      //6
    CHAR    bStu:1;         //Stu被选中         //7
    CHAR    bCourse:1;      //Course被选中      //7
    CHAR    bClass:1;       //Class被选中       //7
    CHAR    nChange:5;      //nChange,几个被改  //7
    CHAR    nChangeOne;     //第一个被改的长度  //8
    CHAR    nChangeTwo;     //第二个被改的长度  //9
    CHAR    nChangeThree;   //第三个被改的长度  //10
    CHAR    nChangeFour;    //第四个被改的长度  //11
}MyTagCDataInfo, *PMyTagCDataInfo;  //Size: 12

//----------------------------------------------------------------------------
//Server---Client  收发协议
typedef struct  _TagClientMidDeal
{
    DWORD           m_dwDataSize; //数据长度    //0
    PACKET_FLAG     m_flag;       //定义消息码  //4
    
    //Client 与 Mid 通信时 使用， Mid 与Server通信时，不使用
    MyTagCDataInfo  m_BufInfo;    //数据如何解析//8     
    LPTSTR          m_pszData;    //数据        //20
}MyTagDeal, *PMyTagDeal;    //Size:24

//服务端-客户端 的协议头大小
#define TagDealSize (sizeof(MyTagDeal) - sizeof(LPTSTR))

//----------------------------------------------------------------------------
#define MYSOCKET unsigned int
//服务端Event模型收发数据时使用的数据结构
typedef struct _TagServerRecvInfo
{
    DWORD       m_dwRecvedSize;     //已接受的数据  //0
    MYSOCKET    m_Socket;           //客户端Socket  //4
    MYSOCKET    m_MidSock;          //中间层套接字  //8
    MyTagDeal   m_Deal;             //C S 协议      //12
}MyTagCSInfo, *PMyTagCSInfo;    //Size:36

#define TagCSInfoSize (sizeof(MyTagCSInfo) - sizeof(LPTSTR))
//----------------------------------------------------------------------------

#pragma pack(pop)


#endif // !defined(SOCKET_DEAL_H__D9605F44_2F0B_4976_9B11_CD65507EA11E__INCLUDED_)