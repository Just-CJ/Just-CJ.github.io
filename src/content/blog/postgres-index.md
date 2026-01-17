---
title: Postgres索引与PostGIS空间数据查询
date: 2015-07-23
description: 为了优化数据查询，一般我们要对数据库中的数据建立索引。本文介绍如何建立索引以及PostGIS空间数据查询。
tags:
  - PostgreSQL
  - PostGIS
categories:
  - Tech
---

## 索引建立

为了优化数据查询，一般我们要对数据库中的数据建立索引。

建立索引可以直接使用sql语句实现，对于一般的table中的column，可以用如下指令

```sql
CREATE INDEX ON table_name(coloumn_name);
```

对于PostGIS的空间数据，则需要用如下指令建立索引

```sql
CREATE INDEX idx_my_table_geom ON my_table USING gist(geom);
```

针对我之前插入数据库的1000w条数据，我在三个column上建立了索引，其中一个空间数据。

### 时间消费

建立三个索引耗时较长。

### 空间消费

- 建立索引前占用空间大小，约800m
- 建立索引后占用空间大小，约2G

可以看到索引占了相当大的空间

## 查询测试

通过查询语句来测试索引的效果，测试用查询语句如下（~~好像有点长~~）

```sql
SELECT plate_number, time, location, ST_Distance(location, 'SRID=4326;POINT(120.83 27.93)'::geometry) as d from taxis WHERE ST_DWithin(location, 'POINT(120.83 27.93)', 500) ORDER BY d limit 1000
```

这个查询语句的作用是查询经纬度129.83,27.93该点周围的1000个点，其中使用了ST_Distance和ST_SWithin函数，这些都是PostGIS独有的函数，需要注意的是ST_Distance这个函数不能使用索引，不然我直接算距离排序就行了。

查询结果：不用索引查询需要8s，用索引需要0.282s，效果还是很明显的。

## pg的查询计划

通过`explain + select`语句可以查看该查询语句的查询计划，这样我们就能知道一条查询语句是否使用了索引。

此外pg有一些配置用来控制是否启用索引，这些配置都在`/etc/postgresql/9.3/main/postgresql.conf`中

可以通过如下sql语句来临时改变这些配置

```sql
SET enable_indexscan TO off
```
