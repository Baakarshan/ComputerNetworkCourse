#include "multiplex.h"

#include <algorithm>

namespace {

unsigned char norm_bit(unsigned char x) {
    return x == 0 ? 0 : 1;
}

unsigned short clamp_len_to_u16(int len) {
    if (len < 0) {
        return 0;
    }
    if (len > 65535) {
        return 65535;
    }
    return static_cast<unsigned short>(len);
}

}  // namespace

int multiplex(unsigned char *c, const int c_size, const unsigned char *a, const int a_len,
              const unsigned char *b, const int b_len) {
    return multiplex_sync_tdm(c, c_size, a, a_len, b, b_len);
}

int demultiplex(unsigned char *a, const int a_size, unsigned char *b, const int b_size,
                const unsigned char *c, const int c_len) {
    return demultiplex_sync_tdm(a, a_size, b, b_size, c, c_len);
}

int multiplex_sync_tdm(unsigned char *c, int c_size, const unsigned char *a, int a_len,
                       const unsigned char *b, int b_len) {
    if (c == nullptr || a == nullptr || b == nullptr || c_size < 0 || a_len < 0 || b_len < 0) {
        return -1;
    }

    const int max_len = std::max(a_len, b_len);
    const int required = max_len * 2;
    if (c_size < required) {
        return -1;
    }

    int index = 0;
    for (int i = 0; i < max_len; ++i) {
        const unsigned char av = (i < a_len) ? norm_bit(a[i]) : 0;
        const unsigned char bv = (i < b_len) ? norm_bit(b[i]) : 0;
        c[index++] = av;
        c[index++] = bv;
    }
    return required;
}

int demultiplex_sync_tdm(unsigned char *a, int a_size, unsigned char *b, int b_size,
                         const unsigned char *c, int c_len) {
    if (a == nullptr || b == nullptr || c == nullptr || a_size < 0 || b_size < 0 || c_len < 0) {
        return -1;
    }

    const int slots = c_len / 2;
    const int recover_a = std::min(slots, a_size);
    const int recover_b = std::min(slots, b_size);
    for (int i = 0; i < recover_a; ++i) {
        a[i] = norm_bit(c[2 * i]);
    }
    for (int i = 0; i < recover_b; ++i) {
        b[i] = norm_bit(c[2 * i + 1]);
    }
    return slots;
}

int multiplex_stat_tdm(unsigned char *c, int c_size, const unsigned char *a, int a_len,
                       const unsigned char *b, int b_len) {
    if (c == nullptr || a == nullptr || b == nullptr || c_size < 0 || a_len < 0 || b_len < 0) {
        return -1;
    }

    // 每个有效位编码为3字节：[源通道(0/1), 索引低字节, 索引高字节]
    int index = 0;
    for (int i = 0; i < a_len; ++i) {
        if (norm_bit(a[i]) == 1) {
            if (index + 3 > c_size) {
                return -1;
            }
            const unsigned short pos = clamp_len_to_u16(i);
            c[index++] = 0;
            c[index++] = static_cast<unsigned char>(pos & 0xFF);
            c[index++] = static_cast<unsigned char>((pos >> 8) & 0xFF);
        }
    }
    for (int i = 0; i < b_len; ++i) {
        if (norm_bit(b[i]) == 1) {
            if (index + 3 > c_size) {
                return -1;
            }
            const unsigned short pos = clamp_len_to_u16(i);
            c[index++] = 1;
            c[index++] = static_cast<unsigned char>(pos & 0xFF);
            c[index++] = static_cast<unsigned char>((pos >> 8) & 0xFF);
        }
    }

    return index;
}

int demultiplex_stat_tdm(unsigned char *a, int a_size, unsigned char *b, int b_size,
                         const unsigned char *c, int c_len) {
    if (a == nullptr || b == nullptr || c == nullptr || a_size < 0 || b_size < 0 || c_len < 0) {
        return -1;
    }

    // 先全部置0，再根据记录位置还原1
    for (int i = 0; i < a_size; ++i) {
        a[i] = 0;
    }
    for (int i = 0; i < b_size; ++i) {
        b[i] = 0;
    }

    if (c_len % 3 != 0) {
        return -1;
    }

    int recovered = 0;
    for (int i = 0; i < c_len; i += 3) {
        const unsigned char src = c[i];
        const int pos = static_cast<int>(c[i + 1]) | (static_cast<int>(c[i + 2]) << 8);
        if (src == 0) {
            if (pos < a_size) {
                a[pos] = 1;
                ++recovered;
            }
        } else if (src == 1) {
            if (pos < b_size) {
                b[pos] = 1;
                ++recovered;
            }
        } else {
            return -1;
        }
    }

    return recovered;
}

