# READY — Spectral Mesmerizer VCV Rack Plan

Active profiles: `software`; flags: `audio=true`, `software.deploys=false`, `courseware=false`.

Counts: 14 claims (8 corroborated, 3 verified, 3 single-source); 28 tests (8 unit, 10 integration, 4 operational, 5 security, 1 performance); 15 steps; 3 defined gates (G-LISTEN, G-002 unscheduled, G-003); residual risks: 0 Critical, 0 High, 6 Medium, 5 Low; assumption audit: 20 confirmed, 0 falsified, 3 unverifiable. Planning review findings: 0 code findings, 0 adversarial execution breaks; implementation reviews have not run. Mutation: opted out D-015.

### S-000 Environment & Access Verification
- Tier: Haiku
- Profile: software/audio/ephemeral
- Depends on: none
- Inputs: `plan/ENVIRONMENT.md`, `plan/PROFILE.md`, GitHub credential named `GITHUB_TOKEN` with contents/workflow permission; stores in A-010
- Actions: 1. Create `impl-<run_id>` from the generation branch. 2. Re-run Phase 0.1 credential, permission, host and ephemeral-environment probes. 3. Stage helpers/toolchains onto executable local disk, verify invocation, keep caches/checkpoints under `/mnt/agents`. 4. Execute the pinned setup commands with five-attempt network retry. 5. Start `EXECUTION_LOG.md` with UTC, step, attempt, pass/fail/blocked, commit and note. 6. Verify frozen hashes and store access.
- Outputs: `EXECUTION_LOG.md`, `.checkpoints/state.json`, `.checkpoints/snapshots/`, staged SDK verification log
- Evidence produced: T024 setup precondition
- Done when: All checks pass; `EXECUTION_LOG.md` and state are nonempty valid JSON/text; `sha256sum -c tests/FROZEN_MANIFEST.sha256` passes; every declared host is reachable or a D alternative is recorded.
- Checkpoint: implementation branch, environment, credentials scope, store HEADs, sequence and log path.
- On failure: Stop before delivery and write `BLOCKED.md` with missing capability; preserve checkpoint. Resume with S-000 after correction.
- Gate: none
- Relevant decisions/claims: D-013, C-008/C-009/C-012/C-013
- Lessons applied: L-20261105T213000Z-mount-credential-helper-exec; L-20261105T214500Z-sdk-source-channel
- Exclusive resources: toolchain staging directory

### S-001 Scaffold Plugin and Compliance
- Tier: Sonnet
- Profile: software
- Depends on: S-000
- Inputs: `src/dsp/Engine.hpp`, `tests/fixtures/plugin.json`, D-001/D-007/D-012, pinned `RACK_DIR/helper.py`
- Actions: 1. Generate the Rack scaffold with the verified helper or checked-in Makefile, using the frozen slugs/version. 2. Add original panel SVG and component layout. 3. Add Apache-2.0, NOTICE, VCV exception, README unofficial-clone and no-network statements. 4. Add `requirements-audio.lock`, `.gitignore`, schema validators and build workflow skeleton. 5. Run T021 static checks.
- Outputs: `plugin.json`, `Makefile`, `src/plugin.*`, `src/SpectralMesmerizer.*`, `res/SpectralMesmerizer.svg`, `LICENSE`, `NOTICE`, `README.md`, `requirements-audio.lock`, `.github/workflows/build.yml`
- Evidence produced: T021, initial T024
- Done when: T021 passes; JSON/SVG parse; manifest has Apache-2.0/version 2.0.0/original identity; all outputs nonempty; no EQD image/logo/layout or credential is present.
- Checkpoint: scaffold commit and validation result.
- On failure: Revert to last green commit, correct scaffold, rerun T021/T024; copied protected identity triggers `BLOCKED.md`.
- Gate: none
- Relevant decisions/claims: D-001/D-007/D-012/D-013, C-010/C-011/C-012
- Lessons applied: L-20261105T214500Z-sdk-source-channel
- Exclusive resources: none

