---
title: 网络LedMatrix
date: 2015-06-16
description: 浙江大学嵌入式系统的终极boss，实现一个网络访问的LED矩阵显示器。
tags:
  - Embedded System
  - Raspberry Pi
categories:
  - Tech
---

浙江大学嵌入式系统的终极boss。

## 实验目的

- 掌握Linux设备驱动程序的开发过程；
- 理解I2C总线协议；
- 复习socket编程（网络原理课）；
- 实现一个网络访问的LED矩阵显示器。

## 实验器材

### 硬件

- 树莓派2代板一块；
- 5V/1A电源一个；
- microUSB线一根；
- 面包板一块；
- 8x8 LED矩阵一块（不带I2C控制器）；
- 360Ω 1/8W电阻8颗，或360Ω 排阻1颗；
- 面包线若干。

以下为自备（可选）器材：
- PC（Windows/Mac OS/Linux）一台；
- USB-TTL串口线一根（FT232RL芯片或PL2303芯片）；
- 以太网线一根（可能还需要路由器等）。

### 软件

- 编译软件。

## 实验步骤

### 设计外部设备方案，画连线示意图

(Fritzing里找不到合适大小的led矩阵，就这么凑合一下了)

### 在面包板上连线，完成外部电路

### 编写Linux应用程序，能通过GPIO库控制LED矩阵显示字母数字

这个相对简单，这里我使用了python编写应用程序，使用RPi.GPIO库。显示的时候需要另起一个线程，可以使用python中的thread库。

### 编写Linux设备驱动程序

编写Linux设备驱动程序，能通过寄存器操纵GPIO控制LED矩阵，将这个LED矩阵做成`/dev/ledmatrix`，之后能通过cat命令输出字母数字来显示。

首先我们需要准备一些工具，包括树莓派的交叉编译环境以及树莓派系统源代码。

树莓派的系统代码我们可以从官方的github上clone下来：

```bash
git clone https://github.com/raspberrypi/linux.git
```

另外在开始之前最好保证你当前的系统与你下载的源码版本一致，不然你插入内核模块的时候会因为内核版本不一致而失败。

树莓派可以简单地通过一条命令更新内核到最新版本：

```bash
sudo rpi-update
```

如果版本还是不一致，可以直接修改`include/generated/utsrelease.h`内的版本号：

```cpp
#define UTS_RELEASE "3.18.14-v7+"
```

实验要求基本上只要写一个简单的字符设备驱动，所以我们要做的其实主要就是在内核中注册好我们的字符设备，并且实现它相应的一些文件操作(read、write之类)。

需要注意的是实验要求能够cat显示字符，cat实际上是做打开读取再关闭的操作，但是读取的时候它会一直读取知道读到EOF(0)，所以我们在实现read函数时不能忘记返回0。

另外关于gpio操作，可以使用linux/gpio.h中提供的一些函数接口，非常方便。显示的时候需要另起一个线程，这里需要编写内核线程的一些知识。

### 编写Linux应用程序，能通过TCP接受一个连接

最后一部分考虑流动显示的部分，利用树莓派做主机，监听10000端口，PC端去连接这个端口，并发送一些信息，树莓派收到信息后会流动显示发来的字符串，显示完成后会给PC发送消息表示显示完成，接着接收之后的消息。

## 实验结果

[Lab8 Demo](http://v.youku.com/v_show/id_XMTI2MzY1Mzc3Ng==.html?from=y1.7-2)
