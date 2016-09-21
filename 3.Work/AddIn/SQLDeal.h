#if !defined(SQL_DEAL_H__D9605F44_2F0B_4976_9B11_CD65507EA11E__INCLUDED_)
#define SQL_DEAL_H__D9605F44_2F0B_4976_9B11_CD65507EA11E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SELECTALL   TEXT("SELECT * FROM ")      //查询所有
#define INSERTALL   TEXT("INSERT INTO ")        //增加
#define INSERTVALUE TEXT(" VALUES ")            //增加附带

#define UPDATESQL   TEXT("UPDATE ")             //更改
#define SETSQL      TEXT(" SET ")               //Set

#define DELSQL      TEXT("DELETE FROM ")        //删除

#define SPLITSQL    TEXT(", ")                  //逗号作为分隔符

#define t_Student   TEXT("t_Student ")         //学生表
#define t_Class     TEXT("t_Class ")           //班级表
#define t_Course    TEXT("t_Course ")          //课程表
#define t_Sel_Cou   TEXT("t_Select_Course ")   //选课表

#define SQL_WHERE   TEXT(" WHERE ")     //where 关键字
#define SQL_ONE     TEXT("1 = 1 ")      // 1 = 1
#define SQL_AND     TEXT("AND")         //And   关键字
#define LIKEBEGIN   TEXT(" LIKE \'%")   //Like命令开始
#define LIKEEND     TEXT("%\'")         //结束

//字段名
#define STUID       TEXT("sID")         //学生ID
#define STUNAME     TEXT("sName")       //学生名
#define STUGENDER   TEXT("sGender")     //性别
#define COUID       TEXT("couID")       //课程ID
#define COUNAME     TEXT("couName")     //课程名
#define CLSID       TEXT("cID")         //班级ID
#define CLSNAME     TEXT("cName")       //班级名

#endif // !defined(SQL_DEAL_H__D9605F44_2F0B_4976_9B11_CD65507EA11E__INCLUDED_)