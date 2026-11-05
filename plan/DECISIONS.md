# Decisions and Interfaces

## Product and DSP

- **D-001 — Clean-room Apache-2.0:** Use original C++17 and original SVG. Include Apache-2.0, NOTICE, VCV Rack Non-Commercial Plugin License Exception, source URL, and unofficial-effect disclaimer. Copy no Rack or pedal code. If any copied Rack code is introduced, halt and write `BLOCKED.md`; do not silently relicense. Claims: C-010/C-011.
- **D-002 — Pitch mapping:** Normalized knob `k∈[0,1]`, connected CV `v` in volts and attenuverter `a∈[-1,1]`: `s=clamp(9*(k-0.5)+12*v*a,-5,4)` semitones; disconnected CV contributes zero. Primary ratio is `2^(s/12)`. Unison is exactly `s=0`. CV is not quantized. Rationale: the ±5 V CV range reaches the hardware limits and noon remains unison.
- **D-003 — Engine:** One circular input history drives two half-period-offset, equal-power-crossfaded, cubic-interpolated read heads. Delay slope is `1-ratio`; grain period and base lag derive from Tracking. Primary processes ratio `2^(s/12)`. Secondary processes ratio `2^((s+12*sign(s))/12)` for nonzero `s`; at unison it is a separately modulated unity-ratio chorus voice. Independent 2× oversampling is not used; intentional aliasing is part of the effect. No source separator, FFT, monophonic pitch tracker, allocation, lock, file I/O, or network call in `process`.
- **D-004 — Wet mix:** `wet = PrimaryGain * primary + SecondaryGain * secondary`. Primary/Secondary gains are squared normalized knobs for perceptual taper. Output starts as dry input; Active controls a 5 ms equal-power dry/wet crossfade. Zero both wet knobs yields dry input even while Active.
- **D-005 — Tracking:** Initial clean-room map `lagMs=250*(1-k)^2+2`, smoothed over 20 ms; exact curve is calibrated against captures without changing monotonicity. Buffer supports 300 ms plus one grain and 192 kHz. Higher Tracking always reduces lag. If captures contradict the 250 ms endpoint, change only the map constants through frozen-test amendment, not the monotonic requirement.
- **D-006 — Tone:** Wet-only one-pole low-pass, cutoff exponentially mapped from 800 Hz at `k=0` to 16 kHz at `k=1`, clamped below `0.45*sampleRate`; clockwise is brighter. Tone must not alter the dry bypass path.
- **D-007 — Rack contract:** Module slug `SpectralMesmerizer`, plugin slug `qualitycoding-rainbows`, display name `Spectral Mesmerizer`, version `2.0.0`. Mono audio input uses `getVoltageSum()`; CV uses channel 0. Audio nominal ±5 V; output soft-saturates and is finite. `configBypass` supplies Rack's bypass route, while the custom Active control uses 5 ms ramps. `onSampleRateChange(e)` reallocates outside `process`; JSON stores only finite normalized parameters and switch states. Malformed JSON leaves safe defaults. Rack component layout is original, not the pedal's layout.
- **D-008 — Flexi UI:** Custom Activate and Magic buttons: press starts a timer; release under 300 ms toggles latched state; press held at least 300 ms temporarily engages and release restores prior state. Magic engagement is ineffective when bypassed but its latched state persists. Parameter smoothing uses 5 ms for Active/Magic and 20 ms for continuous controls.
- **D-009 — Magic matrix:** Feedback uses self and cross injection from both wet paths into the delay history. `magic=0` gives zero feedback; disengaging Magic ramps feedback to zero over 5 ms. Normalize by input/output RMS-independent fixed matrix gains, then soft-saturate only the feedback path. Maximum effective loop gain is calibrated below unbounded runaway; output remains bounded. A 10-minute maximum-Magic test must stay finite. Exact coefficients are calibration outputs, not proprietary claims.
- **D-010 — Numeric contract:** Float audio/state, double delay positions and time coefficients; FTZ/DAZ scoped to the audio thread where Rack permits. DC-block feedback at 5 Hz. Clamp delay reads to allocated history. Nonfinite input is replaced by zero; nonfinite state triggers `reset()` and outputs dry input for that frame. Tolerances: exact control formulas 1e-6; block invariance 1e-6; settled isolated-sine pitch median absolute error ≤10 cents; bypass added discontinuity ≤0.05 V; no arbitrary spectral tolerance before capture analysis.
- **D-011 — Reference capture:** `references.json` requires dry source plus matching V2 hardware recordings, mono 48 kHz/24-bit WAV, same re-amping chain, user-owned redistribution permission, SHA-256 and attribution. Capture matrix: Pitch {-5,0,+4} × Secondary {0,1} × Tracking {0,0.5,1} × Tone {0,1}, plus Magic {0.25,0.65,1} ascending and descending, bypass and momentary gestures. Sources: isolated 110/220/440/880 Hz sines, E-minor chord, picked riff, noise burst. Hardware gain is aligned by dry reference, not by tuning each clip.
- **D-012 — Identity:** Use original name, colors and component arrangement. Do not use `Hardware clone` manifest tag or EQD branding. A future VCV Library submission requires owner permission if identity later changes and always requires G-002.
- **D-013 — Pins/platforms:** Rack SDK 2.6.6 official ZIPs; C++17; versioned runners `ubuntu-24.04`, `windows-2025`, `macos-15-intel`, `macos-15`, all `fail-fast:false`. Linux ARM64 is not an intake platform and has no CI job. Alpine/libc-variant and oldest-toolchain jobs are waived because the deliverable uses the pinned Rack SDK/toolchain rather than a supported dependency range; native package ABI is governed by that SDK.
- **D-014 — Listening:** Twelve start-window pairs, 1.0 s, −23 dBFS RMS, 5 ms fades, 24-bit WAV, base64 key, base seed 20261106. After disclosed results use a new seed from `secrets.randbits(64)` recorded sealed in the bundle. Necessary score ≤8/12; any reported clear defect, 10+/12 one-side bias with confident qualitative discrimination, or reversed-discrimination caveat triggers iteration. Maximum 3 rounds. Demos: melody, fast chord passage, sustained chord, soft-to-loud dynamics, every knob/CV, low/high registers, unison triple chorus, ascending/descending Magic, momentary/latching gestures, maximum-Magic recovery.
- **D-015 — Mutation opt-out:** `S-MUTATE` omitted: the 28-test C++/CI/audio suite is small, native Rack integration and listening provide separate gates, and maintaining another pinned mutation tool/config has poor ROI. Revisit when DSP exceeds 5,000 source lines.

