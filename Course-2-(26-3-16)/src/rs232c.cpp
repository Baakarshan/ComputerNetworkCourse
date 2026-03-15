#include "rs232c.h"

namespace {

// 这里采用最简单的电压映射：
// 逻辑 1 -> -5V，逻辑 0 -> +5V
constexpr double kVoltageOne = -5.0;
constexpr double kVoltageZero = 5.0;

// 每个字符使用：
// 1 个空闲位 + 1 个起始位 + 7 个数据位 + 1 个终止位
constexpr int kFrameSize = 10;

double bit_to_voltage(int bit) {
    return bit == 0 ? kVoltageZero : kVoltageOne;
}

int voltage_to_bit(double voltage) {
    return voltage < 0.0 ? 1 : 0;
}

}  // namespace

int rs232c_encode(double *volts, int volts_size, const char *msg, int size) {
    if (volts == nullptr || msg == nullptr || size < 0) {
        return -1;
    }

    const int required_size = size * kFrameSize;
    if (volts_size < required_size) {
        return -1;
    }

    int index = 0;
    for (int i = 0; i < size; ++i) {
        const unsigned char value = static_cast<unsigned char>(msg[i]) & 0x7F;

        // 空闲位固定为 1
        volts[index++] = bit_to_voltage(1);

        // 起始位固定为 0
        volts[index++] = bit_to_voltage(0);

        // 数据位按位小端序输出，即先发最低位
        for (int bit = 0; bit < 7; ++bit) {
            const int current_bit = (value >> bit) & 0x01;
            volts[index++] = bit_to_voltage(current_bit);
        }

        // 终止位固定为 1
        volts[index++] = bit_to_voltage(1);
    }

    return required_size;
}

int rs232c_decode(char *msg, int size, const double *volts, int volts_size) {
    if (msg == nullptr || volts == nullptr || size < 0) {
        return -1;
    }

    const int available_chars = volts_size / kFrameSize;
    if (size < available_chars) {
        return -1;
    }

    int msg_index = 0;
    for (int frame = 0; frame < available_chars; ++frame) {
        const int base = frame * kFrameSize;

        const int idle_bit = voltage_to_bit(volts[base]);
        const int start_bit = voltage_to_bit(volts[base + 1]);
        const int stop_bit = voltage_to_bit(volts[base + 9]);

        // 做最基本校验：空闲位应为 1，起始位应为 0，终止位应为 1
        if (idle_bit != 1 || start_bit != 0 || stop_bit != 1) {
            return -1;
        }

        unsigned char value = 0;
        for (int bit = 0; bit < 7; ++bit) {
            const int current_bit = voltage_to_bit(volts[base + 2 + bit]);
            value |= static_cast<unsigned char>(current_bit << bit);
        }

        msg[msg_index++] = static_cast<char>(value);
    }

    return msg_index;
}