### S-002 Real-Time Delay Primitives
- Tier: Opus
- Profile: software/audio
- Depends on: S-001
- Inputs: `src/dsp/Engine.hpp`, D-003/D-010, frozen T009/T010/T014/T018–T020
- Actions: 1. Implement circular history, cubic interpolation, dual offset read heads and equal-power crossfade. 2. Add reset/sample-rate preparation, FTZ/DAZ policy and nonfinite recovery. 3. Add deterministic offline block harness. 4. Run focused unit/integration and sanitizer tests.
- Outputs: `src/dsp/Engine.cpp`, `src/dsp/DelayLine.hpp`, `tools/render.cpp`, `tests/results/T002-primitives.json`
- Evidence produced: T009/T010/T014/T018/T019/T020/T022
- Done when: Listed tests pass; no process-loop allocation/lock; result JSON parses and contains commit, lock hash, seed, parameters, UTC, hardware and threads.
- Checkpoint: primitive implementation commit, test report and next parameter mapping.
- On failure: Revert faulty primitive to last green; inspect wrap/interpolation phase; never relax tolerances. Resume from failed test.
- Gate: none
- Relevant decisions/claims: D-003/D-010, C-007/C-009
- Lessons applied: none
- Exclusive resources: benchmark/sanitizer runner

### S-003 Pitch, Secondary, Tracking and Tone
- Tier: Opus
- Profile: software/audio
- Depends on: S-002
- Inputs: D-002–D-006/D-010, C-001–C-004/C-007
- Actions: 1. Implement exact Pitch/CV mapping and direction-sensitive Secondary/unison voice. 2. Implement smoothed Tracking delay and wet-only Tone. 3. Add Primary/Secondary mix and dry path. 4. Render sine/chord/impulse fixtures and assert all parameter tests.
- Outputs: `src/dsp/Engine.cpp`, `tests/results/T003-parameters.json`, `renders/parameter-matrix/`
- Evidence produced: T001/T002/T003/T004/T005/T008/T009/T010/T011/T012/T013/T018
- Done when: T001–T005/T008–T013/T018 pass; report/WAVs nonempty; pitch ≤10 cents after declared settling; lag and Tone monotonic; dry path unchanged by Tone.
- Checkpoint: parameter implementation and measured baseline metrics.
- On failure: Fix mapping/read-head slope/filter coefficient; if hardware curve is contradicted use TEST_CHALLENGE amendment, not tolerance changes.
- Gate: none
- Relevant decisions/claims: D-002–D-006/D-010, C-001–C-004/C-007
- Lessons applied: none
- Exclusive resources: render output directory

### S-004 Magic and Control Smoothing
- Tier: Opus
- Profile: software/audio
- Depends on: S-003
- Inputs: D-004/D-008–D-010, C-005/C-006
- Actions: 1. Add bounded self/cross feedback and 5 Hz feedback DC block. 2. Implement Active/Magic ramps and Flexi state machine. 3. Test ascending/descending echoes, disengage recovery, tap/hold and maximum-Magic soak. 4. Save control traces.
- Outputs: `src/dsp/Engine.cpp`, `src/dsp/FlexiSwitch.cpp`, `tests/results/T004-magic.json`, `renders/control-traces/`
- Evidence produced: T006/T007/T011–T014/T016/T019/T020/T027
- Done when: All listed tests pass; output finite/bounded for 10 minutes; Magic sign follows Pitch; release restores previous latch; reports/traces parse and are nonempty.
- Checkpoint: feedback coefficients, test report and known calibration unknowns.
- On failure: Apply D-019 global gain reduction; if required effect disappears, stop for G-LISTEN rescope rather than per-clip tuning.
- Gate: none
- Relevant decisions/claims: D-004/D-008/D-009/D-010, C-005/C-006
- Lessons applied: none
- Exclusive resources: soak benchmark runner

