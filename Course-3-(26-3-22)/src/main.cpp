#include "modulation.h"
#include "multiplex.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {

using MuxFn = int (*)(unsigned char *, int, const unsigned char *, int, const unsigned char *, int);
using DemuxFn = int (*)(unsigned char *, int, unsigned char *, int, const unsigned char *, int);

bool parse_bit_string(const std::string &text, std::vector<unsigned char> *out) {
    if (out == nullptr || text.empty()) {
        return false;
    }
    out->clear();
    out->reserve(text.size());
    for (char ch : text) {
        if (ch == '0') {
            out->push_back(0);
        } else if (ch == '1') {
            out->push_back(1);
        } else {
            return false;
        }
    }
    return true;
}

void print_u8_sequence(const std::string &title, const std::vector<unsigned char> &data, int count) {
    std::cout << title << " (" << count << "):\n";
    for (int i = 0; i < count; ++i) {
        std::cout << static_cast<int>(data[i]);
        if (i + 1 != count) {
            std::cout << ' ';
        }
    }
    std::cout << "\n";
}

void print_double_sequence(const std::string &title, const std::vector<double> &data, int count) {
    std::cout << title << " (前" << count << "个采样):\n";
    for (int i = 0; i < count; ++i) {
        std::cout << std::fixed << std::setprecision(3) << data[i];
        if (i + 1 != count) {
            std::cout << ' ';
        }
    }
    std::cout << "\n";
}

void run_mux_case(const std::string &name, MuxFn mux_fn, DemuxFn demux_fn,
                  const std::vector<unsigned char> &a, const std::vector<unsigned char> &b) {
    std::vector<unsigned char> c(1024, 0);
    const int c_len = mux_fn(c.data(), static_cast<int>(c.size()), a.data(), static_cast<int>(a.size()),
                             b.data(), static_cast<int>(b.size()));
    if (c_len < 0) {
        std::cout << name << " 复用失败\n";
        return;
    }

    std::vector<unsigned char> a_out(a.size(), 0);
    std::vector<unsigned char> b_out(b.size(), 0);
    const int out_len = demux_fn(a_out.data(), static_cast<int>(a_out.size()), b_out.data(), static_cast<int>(b_out.size()),
                                 c.data(), c_len);
    if (out_len < 0) {
        std::cout << name << " 解复用失败\n";
        return;
    }

    std::cout << "\n===== " << name << " =====\n";
    print_u8_sequence("复用结果c", c, c_len);
    print_u8_sequence("解复用a", a_out, static_cast<int>(a_out.size()));
    print_u8_sequence("解复用b", b_out, static_cast<int>(b_out.size()));
}

void demo_multiplex() {
    const std::vector<unsigned char> a = {1, 0, 1, 1, 0, 1, 0, 1};
    const std::vector<unsigned char> b = {0, 1, 1, 0, 1, 0};

    std::cout << "=== 第25题：多路复用/解复用演示 ===\n";
    print_u8_sequence("原始a", a, static_cast<int>(a.size()));
    print_u8_sequence("原始b", b, static_cast<int>(b.size()));

    run_mux_case("默认接口(同步时分)", multiplex, demultiplex, a, b);
    run_mux_case("统计时分复用", multiplex_stat_tdm, demultiplex_stat_tdm, a, b);
    run_mux_case("频分复用", multiplex_fdm, demultiplex_fdm, a, b);
    run_mux_case("码分复用", multiplex_cdm, demultiplex_cdm, a, b);
}

void demo_multiplex_with_input(const std::vector<unsigned char> &a, const std::vector<unsigned char> &b) {
    std::cout << "=== 第25题：多路复用/解复用演示 ===\n";
    print_u8_sequence("原始a", a, static_cast<int>(a.size()));
    print_u8_sequence("原始b", b, static_cast<int>(b.size()));

    run_mux_case("默认接口(同步时分)", multiplex, demultiplex, a, b);
    run_mux_case("统计时分复用", multiplex_stat_tdm, demultiplex_stat_tdm, a, b);
    run_mux_case("频分复用", multiplex_fdm, demultiplex_fdm, a, b);
    run_mux_case("码分复用", multiplex_cdm, demultiplex_cdm, a, b);
}

void demo_modulation() {
    std::cout << "\n=== 第26题：调制演示 ===\n";

    constexpr int cover_len = 200;
    constexpr int dmsg_len = 40;

    std::vector<double> cover(cover_len, 0.0);
    std::vector<double> analog_msg(cover_len, 0.0);
    std::vector<unsigned char> digital_msg(dmsg_len, 0);

    generate_cover_signal(cover.data(), static_cast<int>(cover.size()));
    simulate_analog_modulation_signal(analog_msg.data(), static_cast<int>(analog_msg.size()));
    simulate_digital_modulation_signal(digital_msg.data(), static_cast<int>(digital_msg.size()));

    print_double_sequence("载波信号", cover, 12);
    print_u8_sequence("数字调制信号", digital_msg, static_cast<int>(digital_msg.size()));
    print_double_sequence("模拟调制信号", analog_msg, 12);

    std::vector<double> signal = cover;

    modulate_digital_frequency(signal.data(), static_cast<int>(signal.size()), digital_msg.data(),
                               static_cast<int>(digital_msg.size()));
    print_double_sequence("数字调频(BFSK)", signal, 12);

    signal = cover;
    modulate_analog_frequency(signal.data(), static_cast<int>(signal.size()), analog_msg.data(),
                              static_cast<int>(analog_msg.size()));
    print_double_sequence("模拟调频(FM)", signal, 12);

    signal = cover;
    modulate_digital_amplitude(signal.data(), static_cast<int>(signal.size()), digital_msg.data(),
                               static_cast<int>(digital_msg.size()));
    print_double_sequence("数字调幅(ASK)", signal, 12);

    signal = cover;
    modulate_analog_amplitude(signal.data(), static_cast<int>(signal.size()), analog_msg.data(),
                              static_cast<int>(analog_msg.size()));
    print_double_sequence("模拟调幅(AM)", signal, 12);

    signal = cover;
    modulate_digital_phase(signal.data(), static_cast<int>(signal.size()), digital_msg.data(),
                           static_cast<int>(digital_msg.size()));
    print_double_sequence("数字调相(BPSK)", signal, 12);

    signal = cover;
    modulate_analog_phase(signal.data(), static_cast<int>(signal.size()), analog_msg.data(),
                          static_cast<int>(analog_msg.size()));
    print_double_sequence("模拟调相(PM)", signal, 12);
}

}  // namespace

int main(int argc, char *argv[]) {
    if (argc == 3) {
        std::vector<unsigned char> a;
        std::vector<unsigned char> b;
        const bool ok_a = parse_bit_string(argv[1], &a);
        const bool ok_b = parse_bit_string(argv[2], &b);
        if (!ok_a || !ok_b) {
            std::cerr << "输入格式错误：请只传0/1字符串，例如：course3_demo 101101 0110\n";
            return 1;
        }
        demo_multiplex_with_input(a, b);
    } else {
        demo_multiplex();
    }

    demo_modulation();
    return 0;
}
