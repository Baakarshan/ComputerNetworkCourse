#include <filesystem>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    std::filesystem::path p = argc > 1 ? argv[1] : "reactos/nslookup";
    std::cout << "scan_path=" << p.string() << "\n";

    if (!std::filesystem::exists(p)) {
        std::cout << "path not found, pass reactos nslookup path.\n";
        return 0;
    }

    int files = 0;
    for (const auto& e : std::filesystem::recursive_directory_iterator(p)) {
        if (e.is_regular_file()) ++files;
    }
    std::cout << "file_count=" << files << "\n";
    std::cout << "use output to analyze nslookup flow.\n";
    return 0;
}
