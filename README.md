# Spectral Mesmerizer (qualitycoding-rainbows) — UNOFFICIAL CLONE

A clean-room VCV Rack 2 mono effect inspired by the *observable behavior* of a
classic boutique pitch-warp delay pedal. Original C++17 code and original SVG
panel artwork, Apache-2.0 licensed, with the VCV Rack Non-Commercial Plugin
License Exception applying to the compiled plugin package.

- Not affiliated with, endorsed by, or licensed from EarthQuaker Devices.
- No proprietary source code, panel artwork, logos, or trade dress were copied.
- No similarity claim beyond behavioral calibration against user-supplied
  hardware recordings; see `references.json` and `GATE-G-LISTEN.md`.
- This software performs no networking, accounts, telemetry, or data collection.

## Controls

| Control | Behavior |
|---|---|
| Pitch | Detented semitone pitch of the primary voice (−5…+4 st, CV ±1 V×attenuverter adds ±12 V→±12 st before clamp). |
| Primary / Secondary | Squared-knob wet voice gains. Secondary follows pitch direction (octave up/down) or a unison chorus voice at unison. |
| Tracking | Delay lag map 252 ms → 2 ms, monotonic; higher Tracking = shorter lag. |
| Tone | Wet-only one-pole low-pass, 800 Hz … 16 kHz. Dry path unaffected. |
| Magic | Regenerative feedback amount when the Magic button is engaged; 5 Hz DC-blocked, soft-saturated, bounded. |
| Active / Magic buttons | Tap (<300 ms) toggles; hold (≥300 ms) is momentary and restores prior state on release. |

## Building

Requires the pinned VCV Rack SDK 2.6.6 (`RACK_DIR`):

```sh
make -j2          # build plugin.so
make dist         # produce dist/qualitycoding-rainbows-2.0.0-<arch>.vcvplugin
make install      # install into your Rack plugins dir
```

## Tests

```sh
make -f tests/Makefile test     # frozen 28-test suite
```

## Status

Implementation branch `impl-20261106T063000Z`. Delivery is via CI artifacts
only; no public release or VCV Library submission occurs without a separate
human release gate (G-002). Listening similarity is validated by the G-LISTEN
blind gate and is currently blocked until user-owned redistribution-compatible
hardware reference recordings are supplied.
