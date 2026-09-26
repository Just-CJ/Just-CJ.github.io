---
cover: /img/gbc-art/riverside.webp
title: 网络LedMatrix
date: 2015-06-16
description: 浙江大学嵌入式系统的终极boss，实现一个网络访问的LED矩阵显示器。
tags:
  - Embedded System
  - Raspberry Pi
categories:
  - Tech
---

浙江大学嵌入式系统的终极 boss。

## 实验目的

- 掌握 Linux 设备驱动程序的开发过程；
- 理解 I2C 总线协议；
- 复习 socket 编程（网络原理课）；
- 实现一个网络访问的 LED 矩阵显示器。

## 实验器材

### 硬件

- 树莓派 2 代板一块；
- 5V/1A 电源一个；
- microUSB 线一根；
- 面包板一块；
- 8x8 LED 矩阵一块（不带 I2C 控制器）；
- 360Ω 1/8W 电阻 8 颗，或 360Ω 排阻 1 颗；
- 面包线若干。

以下为自备（可选）器材：
- PC（Windows/Mac OS/Linux）一台；
- USB-TTL 串口线一根（FT232RL 芯片或 PL2303 芯片）；
- 以太网线一根（可能还需要路由器等）。

### 软件

- 编译软件。

## 实验步骤

### 设计外部设备方案，画连线示意图

(Fritzing 里找不到合适大小的 led 矩阵，就这么凑合一下了)

### 在面包板上连线，完成外部电路

### 编写 Linux 应用程序，能通过 GPIO 库控制 LED 矩阵显示字母数字

这个相对简单，这里我使用了 python 编写应用程序，使用 RPi.GPIO 库。显示的时候需要另起一个线程，可以使用 python 中的 thread 库。

### 编写 Linux 设备驱动程序

编写 Linux 设备驱动程序，能通过寄存器操纵 GPIO 控制 LED 矩阵，将这个 LED 矩阵做成`/dev/ledmatrix`，之后能通过 cat 命令输出字母数字来显示。

首先我们需要准备一些工具，包括树莓派的交叉编译环境以及树莓派系统源代码。

树莓派的系统代码我们可以从官方的 github 上 clone 下来：

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

实验要求基本上只要写一个简单的字符设备驱动，所以我们要做的其实主要就是在内核中注册好我们的字符设备，并且实现它相应的一些文件操作(read、write 之类)。

需要注意的是实验要求能够 cat 显示字符，cat 实际上是做打开读取再关闭的操作，但是读取的时候它会一直读取知道读到 EOF(0)，所以我们在实现 read 函数时不能忘记返回 0。

另外关于 gpio 操作，可以使用 linux/gpio.h 中提供的一些函数接口，非常方便。显示的时候需要另起一个线程，这里需要编写内核线程的一些知识。

### 编写 Linux 应用程序，能通过 TCP 接受一个连接

最后一部分考虑流动显示的部分，利用树莓派做主机，监听 10000 端口，PC 端去连接这个端口，并发送一些信息，树莓派收到信息后会流动显示发来的字符串，显示完成后会给 PC 发送消息表示显示完成，接着接收之后的消息。

## 实验结果

[Lab8 Demo](http://v.youku.com/v_show/id_XMTI2MzY1Mzc3Ng==.html?from=y1.7-2)
