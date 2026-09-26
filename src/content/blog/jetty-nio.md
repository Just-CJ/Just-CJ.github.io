---
cover: /img/gbc-art/momoka-guitar.webp
title: Jetty的网络IO模型
date: 2017-07-09
description: 分析Jetty服务器的网络IO模型架构设计。
tags:
  - java
  - Jetty
  - Nio
categories:
  - Tech
---

Jetty 是一个轻量级的 Java Web 服务器和 Servlet 容器，其网络 IO 模型的设计非常值得研究。

本文主要分析 Jetty 的 NIO（Non-blocking I/O）实现方式，包括其 Connector、SelectorManager 等核心组件的工作原理。

## NIO 简介

Java NIO（New I/O）是一种基于通道（Channel）和缓冲区（Buffer）的 I/O 方式。与传统的阻塞 I/O 不同，NIO 可以让线程在等待 I/O 完成时去做其他事情，从而提高并发处理能力。

## Jetty 的 Connector 架构

Jetty 使用 Connector 来处理网络连接。ServerConnector 是最常用的实现，它支持 HTTP/1.1 和 HTTP/2 协议。

## SelectorManager

SelectorManager 负责管理 Selector，它是 Jetty NIO 实现的核心。每个 ManagedSelector 对应一个 Selector，可以处理多个连接。

## 线程模型

Jetty 采用了 Acceptor-Selector-Handler 的线程模型：

1. **Acceptor 线程**：负责接受新连接
2. **Selector 线程**：负责检测就绪的 I/O 事件
3. **Handler 线程**：负责处理具体的请求

这种设计使得 Jetty 能够高效地处理大量并发连接。
