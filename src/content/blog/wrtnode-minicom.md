---
cover: /img/gbc-art/rehearsal-still-life.webp
title: WRTnode使用串口传输文件
date: 2015-04-08
description: 本以为在WRTnode上使用串口的XModem协议传输文件只需要安装minicom就行了，没想到安装完以后却总是传输不成功，后来发现OpenWrt的系统里根本没装rx，sx这些工具。
tags:
  - Embedded System
  - WRTnode
categories:
  - Tech
---

本以为在 WRTnode 上使用串口的 XModem 协议传输文件只需要安装 minicom 就行了，没想到安装完以后却总是传输不成功，后来发现 OpenWrt 的系统里根本没装 rx，sx 这些工具(￣_￣|||) ……

后来查了一下需要安装 lrzsz 的 package。

嘛……总是事先的准备工作大概就是如下了

```bash
opkg update
opkg install lrzsz
opkg install minicom
```

---

那么简单讲讲如何通过 XModem 协议传输吧~

安装完 minicom 以后首先需要进行配置

```bash
minicom -s
```

可以看到配置画面，选择 Filenames and paths 设置上传与下载的默认路径。

返回上级菜单选择 Serial port setup 设置串口，注意将串口设备改为`/dev/ttyS0`, 流控制改为 No。

修改完返回上级菜单保存,选择 save as dfl。

板子上的设置基本这样，pc 端设置基本相同，主要就是需要注意串口设备不要搞错。

我是在 windows 下使用 ubuntu 虚拟机，还需要把 usb 设备分配给虚拟机。可以通过 lsusb 来看是否挂载成功。

---

OK~那么基本设置就完成了，我们可以开始尝试传输文件了

首先在 PC 端打开运行 minicom，`Ctrl-A S`发送文件，选择 XModem 协议。

选择要传输的文件（按空格键选中），回车便会开始传输。

当然此时并没有传输，因为板子上没有做好接收的准备。

让我们回到板子上，运行 minicom，`Ctrl-A R`接收文件，同样选择 XModem 协议，输入文件名开始接收，一段时间可以看到文件传输完毕。
