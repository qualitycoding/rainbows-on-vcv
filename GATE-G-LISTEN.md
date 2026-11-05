# GATE G-LISTEN — Blind Listening and Demo Gate

**Status: blocked — references (D-023).**

The blind listening gate cannot run: user-owned redistribution-compatible
hardware reference recordings (dry source + matching hardware captures, mono
48 kHz/24-bit WAV, D-011 capture matrix) have not been supplied
(`tests/fixtures/references.json` status `AWAITING_USER_REFERENCES`).

No pairs, scores or similarity claims are fabricated. The 12-pair bundle
builder (`scripts/make_blind_pairs.py`), scorer (`scripts/score_blind.py`) and
all 11 D-014 demos are committed and deterministic; demos render with identical
SHA-256 across repeated runs (`tests/results/T008-audio-evidence.json`).

## What is needed to unblock

1. Dry source recordings: isolated 110/220/440/880 Hz sines, E-minor chord,
   picked riff, noise burst.
2. Matching hardware recordings through one fixed re-amping chain, mono
   48 kHz/24-bit WAV, with redistribution permission, SHA-256 checksums and
   attribution (D-011).
3. Answers to the fixed G-LISTEN questions (plan/GATES.md) for the 12 pairs.

## Consequences

- S-PACKAGE (verified desktop artifacts as *delivered release*) is blocked per
  D-023; the Linux build, package and checksums are nevertheless committed as
  build evidence and CI artifacts are produced per push (no release, G-002
  unscheduled).
- No similarity claim is made anywhere; the effect is a clean-room default
  calibration awaiting capture fitting (research/CALIBRATION.md).
