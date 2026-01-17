---
title: 使用PostgRest快速构建多版本Restful Api
date: 2015-09-30
description: PostgRest是一个基于PostGreSQL数据库的简捷、便捷的restful api的创建工具。
tags:
  - PostgreSQL
  - PostgRest
  - Restful
categories:
  - Tech
---

PostgRest是一个基于PostGreSQL数据库的简捷、便捷的restful api的创建工具。

基本参考这篇博客

> [postgrest-introduction](http://blog.jonharrington.org/postgrest-introduction/)

上文中主要使用sqitch来控制管理数据库的变更，看了一下确实觉得非常实用，尤其回滚想必非常方便，不过这里就不多加讨论，日后再找时间研究。

## 如何使用

简单说PostgRest是通过schema来进行版本控制的，比如它默认的schema是`"1"`，你可以通过在schema`"1"`中创建视图view来暴露你想要使用的api。

## 创建schema

schema的名字实际上可以自己指定，最后发布的时候可以指定到对应的版本。

```sql
CREATE SCHEMA "test";
```

## 创建视图

利用已有的table创建视图

```sql
CREATE OR REPLACE view "test".jgsb as
SELECT name, market, avg_price, prod_place, time
FROM jgsb ORDER BY time desc;
```

## 利用PostgRest创建api

```bash
./postgrest \
--db-host 0.0.0.0 --db-port 5432 \
--db-name yourdb --db-pool 200 \
--anonymous justcj --port 3000 \
--db-user justcj --db-pass xxx \
--v1schema "test"
```

## 利用postman测试

- **查看REST资源**
- **测试GET**
- **指定按csv格式返回** - 增加一个header参数Accept: text/csv