### S-005 Rack Wrapper, Original UI and Patch State
- Tier: Sonnet
- Profile: software
- Depends on: S-004
- Inputs: D-007/D-008, pinned SDK headers, original SVG
- Actions: 1. Configure knobs, CV/attenuverter, audio ports, lights and bypass. 2. Sum poly audio and use channel-0 CV. 3. Call `prepare` on sample-rate event and serialize only finite control state. 4. Build native Linux module and run malformed-patch/UI-state tests.
- Outputs: `src/SpectralMesmerizer.cpp`, `src/SpectralMesmerizerWidget.cpp`, `tests/results/T005-rack.json`
- Evidence produced: T011/T015–T017/T019–T021
- Done when: Listed tests pass; `RACK_DIR="$RACK_DIR" make -j2` succeeds; plugin manifest and panel parse; malformed state yields defaults; no UI copied identity.
- Checkpoint: wrapper build and patch-state report.
- On failure: Adapt only wrapper per D-017; restore safe defaults; public DSP changes require BLOCKED.
- Gate: none
- Relevant decisions/claims: D-007/D-008, C-008/C-009/C-011
- Lessons applied: none
- Exclusive resources: Rack SDK build directory

### S-006 Reference Import and Calibration
- Tier: Opus
- Profile: software/audio
- Depends on: S-003, S-004
- Inputs: D-011, `tests/fixtures/references.json`, user-supplied dry/hardware WAVs when available
- Actions: 1. Implement `tools/import_references.py` to validate format/license/checksums/attribution and capture matrix. 2. Estimate global gain/lag once from dry reference. 3. Fit only general Tracking, modulation, feedback and saturation mechanisms across the entire set. 4. Generate metrics; never tune per-note constants. 5. If references are absent, commit the ready importer and mark calibration blocked for S-LISTEN.
- Outputs: `tools/import_references.py`, `references/manifest.json`, `research/CALIBRATION.md`, `tests/results/T006-reference-metrics.json`
- Evidence produced: T025 metric portion; T001–T014 regression
- Done when: Importer rejects missing/invalid reference metadata; when references exist every metric threshold passes and report has provenance; otherwise `blocked: references` is explicit and no similarity claim is made.
- Checkpoint: reference set hash, calibration coefficients or blocked status.
- On failure: Use D-023 rules; conflicting V2 observation triggers TEST_CHALLENGE, not silent retuning.
- Gate: none
- Relevant decisions/claims: D-005/D-009/D-010/D-011, C-001–C-007
- Lessons applied: none
- Exclusive resources: reference directory

### S-007 Full CI, Security, Performance and Soak
- Tier: Sonnet
- Profile: software/ephemeral
- Depends on: S-005, S-006
- Inputs: `plan/ENVIRONMENT.md`, four-runner matrix, `requirements-audio.lock`
- Actions: 1. Complete native build jobs with `fail-fast:false` and archive hashes. 2. Run all 28 tests on Linux and applicable smoke tests on other runners. 3. Run ASan/UBSan, pip-audit, secret scan, five-run performance and 10-minute soak on Ubuntu. 4. Upload nonempty reports/packages with retention 30 days. 5. Commit/push each green chunk and verify `ls-remote`.
- Outputs: `.github/workflows/build.yml`, `tests/results/ci-summary.json`, `dist/*.vcvplugin`, `tests/results/performance.json`, `tests/results/security.json`
- Evidence produced: T019–T024/T026/T027 and full-suite CI
- Done when: Every job and numeric budget passes; four packages exist with valid ZIP/Zstandard structure; reports contain required fields; artifacts are uploaded; no Critical/High unaccepted finding.
- Checkpoint: CI run URL, commit, per-platform results and benchmark class.
- On failure: Follow dependency/performance/security/platform rules; CI is authoritative for long jobs; do not label locally blocked checks green.
- Gate: none
- Relevant decisions/claims: D-013, C-012/C-013, A-005/A-009
- Lessons applied: L-20261105T213000Z-mount-credential-helper-exec
- Exclusive resources: GitHub Actions runner and artifact namespace

