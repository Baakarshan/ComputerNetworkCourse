#include "modulation.h"

#include <cmath>

namespace {

constexpr double kPi = 3.14159265358979323846;
constexpr double kSampleRate = 1000.0;
constexpr double kCarrierFreq = 20.0;

unsigned char norm_bit(unsigned char x) {
    return x == 0 ? 0 : 1;
}

double pick_message_sample(const double *message, int msg_len, int i, int cover_len) {
    if (message == nullptr || msg_len <= 0 || cover_len <= 0) {
        return 0.0;
    }
    const int idx = (i * msg_len) / cover_len;
    return message[idx < msg_len ? idx : (msg_len - 1)];
}

unsigned char pick_message_bit(const unsigned char *message, int msg_len, int i, int cover_len) {
    if (message == nullptr || msg_len <= 0 || cover_len <= 0) {
        return 0;
    }
    const int idx = (i * msg_len) / cover_len;
    return norm_bit(message[idx < msg_len ? idx : (msg_len - 1)]);
}

}  // namespace

int generate_cover_signal(double *cover, const int size) {
    if (cover == nullptr || size < 0) {
        return -1;
    }

    for (int i = 0; i < size; ++i) {
        const double t = static_cast<double>(i) / kSampleRate;
        cover[i] = std::sin(2.0 * kPi * kCarrierFreq * t);
    }

    return size;
}

int simulate_digital_modulation_signal(unsigned char *message, const int size) {
    if (message == nullptr || size < 0) {
        return -1;
    }

    // 生成一个简单的方波比特流：每8个采样切换一次0/1
    for (int i = 0; i < size; ++i) {
        message[i] = static_cast<unsigned char>((i / 8) % 2);
    }

    return size;
}

int simulate_analog_modulation_signal(double *message, const int size) {
    if (message == nullptr || size < 0) {
        return -1;
    }

    // 低频正弦作为模拟调制信号
    constexpr double kMsgFreq = 2.0;
    for (int i = 0; i < size; ++i) {
        const double t = static_cast<double>(i) / kSampleRate;
        message[i] = 0.8 * std::sin(2.0 * kPi * kMsgFreq * t);
    }

    return size;
}

int modulate_digital_frequency(double *cover, const int cover_len, const unsigned char *message,
                               const int msg_len) {
    if (cover == nullptr || message == nullptr || cover_len < 0 || msg_len <= 0) {
        return -1;
    }

    // BFSK：bit=0使用较低频，bit=1使用较高频
    constexpr double f0 = 12.0;
    constexpr double f1 = 32.0;
    for (int i = 0; i < cover_len; ++i) {
        const double t = static_cast<double>(i) / kSampleRate;
        const unsigned char bit = pick_message_bit(message, msg_len, i, cover_len);
        const double f = bit ? f1 : f0;
        cover[i] = std::sin(2.0 * kPi * f * t);
    }

    return cover_len;
}

int modulate_analog_frequency(double *cover, const int cover_len, const double *message, const int msg_len) {
    if (cover == nullptr || message == nullptr || cover_len < 0 || msg_len <= 0) {
        return -1;
    }

    // FM：f_inst = fc + kf * m(t)
    constexpr double kf = 10.0;
    double phase = 0.0;
    for (int i = 0; i < cover_len; ++i) {
        const double m = pick_message_sample(message, msg_len, i, cover_len);
        const double finst = kCarrierFreq + kf * m;
        phase += 2.0 * kPi * finst / kSampleRate;
        cover[i] = std::sin(phase);
    }

    return cover_len;
}

int modulate_digital_amplitude(double *cover, const int cover_len, const unsigned char *message,
                               const int msg_len) {
    if (cover == nullptr || message == nullptr || cover_len < 0 || msg_len <= 0) {
        return -1;
    }

    // ASK：bit=0低幅值，bit=1高幅值
    constexpr double a0 = 0.3;
    constexpr double a1 = 1.0;
    for (int i = 0; i < cover_len; ++i) {
        const double t = static_cast<double>(i) / kSampleRate;
        const unsigned char bit = pick_message_bit(message, msg_len, i, cover_len);
        const double amp = bit ? a1 : a0;
        cover[i] = amp * std::sin(2.0 * kPi * kCarrierFreq * t);
    }

    return cover_len;
}

int modulate_analog_amplitude(double *cover, const int cover_len, const double *message, const int msg_len) {
    if (cover == nullptr || message == nullptr || cover_len < 0 || msg_len <= 0) {
        return -1;
    }

    // AM：s(t) = (1 + ka*m(t)) * cos(2*pi*fc*t)
    constexpr double ka = 0.8;
    for (int i = 0; i < cover_len; ++i) {
        const double t = static_cast<double>(i) / kSampleRate;
        const double m = pick_message_sample(message, msg_len, i, cover_len);
        cover[i] = (1.0 + ka * m) * std::cos(2.0 * kPi * kCarrierFreq * t);
    }

    return cover_len;
}

int modulate_digital_phase(double *cover, const int cover_len, const unsigned char *message, const int msg_len) {
    if (cover == nullptr || message == nullptr || cover_len < 0 || msg_len <= 0) {
        return -1;
    }

    // BPSK：bit=0相位0，bit=1相位pi
    for (int i = 0; i < cover_len; ++i) {
        const double t = static_cast<double>(i) / kSampleRate;
        const unsigned char bit = pick_message_bit(message, msg_len, i, cover_len);
        const double phase = bit ? kPi : 0.0;
        cover[i] = std::sin(2.0 * kPi * kCarrierFreq * t + phase);
    }

    return cover_len;
}

int modulate_analog_phase(double *cover, const int cover_len, const double *message, const int msg_len) {
    if (cover == nullptr || message == nullptr || cover_len < 0 || msg_len <= 0) {
        return -1;
    }

    // PM：s(t) = sin(2*pi*fc*t + kp*m(t))
    constexpr double kp = 1.2;
    for (int i = 0; i < cover_len; ++i) {
        const double t = static_cast<double>(i) / kSampleRate;
        const double m = pick_message_sample(message, msg_len, i, cover_len);
        cover[i] = std::sin(2.0 * kPi * kCarrierFreq * t + kp * m);
    }

    return cover_len;
}
