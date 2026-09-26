---
cover: /img/gbc-art/tomo-keyboard.webp
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

ng-admin 是一个基于 AngularJS 的用于管理 RESTfu API 的 GUI 工具。

Github 地址如下

> [https://github.com/marmelab/ng-admin](https://github.com/marmelab/ng-admin)

之前我利用 PostgRest 构建了一些简单的 RESTful API，这次正好用 ng-admin 将这些 API 通过 WEB GUI 管理起来。

## 一些概念

### Entity

在 ng-admin 中，一个实体（Entity）对应一个实际的 REST 资源，使用时需要创建实体并加入到 app 中

```javascript
var admin = nga.application('My first App').baseApiUrl('http://baseurl/');
var jgsb = nga.entity('jgsb').label('农业信息网');
// some settings here
admin.addEntity(jgs);
nga.configure(admin);
```

### CRUD

既然是对 RESTful API 的管理，自然需要支持 4 种操作。对应到 ng-admin 中即为

- listView
- creationView
- editionView
- showView (unused by default)
- deletionView

其中 showView 用于处理单条记录的显示。

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

顾名思义，有时我们访问 REST 资源需要进行一些参数控制，ng-admin 自带了一些参数，诸如页码、每页数量、排序顺序等等，此外我们也可以通过 Filters 来自己添加参数。

## 一些问题

### ng-admin 与 PostgRest 的参数适配

ng-admin 中的参数形式与 PostgRest 中的参数形式不太一样，需要我们自己进行转换。
诸如_page 和_perpage 转换至 Range 和 Range-Unit，sort 的参数转换为 order，filter 的参数需要加.eq 之类的参数。

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

### ngRepeat 报错

```plain
[ngRepeat:dupes] Duplicates in a repeater are not allowed. Use 'track by' expression to specify unique keys
```

原因是数据库表中没有主键(id)，可以通过在视图中增加 ROW_NUMBER()来充当 id
