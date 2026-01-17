---
title: Postgresql&Postgis配置
date: 2015-07-20
description: 暑假开始跟着学长做毕设，最初的任务就是在集群的节点机上部署Postgresql数据库。
tags:
  - PostgreSQL
  - Postgis
categories:
  - Tech
---

~~坑了好久的博客终于整好啦~~

## Postgresql数据库安装与配置

暑假开始跟着学长做毕设，最初的任务就是在集群的节点机上部署Postgresql数据库。

安装数据库很简单，直接使用apt-get工具

```bash
sudo apt-get install postgresql
```

本来以为装这么一个就完事了，结果发现学长给的测试代码根本没法跑，后来发现是没有安装 PostGIS 扩展。

PostGIS 是 PostgreSQL 关系数据库的空间操作扩展。它为 PostgreSQL 提供了存储、查询和修改空间关系的能力。

安装也是一样使用apt-get

```bash
sudo apt-get install postgresql-9.3-postgis-2.1
```

这样就算装完了，也不用配置什么东西。

## 创建PostGIS空间数据库

要创建PostGIS数据库首先需要一个PostGIS的模板数据库，没有的话我们需要自己创建一个，创建步骤如下

```bash
$ sudo su postgres
$ createdb template_postgis
$ psql -f /usr/share/postgresql/9.3/contrib/postgis-2.1/postgis.sql -d template_postgis
$ createdb [-U username] -T template_postgis my_spatial_db
```

## 创建Table

根据学长的测试代码，还原了一下数据库里的table，大致如下

```sql
CREATE TABLE taxis (
	plate_number varchar(20),
	location geography(POINT,4326),
	time date,
	is_passenger_in boolean,
	speed numeric(10, 2),
	direction numeric(3, 0)
)
```

可以将上面这个代码存成taxis.sql，然后登录数据库直接执行`\i taxis.sql`执行sql文件
