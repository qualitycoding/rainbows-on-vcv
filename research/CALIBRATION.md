# Calibration Record (S-006)

Status: **blocked: references** (D-023).

- `tools/import_references.py` is committed and ready: validates manifest fields,
  mono 48 kHz/24-bit WAV, SHA-256 checksums, attribution, license and the
  D-011 capture matrix before any metric is computed.
- No user-owned redistribution-compatible hardware recordings are available
  (tests/fixtures/references.json status `AWAITING_USER_REFERENCES`).
- No similarity claim is made. No per-note or per-clip tuning constants exist;
  when references arrive, only global gain/lag and the general Tracking,
  modulation, feedback and saturation mechanisms are fit, once, across the
  whole set (D-011).
- Current clean-room maps (unverified against hardware): Tracking
  `250*(1-k)^2+2` ms; Tone 800 Hz..16 kHz; Magic feedback matrix gain 0.7 with
  tanh soft-saturation and 5 Hz DC block; secondary voice clamped to one
  octave in the pitch direction.
