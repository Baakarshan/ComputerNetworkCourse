#include "parity_check.h"

int parity_check(const unsigned char *msg, const int msg_length) {
    if (msg == nullptr || msg_length <= 0) {
        return 0;
    }

    int one_count = 0;
    for (int i = 0; i < msg_length; ++i) {
        if (msg[i] != 0) {
            ++one_count;
        }
    }

    // 采用偶校验：1 的个数为偶数时通过。
    return (one_count % 2 == 0) ? 1 : 0;
}
