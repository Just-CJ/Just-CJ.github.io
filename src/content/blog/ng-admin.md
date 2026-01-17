---
title: 使用ng-admin管理REST API
date: 2015-10-20
description: ng-admin是一个基于AngularJS的用于管理RESTful API的GUI工具。
tags:
  - PostgRest
  - Restful
  - ng-admin
categories:
  - Tech
---

ng-admin是一个基于AngularJS的用于管理RESTfu API的GUI工具。

Github地址如下

> [https://github.com/marmelab/ng-admin](https://github.com/marmelab/ng-admin)

之前我利用PostgRest构建了一些简单的RESTful API，这次正好用ng-admin将这些API通过WEB GUI管理起来。

## 一些概念

### Entity

在ng-admin中，一个实体（Entity）对应一个实际的REST资源，使用时需要创建实体并加入到app中

```javascript
var admin = nga.application('My first App').baseApiUrl('http://baseurl/');
var jgsb = nga.entity('jgsb').label('农业信息网');
// some settings here
admin.addEntity(jgs);
nga.configure(admin);
```

### CRUD

既然是对RESTful API的管理，自然需要支持4种操作。对应到ng-admin中即为

- listView
- creationView
- editionView
- showView (unused by default)
- deletionView

其中showView用于处理单条记录的显示。

### Field

可以认为与字段对应，可以通过如下方式设置

```javascript
jgs.listView()
	.title('农业信息网')
    .fields([
		nga.field('id'),
        nga.field('name').label('产品名称'),
        nga.field('market').label('市场'),
        nga.field('avg_price').label('均价'),
        nga.field('prod_place').label('产地'),
        nga.field('time').label('时间')
        ])
     .filters([
        nga.field('name').label('产品名称'),
        nga.field('market').label('市场'),
        ])
	 .sortField('id')
     .sortDir('ASC');
```

### param

顾名思义，有时我们访问REST资源需要进行一些参数控制，ng-admin自带了一些参数，诸如页码、每页数量、排序顺序等等，此外我们也可以通过Filters来自己添加参数。

## 一些问题

### ng-admin与PostgRest的参数适配

ng-admin中的参数形式与PostgRest中的参数形式不太一样，需要我们自己进行转换。
诸如_page和_perpage转换至Range和Range-Unit，sort的参数转换为order，filter的参数需要加.eq之类的参数。

```javascript
if (params._page) {
	headers = headers || {};
	headers['Range-Unit'] = what;
	headers['Range'] = ((params._page - 1) * params._perPage) + '-' + (params._page * params._perPage - 1);
	delete params._page;
	delete params._perPage;
}

// custom sort params
if (params._sortField) {
	params.order = params._sortField + '.' + params._sortDir.toLowerCase();
	delete params._sortField;
	delete params._sortDir;
}

// custom filters
if (params._filters) {
	for (var filter in params._filters) {
		params[filter] = 'eq.' + params._filters[filter];
	}
	delete params._filters;
}
```

### ngRepeat报错

```
[ngRepeat:dupes] Duplicates in a repeater are not allowed. Use 'track by' expression to specify unique keys
```

原因是数据库表中没有主键(id)，可以通过在视图中增加ROW_NUMBER()来充当id
