#include "ap_deployment.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <tuple>
#include <vector>

namespace {

// 建筑参数（按题目给定）
constexpr double kBuildingLength = 100.0;   // x 方向长度（米）
constexpr double kBuildingWidth = 80.0;     // y 方向宽度（米）
constexpr int kFloorCount = 3;              // 楼层数
constexpr double kFloorHeight = 3.5;        // 层高（米）
constexpr double kThresholdRssi = -65.0;    // 目标信号阈值（dBm）

// 墙体衰减参数（dB）
constexpr double kLossLoadBearing = 12.0;   // 承重墙
constexpr double kLossPartition = 6.0;      // 普通隔断墙

// AP 与采样点模型
struct Point3D {
    double x;
    double y;
    int floor;
};

enum class Band {
    GHz2_4,
    GHz5,
};

struct AP {
    Point3D pos;
    Band band;
    int channel;
};

std::string band_to_string(Band band) {
    return band == Band::GHz2_4 ? "2.4GHz" : "5GHz";
}

// 简化墙体模型：
// 1) 走廊中心 y=40，走廊宽 2m（y in [39,41]），走廊与教室之间按普通墙衰减。
// 2) 教室沿 x 方向每 15m 分隔，视为普通隔断墙。
// 3) 在 x=50 位置设置一堵贯穿承重墙（简化核心筒影响）。
double estimate_wall_loss(const Point3D &a, const Point3D &b) {
    double loss = 0.0;

    // 走廊分界墙：如果一端在走廊内，另一端在教室区，认为穿过一次普通墙
    const bool a_in_corridor = (a.y >= 39.0 && a.y <= 41.0);
    const bool b_in_corridor = (b.y >= 39.0 && b.y <= 41.0);
    if (a_in_corridor != b_in_corridor) {
        loss += kLossPartition;
    }

    // 教室分隔墙（x=15,30,45,60,75,90）
    for (double wall_x : {15.0, 30.0, 45.0, 60.0, 75.0, 90.0}) {
        if ((a.x - wall_x) * (b.x - wall_x) < 0.0) {
            loss += kLossPartition;
        }
    }

    // 承重墙（x=50）
    if ((a.x - 50.0) * (b.x - 50.0) < 0.0) {
        loss += kLossLoadBearing;
    }

    return loss;
}

// 计算 AP 到目标点的接收信号强度（简化模型）
double estimate_rssi(const AP &ap, const Point3D &target) {
    const double dx = ap.pos.x - target.x;
    const double dy = ap.pos.y - target.y;
    const double dz = (ap.pos.floor - target.floor) * kFloorHeight;

    const double distance = std::max(1.0, std::sqrt(dx * dx + dy * dy + dz * dz));
    const double tx_power = ap.band == Band::GHz2_4 ? 20.0 : 18.0;

    // 自由空间近似路径损耗模型（教学简化）
    const double path_loss = 40.0 + 20.0 * std::log10(distance);
    const double wall_loss = estimate_wall_loss(ap.pos, target);

    return tx_power - path_loss - wall_loss;
}

// 生成楼层采样点（用于覆盖评估与热力图）
std::vector<Point3D> build_floor_samples(int floor, double step) {
    std::vector<Point3D> points;
    for (double x = 0.0; x <= kBuildingLength; x += step) {
        for (double y = 0.0; y <= kBuildingWidth; y += step) {
            points.push_back({x, y, floor});
        }
    }
    return points;
}

// 在走廊中心线生成候选 AP 位置（快速、简单）
std::vector<Point3D> build_ap_candidates(int floor) {
    std::vector<Point3D> cands;
    for (double x = 10.0; x <= 90.0; x += 5.0) {
        cands.push_back({x, 40.0, floor});
    }
    return cands;
}

struct PairChoice {
    Point3D a;
    Point3D b;
    double score;
};

// 对单层选取两个 AP 位置：目标是“覆盖更多，重叠更少”
PairChoice choose_two_ap_positions_for_floor(int floor) {
    const auto candidates = build_ap_candidates(floor);
    const auto samples = build_floor_samples(floor, 5.0);

    PairChoice best{{10.0, 40.0, floor}, {90.0, 40.0, floor}, -std::numeric_limits<double>::infinity()};

    for (size_t i = 0; i < candidates.size(); ++i) {
        for (size_t j = i + 1; j < candidates.size(); ++j) {
            AP ap1{candidates[i], Band::GHz2_4, 1};
            AP ap2{candidates[j], Band::GHz5, 36};

            int covered = 0;
            int overlap = 0;

            for (const auto &p : samples) {
                const double r1 = estimate_rssi(ap1, p);
                const double r2 = estimate_rssi(ap2, p);
                const bool c1 = r1 >= kThresholdRssi;
                const bool c2 = r2 >= kThresholdRssi;

                if (c1 || c2) {
                    ++covered;
                }
                if (c1 && c2) {
                    ++overlap;
                }
            }

            // 简单评分：优先覆盖率，同时抑制重叠
            const double score = static_cast<double>(covered) - 0.35 * static_cast<double>(overlap);
            if (score > best.score) {
                best = {candidates[i], candidates[j], score};
            }
        }
    }

    return best;
}

// 同频干扰代价：同一频段且同信道时，距离越近代价越高
// 代价采用 1/(d+1) 叠加
double channel_interference_cost(const std::vector<AP> &aps, const std::vector<int> &channels, Band band) {
    std::vector<int> idx;
    for (size_t i = 0; i < aps.size(); ++i) {
        if (aps[i].band == band) {
            idx.push_back(static_cast<int>(i));
        }
    }

    double total = 0.0;
    for (size_t i = 0; i < idx.size(); ++i) {
        for (size_t j = i + 1; j < idx.size(); ++j) {
            const AP &a = aps[idx[i]];
            const AP &b = aps[idx[j]];
            if (a.channel != b.channel) {
                continue;
            }
            const double dx = a.pos.x - b.pos.x;
            const double dy = a.pos.y - b.pos.y;
            const double dz = (a.pos.floor - b.pos.floor) * kFloorHeight;
            const double d = std::sqrt(dx * dx + dy * dy + dz * dz);
            total += 1.0 / (d + 1.0);
        }
    }
    return total;
}

// 穷举每个频段的信道组合，选总干扰最小
void assign_channels(std::vector<AP> &aps) {
    // 2.4GHz 仅使用 1/6/11，5GHz 使用 36/44/149
    const std::vector<int> ch24 = {1, 6, 11};
    const std::vector<int> ch5 = {36, 44, 149};

    std::vector<int> idx24;
    std::vector<int> idx5;
    for (size_t i = 0; i < aps.size(); ++i) {
        if (aps[i].band == Band::GHz2_4) {
            idx24.push_back(static_cast<int>(i));
        } else {
            idx5.push_back(static_cast<int>(i));
        }
    }

    double best24 = std::numeric_limits<double>::infinity();
    std::vector<int> best24assign(idx24.size(), ch24[0]);

    // 三层正好 3 台 2.4G AP，直接 3^3 穷举
    for (int a : ch24) {
        for (int b : ch24) {
            for (int c : ch24) {
                for (size_t k = 0; k < idx24.size(); ++k) {
                    aps[idx24[k]].channel = (k == 0 ? a : (k == 1 ? b : c));
                }
                const double cost = channel_interference_cost(aps, ch24, Band::GHz2_4);
                if (cost < best24) {
                    best24 = cost;
                    best24assign = {a, b, c};
                }
            }
        }
    }
    for (size_t k = 0; k < idx24.size(); ++k) {
        aps[idx24[k]].channel = best24assign[k];
    }

    double best5 = std::numeric_limits<double>::infinity();
    std::vector<int> best5assign(idx5.size(), ch5[0]);
    for (int a : ch5) {
        for (int b : ch5) {
            for (int c : ch5) {
                for (size_t k = 0; k < idx5.size(); ++k) {
                    aps[idx5[k]].channel = (k == 0 ? a : (k == 1 ? b : c));
                }
                const double cost = channel_interference_cost(aps, ch5, Band::GHz5);
                if (cost < best5) {
                    best5 = cost;
                    best5assign = {a, b, c};
                }
            }
        }
    }
    for (size_t k = 0; k < idx5.size(); ++k) {
        aps[idx5[k]].channel = best5assign[k];
    }
}

void print_heatmap(const std::vector<AP> &aps, int floor) {
    std::cout << "\n第 " << floor + 1 << " 层信号热力图（# 强, + 中, . 弱, 空格 无覆盖）\n";

    // y 反向输出，视觉上更像平面图（上北下南）
    for (double y = kBuildingWidth; y >= 0.0; y -= 10.0) {
        for (double x = 0.0; x <= kBuildingLength; x += 10.0) {
            Point3D p{x, y, floor};
            double best = -999.0;
            for (const auto &ap : aps) {
                best = std::max(best, estimate_rssi(ap, p));
            }

            char ch = ' ';
            if (best >= -65.0) {
                ch = '#';
            } else if (best >= -75.0) {
                ch = '+';
            } else if (best >= -85.0) {
                ch = '.';
            }
            std::cout << ch;
        }
        std::cout << '\n';
    }
}

}  // namespace

