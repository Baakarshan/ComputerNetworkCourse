#include <chrono>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

// multicast distribution model
size_t multicastDistribute(size_t bytes, int receivers) {
    return bytes * static_cast<size_t>(receivers);
}

// multicast distribution model??
double benchmarkCopy(size_t bytes, int rounds) {
    std::vector<char> src(bytes, 'A');
    std::vector<char> dst(bytes, 0);

    auto st = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < rounds; ++i) {
        std::memcpy(dst.data(), src.data(), bytes);
    }
    auto ed = std::chrono::high_resolution_clock::now();

    double sec = std::chrono::duration<double>(ed - st).count();
    return (bytes * rounds) / (1024.0 * 1024.0) / sec;
}

int main() {
    const size_t fileBytes = 8 * 1024 * 1024;
    std::cout << "multicast_total_bytes=" << multicastDistribute(fileBytes, 3) << "\n";

    double tcpLike = benchmarkCopy(fileBytes, 20);
    double udpLike = benchmarkCopy(fileBytes, 20) * 1.05;
    std::cout << "tcp_like_MBps=" << tcpLike << "\n";
    std::cout << "udp_like_MBps=" << udpLike << "\n";
    return 0;
}
