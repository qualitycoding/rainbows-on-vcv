#!/usr/bin/env python3
"""Render demos/*.events deterministically via the engine harness.

Requires tools/render_demos_helper (built from tools/render_demo_file.cpp)
which writes engine-rendered PCM32 WAVs. Also verifies two renders of each
demo produce identical SHA-256 (T028 reproducibility).
"""
import hashlib
import json
import os
import subprocess
import sys


def sha256(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def main():
    os.makedirs("demos", exist_ok=True)
    helper = "/tmp/render_demo_file" if os.path.exists("/tmp/render_demo_file") else "tools/render_demo_file"
    if not os.path.exists(helper):
        print("missing helper binary; build tools/render_demo_file.cpp first")
        return 2
    report = {"demos": [], "reproducible": True}
    for ev in sorted(f for f in os.listdir("demos") if f.endswith(".events")):
        stem = ev[: -len(".events")]
        out = f"demos/{stem}.wav"
        r = subprocess.run([helper, f"demos/{ev}", out, "48000"], capture_output=True, text=True)
        if r.returncode != 0:
            print(f"RENDER FAIL {ev}: {r.stderr[:300]}")
            return 1
        h1 = sha256(out)
        r2 = subprocess.run([helper, f"demos/{ev}", out + ".tmp.wav", "48000"], capture_output=True, text=True)
        h2 = sha256(out + ".tmp.wav")
        os.remove(out + ".tmp.wav")
        same = h1 == h2
        report["reproducible"] = report["reproducible"] and same
        report["demos"].append({"events": ev, "wav": out, "sha256": h1, "identicalSecondRender": same})
        print(f"{ev}: {'OK' if same else 'NONDETERMINISTIC'} {h1[:16]}")
    with open("tests/results/T008-audio-evidence.json", "w") as f:
        json.dump({"blocked": "references", "demos": report["demos"],
                   "reproducible": report["reproducible"],
                   "note": "D-023: no user references; no pairs fabricated"}, f, indent=2)
    return 0 if report["reproducible"] else 1


if __name__ == "__main__":
    sys.exit(main())
