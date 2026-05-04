#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <string>

struct User {
    std::string name;
    std::string email;
    std::string code;
    std::chrono::system_clock::time_point createAt;
    bool activated = false;
};

class RegisterSystem {
public:
    std::string registerUser(const std::string& name, const std::string& email) {
        User u;
        u.name = name;
        u.email = email;
        u.createAt = std::chrono::system_clock::now();
        u.code = makeCode(name);
        users_[name] = u;
        return "http://127.0.0.1/verify?user=" + name + "&code=" + u.code;
    }

    bool activate(const std::string& name, const std::string& code) {
        auto it = users_.find(name);
        if (it == users_.end()) return false;

        auto now = std::chrono::system_clock::now();
        auto dt = std::chrono::duration_cast<std::chrono::minutes>(now - it->second.createAt).count();
        if (dt > 60) {
            users_.erase(it);
            return false;
        }

        if (it->second.code == code) {
            it->second.activated = true;
            return true;
        }
        return false;
    }

private:
    static std::string makeCode(const std::string& s) {
        return std::to_string(std::hash<std::string>{}(s));
    }

    std::map<std::string, User> users_;
};

int main() {
    RegisterSystem sys;
    auto link = sys.registerUser("hw", "hw@example.com");
    std::cout << "verify_link=" << link << "\n";

    auto code = link.substr(link.find("code=") + 5);
    std::cout << "activate_result=" << sys.activate("hw", code) << "\n";
    std::cout << "Q19 source is blank, skipped.\n";
    return 0;
}
