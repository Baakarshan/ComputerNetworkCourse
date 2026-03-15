#ifndef LIGHT_COMM_H
#define LIGHT_COMM_H

#include <opencv2/opencv.hpp>
#include <string>

namespace light_comm {

enum class Mode {
    Binary,
    Octal
};

struct Config {
    std::string inputPath;
    std::string outputPath = "output.png";
    int imageWidth = 640;
    int imageHeight = 480;
    int cameraIndex = 0;
    int cameraWarmupFrames = 10;
    int displayDelayMs = 800;
    bool showWindow = false;
    bool fullscreen = false;
};

void setMode(Mode mode);
Mode getMode();

void setConfig(const Config& config);
const Config& getConfig();

cv::Scalar encode(int msg);
int decode(cv::Scalar color);

void send(int msg);
int receive();

}  // namespace light_comm

#endif
