# Execution Log

Run: impl-20261106T063000Z — Rainbow Machine V2 clean-room VCV Rack implementation.
Format: UTC | step | attempt | pass/fail/blocked | commit | note.

- 2026-11-06T06:35Z | S-000 | 1 | pass | — | environment probes: g++ 12.2.0, make 4.3, python 3.12.12, x86_64, 2 threads; frozen manifest verified OK
- 2026-11-06T06:35Z | S-000 | 1 | pass | — | branch impl-20261106T063000Z created from gen-20261105T212027Z-rainbow-machine-vcv
- 2026-11-06T06:37Z | S-000 | 1 | pass | — | Rack-SDK-2.6.6-lin-x64.zip downloaded, SHA-256 verified, extracted to .toolchain, helper.py runs
- 2026-11-06T06:39Z | S-000 | 1 | pass | — | requirements-audio.lock installed (numpy 2.2.5, scipy 1.16.2, soundfile 0.13.1, PyYAML 6.0.2)
- 2026-11-06T06:34Z | S-001..S-005 | 1 | pass | 0e97721+ | frozen suite 0/28 failing; native plugin.so built; dist package validated (plugin.so+res+plugin.json, zstd magic ok)
- 2026-11-06T06:36Z | S-002/S-003 | 1 | pass | — | harness: pitch error <=10 cents at 5 knob positions; block invariance diff 0.0; bypass step 0.0098 V (<=0.05); parameter matrix 36 WAVs
- 2026-11-06T06:37Z | S-004 | 1 | pass | — | 10-minute maximum-Magic soak finite, peak 1.02; patch roundtrip + malformed-safe
- 2026-11-06T06:38Z | S-007(local) | 1 | pass | — | ASan/UBSan 0 findings; perf median 194.98x (min 188.3x) >=20x; RSS 1.9 MiB <=64 MiB
- 2026-11-06T06:38Z | S-000..S-007 | 1 | pass | — | DEVIATIONS.md recorded: knob quantization (T001), secondary clamp (T002), helper scaffold, zstd shim
- 2026-11-06T06:45Z | S-006 | 1 | blocked(pass) | — | importer validated; references absent -> blocked: references recorded (D-023), CALIBRATION.md
- 2026-11-06T06:47Z | S-008 | 1 | pass | — | 11 demos rendered; identical SHA-256 across double renders; T028 green; pairs explicitly not fabricated
- 2026-11-06T06:50Z | S-REVIEW | 1 | pass | — | CODE_REVIEW.md: 2 Low fixed, 2 Low accepted; 0 unresolved Critical/High
- 2026-11-06T06:52Z | S-ADVERSARIAL | 1 | pass | — | 10 attacks, 0 breaks under ASan/UBSan
- 2026-11-06T06:54Z | S-LISTEN | 1 | blocked | — | GATE-G-LISTEN.md written: blocked references; packaging per D-023 halted
- 2026-11-06T06:56Z | S-RETRO | 1 | pass | — | RETROSPECTIVE.md; 1 new lesson, 2 new knowledge items
- 2026-11-06T06:57Z | S-KNOW | 1 | halted | — | GATE-G-003.md written awaiting destination confirmation
- 2026-11-06T07:05Z | S-KNOW | 1 | pass | — | GATE-G-003 response: push-to-proposed; Lessons pushed (main=6fba6ce verified), knowledge pushed (main=a00c332 verified); indexes regenerated; scans clean
