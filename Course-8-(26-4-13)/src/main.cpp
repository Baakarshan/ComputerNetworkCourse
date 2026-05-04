#include <algorithm>
#include <iostream>
#include <vector>

struct Fragment {
    int length;
    int offset;
};

int is_in_net(unsigned char *ip, unsigned char *netip, unsigned char *mask) {
    for (int i = 0; i < 4; ++i) {
        if ((ip[i] & mask[i]) != (netip[i] & mask[i])) return 0;
    }
    return 1;
}

int classwise(unsigned char *ip) {
    const unsigned char first = ip[0];
    if (first <= 127) return 0;
    if (first <= 191) return 1;
    if (first <= 223) return 2;
    if (first <= 239) return 3;
    return 4;
}

std::vector<Fragment> fragmentPacket(int packetLength, const std::vector<int>& pathMTUs) {
    std::vector<Fragment> frags;
    if (packetLength <= 20 || pathMTUs.empty()) return frags;

    int payload = packetLength - 20;
    int offset = 0;
    int mtuMin = *std::min_element(pathMTUs.begin(), pathMTUs.end());
    int maxPayload = std::max(8, ((mtuMin - 20) / 8) * 8);

    while (payload > 0) {
        int piece = std::min(payload, maxPayload);
        frags.push_back({piece + 20, offset});
        payload -= piece;
        offset += piece;
    }
    return frags;
}

int main() {
    unsigned char ip[4] = {192, 168, 1, 99};
    unsigned char net[4] = {192, 168, 1, 0};
    unsigned char mask[4] = {255, 255, 255, 0};

    std::cout << "is_in_net=" << is_in_net(ip, net, mask) << "\n";
    std::cout << "classwise=" << classwise(ip) << "\n";

    const auto frags = fragmentPacket(4000, {1500, 1200, 900});
    std::cout << "fragment_count=" << frags.size() << "\n";
    for (const auto& f : frags) {
        std::cout << "len=" << f.length << ", offset=" << f.offset << "\n";
    }
    return 0;
}
