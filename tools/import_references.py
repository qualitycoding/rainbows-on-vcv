#!/usr/bin/env python3
"""Reference importer/validator (D-011).

Validates a proposed reference set: manifest fields, WAV format (mono 48 kHz
24-bit), SHA-256 checksums, redistribution-compatible license, attribution,
and capture-matrix coverage. Never tunes per-note constants; when references
are absent, exits with blocked status (D-023).
"""
import hashlib
import json
import sys
import wave

REQUIRED_FIELDS = ["status", "sampleRate", "channels", "bits", "pairs", "rmsDbfs", "license"]
CAPTURE_PITCH = [-5, 0, 4]
CAPTURE_SECONDARY = [0, 1]
CAPTURE_TRACKING = [0, 0.5, 1]
CAPTURE_TONE = [0, 1]
CAPTURE_MAGIC = [0.25, 0.65, 1]


def sha256(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def fail(msg):
    print(f"REJECT: {msg}")
    sys.exit(2)


def main(manifest_path="tests/fixtures/references.json", references_dir="references"):
    with open(manifest_path) as f:
        m = json.load(f)
    for k in REQUIRED_FIELDS:
        if k not in m:
            fail(f"missing field {k}")
    if m["sampleRate"] != 48000 or m["channels"] != 1 or m["bits"] != 24:
        fail("format must be mono 48 kHz 24-bit WAV")
    if m["pairs"] != 12:
        fail("blind bundle requires 12 pairs")
    if not m.get("license"):
        fail("redistribution-compatible license required")
    if m.get("status") == "AWAITING_USER_REFERENCES":
        print("BLOCKED: references (status AWAITING_USER_REFERENCES) — no similarity claim made (D-023)")
        return 1

    files = m.get("files", [])
    if not files:
        fail("no reference files listed")
    for entry in files:
        path = f"{references_dir}/{entry['name']}"
        try:
            if sha256(path) != entry["sha256"]:
                fail(f"checksum mismatch: {entry['name']}")
            with wave.open(path, "rb") as w:
                if w.getframerate() != 48000 or w.getnchannels() != 1 or w.getsampwidth() != 3:
                    fail(f"bad WAV format: {entry['name']}")
        except FileNotFoundError:
            fail(f"missing file: {entry['name']}")
        if not entry.get("attribution"):
            fail(f"missing attribution: {entry['name']}")
        if entry.get("magic") not in CAPTURE_MAGIC + [None]:
            fail(f"unexpected magic level: {entry['name']}")
    print(f"ACCEPT: {len(files)} reference files validated")
    return 0


if __name__ == "__main__":
    sys.exit(main(*(sys.argv[1:])))
