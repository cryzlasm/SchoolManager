--create table tbl_test(sid int, cid int, primary key(sid, cid))

create table tbl_student(sid varchar(50) primary key, sname varchar(50) not null, cid varchar(50) FOREIGN KEY REFERENCES tbl_class(cid)
)

create table tbl_class(cid varchar(50) primary key,cname varchar(50) not null)

create table tbl_course(couid varchar(50) primary key, sname varchar(50) not null)

create table tbl_select_course
(
sid varchar(50) FOREIGN KEY REFERENCES tbl_student(sid),
 couid varchar(50) FOREIGN KEY REFERENCES tbl_course(couid),
 score float not null, 
 primary key(sid, couid)
)

--插入记录
insert into tbl_class (cid, cname) values('cr01', '科锐1班')
insert  into tbl_class values('cr04', '科锐4班')
insert  tbl_class (cid, cname) values('cr02', '科锐2班')
insert  tbl_class values('cr03', '科锐3班')
insert  tbl_class values('cr001', '科锐001班')


insert into tbl_student values('s001', '张三', 'cr01')
insert into tbl_student values('s002', '李四', 'cr01')
insert into tbl_student values('s003', '王五', 'cr02')
insert into tbl_student values('s004', '李流', 'cr03')


insert into tbl_course values('cou01', 'c语言')
insert into tbl_course values('cou02', 'c++')
insert into tbl_course values('cou03', '数据结构')
insert into tbl_course values('cou04', 'JAVA')

insert into tbl_select_course values('s001', 'cou01', 10)
insert into tbl_select_course values('s002', 'cou01', 99.5)
insert into tbl_select_course values('s002', 'cou02', 99)
insert into tbl_select_course values('s003', 'cou01', 60)
insert into tbl_select_course values('s003', 'cou02', 70)
insert into tbl_select_course values('s003', 'cou03', 80)
insert into tbl_select_course values('s003', 'cou04', 80)

--修改
update tbl_class set cname = '科锐一班'
update tbl_class set cname = '科锐2班' where cid='cr02'
update tbl_class set cname = '科锐三班' where cid<>'cr01' and cid<>'cr02'
update tbl_class set cname = '科锐四班' where cid='cr04'

--删除
delete tbl_class
delete tbl_class where cid = 'cr01'

--查询


--*表示所有字段
select *from  pubs.dbo.sales
select *from  tbl_class

select cname='班级名称' from  tbl_class
select cname as '班级名称' from  tbl_class
select cname='1班' from  tbl_class where cid='cr01'

--逻辑运算
select * from tbl_class where cid ='cr01' and cname = '科锐1班'
select * from tbl_class where cid ='cr01' or cid ='cr02'
select * from tbl_class where not (cid ='cr01' or cid = 'cr02')

--范围
select *from tbl_class where cid between 'cr01' and 'cr02'

--在范围
select *from tbl_class where cid in ('cr01', 'cr04')
select *from tbl_class where cid not in ('cr01', 'cr04')

--空
select *from tbl_class where cid is null
select *from tbl_class where cid is not null

--去除重复
select DISTINCT(cname) from tbl_class
 
--排序
select *from tbl_class order by cid asc
select *from tbl_class order by cid desc

select top 1*from tbl_class order by cid desc

--聚合函数
select * from tbl_select_course
select count(*) as count from tbl_select_course
select max(score)  from tbl_select_course
select min(score)  from tbl_select_course
select sum(score)  from tbl_select_course

--查询是科锐1班的学生
select cid from tbl_class where cname = '科锐1班'

--结果一行一列
select *from tbl_student where cid = (select cid from tbl_class where cname = '科锐1班')

--in 多行一列
select cid from tbl_class where cname = '科锐1班' or cname = '科锐2班'
select *from tbl_student where cid in 
(
select cid from tbl_class where cname = '科锐1班' or cname = '科锐2班'
)

--多表组合 笛卡尔面积
select *from tbl_student --4 * 3
select *from tbl_class  -- 5 * 2
select *from tbl_student,tbl_class where tbl_student.cid = tbl_class.cid

select sname, cname, tbl_class.cid from tbl_student,tbl_class where 
tbl_student.cid = tbl_class.cid

--临时表
select *from 
(
select sname, cname, tbl_class.cid from tbl_student,tbl_class where 
tbl_student.cid = tbl_class.cid
)table1 where sname='张三'

--分组
select * from tbl_select_course
select couid, sum(score) from tbl_select_course group by couid

--临时字段

select *, 
(
select cname from tbl_class where tbl_class.cid = tbl_student.cid
) 
as cname from tbl_student

--having
select couid, sum(score)  from tbl_select_course group by 
couid having sum(score) = 169.5


--例：查找选修了课程的学生
select *from tbl_student where sid in 
(
select DISTINCT(sid) from tbl_select_course 
)

--例：查找无人选修的课程信息
select *from tbl_course where couid not in
(
select DISTINCT(couid) from tbl_select_course 
)

--例：查找选修了C语言课程的学生信息

select *from tbl_student where sid in 
(
	select sid from tbl_select_course where couid  in
	(
		select couid from tbl_course where sname = 'c++'
	)
)

--例：查找成绩大于相应课程的平均分的学生考试情况
--例：查找只选修了cou01的学生

--例：查找选修了所有课程的学生
select *from
(
	select tbl_student.sid, tbl_student.sname, count
	from tbl_student, 
	(
		select sid, count(*) as count from tbl_select_course group by sid
	)tabel1 where tabel1.sid = tbl_student.sid
)table2 where count = (select count(*) from tbl_course)


