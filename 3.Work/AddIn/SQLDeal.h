#if !defined(SQL_DEAL_H__D9605F44_2F0B_4976_9B11_CD65507EA11E__INCLUDED_)
#define SQL_DEAL_H__D9605F44_2F0B_4976_9B11_CD65507EA11E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SELECTALL   TEXT("SELECT * FROM ")      //��ѯ����
#define INSERTALL   TEXT("INSERT INTO ")        //����
#define INSERTVALUE TEXT(" VALUES ")            //���Ӹ���

#define UPDATESQL   TEXT("UPDATE ")             //����
#define SETSQL      TEXT(" SET ")               //Set

#define DELSQL      TEXT("DELETE FROM ")        //ɾ��

#define SPLITSQL    TEXT(", ")                  //������Ϊ�ָ���

#define t_Student   TEXT("t_Student ")         //ѧ����
#define t_Class     TEXT("t_Class ")           //�༶��
#define t_Course    TEXT("t_Course ")          //�γ̱�
#define t_Sel_Cou   TEXT("t_Select_Course ")   //ѡ�α�

#define SQL_WHERE   TEXT(" WHERE ")     //where �ؼ���
#define SQL_ONE     TEXT("1 = 1 ")      // 1 = 1
#define SQL_AND     TEXT("AND")         //And   �ؼ���
#define LIKEBEGIN   TEXT(" LIKE \'%")   //Like���ʼ
#define LIKEEND     TEXT("%\'")         //����

//�ֶ���
#define STUID       TEXT("sID")         //ѧ��ID
#define STUNAME     TEXT("sName")       //ѧ����
#define STUGENDER   TEXT("sGender")     //�Ա�
#define COUID       TEXT("couID")       //�γ�ID
#define COUNAME     TEXT("couName")     //�γ���
#define CLSID       TEXT("cID")         //�༶ID
#define CLSNAME     TEXT("cName")       //�༶��

#endif // !defined(SQL_DEAL_H__D9605F44_2F0B_4976_9B11_CD65507EA11E__INCLUDED_)