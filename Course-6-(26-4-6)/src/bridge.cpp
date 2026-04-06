#include "bridge.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace {

// 将 4 bit 地址格式化为 0x0 ~ 0xF 形式
std::string to_hex4(uint8_t address) {
    std::ostringstream oss;
    oss << "0x" << std::uppercase << std::hex << static_cast<int>(address & 0x0F);
    return oss.str();
}

int other_port(int port) {
    return port == 1 ? 2 : 1;
}

}  // namespace

std::string Bridge::process_frame(const BridgeFrame &frame) {
    const int src_addr = frame.source_address & 0x0F;
    const int dst_addr = frame.destination_address & 0x0F;

    // 第一步：自学习（根据源地址学习“该地址从哪个端口来”）
    mac_table_[src_addr] = frame.source_port;

    // 第二步：根据目的地址决策
    if (dst_addr == 0x0F) {
        // 广播帧：向除源端口外的端口转发
        return std::to_string(other_port(frame.source_port));
    }

    auto it = mac_table_.find(dst_addr);
    if (it == mac_table_.end()) {
        // 未知单播：泛洪（两端口场景下，等价为发往另一个端口）
        return std::to_string(other_port(frame.source_port));
    }

    const int learned_port = it->second;
    if (learned_port == frame.source_port) {
        // 目的地址与源地址在同一端口侧，桥不过滤转发
        return "丢弃(同端口)";
    }

    // 已知单播：定向转发到学习到的目的端口
    return std::to_string(learned_port);
}

void Bridge::print_mac_table() const {
    std::vector<std::pair<int, int>> entries(mac_table_.begin(), mac_table_.end());
    std::sort(entries.begin(), entries.end(), [](const auto &a, const auto &b) {
        return a.first < b.first;
    });

    std::cout << "当前 MAC 地址表：\n";
    if (entries.empty()) {
        std::cout << "  (空)\n";
        return;
    }

    for (const auto &entry : entries) {
        std::cout << "  地址 " << to_hex4(static_cast<uint8_t>(entry.first))
                  << " -> 端口 " << entry.second << '\n';
    }
}

void run_bridge_demo() {
    std::cout << "========== 题37：网桥自学习功能模拟 ==========" << '\n';

    // 示例输入帧：每帧包含（源地址、源端口、目的地址）
    // 地址为 4 bit，0xF 代表广播
    const std::vector<BridgeFrame> frames = {
        {0x1, 1, 0x2},
        {0x2, 2, 0x1},
        {0x3, 1, 0xF},
        {0x4, 2, 0x3},
        {0x1, 1, 0x4},
        {0x2, 2, 0x9},
    };

    Bridge bridge;

    for (size_t i = 0; i < frames.size(); ++i) {
        const auto &frame = frames[i];
        const std::string out_port = bridge.process_frame(frame);

        std::cout << "帧 " << (i + 1)
                  << ": 源地址=" << to_hex4(frame.source_address)
                  << ", 源端口=" << frame.source_port
                  << ", 目的地址=" << to_hex4(frame.destination_address)
                  << " -> 目的端口决策=" << out_port << '\n';

        bridge.print_mac_table();
        std::cout << '\n';
    }
}