### S-008 Deterministic Demos and Blind-Pair Builder
- Tier: Sonnet
- Profile: software/audio
- Depends on: S-005, S-006, S-007
- Inputs: D-014, reference WAVs, event-file schema, `scripts/make_blind_pairs.py`, `scripts/score_blind.py`
- Actions: 1. Vendor/adapt the pinned listening tools and test wrong-format/side-key failure paths. 2. Commit all D-014 event files and headlessly render demos. 3. Build 12 start-window pairs at 1.0 s, −23 dBFS RMS, 5 ms fades, 24-bit WAV. 4. Seal base64 key, create SHA256SUMS/attribution and validate reproducible render hashes. 5. Do not decode key before answers.
- Outputs: `demos/*.events`, `demos/*.wav`, `scripts/make_blind_pairs.py`, `scripts/score_blind.py`, `evidence/G-LISTEN/`, `tests/results/T008-audio-evidence.json`
- Evidence produced: T025/T028
- Done when: When references exist, all files parse/play as mono 48 kHz/24-bit; 12 pairs and all demos exist; checksum manifest verifies; key remains sealed; two demo renders have identical SHA-256. When references are absent, all reproducible demos exist and `tests/results/T008-audio-evidence.json` explicitly records `blocked: references`; no pair or score is fabricated.
- Checkpoint: seed, bundle commit, render hashes and pair count.
- On failure: Regenerate from event files; missing references blocks S-LISTEN; never fabricate a pair or score.
- Gate: none
- Relevant decisions/claims: D-011/D-014, C-014
- Lessons applied: prior qualitative-listening prevention rule
- Exclusive resources: evidence staging branch

### S-REVIEW Traditional Code Review
- Tier: Opus
- Profile: software
- Depends on: S-007, S-008
- Inputs: Full implementation diff, frozen suite, manifests, dependency releases/notices
- Actions: 1. A fresh reviewer who wrote no code reviews anti-patterns, hidden mutation, real-time safety, test mapping, native-memory/security and outdated dependencies. 2. Classify findings per Rule 4. 3. Fix Critical/High before later delivery; link every finding to fix commit or accepted risk. 4. New tests follow Phase 2 amendment/refreeze.
- Outputs: `reviews/CODE_REVIEW.md`, fix commits, `EXECUTION_LOG.md`
- Evidence produced: review coverage of T001–T028
- Done when: Report is nonempty, every new code path maps to a test, Critical/High count unresolved is zero, all findings link to fixes/risks, suite remains green.
- Checkpoint: reviewer identity/context, findings and fix commits.
- On failure: Return to owning implementation step; review cannot weaken tests; unresolved High blocks packaging/listening.
- Gate: none
- Relevant decisions/claims: D-001–D-015
- Lessons applied: all planning lessons
- Exclusive resources: implementation diff under review

### S-ADVERSARIAL Adversarial Review
- Tier: Fable (Mythos)
- Profile: software/audio
- Depends on: S-REVIEW
- Inputs: `reviews/CODE_REVIEW.md`, A-001…A-011, malformed inputs, patch state, dependency-failure harness
- Actions: 1. A separate fresh reviewer attacks delay wraparound, extreme rates/CV, NaN/Inf, feedback runaway, rapid gestures, malformed JSON, block-size changes, resource exhaustion and dependency failures. 2. Confirm or reject each break. 3. Convert confirmed breaks to frozen tests via amendment or document accepted severity; Critical/High block. 4. Re-run sanitizers and full suite.
- Outputs: `reviews/ADVERSARIAL_REVIEW.md`, challenge/fix commits
- Evidence produced: T017–T023/T026/T027 strengthened
- Done when: Report lists every attack/outcome; zero unresolved Critical/High; all confirmed fixes and frozen amendments linked; CI green.
- Checkpoint: attacks, confirmed breaks, resolutions and residual risks.
- On failure: Return to owning step; unanticipated integrity/security/public-interface issue writes BLOCKED.
- Gate: none
- Relevant decisions/claims: D-003/D-007–D-010/D-015
- Lessons applied: all planning lessons
- Exclusive resources: sanitizer and stress runner

### S-LISTEN Blind Listening and Demo Gate
- Tier: Opus
- Profile: software/audio
- Depends on: S-ADVERSARIAL
- Inputs: `evidence/G-LISTEN/`, CI green commit, `plan/GATES.md`, metric table, sealed key
- Actions: 1. Commit with marker `[gate-evidence G-LISTEN]`; CI pushes orphan `evidence/G-LISTEN` under its own concurrency group. 2. Supply branch ZIP/raw URLs and phone instructions. 3. Write `GATE-G-LISTEN.md` with fixed questions and halt. 4. After response decode key, run scorer and record score/p-value/verbatim remarks/seed/commit. 5. Apply selected branch; new rounds use fresh undisclosed seeds.
- Outputs: `GATE-G-LISTEN.md`, `GATE-G-LISTEN.RESPONSE.md`, `listening/round-N.md`, evidence branch
- Evidence produced: G-LISTEN result; T025 final validation
- Done when: Proceed has ≤8/12 and no qualitative veto; or signed rescope/stop is recorded; round count ≤3; no result is fabricated; evidence URLs resolve.
- Checkpoint: round, score, remarks, response and next branch.
- On failure: Missing references remains blocked; iterate by reported mechanism only; after round 3 offer rescope/stop.
- Gate: G-LISTEN
- Relevant decisions/claims: D-011/D-014, C-014
- Lessons applied: prior qualitative-listening prevention rule
- Exclusive resources: listener and evidence/G-LISTEN branch

