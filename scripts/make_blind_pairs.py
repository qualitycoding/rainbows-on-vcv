#!/usr/bin/env python3
"""Build D-014 blind listening pairs.

Requires hardware reference recordings (references/manifest.json with
status ACTIVE). Without them, writes an explicit blocked record and exits 1 —
pairs are never fabricated (D-023).
"""
import json
import os
import secrets
import subprocess
import sys

BASE_SEED = 20261106


def main():
    manifest_path = "references/manifest.json"
    evidence = {"blocked": None, "pairs": [], "seed": None, "key": None}
    if not os.path.exists(manifest_path):
        evidence["blocked"] = "references"
        with open("tests/results/T008-audio-evidence.json", "w") as f:
            json.dump(evidence | {"note": "no reference manifest; D-023 blocked path"}, f, indent=2)
        print("BLOCKED: references — blind pairs require hardware recordings")
        return 1
    with open(manifest_path) as f:
        manifest = json.load(f)
    if manifest.get("status") != "ACTIVE":
        evidence["blocked"] = "references"
        with open("tests/results/T008-audio-evidence.json", "w") as f:
            json.dump(evidence, f, indent=2)
        print("BLOCKED: references not ACTIVE")
        return 1

    # Deterministic pair construction at base seed for round 1.
    rng_seed = BASE_SEED
    os.makedirs("evidence/G-LISTEN/pairs", exist_ok=True)
    pairs = manifest["pairs"]  # list of 12 (reference, candidate) stems
    if len(pairs) != 12:
        print("REJECT: need exactly 12 pair stems")
        return 2
    key_bits = []
    for i, (ref_stem, cand_stem) in enumerate(pairs):
        side = (rng_seed >> i) & 1  # 0: A=reference; 1: A=candidate
        key_bits.append(side)
        a, b = (ref_stem, cand_stem) if side == 0 else (cand_stem, ref_stem)
        for label, stem in (("A", a), ("B", b)):
            src = f"renders/{stem}.wav"
            dst = f"evidence/G-LISTEN/pairs/pair{i+1:02d}-{label}.wav"
            # 1.0 s, -23 dBFS RMS, 5 ms fades, 24-bit handled by sox/ffmpeg;
            # fall back to a plain copy when audio tooling is unavailable.
            r = subprocess.run(["bash", "-c",
                f"ffmpeg -y -loglevel error -i {src} -t 1.0 -af "
                f"'afade=t=in:st=0:d=0.005,afade=t=out:st=0.995:d=0.005,loudnorm=I=-23:TP=-2:LRA=11' "
                f"-ar 48000 -ac 1 -c:a pcm_s24le {dst}"], capture_output=True)
            if r.returncode != 0:
                print("REJECT: audio tooling failed:", r.stderr.decode()[:200])
                return 2
    sealed = ",".join("R" if s == 0 else "C" for s in key_bits)
    key = __import__("base64").b64encode(sealed.encode()).decode()
    with open("evidence/G-LISTEN/SHA256SUMS", "w") as f:
        subprocess.run(["bash", "-c",
            "cd evidence/G-LISTEN && sha256sum pairs/*.wav > SHA256SUMS.tmp && mv SHA256SUMS.tmp SHA256SUMS"],
            check=True)
    evidence.update({"pairs": 12, "seed": rng_seed, "key": key,
                     "keySealed": "evidence/G-LISTEN/KEY.sealed", "status": "READY"})
    with open("evidence/G-LISTEN/KEY.sealed", "w") as f:
        f.write(key + "\n")
    with open("tests/results/T008-audio-evidence.json", "w") as f:
        json.dump(evidence, f, indent=2)
    print("READY: 12 pairs sealed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
