---
id: L-20261106T123000Z-cold-read-explicit-references
title: Cold reads require explicit identifiers and blocked-path reachability
status: active
supersedes: []
recurrence_of: null
occurrences: 1
severity: High
tags: [tech:github-actions, phase:verification, kind:verification-gap]
recorded_by: planner
run: gen-20261105T212027Z-rainbow-machine-vcv
recorded_at: 2026-11-06T12:30:00Z
---
## Trigger
A plan uses shorthand test ranges, prose-only failure rule names, or a gate downstream of a step that cannot finish without gate evidence.
## What went wrong
Mechanical cold reading found unmapped test IDs and an unreachable missing-reference listening path; command review also found an unverified custom Make target.
## Correction
Numbered all decision rules, mapped every test individually, allowed an explicit blocked evidence bundle to reach G-LISTEN, and added/executed the frozen Make target.
## Prevention rule
Before freezing a plan, mechanically resolve every referenced ID and simulate every blocked path through its defined gate.
## Detection check
Run `python3 research/spikes/check_plan.py`; it must report zero items and the missing-reference branch must reach S-LISTEN.
## Evidence
`research/spikes/cold-read-mechanical.txt` and `reviews/COLD_READ.md`.
