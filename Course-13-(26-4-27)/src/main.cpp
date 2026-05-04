#include <algorithm>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <tuple>
#include <vector>

struct RouteItem {
    std::string dest;
    int cost;
    std::string nextHop;
};

// simplified RIP update
std::vector<RouteItem> ripUpdate(const std::vector<RouteItem>& r1, const std::vector<RouteItem>& r2, const std::string& via) {
    std::map<std::string, RouteItem> table;
    for (const auto& x : r1) table[x.dest] = x;

    for (const auto& x : r2) {
        int newCost = std::min(16, x.cost + 1);
        auto it = table.find(x.dest);
        if (it == table.end() || newCost < it->second.cost) {
            table[x.dest] = {x.dest, newCost, via};
        }
    }

    std::vector<RouteItem> out;
    out.reserve(table.size());
    for (const auto& kv : table) out.push_back(kv.second);
    return out;
}

std::vector<int> dijkstra(int n, const std::vector<std::tuple<int,int,int>>& edges, int src) {
    const int INF = std::numeric_limits<int>::max() / 4;
    std::vector<std::vector<std::pair<int,int>>> g(n);
    for (auto [u,v,w] : edges) {
        g[u].push_back({v,w});
        g[v].push_back({u,w});
    }

    std::vector<int> dist(n, INF), vis(n, 0);
    dist[src] = 0;
    for (int i = 0; i < n; ++i) {
        int u = -1;
        for (int j = 0; j < n; ++j) {
            if (!vis[j] && (u == -1 || dist[j] < dist[u])) u = j;
        }
        if (u == -1 || dist[u] == INF) break;
        vis[u] = 1;
        for (auto [v,w] : g[u]) {
            if (dist[u] + w < dist[v]) dist[v] = dist[u] + w;
        }
    }
    return dist;
}

int main() {
    std::vector<RouteItem> r1 = {{"10.0.0.0/24",1,"R1"},{"20.0.0.0/24",3,"R3"}};
    std::vector<RouteItem> r2 = {{"20.0.0.0/24",1,"R2"},{"30.0.0.0/24",2,"R2"}};
    auto updated = ripUpdate(r1, r2, "R2");

    std::cout << "RIP update result\n";
    for (const auto& x : updated) {
        std::cout << x.dest << " cost=" << x.cost << " next=" << x.nextHop << "\n";
    }

    auto dist = dijkstra(5, {{0,1,2},{1,2,1},{0,3,4},{3,4,1},{2,4,3}}, 0);
    std::cout << "Dijkstra from 0\n";
    for (size_t i = 0; i < dist.size(); ++i) {
        std::cout << "to " << i << " = " << dist[i] << "\n";
    }
    return 0;
}
