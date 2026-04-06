#ifndef ETHERNET_H
#define ETHERNET_H

#include <vector>

// 题目要求：先定义宏 MAC_ADDRESS_LENGTH
#define MAC_ADDRESS_LENGTH 6

// 题目要求：再用 typedef 定义 MAC_address 类型
typedef unsigned char MAC_address[MAC_ADDRESS_LENGTH];

// 题目要求：定义以太网帧结构体 EthernetFrame
struct EthernetFrame {
    MAC_address destination;      // 目的 MAC 地址
    MAC_address source;           // 源 MAC 地址
    unsigned short ether_type;    // 以太网类型字段（例如 0x0800 表示 IPv4）
    std::vector<unsigned char> payload;  // 负载数据（本题中不参与匹配，仅用于模拟完整帧）
};

// 题目要求：定义本机地址
extern MAC_address this_mac_address;

// 题目要求：实现匹配函数
// 返回值：0 为不匹配，1 为匹配
int mac_address_match(const struct EthernetFrame *frame);

// 辅助函数：将 MAC 地址格式化为可读字符串（用于演示输出）
const char *mac_to_string(const MAC_address mac);

#endif
