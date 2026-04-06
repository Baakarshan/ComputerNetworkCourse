#include "ethernet.h"

#include <array>
#include <cstdio>
#include <cstring>

// 本机 MAC 地址（示例值）
MAC_address this_mac_address = {0x10, 0x22, 0x33, 0x44, 0x55, 0x66};

namespace {

// 判断是否广播地址：FF:FF:FF:FF:FF:FF
bool is_broadcast_mac(const MAC_address mac) {
    for (int i = 0; i < MAC_ADDRESS_LENGTH; ++i) {
        if (mac[i] != 0xFF) {
            return false;
        }
    }
    return true;
}

// 判断是否多播地址：首字节最低位（I/G 位）为 1
bool is_multicast_mac(const MAC_address mac) {
    return (mac[0] & 0x01U) != 0;
}

}  // namespace

int mac_address_match(const struct EthernetFrame *frame) {
    // 安全防御：空指针直接不匹配
    if (frame == nullptr) {
        return 0;
    }

    // 1) 目的地址与本机地址完全一致 -> 匹配
    if (std::memcmp(frame->destination, this_mac_address, MAC_ADDRESS_LENGTH) == 0) {
        return 1;
    }

    // 2) 目的地址是广播地址 -> 匹配
    if (is_broadcast_mac(frame->destination)) {
        return 1;
    }

    // 3) 目的地址是多播地址 -> 匹配
    if (is_multicast_mac(frame->destination)) {
        return 1;
    }

    // 其他情况不匹配
    return 0;
}

const char *mac_to_string(const MAC_address mac) {
    // 使用线程局部静态缓冲，避免并发输出时互相覆盖
    thread_local char buffer[18];
    std::snprintf(
        buffer,
        sizeof(buffer),
        "%02X:%02X:%02X:%02X:%02X:%02X",
        mac[0],
        mac[1],
        mac[2],
        mac[3],
        mac[4],
        mac[5]);
    return buffer;
}
