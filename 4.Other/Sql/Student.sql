
--������
create database CR23

--ʹ��CR23��
use CR23

--������
CREATE TABLE t_Student
(
	sId		varchar(50)  	PRIMARY KEY,
	sName		varchar(50)  	NOT NULL DEFAULT 'NoName',
	sBirth		datetime	NOT NULL DEFAULT '1970/1/1',
	sGender	varchar(50)	NOT NULL DEFAULT 'UnKnow',
	sAddr		varchar(256)	NOT NULL DEFAULT 'UnKnow',
	sCallPhone	varchar(50)	NOT NULL DEFAULT 'UnKnow'
)

--������Ϣ
INSERT INTO t_Student 
(sId, sName, sBirth, sGender) 
values
('s001', '����', '2010/1/1', '��')

INSERT INTO t_Student 
(sId, sName, sBirth, sGender) 
values
('s002', '����', '2010/1/1', '��')

INSERT INTO t_Student 
(sId, sName, sBirth, sGender) 
values
('s003', '����', '2010/1/1', '��')

INSERT INTO t_Student 
(sId, sName, sBirth, sGender) 
values
('s004', 'ţ��', '2010/1/1', '��')

--��ѯ
select * from t_Student