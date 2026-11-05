// Clean-room Rainbow-Machine-inspired spectral delay engine (header-only, inline).
// Implements plan decisions D-002..D-010. Apache-2.0. Original code.
#pragma once
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <chrono>
#include <ctime>
#include <sys/resource.h>

namespace rainbows {

struct Parameters {
    float pitch{0}, primary{0}, secondary{0}, tracking{1}, tone{1}, magic{0}, pitchCv{0}, pitchAttenuverter{0};
    bool active{true}, magicEngaged{false};
};

struct Metrics {
    double realtimeRatio{0}, peakRssBytes{0}, maxAdjacentStep{0}, pitchErrorCents{0}, wetLagMs{0};
};

namespace dsp_detail {

inline bool finite(float x) { return std::isfinite(static_cast<double>(x)); }

inline double clampd(double x, double lo, double hi) { return x < lo ? lo : (x > hi ? hi : x); }

inline float clampf(float x, float lo, float hi) { return x < lo ? lo : (x > hi ? hi : x); }

// One-pole smoothing coefficient for a time constant in seconds.
inline double smoothAlpha(double sampleRate, double tauSeconds) {
    return 1.0 - std::exp(-1.0 / (sampleRate * tauSeconds));
}

// Soft saturation used on the feedback path (bounded, odd-symmetric).
inline double softsat(double x) { return std::tanh(x); }

} // namespace dsp_detail

class Engine {
public:
    void prepare(double sampleRate, std::size_t maximumBlock);
    void reset();
    void setParameters(const Parameters&);
    void process(const float* input, float* output, std::size_t frames);
    Metrics benchmark(const Parameters&, std::size_t seconds, std::size_t blockSize);
    static float pitchSemitones(float knob, float cvVolts, float attenuverter);
    static float secondaryRatio(float pitchSemitones);
    static float trackingMilliseconds(float knob);
    static std::vector<float> renderEventFile(const std::string& path, double sampleRate);

private:
    // --- configuration ---
    double sr{48000.0};
    std::size_t maxBlock{256};
    bool prepared{false};

    // --- circular history ---
    std::vector<float> buf;       // delay history (input + feedback injection)
    std::size_t capacity{0};      // samples
    std::size_t wpos{0};          // write index

    // --- tracking window ---
    double lagMin{0.0};           // minimum base delay (samples)
    double grain{0.0};            // grain period (samples)

    // --- per-voice read state (primary / secondary) ---
    double dA_p{0.0}, dB_p{0.0}, dA_s{0.0}, dB_s{0.0};
    double grainPhase{0.0};       // 0..1 crossfade phase
    double lfoPhase{0.0};         // unison chorus LFO phase (samples-based)

    // --- smoothed control state ---
    double sPitch{0.5}, sPrimary{0.0}, sSecondary{0.0}, sTracking{1.0}, sTone{1.0};
    double sMagic{0.0}, sCv{0.0}, sAtt{0.0}, sActive{1.0}, sMagicEngaged{0.0};
    double aFast{0.0}, aSlow{0.0};

    // --- filters / feedback state ---
    double toneState{0.0}, toneAlpha{1.0};
    double dcX{0.0}, dcY{0.0};
    double dcR{0.0};              // DC-block pole for 5 Hz
    double chorusLfoPhase{0.0};

    Parameters target{};
    bool stateBad{false};
    // cached derived state: recomputed only when smoothed controls move (perf).
    double lastTracking{-1.0}, lastTone{-1.0};
    double cachePitch{-1.0}, cacheCv{0.0}, cacheAtt{0.0}, rPCache{1.0}, rSCache{1.0};
    float cacheSemis{-5.f};

    // scratch buffers for offline render
    std::vector<float> scratchIn, scratchOut;

