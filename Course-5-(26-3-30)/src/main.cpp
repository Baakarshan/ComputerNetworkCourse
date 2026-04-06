#include "csma_cd.h"
#include "ethernet.h"
#include "ethernet_frame_header.h"

#include <array>
#include <cstring>
#include <iostream>

namespace {

void demo_mac_match() {
    std::cout << "========== 题35：网卡目的地址匹配 ==========" << '\n';
    std::cout << "本机 MAC 地址: " << mac_to_string(this_mac_address) << "\n\n";

    EthernetFrame frame_for_self{};
    std::memcpy(frame_for_self.destination, this_mac_address, MAC_ADDRESS_LENGTH);

    EthernetFrame frame_for_multicast{};
    // 首字节最低位为 1，表示多播地址
    MAC_address multicast_addr = {0x01, 0x00, 0x5E, 0x10, 0x10, 0x10};
    std::memcpy(frame_for_multicast.destination, multicast_addr, MAC_ADDRESS_LENGTH);

    EthernetFrame frame_for_broadcast{};
    MAC_address broadcast_addr = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    std::memcpy(frame_for_broadcast.destination, broadcast_addr, MAC_ADDRESS_LENGTH);

    EthernetFrame frame_for_other{};
    MAC_address other_addr = {0x20, 0x22, 0x33, 0x44, 0x55, 0x66};
    std::memcpy(frame_for_other.destination, other_addr, MAC_ADDRESS_LENGTH);

    const EthernetFrame test_frames[] = {
        frame_for_self,
        frame_for_multicast,
        frame_for_broadcast,
        frame_for_other,
    };

    for (const auto &frame : test_frames) {
        const int matched = mac_address_match(&frame);
        std::cout << "目的地址: " << mac_to_string(frame.destination)
                  << " -> 匹配结果: " << (matched == 1 ? "接收" : "丢弃") << '\n';
    }

    std::cout << '\n';
}

void demo_c_ethernet_header() {
    std::cout << "========== 题36：C语言传统以太网帧头定义 ==========" << '\n';
    std::cout << "struct ethernet_frame_header 大小: " << sizeof(ethernet_frame_header) << " 字节\n";
    std::cout << "说明：传统以太网 II 头部为 14 字节（6 + 6 + 2）。\n\n";
}

}  // namespace

int main() {
    demo_mac_match();
    demo_c_ethernet_header();

    std::cout << "========== 题37：CSMA/CD 多线程模拟 ==========" << '\n';
    simulate_csma_cd();
    return 0;
}
