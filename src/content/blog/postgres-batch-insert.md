---
title: PostGreSQL使用Nodejs进行海量数据批量插入
date: 2015-07-22
description: 上次配置完数据库以后要求在其中插入一些数据，而且要使用nodejs，前前后后写了五个版本的代码最后才成功的往数据库里插了1000w条的数据。
tags:
  - PostgreSQL
  - Nodejs
categories:
  - Tech
---

上次配置完数据库以后要求在其中插入一些数据，而且要使用nodejs，小白第一次接触nodejs，真是被nodejs的各种回调搞得不要不要的，前前后后写了五个版本的代码最后才成功的往数据库里插了1000w条的数据。

## nodejs的pg库

既然使用nodejs来写数据库的操作，那肯定得找到相应的api库，pg就是一个比较常用的库

项目地址: [https://github.com/brianc/node-postgres](https://github.com/brianc/node-postgres)

可以使用npm进行安装

```bash
npm install pg
```

使用时需要require

## pg的批量插入操作

pg本身其实没有什么批量插入的接口，因此只能通过sql语句实现。

批量操作的原理是数据库原本是默认自动提交的，也就是执行一条sql语句需要提交一下。但是我们可以通过`BEGIN`语句关闭自动提交，然后做大量操作，最后再执行`COMMIT`语句提交，这样效率就被大大提高。

上述从`BEGIN`到`COMMIT`的一个过程被定义为一个`Transaction`。

## nodejs的异步流程控制

node的异步真的是很蛋疼的东西……基本上所有问题都出在这。

流程控制这玩意自己写真的不太好写，只好借助一下现成的库。这里使用了比较流行的async。

项目地址: [https://github.com/caolan/async](https://github.com/caolan/async)

先说下为什么要流程控制，本来我的做法狠简单粗暴，一次性把数据文件全部读出来，然后直接全部给query，pg在内部有一个QueryQueue会维护这些query的指令，所以我想我只要等数据库全部做完就行了。

事实上在我插入少量数据的时候这样做完全没有什么问题。但是！当我真的开始插入千万级数据的时候，程序却在读文件的时候从一开始的很快慢慢的变慢了，这让我非常不能理解，只能猜测是queue太长导致的副作用。

基于此，我只能将数据分批插入。下一批数据必须等上一批数据插完再插。

首先想到的解决方法是使用async.whilst函数，因为whilst函数中每个循环都必须等待上一次循环结束再开始，感觉很合我胃口，于是愉快改代码。

辛辛苦苦改完一跑结果报了个奇怪的错。

```bash
RangeError: Maximum call stack size exceeded error
```

上网一查，这是同时发起了太多异步回调操作，结果异步回调池满了。我一想不可能啊，我哪做了那么多回调，于是写了个小的测试代码测试这个whilst函数。

```javascript
async = require("async");
var count = 0;
async.whilst(
        function () { return count < 10000; },
        function (callback) {
            count++;
            callback();
        },
        function (err) {
            console.log("END");
        });
```

就这么一个单纯循环10000次的函数也报错了……说明whilst这个函数不支持太长的循环，猜测内部实现使用了迭代才会导致这么坑爹的情况。

既然不行那只好再换一个方法，看了看async那些函数找到一个cargo的函数，可以自动帮你进行负载控制，他的原理主要是有一个任务队列，每当队列里任务达到一个数量他就会先完成这些任务，如果任务超过负载数量，那么他会只处理负载数量个任务。

于是我把我的代码逻辑改成下面这样：

```javascript
var cargo = async(function(tasks, callback){
	// Process file read and insert here
}, 200); // process 200 files each time

// get the file path here and push to cargo
```

这样一来基本所有问题就都解决了。

## 多连接

事实上创建多个client对数据库同时操作也可以提升性能，对于每一个client，数据库会创建一个单独的进程来处理。实测开8个client可以提升大约1.5倍的性能，应该和处理器核心数有关，在往上就没什么明显的变化了。

## 结果

插入1000w条数据测试，大约半小时不到一点。

PS: 之前有次测试的时候跟服务器断连了，结果插了一半的程序被中断了，后来我就把把程序放到后台跑输出到日志，然后用watch指令监控。（~~真蛋疼~~）