int multiplex_fdm(unsigned char *c, int c_size, const unsigned char *a, int a_len,
                  const unsigned char *b, int b_len) {
    if (c == nullptr || a == nullptr || b == nullptr || c_size < 0 || a_len < 0 || b_len < 0) {
        return -1;
    }

    // 教学简化：把两个“子频带”并到同一个采样点里，bit0表示a，bit1表示b
    const int max_len = std::max(a_len, b_len);
    if (c_size < max_len) {
        return -1;
    }

    for (int i = 0; i < max_len; ++i) {
        const unsigned char av = (i < a_len) ? norm_bit(a[i]) : 0;
        const unsigned char bv = (i < b_len) ? norm_bit(b[i]) : 0;
        c[i] = static_cast<unsigned char>((av & 0x01) | ((bv & 0x01) << 1));
    }

    return max_len;
}

int demultiplex_fdm(unsigned char *a, int a_size, unsigned char *b, int b_size,
                    const unsigned char *c, int c_len) {
    if (a == nullptr || b == nullptr || c == nullptr || a_size < 0 || b_size < 0 || c_len < 0) {
        return -1;
    }

    const int recover_a = std::min(c_len, a_size);
    const int recover_b = std::min(c_len, b_size);
    for (int i = 0; i < recover_a; ++i) {
        a[i] = static_cast<unsigned char>(c[i] & 0x01);
    }
    for (int i = 0; i < recover_b; ++i) {
        b[i] = static_cast<unsigned char>((c[i] >> 1) & 0x01);
    }
    return c_len;
}

int multiplex_cdm(unsigned char *c, int c_size, const unsigned char *a, int a_len,
                  const unsigned char *b, int b_len) {
    if (c == nullptr || a == nullptr || b == nullptr || c_size < 0 || a_len < 0 || b_len < 0) {
        return -1;
    }

    // 采用长度为2的正交码：a=[1,1], b=[1,-1]
    // 每个时隙输出两个码片，取值{-2,0,2}，存储时加2偏置成{0,2,4}
    const int max_len = std::max(a_len, b_len);
    const int required = max_len * 2;
    if (c_size < required) {
        return -1;
    }

    int out = 0;
    for (int i = 0; i < max_len; ++i) {
        const int abit = (i < a_len && norm_bit(a[i]) == 1) ? 1 : -1;
        const int bbit = (i < b_len && norm_bit(b[i]) == 1) ? 1 : -1;

        const int chip0 = abit + bbit;
        const int chip1 = abit - bbit;

        c[out++] = static_cast<unsigned char>(chip0 + 2);
        c[out++] = static_cast<unsigned char>(chip1 + 2);
    }

    return required;
}

int demultiplex_cdm(unsigned char *a, int a_size, unsigned char *b, int b_size,
                    const unsigned char *c, int c_len) {
    if (a == nullptr || b == nullptr || c == nullptr || a_size < 0 || b_size < 0 || c_len < 0) {
        return -1;
    }

    const int slots = c_len / 2;
    const int recover = std::max(a_size, b_size);
    const int limit = std::min(slots, recover);

    for (int i = 0; i < limit; ++i) {
        const int chip0 = static_cast<int>(c[2 * i]) - 2;
        const int chip1 = static_cast<int>(c[2 * i + 1]) - 2;

        // 相关解码：sa ~ (chip0+chip1), sb ~ (chip0-chip1)
        const int sa = chip0 + chip1;
        const int sb = chip0 - chip1;

        if (i < a_size) {
            a[i] = (sa >= 0) ? 1 : 0;
        }
        if (i < b_size) {
            b[i] = (sb >= 0) ? 1 : 0;
        }
    }

    return slots;
}
