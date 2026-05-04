#include <cmath>
#include <iostream>
#include <vector>

// mean square error
static double mse(const std::vector<double>& a, const std::vector<double>& b) {
    if (a.size() != b.size() || a.empty()) return 1e9;
    double s = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        const double d = a[i] - b[i];
        s += d * d;
    }
    return s / static_cast<double>(a.size());
}

static std::vector<double> sampleSignal(double freq, double fs, double duration) {
    const int n = static_cast<int>(fs * duration);
    std::vector<double> out;
    out.reserve(n);
    for (int i = 0; i < n; ++i) {
        const double t = i / fs;
        out.push_back(std::sin(2.0 * 3.141592653589793 * freq * t));
    }
    return out;
}

int main() {
    const double signalFreq = 50.0;
    const double duration = 1.0;

    const auto ref = sampleSignal(signalFreq, 2000.0, duration);
    const auto good = sampleSignal(signalFreq, 130.0, duration);
    const auto bad = sampleSignal(signalFreq, 70.0, duration);

    std::vector<double> goodMap(ref.size()), badMap(ref.size());
    for (size_t i = 0; i < ref.size(); ++i) {
        size_t gi = (i * good.size()) / ref.size();
        size_t bi = (i * bad.size()) / ref.size();
        if (gi >= good.size()) gi = good.size() - 1;
        if (bi >= bad.size()) bi = bad.size() - 1;
        goodMap[i] = good[gi];
        badMap[i] = bad[bi];
    }

    std::cout << "Nyquist demo\n";
    std::cout << "freq=" << signalFreq << "Hz\n";
    std::cout << "sample_good=130Hz, mse=" << mse(ref, goodMap) << "\n";
    std::cout << "sample_bad=70Hz, mse=" << mse(ref, badMap) << "\n";
    std::cout << "Conclusion: >2f has smaller error, <2f has stronger distortion.\n";
    return 0;
}
