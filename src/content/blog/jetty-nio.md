---
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

Jetty是一个轻量级的Java Web服务器和Servlet容器，其网络IO模型的设计非常值得研究。

本文主要分析Jetty的NIO（Non-blocking I/O）实现方式，包括其Connector、SelectorManager等核心组件的工作原理。

## NIO简介

Java NIO（New I/O）是一种基于通道（Channel）和缓冲区（Buffer）的I/O方式。与传统的阻塞I/O不同，NIO可以让线程在等待I/O完成时去做其他事情，从而提高并发处理能力。

## Jetty的Connector架构

Jetty使用Connector来处理网络连接。ServerConnector是最常用的实现，它支持HTTP/1.1和HTTP/2协议。

## SelectorManager

SelectorManager负责管理Selector，它是Jetty NIO实现的核心。每个ManagedSelector对应一个Selector，可以处理多个连接。

## 线程模型

Jetty采用了Acceptor-Selector-Handler的线程模型：

1. **Acceptor线程**：负责接受新连接
2. **Selector线程**：负责检测就绪的I/O事件
3. **Handler线程**：负责处理具体的请求

这种设计使得Jetty能够高效地处理大量并发连接。
