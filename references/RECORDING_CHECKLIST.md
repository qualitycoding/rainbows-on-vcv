# Recording Checklist — G-LISTEN Reference Capture

Purpose: recordings of a Rainbow Machine V2 (hardware) to calibrate and blind-test
the clean-room VCV plugin. One fixed re-amping chain for every clip: play the dry
file → pedal (settings below) → recorder. Align pedal output gain once using the
dry reference; do not adjust gain between clips.

## Output format for every clip

- Mono WAV, 48 kHz, 24-bit
- A few seconds of tail after the source ends (the sines/chord are 3 s; let it ring)
- Name clips exactly as in the tables: `NN-slug.wav`
- Record one line per clip in `references/manifest-user.json` (template below):
  file, knobs, source attribution, redistribution permission

## Fixed knob values unless the table says otherwise

- Primary = 75%, Magic = 0 (off), Magic button OFF, Active/bypass switch ON (effect engaged)
- Pitch knob: **-5 st → 0%**, **0 st (unison) → 50%**, **+4 st → 100%** (detented knob)

## Part A — core grid (36 clips, re-amp `sine-440hz.wav`)

Pitch {−5, 0, +4} × Secondary {0, 1} × Tracking {0, 0.5, 1} × Tone {0, 1}

| Clip | Pitch | Secondary | Tracking | Tone | Clip | Pitch | Secondary | Tracking | Tone |
|---|---|---|---|---|---|---|---|---|---|
| 01 | −5 | 0 | 0 | 0 | 19 | 0 | 0 | 0 | 0 |
| 02 | −5 | 0 | 0 | 1 | 20 | 0 | 0 | 0 | 1 |
| 03 | −5 | 0 | 0.5 | 0 | 21 | 0 | 0 | 0.5 | 0 |
| 04 | −5 | 0 | 0.5 | 1 | 22 | 0 | 0 | 0.5 | 1 |
| 05 | −5 | 0 | 1 | 0 | 23 | 0 | 0 | 1 | 0 |
| 06 | −5 | 0 | 1 | 1 | 24 | 0 | 0 | 1 | 1 |
| 07 | −5 | 1 | 0 | 0 | 25 | 0 | 1 | 0 | 0 |
| 08 | −5 | 1 | 0 | 1 | 26 | 0 | 1 | 0 | 1 |
| 09 | −5 | 1 | 0.5 | 0 | 27 | 0 | 1 | 0.5 | 0 |
| 10 | −5 | 1 | 0.5 | 1 | 28 | 0 | 1 | 0.5 | 1 |
| 11 | −5 | 1 | 1 | 0 | 29 | 0 | 1 | 1 | 0 |
| 12 | −5 | 1 | 1 | 1 | 30 | 0 | 1 | 1 | 1 |
| 13 | +4 | 0 | 0 | 0 | 31 | +4 | 1 | 0 | 0* |
| 14 | +4 | 0 | 0 | 1 | 32 | +4 | 1 | 0 | 1 |
| 15 | +4 | 0 | 0.5 | 0 | 33 | +4 | 1 | 0.5 | 0 |
| 16 | +4 | 0 | 0.5 | 1 | 34 | +4 | 1 | 0.5 | 1 |
| 17 | +4 | 0 | 1 | 0 | 35 | +4 | 1 | 1 | 0 |
| 18 | +4 | 0 | 1 | 1 | 36 | +4 | 1 | 1 | 1 |

\* numbering continues 13–18 then 31–36; record all 36.

## Part B — other sources through a small reference set (12 clips)

Pitch 0 / Secondary 1 / Tracking 0.5 / Tone 1 for all; only the source changes:

| Clip | Source dry file | Slug |
|---|---|---|
| 37 | sine-110hz.wav | src-110 |
| 38 | sine-220hz.wav | src-220 |
| 39 | sine-880hz.wav | src-880 |
| 40 | eminor-chord.wav | src-chord |
| 41 | picked-riff.wav | src-riff |
| 42 | noise-burst.wav | src-burst |
| 43 | eminor-chord.wav, Pitch −5, Secondary 0 | chord-down |
| 44 | eminor-chord.wav, Pitch +4, Secondary 1 | chord-up |
| 45 | picked-riff.wav, Tracking 0 | riff-track0 |
| 46 | picked-riff.wav, Tracking 1 | riff-track1 |
| 47 | eminor-chord.wav, Tone 0 | chord-tone0 |
| 48 | noise-burst.wav, Secondary 1 | burst-sec |

## Part C — Magic, gestures, bypass (7 clips)

| Clip | Source | Settings / action |
|---|---|---|
| 49 | picked-riff.wav | Magic knob 25%, engage; let it regenerate; disengage at ~4 s |
| 50 | picked-riff.wav | Magic knob 65%, engage, disengage at ~4 s |
| 51 | picked-riff.wav | Magic knob 100%, engage, disengage at ~4 s, record recovery tail |
| 52 | eminor-chord.wav | Magic 100%, start disengaged; sweep knob upward over 3 s (ascending) |
| 53 | eminor-chord.wav | Magic 100%, start engaged; sweep knob downward over 3 s (descending) |
| 54 | picked-riff.wav | Bypass off→on→off at 2 s and 4 s (effect toggling, not power) |
| 55 | picked-riff.wav | Magic engaged momentarily (hold <300 ms) twice, then latched |

## Manifest template (per clip)

```json
{"file": "01-pitch-5-sec0-trk0-tone0.wav", "source": "sine-440hz.wav",
 "knobs": {"pitch": -5, "secondary": 0, "tracking": 0, "tone": 0, "primary": 0.75, "magic": 0},
 "attribution": "<your name/handle, instrument, date>",
 "permission": "I own this recording and grant redistribution for blind listening and calibration"}
```

## Minimum viable set (if 55 clips is too many)

Clips 01–06, 19–24, 31–36 (the s=−5/0/+4 extremes on sine-440), 40, 41, 51, 54, 55
— 26 clips. The 12 blind pairs will be built from whatever arrives.

## What happens after you send them

1. `tools/import_references.py` validates format, checksums, attribution, permission.
2. Global gain/lag fitted once from the dry reference; only general Tracking,
   modulation, feedback and saturation mechanisms calibrated — never per-note
   constants (D-011).
3. Metrics regenerate; 12 blind pairs are built at base seed 20261106; the
   G-LISTEN gate questions (plan/GATES.md) are put to you.
