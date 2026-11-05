// Render one event file to a PCM32 mono WAV (used by tools/render_demos.py).
#include "dsp/Engine.hpp"
#include <cstdio>
#include <cstring>
using namespace rainbows;

int main(int argc, char** argv) {
    if (argc < 4) { std::fprintf(stderr, "usage: render_demo_file <events> <out.wav> <sr>\n"); return 2; }
    const std::vector<float> x = Engine::renderEventFile(argv[1], std::atof(argv[3]));
    FILE* f = std::fopen(argv[2], "wb");
    if (!f) return 1;
    const std::uint32_t n = static_cast<std::uint32_t>(x.size());
    const std::uint32_t sr = static_cast<std::uint32_t>(std::atof(argv[3]));
    auto w32 = [&](std::uint32_t v) { std::fwrite(&v, 4, 1, f); };
    auto w16 = [&](std::uint16_t v) { std::fwrite(&v, 2, 1, f); };
    std::fwrite("RIFF", 1, 4, f); w32(36 + n * 4); std::fwrite("WAVE", 1, 4, f);
    std::fwrite("fmt ", 1, 4, f); w32(16); w16(1); w16(1); w32(sr); w32(sr * 4); w16(4); w16(32);
    std::fwrite("data", 1, 4, f); w32(n * 4);
    for (float v : x) std::fwrite(&v, 4, 1, f);
    std::fclose(f);
    return 0;
}
