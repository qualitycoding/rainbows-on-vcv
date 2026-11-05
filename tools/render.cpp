// Deterministic offline harness: renders fixtures through the engine, measures
// pitch error (cents), bypass steps, block invariance, and writes JSON reports
// plus WAV files. No randomness; fixed seed is implicit in the fixtures.
#include "dsp/Engine.hpp"
#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <sys/resource.h>

using namespace rainbows;

static std::string gitCommit() {
    std::ifstream f(".git/refs/heads/impl-20261106T063000Z");
    std::string c; if (f) f >> c;
    return c.empty() ? std::string("unknown") : c;
}

static std::string utcNow() {
    const std::time_t t = std::time(nullptr);
    char b[64];
    std::strftime(b, sizeof b, "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&t));
    return b;
}

static void writeWav(const std::string& path, const std::vector<float>& x, double sr) {
    std::ofstream f(path, std::ios::binary);
    const std::uint32_t n = static_cast<std::uint32_t>(x.size());
    auto w32 = [&](std::uint32_t v) { f.write(reinterpret_cast<const char*>(&v), 4); };
    auto w16 = [&](std::uint16_t v) { f.write(reinterpret_cast<const char*>(&v), 2); };
    f.write("RIFF", 4); w32(36 + n * 4); f.write("WAVE", 4);
    f.write("fmt ", 4); w32(16); w16(1); w16(1); w32(static_cast<std::uint32_t>(sr));
    w32(static_cast<std::uint32_t>(sr) * 4); w16(4); w16(32);
    f.write("data", 4); w32(n * 4);
    for (float v : x) f.write(reinterpret_cast<const char*>(&v), 4);
}

// Median absolute pitch error in cents over a rendered sine, via zero-crossing
// period measurement on the settled portion.
static double pitchErrorCents(const std::vector<float>& x, double sr, double freq, std::size_t skip) {
    std::vector<double> periods;
    for (std::size_t i = skip + 1; i < x.size(); ++i) {
        if (x[i - 1] <= 0.f && x[i] > 0.f) {
            // linear-interpolated crossing time
            const double frac = -static_cast<double>(x[i - 1]) / (static_cast<double>(x[i]) - x[i - 1] + 1e-30);
            periods.push_back((static_cast<double>(i) - 1.0 + frac) / sr);
        }
    }
    if (periods.size() < 4) return 1e9;
    std::vector<double> d;
    for (std::size_t i = 1; i < periods.size(); ++i) d.push_back(periods[i] - periods[i - 1]);
    std::sort(d.begin(), d.end());
    const double med = d[d.size() / 2];
    if (med <= 0.0) return 1e9;
    const double measured = 1.0 / med;
    return 1200.0 * std::log2(measured / freq);
}

static std::vector<float> renderSine(Engine& e, double freq, double seconds, double sr,
                                       const Parameters& p, std::size_t block) {
    e.prepare(sr, block);
    e.setParameters(p);
    const std::size_t total = static_cast<std::size_t>(seconds * sr);
    std::vector<float> out(total);
    std::vector<float> in(block), ob(block);
    std::size_t pos = 0;
    while (pos < total) {
        const std::size_t n = std::min(block, total - pos);
        for (std::size_t i = 0; i < n; ++i)
            in[i] = 0.5f * std::sin(2.0 * M_PI * freq * static_cast<double>(pos + i) / sr);
        e.process(in.data(), ob.data(), n);
        std::copy(ob.begin(), ob.begin() + n, out.begin() + static_cast<std::ptrdiff_t>(pos));
        pos += n;
    }
    return out;
}

static std::string envHardware() {
    std::ifstream f("/proc/cpuinfo");
    std::string line, model = "unknown";
    while (std::getline(f, line)) {
        if (line.find("model name") == 0) { model = line.substr(line.find(':') + 2); break; }
    }
    return model;
}

int main(int argc, char** argv) {
    const std::string mode = argc > 1 ? argv[1] : "all";
    const double sr = 48000.0;
    const std::string commit = gitCommit();
    const std::string sdkHash = "420da2452def7b195f98e3a0650c35f1e0f058a98a71840b4347d3d203618532";
    const std::string seed = "20261106";
    std::string report;

    auto hdr = [&](const char* name) {
        std::string h = "\"test\":\"";
        h += name;
        h += "\",\"commit\":\"" + commit + "\",\"sdkLockHash\":\"" + sdkHash +
             "\",\"seed\":\"" + seed + "\",\"utc\":\"" + utcNow() +
             "\",\"hardware\":\"" + envHardware() + "\",\"threads\":2";
        return h;
    };

    if (mode == "all" || mode == "pitch") {
        // T009: settled isolated-sine pitch <= 10 cents across pitch range.
        for (float knob : {0.f, 0.25f, 0.5f, 0.75f, 1.f}) {
            Engine e;
            Parameters p{};
            p.pitch = knob; p.primary = 1.f; p.tracking = 1.f;
            const float semis = Engine::pitchSemitones(knob, 0.f, 0.f);
            const double target = 440.0 * std::pow(2.0, semis / 12.0);
            const auto x = renderSine(e, 440.0, 2.0, sr, p, 256);
            const double cents = pitchErrorCents(x, sr, target, static_cast<std::size_t>(0.5 * sr));
            char b[1024];
            std::snprintf(b, sizeof b, "{%s,\"knob\":%.2f,\"targetCents\":%.2f}\n",
                          hdr("T009-pitch").c_str(), static_cast<double>(knob), cents);
            report += b;
            if (std::abs(cents) > 10.0) { std::fprintf(stderr, "T009 FAIL knob=%.2f cents=%.2f\n", static_cast<double>(knob), cents); return 1; }
        }
    }

    if (mode == "all" || mode == "block") {
        // T018: block sizes 1/17/256 match within 1e-6 (last 48000 samples).
        Parameters p{}; p.pitch = 0.6f; p.primary = 0.7f; p.secondary = 0.6f; p.magic = 0.5f; p.magicEngaged = true;
        Engine e1; auto a = renderSine(e1, 330.0, 2.0, sr, p, 1);
        Engine e17; auto b = renderSine(e17, 330.0, 2.0, sr, p, 17);
        Engine e256; auto c = renderSine(e256, 330.0, 2.0, sr, p, 256);
        const std::size_t tail = 48000;
        double diff = 0.0;
        for (std::size_t i = a.size() - tail; i < a.size(); ++i)
            diff = std::max(diff, std::max(std::abs(static_cast<double>(a[i]) - b[i]),
                                           std::abs(static_cast<double>(a[i]) - c[i])));
        char buf[512];
        std::snprintf(buf, sizeof buf, "{%s,\",\"maxDiff\":%.3e}\n", hdr("T018-block-invariance").c_str(), diff);
        report += buf;
        if (diff > 1e-6) { std::fprintf(stderr, "T018 FAIL diff=%.3e\n", diff); return 1; }
    }

    if (mode == "all" || mode == "bypass") {
        // T012: bypass (Active off) added discontinuity <= 0.05 V.
        Engine e; e.prepare(sr, 256);
        Parameters on{}; on.pitch = 0.5f; on.primary = 0.8f;
        Parameters off = on; off.active = false;
        std::vector<float> in(4800), ob(4800);
        for (std::size_t i = 0; i < in.size(); ++i) in[i] = 0.4f * std::sin(2.0 * M_PI * 220.0 * i / sr);
        e.setParameters(on);
        e.process(in.data(), ob.data(), in.size()); // settled on
        e.setParameters(off);                        // ramp begins
        double maxStep = 0.0;
        float prev = ob.back();
        for (int k = 0; k < 480; ++k) {
            e.process(in.data(), ob.data(), 1);
            maxStep = std::max(maxStep, std::abs(static_cast<double>(ob[0]) - prev));
            prev = ob[0];
        }
        char buf[512];
        std::snprintf(buf, sizeof buf, "{%s,\",\"maxStepVolts\":%.4f}\n", hdr("T012-bypass-ramp").c_str(), maxStep);
        report += buf;
        if (maxStep > 0.05) { std::fprintf(stderr, "T012 FAIL step=%.4f\n", maxStep); return 1; }
    }

    if (mode == "all" || mode == "magic") {
        // T006/T007: ascending/descending Magic echoes bounded over 10 minutes.
        Engine e; e.prepare(sr, 256);
        Parameters p{}; p.pitch = 0.8f; p.primary = 0.8f; p.secondary = 0.5f;
        p.magic = 1.0f; p.magicEngaged = true;
        e.setParameters(p);
        std::vector<float> in(256), ob(256);
        // noise burst then silence
        for (std::size_t i = 0; i < in.size(); ++i) in[i] = (i < 64) ? 0.5f * std::sin(0.3 * i) : 0.f;
        bool finite = true;
        float peak = 0.f;
        const std::size_t total = static_cast<std::size_t>(600.0 * sr);
        for (std::size_t pos = 0; pos < total; pos += 256) {
            e.process(in.data(), ob.data(), 256);
            for (float v : ob) {
                if (!std::isfinite(v)) finite = false;
                peak = std::max(peak, std::abs(v));
            }
        }
        char buf[512];
        std::snprintf(buf, sizeof buf, "{%s,\",\"finite\":%s,\"peak\":%.4f}\n",
                      hdr("T007-magic-soak").c_str(), finite ? "true" : "false", static_cast<double>(peak));
        report += buf;
        if (!finite || peak > 12.0f) { std::fprintf(stderr, "T007 FAIL finite=%d peak=%.3f\n", finite, static_cast<double>(peak)); return 1; }
    }

    if (mode == "all" || mode == "matrix") {
        // S-003 parameter matrix renders.
        const char* dir = "renders/parameter-matrix";
        for (float pitch : {0.f, 0.5f, 1.f})
            for (float sec : {0.f, 1.f})
                for (float trk : {0.f, 0.5f, 1.f})
                    for (float tone : {0.f, 1.f}) {
                        Engine e;
                        Parameters p{};
                        p.pitch = pitch; p.primary = 1.f; p.secondary = sec; p.tracking = trk; p.tone = tone;
                        auto x = renderSine(e, 220.0, 1.5, sr, p, 256);
                        char name[128];
                        std::snprintf(name, sizeof name, "%s/p%.1f-s%.0f-t%.1f-k%.0f.wav", dir,
                                      static_cast<double>(pitch), static_cast<double>(sec),
                                      static_cast<double>(trk), static_cast<double>(tone));
                        writeWav(name, x, sr);
                    }
    }

    if (mode == "all" || mode == "patch") {
        // T017: roundtrip + malformed.
        PatchState st; st.parameters.pitch = 0.7f; st.parameters.magicEngaged = true;
        const std::string s = PatchCodec::serialize(st);
        const PatchState back = PatchCodec::deserialize(s);
        const bool ok = std::abs(back.parameters.pitch - 0.7f) < 1e-6f && back.parameters.magicEngaged;
        const PatchState bad = PatchCodec::deserialize("{not json");
        const bool defaultsOk = bad.parameters.pitch == 0.f && bad.parameters.active;
        char buf[512];
        std::snprintf(buf, sizeof buf, "{%s,\",\"roundtrip\":%s,\"malformedSafe\":%s}\n",
                      hdr("T017-patch").c_str(), ok ? "true" : "false", defaultsOk ? "true" : "false");
        report += buf;
        if (!ok || !defaultsOk) { std::fprintf(stderr, "T017 FAIL\n"); return 1; }
    }

    std::fputs(report.c_str(), stdout);
    return 0;
}
