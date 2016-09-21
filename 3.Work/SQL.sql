--创建School

CREATE DATABASE School

--使用School
use School

--创建 t_class
CREATE TABLE t_Class
(
    cID     varchar(50)     PRIMARY key,    --班级ID
    cName   varchar(50)     NOT NULL        --班级名
)

--创建 t_student
CREATE TABLE t_Student
(
    sID     varchar(50)     PRIMARY key,                        --学生ID
    sName   varchar(50)     NOT NULL,                           --学生名

    sGender varchar(50)     NOT NULL DEFAULT 'UnKnow',          --学生性别
    cID     varchar(50)     FOREIGN KEY REFERENCES t_Class(cID) --外键，班级ID
)

--创建 t_course
CREATE TABLE t_Course
(
    couID   varchar(50)     PRIMARY key,    --课程ID
    couName varchar(50)     NOT NULL        --课程名

)


--插入记录
insert into t_Class (cID, cName) values('cr01', '科锐1班')
insert into t_Class (cID, cName) values('cr02', '科锐2班')
insert into t_Class (cID, cName) values('cr03', '科锐3班')
insert into t_Class (cID, cName) values('cr04', '科锐4班')


insert into t_Student (sID, sName, sGender, cID) values('s001', '张三', '男', 'cr01')
insert into t_Student (sID, sName, sGender, cID) values('s002', '李四', '男', 'cr01')
insert into t_Student (sID, sName, sGender, cID) values('s003', '王五', '男', 'cr02')
insert into t_Student (sID, sName, sGender, cID) values('s004', '李流', '男', 'cr03')


insert into t_Course (couID, couName) values('cou01', 'c语言')
insert into t_Course (couID, couName) values('cou02', 'c++')
insert into t_Course (couID, couName) values('cou03', '数据结构')
insert into t_Course (couID, couName) values('cou04', 'JAVA')