## Public interfaces

`src/dsp/Engine.hpp` is the frozen pure-C++ stub contract: `Parameters`, `Metrics`, `Engine::{prepare,reset,setParameters,process,benchmark,pitchSemitones,secondaryRatio,trackingMilliseconds,renderEventFile}`, `FlexiSwitch`, `PatchState`, `PatchCodec`, and `ReferenceSet::validate`. Errors: invalid sample rate/block size and malformed patch JSON reject safely; no exceptions escape audio `process`. Offline tools emit WAV plus JSON with commit, SDK lock hash, seed, parameters, UTC, hardware and thread count.

## Numbered failure rules

- **D-016 — Dependency failure:** retry and hash-verify; otherwise halt as specified below.
- **D-017 — Rack API mismatch:** adapt only the wrapper against pinned headers; otherwise halt.
- **D-018 — DSP correctness failure:** fix interpolation/mapping, never tolerances.
- **D-019 — Magic instability:** globally reduce feedback; seek listening rescope if the effect disappears.
- **D-020 — Performance miss:** optimize in the prescribed order; only a human may relax.
- **D-021 — Security finding:** upgrade/remove/halt; no silent acceptance.
- **D-022 — CI platform failure:** retain all jobs; waiver only by human amendment.
- **D-023 — Missing references:** mark the listening gate blocked and stop dependent packaging.
- **D-024 — Listening iteration:** one general mechanism per reported difference; fresh seed; three rounds maximum.
- **D-025 — Unanticipated situation:** most reversible option, with the halt conditions below.

## Decision rules

- **Dependency install/download fails:** retry five times with 15/30/45/60/75 s backoff; verify SHA-256; if still failing use the documented mirror only after its hash matches, otherwise halt `BLOCKED.md`.
- **Unexpected Rack API shape:** stop at the pure-C++ boundary, consult pinned 2.6.6 headers, adapt only the wrapper; public DSP interfaces stay unchanged. If that is impossible, halt `BLOCKED.md`.
- **Pitch or block tests fail:** fix delay interpolation/read-head phase, not tolerance. A frozen-test defect requires `TEST_CHALLENGE.md` and amendment.
- **Magic becomes unstable:** reduce feedback matrix gain globally and rerun all metrics; do not add per-clip constants. If bounded output removes required regeneration, halt at G-LISTEN with rescope options.
- **Performance below 20× real time:** profile, then optimize interpolation/control update rate/SIMD in that order while retaining T018 determinism. If still below target, halt `BLOCKED.md`; only a human gate may relax it.
- **Security scan Critical/High:** upgrade or remove dependency and rerun. If no fix exists, remove the affected feature or halt; never accept silently.
- **CI platform failure:** keep other matrix jobs running, fix platform wrapper or record a new D waiver only through human amendment; do not delete the job.
- **Missing references at S-LISTEN:** write `GATE-G-LISTEN.md` with `blocked: references` and stop dependent packaging; independent documentation/review cleanup may continue.
- **Listening `iterate`:** analyze the reported mechanism, make one general change per difference, rerun all metrics, render a new-seed bundle, and repeat. After round 3 allow only `proceed-with-rescope` or `stop`.
- **Unanticipated situation:** choose the most reversible non-scope-expanding option, log `DEVIATIONS.md`, and continue unless it touches frozen tests, security, integrity, public interface, or release; then halt `BLOCKED.md`.
