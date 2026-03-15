#include "light_comm.h"

#include <exception>
#include <iostream>
#include <string>

namespace {

void printUsage() {
    std::cout
        << "Usage:\n"
        << "  light_comm send <binary|octal> <msg> [output_image]\n"
        << "  light_comm receive <binary|octal> <input_image|camera>\n"
        << "  light_comm demo\n\n"
        << "Notes:\n"
        << "  send    generate one solid-color image for the given message\n"
        << "  receive decode the center area of an image or a camera frame\n"
        << "  demo    run one binary test and one octal test using image files\n";
}

light_comm::Mode parseMode(const std::string& text) {
    if (text == "binary") {
        return light_comm::Mode::Binary;
    }
    if (text == "octal") {
        return light_comm::Mode::Octal;
    }
    throw std::invalid_argument("mode must be binary or octal");
}

void runSend(const std::string& modeText, const std::string& msgText, const std::string& outputPath) {
    light_comm::setMode(parseMode(modeText));

    light_comm::Config config = light_comm::getConfig();
    config.outputPath = outputPath;
    light_comm::setConfig(config);

    const int msg = std::stoi(msgText);
    light_comm::send(msg);
    std::cout << "send complete, saved image to: " << outputPath << '\n';
}

void runReceive(const std::string& modeText, const std::string& inputSource) {
    light_comm::setMode(parseMode(modeText));

    light_comm::Config config = light_comm::getConfig();
    config.inputPath = (inputSource == "camera") ? "" : inputSource;
    light_comm::setConfig(config);

    const int msg = light_comm::receive();
    std::cout << "received message: " << msg << '\n';
}

void runDemo() {
    {
        light_comm::setMode(light_comm::Mode::Binary);
        light_comm::Config config = light_comm::getConfig();
        config.outputPath = "binary_demo.png";
        config.inputPath = "binary_demo.png";
        light_comm::setConfig(config);

        light_comm::send(1);
        std::cout << "binary demo, send 1, receive " << light_comm::receive() << '\n';
    }

    {
        light_comm::setMode(light_comm::Mode::Octal);
        light_comm::Config config = light_comm::getConfig();
        config.outputPath = "octal_demo.png";
        config.inputPath = "octal_demo.png";
        light_comm::setConfig(config);

        light_comm::send(6);
        std::cout << "octal demo, send 6, receive " << light_comm::receive() << '\n';
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            printUsage();
            return 0;
        }

        const std::string command = argv[1];

        if (command == "send") {
            if (argc < 4) {
                printUsage();
                return 1;
            }
            const std::string outputPath = (argc >= 5) ? argv[4] : "output.png";
            runSend(argv[2], argv[3], outputPath);
            return 0;
        }

        if (command == "receive") {
            if (argc < 4) {
                printUsage();
                return 1;
            }
            runReceive(argv[2], argv[3]);
            return 0;
        }

        if (command == "demo") {
            runDemo();
            return 0;
        }

        printUsage();
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "error: " << ex.what() << '\n';
        return 1;
    }
}
