# Intake and Assumptions

- **A-001 — Goal:** Build a clean-room behavioral clone of EarthQuaker Devices Rainbow Machine V2 as a VCV Rack 2 module. Status: user-confirmed.
- **A-002 — Scope:** Mono audio I/O; chord-processing audio; Pitch, Primary, Secondary, Tracking, Tone, Magic; Magic engage, bypass, Pitch CV and attenuverter. No presets, polyphonic cables, stereo, expander, copied panel artwork, or implied endorsement. Status: user-confirmed default.
- **A-003 — Platforms:** VCV Rack 2 on Windows x64, Linux x64, macOS x64, macOS ARM64. Status: user-confirmed default.
- **A-004 — Success:** Hardware-range pitch behavior, direction-sensitive Secondary, Tracking-dependent wet latency, wet-only Tone, regenerative Magic, click-resistant bypass/rate changes, deterministic tests, clean builds on A-003. Status: user-confirmed default.
- **A-005 — Performance:** At 48 kHz, one mono instance processes at least 20× real time on a pinned CI reference runner; bounded memory and no denormal-driven sustained slowdown. Status: user-confirmed default.
- **A-006 — Listening:** The user listens and later supplies redistribution-compatible hardware reference recordings. Implementation halts at the listening gate; at most three rounds. Status: user-confirmed default.
- **A-007 — License:** Apache-2.0, original code/artwork, unofficial-clone notice. Status: explicitly user-selected.
- **A-008 — Delivery:** CI-built installable packages only; public release and VCV Library submission require a separate human release gate. Semantic versioning. Status: user-confirmed default.
- **A-009 — Operations:** No networking, accounts, telemetry, personal-data storage, updater; best-effort maintenance; security scope native-memory safety, malformed patch state, dependencies, secret exclusion. Status: user-confirmed default.
- **A-010 — Stores:** `qualitycoding/Lessons` and `qualitycoding/knowledge`, subject to G-003. Status: user-confirmed default.
- **A-011 — Reference algorithm:** Public descriptions define observable behavior but not proprietary internal topology; exact similarity is established through recordings and blind listening, not source-code equivalence. Planner assumption; implementation must not claim bit-identical cloning.
