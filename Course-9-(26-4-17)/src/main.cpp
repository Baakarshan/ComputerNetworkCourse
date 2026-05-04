#include <chrono>
#include <iostream>
#include <map>
#include <string>
#include <thread>

class PingServer {
public:
    std::string handle(const std::string& req) {
        if (req == "PING") return "PONG";
        return "ERR";
    }
};

class DhcpServer {
public:
    explicit DhcpServer(std::string fixedIp) : fixedIp_(std::move(fixedIp)) {}

    std::string requestIp(const std::string& clientId) {
        leases_[clientId] = fixedIp_;
        return fixedIp_;
    }

private:
    std::string fixedIp_;
    std::map<std::string, std::string> leases_;
};

int main() {
    PingServer ping;
    std::cout << "ping_reply=" << ping.handle("PING") << "\n";

    DhcpServer dhcp("192.168.1.2");
    std::cout << "lease_clientA=" << dhcp.requestIp("clientA") << "\n";
    std::cout << "lease_clientB=" << dhcp.requestIp("clientB") << "\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::cout << "simulation_done\n";
    return 0;
}
