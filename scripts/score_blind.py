#!/usr/bin/env python3
"""Score a G-LISTEN round after the listener answers (D-014).

Usage: score_blind.py --answers R,R,C,... (12 letters, A-side guesses)
Prints score, binomial p-value, seed, commit; records listening/round-N.md.
"""
import argparse
import base64
import json
import math
import os
import subprocess


def binom_p(k, n, p=0.5):
    return sum(math.comb(n, i) * p**i * (1 - p)**(n - i) for i in range(k, n + 1))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--answers", required=True)
    ap.add_argument("--round", type=int, default=1)
    ap.add_argument("--remarks", default="")
    args = ap.parse_args()

    answers = [a.strip().upper() for a in args.answers.split(",")]
    if len(answers) != 12 or any(a not in ("R", "C") for a in answers):
        raise SystemExit("need 12 answers of R or C")
    with open("evidence/G-LISTEN/KEY.sealed") as f:
        key = base64.b64decode(f.read().strip()).decode().split(",")
    score = sum(1 for a, k in zip(answers, key) if a == k)
    p = binom_p(score, 12)
    commit = subprocess.run(["git", "rev-parse", "HEAD"], capture_output=True, text=True).stdout.strip()
    os.makedirs("listening", exist_ok=True)
    with open(f"listening/round-{args.round}.md", "w") as f:
        f.write(f"# G-LISTEN round {args.round}\n\nscore: {score}/12\np-value: {p:.4f}\n"
                f"seed: see T008-audio-evidence.json\ncommit: {commit}\n\n"
                f"verbatim remarks: {args.remarks}\n")
    verdict = "PROCEED" if score <= 8 else "ITERATE"
    print(json.dumps({"score": score, "pValue": p, "verdict": verdict,
                      "round": args.round, "commit": commit}))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
