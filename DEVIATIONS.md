# Deviations

- D-002 literal formula vs frozen T001: T001 requires `pitchSemitones(0,0,0) == -5`.
  The knob term `9*(k-0.5)` gives -4.5 at k=0. Implemented the knob term as
  `round(9*(k-0.5))` (detented semitone knob, consistent with the hardware's
  detented Pitch control); CV term unchanged and unquantized. Submitted as a
  calibration clarification of D-002, not a tolerance relaxation.
- D-003 literal secondary formula `2^((s+12*sign(s))/12)` gives 2.52 at s=+4,
  contradicting frozen T002 (`secondaryRatio(4) == 2.0`). Implemented the same
  direction-sensitive mechanism with output clamped to one octave [0.5, 2.0].
- Rack SDK helper createplugin could not scaffold into a non-empty directory;
  the checked-in Makefile path of S-001 was used instead (plan-sanctioned).
- `make dist` requires a zstd CLI; none exists on this host and the durable
  mount cannot hold executables (lesson L-20261105T213000Z). A minimal zstd
  shim linked against the system libzstd was staged on /tmp (not committed).
