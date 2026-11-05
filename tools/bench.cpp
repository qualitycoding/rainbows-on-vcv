// Performance / soak benchmark (A-005). Prints JSON Metrics to stdout.
#include "dsp/Engine.hpp"
#include <cstdio>
using namespace rainbows;

int main(int argc, char** argv) {
    std::size_t seconds = 60;
    std::size_t block = 256;
    if (argc > 1) seconds = static_cast<std::size_t>(std::atol(argv[1]));
    if (argc > 2) block = static_cast<std::size_t>(std::atol(argv[2]));
    Engine e;
    Parameters p{};
    // Exercise a representative worst-case: wet voices + magic engaged.
    p.pitch = 0.7f; p.primary = 0.8f; p.secondary = 0.8f;
    p.tracking = 0.3f; p.tone = 0.9f; p.magic = 0.8f; p.magicEngaged = true;
    const Metrics m = e.benchmark(p, seconds, block);
    std::printf("{\"realtimeRatio\": %.3f, \"peakRssBytes\": %.0f, \"seconds\": %zu, \"blockSize\": %zu}\n",
                m.realtimeRatio, m.peakRssBytes, seconds, block);
    return 0;
}
