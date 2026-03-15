#include "light_comm.h"

#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace light_comm {
namespace {

Mode g_mode = Mode::Binary;
Config g_config;

const std::array<cv::Scalar, 8> kOctalColors = {
    cv::Scalar(0, 0, 0),
    cv::Scalar(255, 255, 255),
    cv::Scalar(0, 0, 255),
    cv::Scalar(255, 0, 0),
    cv::Scalar(0, 255, 0),
    cv::Scalar(255, 0, 255),
    cv::Scalar(0, 255, 255),
    cv::Scalar(255, 255, 0)
};

double colorDistance(const cv::Scalar& a, const cv::Scalar& b) {
    const double db = a[0] - b[0];
    const double dg = a[1] - b[1];
    const double dr = a[2] - b[2];
    return std::sqrt(db * db + dg * dg + dr * dr);
}

cv::Scalar sampleCenterColor(const cv::Mat& frame) {
    if (frame.empty()) {
        throw std::runtime_error("cannot sample color from empty image");
    }

    const int roiWidth = std::max(1, frame.cols / 3);
    const int roiHeight = std::max(1, frame.rows / 3);
    const int x = (frame.cols - roiWidth) / 2;
    const int y = (frame.rows - roiHeight) / 2;
    const cv::Rect roi(x, y, roiWidth, roiHeight);

    return cv::mean(frame(roi));
}
}  // namespace

void setMode(Mode mode) {
    g_mode = mode;
}

Mode getMode() {
    return g_mode;
}

void setConfig(const Config& config) {
    g_config = config;
}

const Config& getConfig() {
    return g_config;
}

cv::Scalar encode(int msg) {
    if (g_mode == Mode::Binary) {
        if (msg == 1) {
            return cv::Scalar(0, 0, 0);
        }
        if (msg == 0) {
            return cv::Scalar(255, 255, 255);
        }
        throw std::invalid_argument("binary mode only accepts 0 or 1");
    }

    if (msg < 0 || msg > 7) {
        throw std::invalid_argument("octal color mode only accepts 0 to 7");
    }

    return kOctalColors[static_cast<std::size_t>(msg)];
}

int decode(cv::Scalar color) {
    if (g_mode == Mode::Binary) {
        const double brightness = (color[0] + color[1] + color[2]) / 3.0;
        return brightness < 128.0 ? 1 : 0;
    }

    double bestDistance = std::numeric_limits<double>::max();
    int bestValue = 0;
    for (int value = 0; value < 8; ++value) {
        const double distance = colorDistance(color, kOctalColors[value]);
        if (distance < bestDistance) {
            bestDistance = distance;
            bestValue = value;
        }
    }
    return bestValue;
}

void send(int msg) {
    const cv::Scalar color = encode(msg);
    cv::Mat image(g_config.imageHeight, g_config.imageWidth, CV_8UC3, color);

    if (!g_config.outputPath.empty()) {
        cv::imwrite(g_config.outputPath, image);
    }

    if (!g_config.showWindow) {
        return;
    }

    const std::string windowName = "light_comm_sender";
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    if (g_config.fullscreen) {
        cv::setWindowProperty(windowName, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
    }
    cv::imshow(windowName, image);
    cv::waitKey(g_config.displayDelayMs);
    cv::destroyWindow(windowName);
}

int receive() {
    cv::Mat frame;

    if (!g_config.inputPath.empty()) {
        frame = cv::imread(g_config.inputPath, cv::IMREAD_COLOR);
        if (frame.empty()) {
            throw std::runtime_error("cannot read input image: " + g_config.inputPath);
        }
    } else {
        cv::VideoCapture cap(g_config.cameraIndex);
        if (!cap.isOpened()) {
            throw std::runtime_error("cannot open camera");
        }

        for (int i = 0; i < g_config.cameraWarmupFrames; ++i) {
            cap >> frame;
        }

        if (frame.empty()) {
            throw std::runtime_error("cannot read frame from camera");
        }
    }

    const cv::Scalar meanColor = sampleCenterColor(frame);
    return decode(meanColor);
}

}  // namespace light_comm