void run_ap_deployment_demo() {
    std::cout << "========== 题38：教学楼 AP 部署模拟 ==========" << '\n';

    // 每层 2 个 AP（一个 2.4G，一个 5G），共 6 个 AP
    std::vector<AP> aps;

    for (int floor = 0; floor < kFloorCount; ++floor) {
        const PairChoice choice = choose_two_ap_positions_for_floor(floor);
        aps.push_back({choice.a, Band::GHz2_4, 1});
        aps.push_back({choice.b, Band::GHz5, 36});
    }

    assign_channels(aps);

    std::cout << "AP 最优位置与信道分配（简化模型结果）：\n";
    for (size_t i = 0; i < aps.size(); ++i) {
        const auto &ap = aps[i];
        std::cout << "  AP" << (i + 1)
                  << ": 楼层=" << (ap.pos.floor + 1)
                  << ", 坐标=(" << std::fixed << std::setprecision(1) << ap.pos.x << "m, " << ap.pos.y << "m)"
                  << ", 频段=" << band_to_string(ap.band)
                  << ", 信道=" << ap.channel << '\n';
    }

    // 输出每层覆盖率
    for (int floor = 0; floor < kFloorCount; ++floor) {
        const auto samples = build_floor_samples(floor, 5.0);
        int covered = 0;
        for (const auto &p : samples) {
            double best = -999.0;
            for (const auto &ap : aps) {
                best = std::max(best, estimate_rssi(ap, p));
            }
            if (best >= kThresholdRssi) {
                ++covered;
            }
        }

        const double coverage_ratio = samples.empty() ? 0.0 : (100.0 * covered / samples.size());
        std::cout << "第 " << (floor + 1) << " 层覆盖率(>=-65dBm): "
                  << std::fixed << std::setprecision(2) << coverage_ratio << "%\n";
    }

    for (int floor = 0; floor < kFloorCount; ++floor) {
        print_heatmap(aps, floor);
    }

    std::cout << '\n';
}
