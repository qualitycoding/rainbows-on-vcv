# Adversarial Review (S-ADVERSARIAL)

Attacker: implementing agent in a separated pass, running `tools/adversarial.cpp`
under ASan/UBSan plus sanitizer cleanups of the frozen suite.

## Attacks and outcomes

| Attack | Outcome |
|---|---|
| NaN / +Inf / -Inf / +1000 V / -1000 V inputs at full wet + max Magic | Rejected: nonfinite never propagates; output bounded (D-010) |
| Feedback runaway at maximum Magic, noise burst, 10-minute soak | Bounded, peak 1.02, all frames finite (T020/T027) |
| Rapid gesture thrash: randomized pitch/tracking/magic/active every block, 2000 blocks | All outputs finite |
| Block-size changes 1/3/17/128/256/512 mid-stream | All outputs finite; T018 invariance measured 0.0 |
| Invalid prepare: sample rate 0, block size 0 | `std::invalid_argument` rejected safely |
| Extreme CV: 100 V through full attenuverter | Finite; pitch clamps to [-5, +4] st (D-002) |
| Malformed patch JSON corpus: `""`, `"{"`, `{"pitch":}`, NaN literal, `1e999`, array, wrong types | Safe defaults, no exceptions escape |
| FlexiSwitch gesture edge cases: taps, holds, release-restore | Matches D-008 state machine exactly |
| Delay wraparound: Tracking swept 0..1 repeatedly at 44.1/48/96/192 kHz | No wrap artifacts escape the allocated window; clamping holds (capacity check in `recomputeTrackingWindow`) |
| Resource exhaustion: prepare at 192 kHz with max block 512 | Capacity sized for 300 ms + 2 grains + block; no overflow |

## Result

Zero confirmed breaks. Zero unresolved Critical/High. No frozen-test amendment
was required; no new frozen tests were added (D-015 mutation opt-out applies).

Residual risks carried to the risk register: similarity to hardware is
UNVALIDATED until user references arrive (G-LISTEN blocked, D-023); clean-room
calibration constants are defaults, not hardware-fitted values.
