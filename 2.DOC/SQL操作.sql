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

--�����¼
insert into tbl_class (cid, cname) values('cr01', '����1��')
insert  into tbl_class values('cr04', '����4��')
insert  tbl_class (cid, cname) values('cr02', '����2��')
insert  tbl_class values('cr03', '����3��')
insert  tbl_class values('cr001', '����001��')


insert into tbl_student values('s001', '����', 'cr01')
insert into tbl_student values('s002', '����', 'cr01')
insert into tbl_student values('s003', '����', 'cr02')
insert into tbl_student values('s004', '����', 'cr03')


insert into tbl_course values('cou01', 'c����')
insert into tbl_course values('cou02', 'c++')
insert into tbl_course values('cou03', '���ݽṹ')
insert into tbl_course values('cou04', 'JAVA')

insert into tbl_select_course values('s001', 'cou01', 10)
insert into tbl_select_course values('s002', 'cou01', 99.5)
insert into tbl_select_course values('s002', 'cou02', 99)
insert into tbl_select_course values('s003', 'cou01', 60)
insert into tbl_select_course values('s003', 'cou02', 70)
insert into tbl_select_course values('s003', 'cou03', 80)
insert into tbl_select_course values('s003', 'cou04', 80)

--�޸�
update tbl_class set cname = '����һ��'
update tbl_class set cname = '����2��' where cid='cr02'
update tbl_class set cname = '��������' where cid<>'cr01' and cid<>'cr02'
update tbl_class set cname = '�����İ�' where cid='cr04'

--ɾ��
delete tbl_class
delete tbl_class where cid = 'cr01'

--��ѯ


--*��ʾ�����ֶ�
select *from  pubs.dbo.sales
select *from  tbl_class

select cname='�༶����' from  tbl_class
select cname as '�༶����' from  tbl_class
select cname='1��' from  tbl_class where cid='cr01'

--�߼�����
select * from tbl_class where cid ='cr01' and cname = '����1��'
select * from tbl_class where cid ='cr01' or cid ='cr02'
select * from tbl_class where not (cid ='cr01' or cid = 'cr02')

--��Χ
select *from tbl_class where cid between 'cr01' and 'cr02'

--�ڷ�Χ
select *from tbl_class where cid in ('cr01', 'cr04')
select *from tbl_class where cid not in ('cr01', 'cr04')

--��
select *from tbl_class where cid is null
select *from tbl_class where cid is not null

--ȥ���ظ�
select DISTINCT(cname) from tbl_class
 
--����
select *from tbl_class order by cid asc
select *from tbl_class order by cid desc

select top 1*from tbl_class order by cid desc

--�ۺϺ���
select * from tbl_select_course
select count(*) as count from tbl_select_course
select max(score)  from tbl_select_course
select min(score)  from tbl_select_course
select sum(score)  from tbl_select_course

--��ѯ�ǿ���1���ѧ��
select cid from tbl_class where cname = '����1��'

--���һ��һ��
select *from tbl_student where cid = (select cid from tbl_class where cname = '����1��')

--in ����һ��
select cid from tbl_class where cname = '����1��' or cname = '����2��'
select *from tbl_student where cid in 
(
select cid from tbl_class where cname = '����1��' or cname = '����2��'
)

--������ �ѿ������
select *from tbl_student --4 * 3
select *from tbl_class  -- 5 * 2
select *from tbl_student,tbl_class where tbl_student.cid = tbl_class.cid

select sname, cname, tbl_class.cid from tbl_student,tbl_class where 
tbl_student.cid = tbl_class.cid

--��ʱ��
select *from 
(
select sname, cname, tbl_class.cid from tbl_student,tbl_class where 
tbl_student.cid = tbl_class.cid
)table1 where sname='����'

--����
select * from tbl_select_course
select couid, sum(score) from tbl_select_course group by couid

--��ʱ�ֶ�

select *, 
(
select cname from tbl_class where tbl_class.cid = tbl_student.cid
) 
as cname from tbl_student

--having
select couid, sum(score)  from tbl_select_course group by 
couid having sum(score) = 169.5


--��������ѡ���˿γ̵�ѧ��
select *from tbl_student where sid in 
(
select DISTINCT(sid) from tbl_select_course 
)

--������������ѡ�޵Ŀγ���Ϣ
select *from tbl_course where couid not in
(
select DISTINCT(couid) from tbl_select_course 
)

--��������ѡ����C���Կγ̵�ѧ����Ϣ

select *from tbl_student where sid in 
(
	select sid from tbl_select_course where couid  in
	(
		select couid from tbl_course where sname = 'c++'
	)
)

--�������ҳɼ�������Ӧ�γ̵�ƽ���ֵ�ѧ���������
--��������ֻѡ����cou01��ѧ��

--��������ѡ�������пγ̵�ѧ��
select *from
(
	select tbl_student.sid, tbl_student.sname, count
	from tbl_student, 
	(
		select sid, count(*) as count from tbl_select_course group by sid
	)tabel1 where tabel1.sid = tbl_student.sid
)table2 where count = (select count(*) from tbl_course)


