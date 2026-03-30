#include "parity_check.h"

#include <iostream>
#include <string>
#include <vector>

namespace {

void print_usage() {
    std::cout
        << "Usage:\n"
        << "  course4_demo <bit-string>\n"
        << "Example:\n"
        << "  course4_demo 1010\n";
}

bool parse_message(const std::string &input, std::vector<unsigned char> &msg) {
    msg.clear();
    msg.reserve(input.size());

    for (char ch : input) {
        if (ch == '0') {
            msg.push_back(0);
        } else if (ch == '1') {
            msg.push_back(1);
        } else {
            return false;
        }
    }

    return !msg.empty();
}

}  // namespace

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    const std::string input = argv[1];
    std::vector<unsigned char> msg;
    if (!parse_message(input, msg)) {
        std::cerr << "输入错误：仅允许由 0 和 1 组成的非空字符串。\n";
        return 1;
    }

    const int result = parity_check(msg.data(), static_cast<int>(msg.size()));

    std::cout << "输入消息: " << input << '\n';
    std::cout << "校验结果: " << (result == 1 ? "通过" : "失败") << '\n';
    return 0;
}
