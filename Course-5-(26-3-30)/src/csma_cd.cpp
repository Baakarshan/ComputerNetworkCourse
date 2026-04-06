#include "csma_cd.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

namespace {

// 共享介质：用于模拟以太网总线
struct SharedMedium {
    std::mutex medium_mutex;   // 保护活跃发送计数
    int active_senders = 0;    // 当前正在“发送”的站点数量
};

std::mutex g_print_mutex;

void safe_print(const std::string &text) {
    std::lock_guard<std::mutex> lock(g_print_mutex);
    std::cout << text << std::endl;
}

// 单个站点发送一帧，返回是否成功
bool send_one_frame(SharedMedium &medium, int node_id, int frame_id, std::mt19937 &rng) {
    constexpr int kMaxAttempts = 16;          // 以太网经典最大重传次数
    constexpr int kSlotTimeMs = 20;           // 槽时间（教学模拟值）
    constexpr int kTransmitWindowMs = 15;     // 一次发送占用介质的时间（教学模拟值）

    int collision_count = 0;

    for (int attempt = 1; attempt <= kMaxAttempts; ++attempt) {
        // 监听信道（简化）：若当前有发送者，等待一个槽后再试
        while (true) {
            {
                std::lock_guard<std::mutex> lock(medium.medium_mutex);
                if (medium.active_senders == 0) {
                    break;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(kSlotTimeMs));
        }

        bool collision = false;

        // 模拟传播时延窗口：多个节点可能几乎同时判断“空闲”，从而触发冲突
        std::uniform_int_distribution<int> propagation_delay_ms(0, 3);
        std::this_thread::sleep_for(std::chrono::milliseconds(propagation_delay_ms(rng)));

        // 开始发送并做碰撞检测：若同一时刻活跃发送者超过 1，则视为发生碰撞
        {
            std::lock_guard<std::mutex> lock(medium.medium_mutex);
            ++medium.active_senders;
            if (medium.active_senders > 1) {
                collision = true;
            }
        }

        // 发送窗口期间持续占用介质
        std::this_thread::sleep_for(std::chrono::milliseconds(kTransmitWindowMs));

        // 发送结束，离开介质
        {
            std::lock_guard<std::mutex> lock(medium.medium_mutex);
            --medium.active_senders;
        }

        if (!collision) {
            safe_print("节点 " + std::to_string(node_id) + " 的帧 " + std::to_string(frame_id) +
                       " 发送成功（尝试次数=" + std::to_string(attempt) + "）");
            return true;
        }

        ++collision_count;
        safe_print("节点 " + std::to_string(node_id) + " 的帧 " + std::to_string(frame_id) +
                   " 发生碰撞，执行二进制指数退避（碰撞次数=" +
                   std::to_string(collision_count) + "）");

        // 二进制指数退避：随机等待 [0, 2^k - 1] 个槽
        const int k = std::min(collision_count, 10);
        const int max_backoff_slots = (1 << k) - 1;
        std::uniform_int_distribution<int> backoff_dist(0, std::max(0, max_backoff_slots));
        const int backoff_slots = backoff_dist(rng);
        std::this_thread::sleep_for(std::chrono::milliseconds(backoff_slots * kSlotTimeMs));
    }

    safe_print("节点 " + std::to_string(node_id) + " 的帧 " + std::to_string(frame_id) +
               " 发送失败（达到最大重传次数）");
    return false;
}

void node_worker(SharedMedium &medium, int node_id, int frame_count) {
    // 每个线程使用独立随机数发生器，避免竞争与重复序列
    std::mt19937 rng(static_cast<unsigned int>(
        std::chrono::steady_clock::now().time_since_epoch().count() + node_id * 97));

    for (int frame_id = 1; frame_id <= frame_count; ++frame_id) {
        (void)send_one_frame(medium, node_id, frame_id, rng);
        // 两帧之间留一点随机间隔，模拟业务层到达的随机性
        std::uniform_int_distribution<int> idle_dist(5, 40);
        std::this_thread::sleep_for(std::chrono::milliseconds(idle_dist(rng)));
    }
}

}  // namespace

void simulate_csma_cd() {
    constexpr int kNodeCount = 4;     // 4 个站点并发
    constexpr int kFramePerNode = 2;  // 每个站点发送 2 帧

    SharedMedium medium;
    std::vector<std::thread> workers;
    workers.reserve(kNodeCount);

    safe_print("开始模拟 CSMA/CD：4 个节点并发发送，每个节点 2 帧。\n");

    for (int node_id = 1; node_id <= kNodeCount; ++node_id) {
        workers.emplace_back(node_worker, std::ref(medium), node_id, kFramePerNode);
    }

    for (auto &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    safe_print("\nCSMA/CD 模拟结束。\n");
}
