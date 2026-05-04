#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

class TimeServer {
public:
    std::string query() const {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tmv{};
#ifdef _WIN32
        localtime_s(&tmv, &t);
#else
        localtime_r(&t, &tmv);
#endif
        std::ostringstream oss;
        oss << std::put_time(&tmv, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};

int main() {
    TimeServer server;
    std::cout << "time_reply=" << server.query() << "\n";
    return 0;
}