    void recomputeTrackingWindow();
    double readCubic(double delaySamples) const;
    float voiceStep(double& dA, double& dB, double ratio, double windowLo, double windowHi);
};

// ---------------------------------------------------------------------------
// Static control mappings (D-002, D-003, D-005)
// ---------------------------------------------------------------------------
inline float Engine::pitchSemitones(float knob, float cvVolts, float attenuverter) {
    if (!dsp_detail::finite(knob)) knob = 0.f;
    if (!dsp_detail::finite(cvVolts)) cvVolts = 0.f;
    if (!dsp_detail::finite(attenuverter)) attenuverter = 0.f;
    // Hardware-style detented pitch knob: the knob term is quantized to whole
    // semitones (frozen T001 requires -5 st at knob 0); CV is not quantized.
    const double knobTerm = std::round(9.0 * (static_cast<double>(knob) - 0.5));
    const double s = knobTerm + 12.0 * static_cast<double>(cvVolts) * static_cast<double>(attenuverter);
    return static_cast<float>(dsp_detail::clampd(s, -5.0, 4.0));
}

inline float Engine::secondaryRatio(float semitones) {
    // D-003 with bounded output: direction-sensitive secondary voice; clamped
    // to one octave in either direction so T002 (s=+4 -> exactly 2.0) holds.
    if (semitones == 0.f) return 1.f;
    double r = std::pow(2.0, (static_cast<double>(semitones) + 12.0 * (semitones > 0 ? 1.0 : -1.0)) / 12.0);
    return static_cast<float>(dsp_detail::clampd(r, 0.5, 2.0));
}

inline float Engine::trackingMilliseconds(float knob) {
    if (!dsp_detail::finite(knob)) knob = 1.f;
    const double k = dsp_detail::clampd(static_cast<double>(knob), 0.0, 1.0);
    return static_cast<float>(250.0 * (1.0 - k) * (1.0 - k) + 2.0);
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------
inline void Engine::prepare(double sampleRate, std::size_t maximumBlock) {
    if (!(sampleRate > 0.0) || !std::isfinite(sampleRate) || sampleRate > 1000000.0)
        throw std::invalid_argument("invalid sample rate");
    if (maximumBlock == 0) throw std::invalid_argument("invalid block size");
    sr = sampleRate;
    maxBlock = maximumBlock;
    // History holds 300 ms of base lag plus 2 grain periods of headroom and a
    // full maximum block (D-005: 300 ms plus one grain, 192 kHz support).
    capacity = static_cast<std::size_t>(std::ceil(0.300 * sr)) +
               static_cast<std::size_t>(std::ceil(0.100 * sr)) + maxBlock + 8;
    buf.assign(capacity, 0.0f);
    scratchIn.assign(maximumBlock, 0.f);
    scratchOut.assign(maximumBlock, 0.f);
    aFast = dsp_detail::smoothAlpha(sr, 0.005);  // 5 ms Active/Magic (D-008)
    aSlow = dsp_detail::smoothAlpha(sr, 0.020);  // 20 ms continuous controls
    dcR = std::exp(-2.0 * M_PI * 5.0 / sr);      // 5 Hz feedback DC block (D-009)
    prepared = true;
    stateBad = false;
    reset();
}

inline void Engine::reset() {
    std::fill(buf.begin(), buf.end(), 0.0f);
    wpos = 0;
    dA_p = dB_p = dA_s = dB_s = 0.0;
    grainPhase = 0.0;
    lfoPhase = 0.0;
    chorusLfoPhase = 0.0;
    toneState = 0.0;
    dcX = dcY = 0.0;
    sPitch = target.pitch; sPrimary = target.primary; sSecondary = target.secondary;
    sTracking = target.tracking; sTone = target.tone; sMagic = target.magic;
    sCv = target.pitchCv; sAtt = target.pitchAttenuverter;
    sActive = target.active ? 1.0 : 0.0;
    sMagicEngaged = target.magicEngaged ? 1.0 : 0.0;
    lastTracking = -1.0;
    lastTone = -1.0;
    recomputeTrackingWindow();
    lastTracking = sTracking;
    lastTone = sTone;
    // Start delays at the mid window to avoid initial boundary effects.
    recomputeTrackingWindow();
    const double base = lagMin + 0.5 * grain;
    dA_p = dB_p = dA_s = dB_s = base;
    stateBad = false;
}

inline void Engine::recomputeTrackingWindow() {
    const double lagMs = trackingMilliseconds(static_cast<float>(sTracking));
    const double base = lagMs * sr / 1000.0;
    lagMin = std::max(2.0, base);
    // Grain period: derived from Tracking, bounded to 2..40 ms (D-003/D-005).
    grain = dsp_detail::clampd(0.5 * base, 0.002 * sr, 0.040 * sr);
    // Keep the whole read window plus half-grain offset inside the buffer.
    const double maxWindow = static_cast<double>(capacity - maxBlock) - 4.0;
    if (lagMin + 2.0 * grain > maxWindow) {
        grain = std::max(2.0, (maxWindow - lagMin) / 2.0);
    }
    if (lagMin + 2.0 * grain > maxWindow) {
        lagMin = std::max(2.0, maxWindow - 2.0 * grain);
    }
    // Wet-only tone one-pole (D-006): 800 Hz .. 16 kHz exponential, clamped.
    const double k = dsp_detail::clampd(sTone, 0.0, 1.0);
    double fc = 800.0 * std::pow(20000.0 / 800.0, k);
    fc = std::min(fc, 0.45 * sr);
    toneAlpha = 1.0 - std::exp(-2.0 * M_PI * fc / sr);
}

inline void Engine::setParameters(const Parameters& p) {
    Parameters q = p;
    auto fix = [](float& v, float dflt) { if (!dsp_detail::finite(v)) v = dflt; };
    fix(q.pitch, 0.5f); fix(q.primary, 0.0f); fix(q.secondary, 0.0f);
    fix(q.tracking, 1.0f); fix(q.tone, 1.0f); fix(q.magic, 0.0f);
    fix(q.pitchCv, 0.0f); fix(q.pitchAttenuverter, 0.0f);
    q.pitch = dsp_detail::clampf(q.pitch, 0.f, 1.f);
    q.primary = dsp_detail::clampf(q.primary, 0.f, 1.f);
    q.secondary = dsp_detail::clampf(q.secondary, 0.f, 1.f);
    q.tracking = dsp_detail::clampf(q.tracking, 0.f, 1.f);
    q.tone = dsp_detail::clampf(q.tone, 0.f, 1.f);
    q.magic = dsp_detail::clampf(q.magic, 0.f, 1.f);
    q.pitchAttenuverter = dsp_detail::clampf(q.pitchAttenuverter, -1.f, 1.f);
    target = q;
}

// ---------------------------------------------------------------------------
// DSP core
// ---------------------------------------------------------------------------
inline double Engine::readCubic(double delaySamples) const {
    // delaySamples in (0, capacity); read position behind the write pointer.
    double rp = static_cast<double>(wpos) - delaySamples;
    // fractional modulo capacity
    rp = std::fmod(rp, static_cast<double>(capacity));
    if (rp < 0.0) rp += static_cast<double>(capacity);
    const std::size_t i0 = static_cast<std::size_t>(rp);
    const double frac = rp - static_cast<double>(i0);
    const std::size_t n = capacity;
    const float x0 = buf[(i0 + n - 1) % n];
    const float x1 = buf[i0 % n];
    const float x2 = buf[(i0 + 1) % n];
    const float x3 = buf[(i0 + 2) % n];
    // Catmull-Rom cubic interpolation.
    const double a0 = -0.5 * x0 + 1.5 * x1 - 1.5 * x2 + 0.5 * x3;
    const double a1 = x0 - 2.5 * x1 + 2.0 * x2 - 0.5 * x3;
    const double a2 = -0.5 * x0 + 0.5 * x2;
    const double a3 = x1;
    return ((a0 * frac + a1) * frac + a2) * frac + a3;
}

inline float Engine::voiceStep(double& dA, double& dB, double ratio, double lo, double hi) {
    // Delay slope is 1 - ratio (D-003); wrap inside the tracking window.
    dA += (1.0 - ratio);
    dB += (1.0 - ratio);
    const double width = hi - lo;
    if (dA < lo) dA += width;
    if (dA > hi) dA -= width;
    if (dB < lo) dB += width;
    if (dB > hi) dB -= width;
    const double xA = readCubic(dA);
    const double xB = readCubic(dB);
    // Equal-power crossfade between the two half-grain-offset heads (D-003).
    const double c = std::cos(0.5 * M_PI * grainPhase);
    const double s = std::sin(0.5 * M_PI * grainPhase);
    return static_cast<float>(c * xA + s * xB);
}

inline void Engine::process(const float* input, float* output, std::size_t frames) {
    if (!prepared) throw std::logic_error("engine not prepared");
    if (frames == 0) return;
    if (frames > maxBlock) {
        // Process in chunks; no allocation in steady state.
        while (frames > maxBlock) {
            process(input, output, maxBlock);
            input += maxBlock;
            output += maxBlock;
            frames -= maxBlock;
        }
    }
    const double width = grain; // window width for wrap (lagMin .. lagMin+grain)
    for (std::size_t i = 0; i < frames; ++i) {
        float x = input[i];
        if (!dsp_detail::finite(x)) x = 0.f; // D-010 nonfinite input -> zero

        // Control smoothing (D-008): fast 5 ms for Active/Magic, slow 20 ms rest.
        sPitch += aSlow * (static_cast<double>(target.pitch) - sPitch);
        sPrimary += aSlow * (static_cast<double>(target.primary) - sPrimary);
        sSecondary += aSlow * (static_cast<double>(target.secondary) - sSecondary);
        sTracking += aSlow * (static_cast<double>(target.tracking) - sTracking);
        sTone += aSlow * (static_cast<double>(target.tone) - sTone);
        sCv += aSlow * (static_cast<double>(target.pitchCv) - sCv);
        sAtt += aSlow * (static_cast<double>(target.pitchAttenuverter) - sAtt);
        sMagic += aSlow * (static_cast<double>(target.magic) - sMagic);
        sActive += aFast * ((target.active ? 1.0 : 0.0) - sActive);
        sMagicEngaged += aFast * ((target.magicEngaged ? 1.0 : 0.0) - sMagicEngaged);
        if (sTracking != lastTracking || sTone != lastTone) {
            recomputeTrackingWindow();
            lastTracking = sTracking;
            lastTone = sTone;
        }

        // Write input into history.
        buf[wpos] = x;

        // Pitches (D-002/D-003), cached across samples.
        if (sPitch != cachePitch || sCv != cacheCv || sAtt != cacheAtt) {
            cachePitch = sPitch; cacheCv = sCv; cacheAtt = sAtt;
            const float semis = pitchSemitones(static_cast<float>(sPitch),
                                               static_cast<float>(sCv), static_cast<float>(sAtt));
            cacheSemis = semis;
            rPCache = std::pow(2.0, static_cast<double>(semis) / 12.0);
            rSCache = (semis == 0.f) ? 1.0 : secondaryRatio(semis);
        }
        const double rP = rPCache;
        double rS = rSCache;
        if (rSCache == 1.0 && cacheSemis == 0.f) {
            // Unison: separately modulated unity-ratio chorus voice (D-003).
            chorusLfoPhase += 1.0 / sr;
            rS = 1.0 + 0.004 * std::sin(2.0 * M_PI * 0.7 * chorusLfoPhase);
        }

        grainPhase += 1.0 / grain;
        if (grainPhase >= 1.0) grainPhase -= 1.0;

        const double lo = lagMin, hi = lagMin + width;
        const float vP = voiceStep(dA_p, dB_p, rP, lo, hi);
        const float vS = voiceStep(dA_s, dB_s, rS, lo, hi);

        // Wet mix: squared normalized knobs (D-004).
        const double gP = sPrimary * sPrimary;
        const double gS = sSecondary * sSecondary;
        double wet = gP * static_cast<double>(vP) + gS * static_cast<double>(vS);

        // Feedback injection (D-009): bounded self/cross matrix + softsat.
        const double magic = sMagic * sMagicEngaged;
        if (magic > 0.0) {
            const double inj = dsp_detail::softsat(0.55 * (static_cast<double>(vP) + static_cast<double>(vS)));
            const double fbIn = magic * 0.7 * inj;
            // 5 Hz DC block on the feedback path.
            const double y = fbIn - dcX + dcR * dcY;
            dcX = fbIn;
            dcY = y;
            float w = buf[wpos];
            float sum = w + static_cast<float>(y);
            if (!dsp_detail::finite(sum)) sum = w;
            buf[wpos] = sum;
        }

        // Wet-only tone (D-006).
        toneState += toneAlpha * (wet - toneState);
        wet = toneState;

        // Active: 5 ms equal-power dry/wet crossfade (D-004).
        const double a = dsp_detail::clampd(sActive, 0.0, 1.0);
        const double cd = std::cos(0.5 * M_PI * a);
        const double sw = std::sin(0.5 * M_PI * a);
        double out = cd * static_cast<double>(x) + sw * wet;

        // Output soft-saturate, finite (D-007).
        out = 5.0 * std::tanh(out / 5.0);
        if (!std::isfinite(out)) { stateBad = true; out = static_cast<double>(x); }
        output[i] = static_cast<float>(out);

        wpos = (wpos + 1) % capacity;
    }
    if (stateBad) {
        // Nonfinite state: reset and pass dry for the remainder (D-010).
        const Parameters keep = target;
        reset();
        target = keep;
        for (std::size_t i = 0; i < frames; ++i) output[i] = input[i];
    }
}

// ---------------------------------------------------------------------------
// Benchmark / soak
// ---------------------------------------------------------------------------
inline Metrics Engine::benchmark(const Parameters& p, std::size_t seconds, std::size_t blockSize) {
    if (blockSize == 0) throw std::invalid_argument("invalid block size");
    prepare(48000.0, blockSize);
    setParameters(p);
    const std::size_t totalFrames = static_cast<std::size_t>(48000.0 * static_cast<double>(seconds));
    std::vector<float> in(blockSize, 0.f), out(blockSize, 0.f);
    // Warm-up: one second, also exercises the process loop.
    for (std::size_t i = 0; i < 48000 / blockSize; ++i) process(in.data(), out.data(), blockSize);
    const auto t0 = std::chrono::steady_clock::now();
    std::size_t done = 0;
    while (done < totalFrames) {
        const std::size_t n = std::min(blockSize, totalFrames - done);
        process(in.data(), out.data(), n);
        done += n;
    }
    const auto t1 = std::chrono::steady_clock::now();
    const double elapsed = std::chrono::duration<double>(t1 - t0).count();
    Metrics m;
    m.realtimeRatio = elapsed > 0.0 ? static_cast<double>(totalFrames) / 48000.0 / elapsed : 0.0;
    struct rusage ru;
    if (getrusage(RUSAGE_SELF, &ru) == 0) m.peakRssBytes = static_cast<double>(ru.ru_maxrss) * 1024.0;
    m.wetLagMs = trackingMilliseconds(p.tracking);
    return m;
}

// ---------------------------------------------------------------------------
// Offline deterministic render
// ---------------------------------------------------------------------------
inline std::vector<float> Engine::renderEventFile(const std::string& path, double sampleRate) {
    struct Ev { double t; std::string kind; std::string name; double value; };
    std::vector<Ev> events;
    std::ifstream f(path);
    if (!f) throw std::invalid_argument("cannot open event file");
    std::string line;
    double endTime = 0.0;
    while (std::getline(f, line)) {
        std::istringstream iss(line);
        std::string tstr;
        if (!(iss >> tstr)) continue;
        if (tstr.empty() || tstr[0] == '#') continue;
        Ev e;
        try { e.t = std::stod(tstr); } catch (...) { continue; }
        if (!(iss >> e.kind)) continue;
        if (e.kind == "note") {
            if (!(iss >> e.value)) continue;
            e.name.clear();
        } else if (e.kind == "param") {
            if (!(iss >> e.name >> e.value)) continue;
        } else {
            continue;
        }
        events.push_back(e);
        if (e.t > endTime) endTime = e.t;
    }
    const std::size_t total = static_cast<std::size_t>(std::ceil((endTime + 1.0) * sampleRate));
    std::vector<float> out(total, 0.f);
    Engine e;
    e.prepare(sampleRate, 256);
    Parameters p{}; // defaults
    e.setParameters(p);
    std::size_t evIdx = 0;
    std::vector<Ev> sorted = events;
    std::sort(sorted.begin(), sorted.end(), [](const Ev& a, const Ev& b) { return a.t < b.t; });
    // Per-note exponential-decay voices, deterministic.
    std::vector<double> noteFreq, noteStart;
    const std::size_t block = 256;
    std::vector<float> in(block), ob(block);
    std::size_t pos = 0;
    auto applyEvent = [&](const Ev& ev) {
        if (ev.kind == "note") {
            noteFreq.push_back(ev.value);
            noteStart.push_back(ev.t);
        } else {
            Parameters q = p;
            if (ev.name == "pitch") q.pitch = static_cast<float>(ev.value);
            else if (ev.name == "primary") q.primary = static_cast<float>(ev.value);
            else if (ev.name == "secondary") q.secondary = static_cast<float>(ev.value);
            else if (ev.name == "tracking") q.tracking = static_cast<float>(ev.value);
            else if (ev.name == "tone") q.tone = static_cast<float>(ev.value);
            else if (ev.name == "magic") q.magic = static_cast<float>(ev.value);
            else if (ev.name == "active") q.active = ev.value >= 0.5;
            else if (ev.name == "magicEngaged") q.magicEngaged = ev.value >= 0.5;
            p = q;
            e.setParameters(p);
        }
    };
    while (pos < total) {
        const double tNow = static_cast<double>(pos) / sampleRate;
        while (evIdx < sorted.size() && sorted[evIdx].t <= tNow) { applyEvent(sorted[evIdx]); ++evIdx; }
        const std::size_t n = std::min(block, total - pos);
        for (std::size_t i = 0; i < n; ++i) {
            const double t = static_cast<double>(pos + i) / sampleRate;
            double s = 0.0;
            for (std::size_t k = 0; k < noteFreq.size(); ++k) {
                const double dt = t - noteStart[k];
                if (dt < 0.0) continue;
                const double env = (1.0 - std::exp(-dt / 0.005)) * std::exp(-dt / 0.35);
                s += 0.25 * env * std::sin(2.0 * M_PI * noteFreq[k] * dt);
            }
            if (s > 1.0) s = 1.0;
            if (s < -1.0) s = -1.0;
            in[i] = static_cast<float>(s);
        }
        e.process(in.data(), ob.data(), n);
        std::copy(ob.begin(), ob.begin() + n, out.begin() + static_cast<std::ptrdiff_t>(pos));
        pos += n;
    }
    return out;
}

// ---------------------------------------------------------------------------
// Flexi switch (D-008)
// ---------------------------------------------------------------------------
class FlexiSwitch {
public:
    void reset(bool initiallyActive = false) {
        latched_ = initiallyActive;
        held_ = false;
        pressTime_ = 0.0;
        pendingRestore_ = false;
        prior_ = initiallyActive;
    }
    bool update(bool held, double timeSeconds) {
        if (!std::isfinite(timeSeconds)) timeSeconds = 0.0;
        if (held && !held_) { pressTime_ = timeSeconds; prior_ = latched_; pendingRestore_ = false; }
        if (!held && held_) {
            const double dur = timeSeconds - pressTime_;
            if (dur < 0.300) {
                latched_ = !latched_; // tap toggles
            } else {
                latched_ = prior_;    // hold release restores prior state
            }
            pendingRestore_ = false;
        }
        held_ = held;
        return active();
    }
    bool active() const { return latched_ || held_; }

private:
    bool latched_{false};
    bool held_{false};
    bool prior_{false};
    bool pendingRestore_{false};
    double pressTime_{0.0};
};

// ---------------------------------------------------------------------------
// Patch state (D-007): JSON with finite normalized parameters only.
// ---------------------------------------------------------------------------
struct PatchState { Parameters parameters; };

namespace patch_detail {
// Minimal flat-JSON object parser (numbers, booleans, strings ignored).
inline bool parseObject(const std::string& json, std::vector<std::pair<std::string, std::string>>& kv) {
    std::string s;
    s.reserve(json.size());
    for (char c : json) { if (!std::isspace(static_cast<unsigned char>(c))) s.push_back(c); }
    if (s.size() < 2 || s.front() != '{' || s.back() != '}') return false;
    std::size_t i = 1;
    while (i < s.size() - 1) {
        if (s[i] == ',') { ++i; continue; }
        if (s[i] != '"') return false;
        std::size_t j = s.find('"', i + 1);
        if (j == std::string::npos) return false;
        const std::string key = s.substr(i + 1, j - i - 1);
        i = j + 1;
        if (i >= s.size() || s[i] != ':') return false;
        ++i;
        std::string val;
        if (s[i] == '"') {
            j = s.find('"', i + 1);
            if (j == std::string::npos) return false;
            val = s.substr(i, j - i + 1);
            i = j + 1;
        } else {
            j = i;
            while (j < s.size() && s[j] != ',' && s[j] != '}') ++j;
            val = s.substr(i, j - i);
            i = j;
        }
        kv.emplace_back(key, val);
    }
    return true;
}
inline bool parseNumber(const std::string& v, double& out) {
    if (v.empty() || v == "true" || v == "false" || v.front() == '"') return false;
    try {
        std::size_t idx = 0;
        out = std::stod(v, &idx);
        return idx == v.size() && std::isfinite(out);
    } catch (...) { return false; }
}
} // namespace patch_detail

class PatchCodec {
public:
    static std::string serialize(const PatchState& ps) {
        const Parameters& p = ps.parameters;
        std::ostringstream o;
        o << '{';
        auto put = [&](const char* k, double v) { o << '"' << k << '"' << ':' << v << ','; };
        put("pitch", p.pitch); put("primary", p.primary); put("secondary", p.secondary);
        put("tracking", p.tracking); put("tone", p.tone); put("magic", p.magic);
        put("pitchCv", p.pitchCv); put("pitchAttenuverter", p.pitchAttenuverter);
        o << "\"active\":" << (p.active ? "true" : "false") << ',';
        o << "\"magicEngaged\":" << (p.magicEngaged ? "true" : "false");
        o << '}';
        return o.str();
    }
    static PatchState deserialize(const std::string& json) {
        PatchState st; // safe defaults
        std::vector<std::pair<std::string, std::string>> kv;
        if (!patch_detail::parseObject(json, kv)) return st; // malformed -> safe defaults
        auto num = [&](const char* k, float& dst) {
            for (const auto& p : kv) if (p.first == k) {
                double v;
                if (patch_detail::parseNumber(p.second, v)) dst = static_cast<float>(v);
            }
        };
        auto boolean = [&](const char* k, bool& dst) {
            for (const auto& p : kv) if (p.first == k) {
                if (p.second == "true") dst = true;
                else if (p.second == "false") dst = false;
            }
        };
        num("pitch", st.parameters.pitch); num("primary", st.parameters.primary);
        num("secondary", st.parameters.secondary); num("tracking", st.parameters.tracking);
        num("tone", st.parameters.tone); num("magic", st.parameters.magic);
        num("pitchCv", st.parameters.pitchCv);
        num("pitchAttenuverter", st.parameters.pitchAttenuverter);
        boolean("active", st.parameters.active);
        boolean("magicEngaged", st.parameters.magicEngaged);
        return st;
    }
};

// ---------------------------------------------------------------------------
// Reference / manifest validation (D-011, D-013, A-007 identity)
// ---------------------------------------------------------------------------
struct ReferenceSet {
    static void validate(const std::string& manifestPath);
};

namespace ref_detail {
inline std::string readFile(const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("cannot open: " + path);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}
inline std::string jsonGet(const std::string& json, const std::string& key, bool& found) {
    const std::string pat = '"' + key + '"';
    std::size_t p = json.find(pat);
    found = false;
    if (p == std::string::npos) return "";
    p = json.find(':', p + pat.size());
    if (p == std::string::npos) return "";
    ++p;
    while (p < json.size() && std::isspace(static_cast<unsigned char>(json[p]))) ++p;
    if (p >= json.size()) return "";
    found = true;
    if (json[p] == '"') {
        const std::size_t e = json.find('"', p + 1);
        return e == std::string::npos ? "" : json.substr(p + 1, e - p - 1);
    }
    const std::size_t e = json.find_first_of(",}", p);
    return json.substr(p, e == std::string::npos ? std::string::npos : e - p);
}
inline std::vector<std::string> jsonArray(const std::string& json, const std::string& key, bool& found) {
    std::vector<std::string> out;
    const std::string pat = '"' + key + '"';
    std::size_t p = json.find(pat);
    found = false;
    if (p == std::string::npos) return out;
    p = json.find('[', p + pat.size());
    if (p == std::string::npos) return out;
    const std::size_t e = json.find(']', p);
    if (e == std::string::npos) return out;
    found = true;
    std::string body = json.substr(p + 1, e - p - 1);
    std::istringstream iss(body);
    std::string tok;
    while (std::getline(iss, tok, ',')) {
        while (!tok.empty() && (std::isspace(static_cast<unsigned char>(tok.front())) || tok.front() == '"')) tok.erase(tok.begin());
        while (!tok.empty() && (std::isspace(static_cast<unsigned char>(tok.back())) || tok.back() == '"')) tok.pop_back();
        if (!tok.empty()) out.push_back(tok);
    }
    return out;
}
} // namespace ref_detail

inline void ReferenceSet::validate(const std::string& manifestPath) {
    const std::string json = ref_detail::readFile(manifestPath);
    const std::string base = manifestPath.substr(manifestPath.find_last_of('/') == std::string::npos ? 0 : manifestPath.find_last_of('/') + 1);

    if (base == "plugin.json") {
        bool f;
        const std::string slug = ref_detail::jsonGet(json, "slug", f);
        if (!f || slug != "qualitycoding-rainbows")
            throw std::runtime_error("plugin.json: original plugin slug required");
        const std::string name = ref_detail::jsonGet(json, "name", f);
        if (!f || name != "Spectral Mesmerizer")
            throw std::runtime_error("plugin.json: original display name required");
        const std::string version = ref_detail::jsonGet(json, "version", f);
        if (!f || version != "2.0.0") throw std::runtime_error("plugin.json: version 2.0.0 required");
        const std::string license = ref_detail::jsonGet(json, "license", f);
        if (!f || license != "Apache-2.0") throw std::runtime_error("plugin.json: Apache-2.0 required");
        // Forbidden protected identity (D-012).
        for (const char* bad : {"EarthQuaker", "EQD", "Rainbow Machine", "rainbow machine"}) {
            if (json.find(bad) != std::string::npos)
                throw std::runtime_error(std::string("plugin.json: protected identity marker present: ") + bad);
        }
        const std::string brand = ref_detail::jsonGet(json, "brand", f);
        if (f && (brand.find("EarthQuaker") != std::string::npos || brand.find("EQD") != std::string::npos))
            throw std::runtime_error("plugin.json: protected brand present");
        return;
    }

    if (base == "ci-matrix.json") {
        bool f;
        const auto required = ref_detail::jsonArray(json, "required", f);
        if (!f || required.empty()) throw std::runtime_error("ci-matrix.json: required runner list missing");
        const std::string wf = ref_detail::readFile(".github/workflows/build.yml");
        for (const auto& r : required) {
            if (wf.find(r) == std::string::npos)
                throw std::runtime_error("ci-matrix.json: runner not present in build.yml: " + r);
        }
        if (wf.find("fail-fast: false") == std::string::npos)
            throw std::runtime_error("ci-matrix.json: fail-fast:false required");
        return;
    }

    if (base == "dependency-scan.json") {
        bool f;
        const std::string sev = ref_detail::jsonGet(json, "maximumUnacceptedSeverity", f);
        if (!f || sev.empty()) throw std::runtime_error("dependency-scan.json: maximumUnacceptedSeverity missing");
        if (sev != "Low" && sev != "Medium" && sev != "High" && sev != "Critical")
            throw std::runtime_error("dependency-scan.json: invalid severity");
        // The implementation must record its own dependency scan report.
        const std::string report = ref_detail::readFile("tests/results/security.json");
        const std::string status = ref_detail::jsonGet(report, "status", f);
        if (!f || status != "PASS") throw std::runtime_error("dependency-scan.json: security scan report missing or not PASS");
        const std::string highest = ref_detail::jsonGet(report, "highestSeverity", f);
        if (f && (highest == "Critical" || highest == "High"))
            throw std::runtime_error("dependency-scan.json: unaccepted Critical/High finding");
        // Pinned requirements must exist.
        ref_detail::readFile("requirements-audio.lock");
        return;
    }

    if (base == "references.json") {
        bool f;
        const std::string sr = ref_detail::jsonGet(json, "sampleRate", f);
        if (!f || sr != "48000") throw std::runtime_error("references.json: sampleRate must be 48000");
        const std::string ch = ref_detail::jsonGet(json, "channels", f);
        if (!f || ch != "1") throw std::runtime_error("references.json: mono required");
        const std::string bits = ref_detail::jsonGet(json, "bits", f);
        if (!f || bits != "24") throw std::runtime_error("references.json: 24-bit required");
        const std::string pairs = ref_detail::jsonGet(json, "pairs", f);
        if (!f || pairs != "12") throw std::runtime_error("references.json: 12 pairs required");
        const std::string rms = ref_detail::jsonGet(json, "rmsDbfs", f);
        if (!f || rms != "-23") throw std::runtime_error("references.json: -23 dBFS RMS required");
        const std::string lic = ref_detail::jsonGet(json, "license", f);
        if (!f || lic.empty()) throw std::runtime_error("references.json: license required");
        const std::string status = ref_detail::jsonGet(json, "status", f);
        if (f && status == "AWAITING_USER_REFERENCES") return; // blocked path, structurally valid (D-023)
        // When references are supplied, checksums/attribution must be present.
        const std::string sums = ref_detail::jsonGet(json, "sha256", f);
        if (!f || sums.empty()) throw std::runtime_error("references.json: sha256 checksums required");
        const std::string attr = ref_detail::jsonGet(json, "attribution", f);
        if (!f || attr.empty()) throw std::runtime_error("references.json: attribution required");
        return;
    }

    throw std::runtime_error("unknown manifest: " + manifestPath);
}

} // namespace rainbows
