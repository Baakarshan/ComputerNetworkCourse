#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <thread>

static void stopAndWaitDemo(double lossProb) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    int seq = 0;
    for (int i = 0; i < 5; ++i) {
        bool ok = dist(rng) >= lossProb;
        if (ok) {
            std::cout << "SAW send seq=" << seq << " ack=" << seq << "\n";
            ++seq;
        } else {
            std::cout << "SAW send seq=" << seq << " lost, retransmit\n";
        }
    }
}

static void slidingWindowDemo(int windowSize) {
    int base = 0;
    int nextSeq = 0;
    const int total = 8;
    while (base < total) {
        while (nextSeq < base + windowSize && nextSeq < total) {
            std::cout << "SW send=" << nextSeq << "\n";
            ++nextSeq;
        }
        std::cout << "SW ack=" << base << "\n";
        ++base;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

int main(int argc, char** argv) {
    std::string ntpTime = "2019-05-01 10:41:00";
    if (argc > 1) ntpTime = argv[1];

    std::cout << "NTP server time=" << ntpTime << "\n";
    stopAndWaitDemo(0.3);
    slidingWindowDemo(4);
    return 0;
}
