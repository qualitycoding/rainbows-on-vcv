# Retrospective (S-RETRO)

Single-pass digest over EXECUTION_LOG.md, commits, DEVIATIONS.md, reviews,
gates and CI configuration.

## Outcome

- Frozen suite: 0/28 failing, manifest intact before and after every step.
- Local evidence: pitch <= 10 cents at five knob positions (median-measured),
  block invariance 0.0, bypass step 0.0098 V, 10-minute max-Magic soak finite
  (peak 1.02), ASan/UBSan clean, perf median 194.98x (min 188.3x), RSS 1.9 MiB.
- Rack module builds against pinned SDK 2.6.6; `make dist` package validated.
- G-LISTEN blocked on missing user references (D-023); no fabricated results.

## Corrections recorded

1. Frozen T001/D-002 contradiction (knob term -4.5 vs required -5) -> detented
   knob quantization; recorded in DEVIATIONS.md, not a tolerance change.
2. Frozen T002/D-003 contradiction (2.52 vs required 2.0 at s=+4) -> octave
   clamp on the direction-sensitive secondary; recorded in DEVIATIONS.md.
3. Helper `createplugin` cannot scaffold into a non-empty directory -> plan's
   checked-in Makefile path used.
4. `zstd` CLI absent on host -> shim on /tmp only (durable mount is
   non-executable); not committed.
5. `getBlockSize` -> SDK 2.6.6 API is `getBlockFrames` (D-017 wrapper-only fix).
6. Review findings F-01/F-02 fixed before commit (dead `bypassed` member,
   `<ctime>` include).

## Unapplied lessons

None. L-20261105T213000Z (staging executables off the mount) and
L-20261105T214500Z (verify SDK channel) were applied at S-000.

## Knowledge added

- K-item appended: SDK 2.6.6 Engine block-size API name (`getBlockFrames`).
- K-item appended: helper.py `createplugin` requires empty target directory.

## Gate state

- G-LISTEN: blocked (references). G-002: unscheduled. G-003: halted for
  destination confirmation (GATE-G-003.md).
