
--创建库
create database CR23

--使用CR23库
use CR23

--创建表
CREATE TABLE t_Student
(
	sId		varchar(50)  	PRIMARY KEY,
	sName		varchar(50)  	NOT NULL DEFAULT 'NoName',
	sBirth		datetime	NOT NULL DEFAULT '1970/1/1',
	sGender	varchar(50)	NOT NULL DEFAULT 'UnKnow',
	sAddr		varchar(256)	NOT NULL DEFAULT 'UnKnow',
	sCallPhone	varchar(50)	NOT NULL DEFAULT 'UnKnow'
)

--插入信息
INSERT INTO t_Student 
(sId, sName, sBirth, sGender) 
values
('s001', '张三', '2010/1/1', '男')

INSERT INTO t_Student 
(sId, sName, sBirth, sGender) 
values
('s002', '李四', '2010/1/1', '男')

INSERT INTO t_Student 
(sId, sName, sBirth, sGender) 
values
('s003', '王五', '2010/1/1', '男')

INSERT INTO t_Student 
(sId, sName, sBirth, sGender) 
values
('s004', '牛八', '2010/1/1', '男')

--查询
select * from t_Student