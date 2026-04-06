# Course-5 作业实现（第35/36/37题）

> 日期：2026-03-30（相对 2026-04-06 为七天前）

## 1. 作业要求概述

- 题35：用 C++ 模拟以太网网卡按目的地址判断是否接收帧。
- 题36：用 C 语言定义传统以太网帧头 `struct ethernet_frame_header`。
- 题37：用 C++ 多线程模拟 CSMA/CD 机制。

## 2. 目录结构

```text
src/
  main.cpp
  ethernet.h
  ethernet.cpp
  ethernet_frame_header.h
  csma_cd.h
  csma_cd.cpp
CMakeLists.txt
README.md
```

## 3. 实现说明

### 3.1 题35：MAC 地址匹配

按题意定义：

- `#define MAC_ADDRESS_LENGTH 6`
- `typedef unsigned char MAC_address[MAC_ADDRESS_LENGTH];`
- `struct EthernetFrame`
- `MAC_address this_mac_address;`
- `int mac_address_match(const struct EthernetFrame *frame);`

匹配规则：

- 目的地址等于本机地址：匹配。
- 目的地址为广播地址 `FF:FF:FF:FF:FF:FF`：匹配。
- 目的地址为多播地址（首字节最低位为 1）：匹配。
- 其他：不匹配。

返回值：

- `1`：匹配（接收）
- `0`：不匹配（丢弃）

### 3.2 题36：C 语言传统以太网帧头

`struct ethernet_frame_header` 按传统 Ethernet II 头部定义：

- 6 字节目的地址
- 6 字节源地址
- 2 字节类型字段

### 3.3 题37：CSMA/CD 多线程模拟

采用简化教学模型，演示以下机制：

- 监听信道（Carrier Sense）
- 多点接入（Multiple Access）
- 冲突检测（Collision Detection）
- 二进制指数退避（Binary Exponential Backoff）

程序中创建 4 个节点线程，每个节点发送 2 帧，输出发送成功、碰撞和退避重传过程。

## 4. 构建与运行

```powershell
cmake -S . -B build
cmake --build build
./build/course5_demo.exe
```
