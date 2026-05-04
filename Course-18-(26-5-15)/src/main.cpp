#include <iostream>
#include <string>

// minimal HTTP request handling
std::string handleHttpRequest(const std::string& req) {
    if (req.find("GET / ") == 0 || req.find("GET /index.html") == 0) {
        const std::string body = "<html><body><h1>Simple HTTP Server</h1></body></html>";
        return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
    }
    const std::string body = "404 Not Found";
    return "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
}

int main() {
    std::string req = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    auto resp = handleHttpRequest(req);
    std::cout << "response_head=" << resp.substr(0, resp.find("\r\n\r\n")) << "\n";
    std::cout << "Q23 tinyhttpd reading notes are in README/report.\n";
    std::cout << "Q24 source is blank, skipped.\n";
    return 0;
}
