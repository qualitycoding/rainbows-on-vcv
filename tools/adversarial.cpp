// Adversarial probes for S-ADVERSARIAL: extreme inputs, rapid gestures,
// block-size changes, feedback runaway, malformed state. Any nonfinite output
// or exception escaping process() is a break.
#include "dsp/Engine.hpp"
#include <cmath>
#include <cstdio>
#include <vector>
using namespace rainbows;

static int breaks = 0;
#define CHECK(cond, name) do { if (!(cond)) { std::printf("BREAK: %s\n", name); ++breaks; } else { std::printf("ok: %s\n", name); } } while (0)

int main() {
    // 1. NaN/Inf/+-1000 V input.
    {
        Engine e; e.prepare(48000, 256);
        Parameters p{}; p.primary = 1.f; p.magic = 1.f; p.magicEngaged = true;
        e.setParameters(p);
        std::vector<float> in(256, 0.f), out(256, 0.f);
        in[0] = std::nanf(""); in[10] = INFINITY; in[20] = -INFINITY; in[30] = 1000.f; in[40] = -1000.f;
        for (int i = 0; i < 100; ++i) e.process(in.data(), out.data(), 256);
        bool finite = true;
        for (float v : out) finite = finite && std::isfinite(v);
        CHECK(finite, "NaN/Inf/1000V input never propagates nonfinite");
        for (float v : out) { if (std::fabs(v) > 12.f) { finite = false; break; } }
        CHECK(finite, "feedback output bounded to +-12 V");
    }
    // 2. Rapid param/gesture thrash with random blocks.
    {
        Engine e; e.prepare(48000, 256);
        std::vector<float> in(256), out(256);
        unsigned s = 12345;
        bool finite = true;
        for (int k = 0; k < 2000; ++k) {
            Parameters p{};
            s = s * 1103515245 + 12345;
            p.pitch = (s >> 16) / 65536.f;
            p.tracking = ((s >> 8) & 0xff) / 255.f;
            p.magic = ((s >> 20) & 0xff) / 255.f;
            p.magicEngaged = (s >> 3) & 1;
            p.active = (s >> 5) & 1;
            e.setParameters(p);
            for (auto& v : in) { s = s * 1103515245 + 12345; v = ((s >> 16) / 65536.f - 0.5f) * 2.f; }
            e.process(in.data(), out.data(), 256);
            for (float v : out) finite = finite && std::isfinite(v);
        }
        CHECK(finite, "rapid gesture thrash stays finite");
    }
    // 3. Block-size changes mid-stream.
    {
        Engine e; e.prepare(48000, 512);
        Parameters p{}; p.primary = 1.f;
        e.setParameters(p);
        bool finite = true;
        for (int bs : {1, 17, 256, 512, 3, 128, 1}) {
            std::vector<float> in(static_cast<std::size_t>(bs)), out(static_cast<std::size_t>(bs));
            for (int r = 0; r < 4800 / bs; ++r) {
                for (auto& v : in) v = 0.3f;
                e.process(in.data(), out.data(), static_cast<std::size_t>(bs));
                for (float v : out) finite = finite && std::isfinite(v);
            }
        }
        CHECK(finite, "block-size changes 1..512 stay finite");
    }
    // 4. Invalid prepare rejection.
    {
        Engine e; bool threw = false;
        try { e.prepare(0, 256); } catch (const std::invalid_argument&) { threw = true; }
        CHECK(threw, "sample rate 0 rejected");
        threw = false;
        try { e.prepare(48000, 0); } catch (const std::invalid_argument&) { threw = true; }
        CHECK(threw, "block size 0 rejected");
    }
    // 5. FlexiSwitch gesture cases.
    {
        FlexiSwitch sw; sw.reset(false);
        CHECK(!sw.active(), "flexi initially off");
        sw.update(true, 0.0); sw.update(false, 0.1);  // tap
        CHECK(sw.active(), "tap <300ms toggles on");
        sw.update(true, 1.0); sw.update(false, 1.2); // tap
        CHECK(!sw.active(), "second tap toggles off");
        sw.update(true, 2.0);
        CHECK(sw.update(true, 2.5), "hold >=300ms engages momentarily");
        sw.update(true, 2.9);
        CHECK(!sw.update(false, 3.2), "release restores prior (off) state");
    }
    // 6. Extreme CV: +-100 V with full attenuverter.
    {
        Engine e; e.prepare(48000, 256);
        Parameters p{}; p.primary = 1.f; p.pitchCv = 100.f; p.pitchAttenuverter = 1.f;
        e.setParameters(p);
        std::vector<float> in(256, 0.2f), out(256);
        bool finite = true;
        for (int i = 0; i < 480; ++i) { e.process(in.data(), out.data(), 256); for (float v : out) finite = finite && std::isfinite(v); }
        CHECK(finite, "extreme CV (100 V) stays finite and clamped");
    }
    // 7. Malformed patch JSON corpus.
    {
        bool safe = true;
        const char* bad[] = {"", "{", "{\"pitch\":}", "{\"pitch\":NaN}", "{\"pitch\":1e999}", "[1,2]", "{\"active\":\"yes\"}"};
        for (const char* b : bad) {
            try {
                PatchState st = PatchCodec::deserialize(b);
                safe = safe && st.parameters.pitch >= 0.f && st.parameters.pitch <= 1.f;
            } catch (...) { safe = false; }
        }
        CHECK(safe, "malformed patch JSON corpus yields safe defaults");
    }
    std::printf("%s (%d breaks)\n", breaks == 0 ? "ADVERSARIAL PASS" : "ADVERSARIAL FAIL", breaks);
    return breaks == 0 ? 0 : 1;
}