### S-PACKAGE Verified Desktop Artifacts
- Tier: Sonnet
- Profile: software
- Depends on: S-LISTEN
- Inputs: Four native CI builds, approved listening/rescope record, version 2.0.0
- Actions: 1. Rebuild from the exact approved commit with `RACK_DIR="$RACK_DIR" make dist`. 2. Validate each package, README limitations and notices. 3. Generate SHA256SUMS and install-smoke record. 4. Upload CI artifacts only; create no tag/release/Library submission.
- Outputs: `dist/*.vcvplugin`, `dist/SHA256SUMS`, `PACKAGE.md`
- Evidence produced: T021/T024/T025 delivery
- Done when: Four nonempty platform packages and checksums verify; PACKAGE.md names commit and listening outcome; no external release occurred.
- Checkpoint: package hashes, artifact URL and commit.
- On failure: Rebuild from clean checkout; platform failure follows D-022; any release request goes to G-002.
- Gate: none
- Relevant decisions/claims: D-001/D-012/D-013, C-010–C-013
- Lessons applied: none
- Exclusive resources: dist directory and CI artifact namespace

### S-RETRO Retrospective
- Tier: Opus
- Profile: software/audio
- Depends on: S-PACKAGE
- Inputs: `EXECUTION_LOG.md`, commits, `DEVIATIONS.md`, `BLOCKED.md`, `TEST_CHALLENGE.md`, `GATE-*.md`, CI, prior knowledge, lessons/knowledge
- Actions: 1. Haiku collects a single digest; fresh Opus performs one pass without rerunning or changing deliverables. 2. Check every correction, overlooked step, unapplied lesson, missing knowledge item and log completeness. 3. Diff workflows/manifests against environment pins; every unexplained substitution gets a process lesson. 4. Link every finding to L/K records and run Appendix C schema/secret check.
- Outputs: `RETROSPECTIVE.md`, new `lessons/L-*.md`, new `knowledge/K-*.md`
- Evidence produced: none
- Done when: Every finding links to L/K; log has one entry per attempt; deviations match diff; schema and secret scan pass; outputs nonempty.
- Checkpoint: retrospective findings and final L/K list.
- On failure: Add missing records only; do not change delivered code or rerun implementation.
- Gate: none
- Relevant decisions/claims: all
- Lessons applied: all
- Exclusive resources: none

### S-KNOW Lessons & Knowledge Push
- Tier: Haiku
- Profile: software/audio
- Depends on: S-RETRO
- Inputs: `lessons/`, `knowledge/`, A-010, `plan/GATES.md`
- Actions: 1. Write `GATE-G-003.md` and halt for destination confirmation. 2. On approval copy lessons and knowledge separately into Appendix C layouts, regenerate indexes, schema/secret scan and push each store. 3. On moved target sync and retry at most 3 times. 4. If rejected/unavailable/no response, write the specified Git bundle(s) and report paths.
- Outputs: `GATE-G-003.md`, updated store indexes or `lessons-<run_id>.bundle`/`knowledge-<run_id>.bundle`
- Evidence produced: none
- Done when: Both pushes and indexes verify, response is do-not-push, or required bundle fallback exists and is reported; no credentials/personal data in entries.
- Checkpoint: store destinations, remote heads or bundle paths.
- On failure: Three sync/push retries per store then bundle fallback; never push without sign-off.
- Gate: G-003
- Relevant decisions/claims: A-010
- Lessons applied: L-20261105T213000Z-mount-credential-helper-exec
- Exclusive resources: lesson and knowledge repository locks
