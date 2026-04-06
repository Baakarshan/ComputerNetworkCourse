#ifndef BRIDGE_H
#define BRIDGE_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

// 题37：输入帧（源地址、源端口、目的地址）
struct BridgeFrame {
    uint8_t source_address;      // 4 bit 地址，实际用 uint8_t 存放（只使用低 4 位）
    int source_port;             // 网段端口：1 或 2
    uint8_t destination_address; // 4 bit 地址，0xF 表示广播
};

// 网桥自学习与转发模拟器
class Bridge {
public:
    // 处理一帧，返回“目的端口决策”文本（例如 "2"、"1"、"丢弃(同端口)"）
    std::string process_frame(const BridgeFrame &frame);

    // 输出当前 MAC 地址表（地址 -> 端口）
    void print_mac_table() const;

private:
    // MAC 地址表：key=地址(0x0~0xE)，value=端口(1/2)
    std::unordered_map<int, int> mac_table_;
};

// 运行题37演示
void run_bridge_demo();

#endif
