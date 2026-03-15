#include "rs232c.h"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {

void print_usage() {
    std::cout
        << "Usage:\n"
        << "  rs232c_demo <text>\n";
}

void print_voltages(const std::vector<double>& volts) {
    std::cout << "电压序列:\n";
    for (std::size_t i = 0; i < volts.size(); ++i) {
        std::cout << std::fixed << std::setprecision(1) << volts[i];
        if (i + 1 != volts.size()) {
            std::cout << ' ';
        }
    }
    std::cout << '\n';
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    const std::string input = argv[1];
    std::vector<double> volts(input.size() * 10);

    const int encoded_size = rs232c_encode(volts.data(), static_cast<int>(volts.size()), input.c_str(),
                                           static_cast<int>(input.size()));
    if (encoded_size < 0) {
        std::cerr << "encode failed\n";
        return 1;
    }

    volts.resize(encoded_size);
    print_voltages(volts);

    std::vector<char> output(input.size() + 1, '\0');
    const int decoded_size =
        rs232c_decode(output.data(), static_cast<int>(output.size()), volts.data(), static_cast<int>(volts.size()));
    if (decoded_size < 0) {
        std::cerr << "decode failed\n";
        return 1;
    }

    std::cout << "解码结果:\n";
    std::cout.write(output.data(), decoded_size);
    std::cout << '\n';
    return 0;
}
