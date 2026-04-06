#ifndef ETHERNET_FRAME_HEADER_H
#define ETHERNET_FRAME_HEADER_H

#include <stdint.h>

// 题目36：使用 C 语言风格定义传统以太网帧头
// 传统以太网 II 头部固定为 14 字节：
// 6 字节目的地址 + 6 字节源地址 + 2 字节类型字段
struct ethernet_frame_header {
    uint8_t destination[6];
    uint8_t source[6];
    uint16_t ether_type;
};

#endif
