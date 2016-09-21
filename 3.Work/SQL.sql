--����School

CREATE DATABASE School

--ʹ��School
use School

--���� t_class
CREATE TABLE t_Class
(
    cID     varchar(50)     PRIMARY key,    --�༶ID
    cName   varchar(50)     NOT NULL        --�༶��
)

--���� t_student
CREATE TABLE t_Student
(
    sID     varchar(50)     PRIMARY key,                        --ѧ��ID
    sName   varchar(50)     NOT NULL,                           --ѧ����

    sGender varchar(50)     NOT NULL DEFAULT 'UnKnow',          --ѧ���Ա�
    cID     varchar(50)     FOREIGN KEY REFERENCES t_Class(cID) --������༶ID
)

--���� t_course
CREATE TABLE t_Course
(
    couID   varchar(50)     PRIMARY key,    --�γ�ID
    couName varchar(50)     NOT NULL        --�γ���

)


--�����¼
insert into t_Class (cID, cName) values('cr01', '����1��')
insert into t_Class (cID, cName) values('cr02', '����2��')
insert into t_Class (cID, cName) values('cr03', '����3��')
insert into t_Class (cID, cName) values('cr04', '����4��')


insert into t_Student (sID, sName, sGender, cID) values('s001', '����', '��', 'cr01')
insert into t_Student (sID, sName, sGender, cID) values('s002', '����', '��', 'cr01')
insert into t_Student (sID, sName, sGender, cID) values('s003', '����', '��', 'cr02')
insert into t_Student (sID, sName, sGender, cID) values('s004', '����', '��', 'cr03')


insert into t_Course (couID, couName) values('cou01', 'c����')
insert into t_Course (couID, couName) values('cou02', 'c++')
insert into t_Course (couID, couName) values('cou03', '���ݽṹ')
insert into t_Course (couID, couName) values('cou04', 'JAVA')

