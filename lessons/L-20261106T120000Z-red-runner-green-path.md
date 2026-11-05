---
id: L-20261106T120000Z-red-runner-green-path
title: Red-verification runner must retain a green exit path
status: active
supersedes: []
recurrence_of: null
occurrences: 1
severity: High
tags: [tech:cpp, phase:verification, kind:verification-gap]
recorded_by: planner
run: gen-20261105T212027Z-rainbow-machine-vcv
recorded_at: 2026-11-06T12:00:00Z
---
## Trigger
A custom test runner is first written to prove that every test is red against stubs.
## What went wrong
The initial return expression made an eventual all-green implementation exit nonzero too.
## Correction
Changed the runner to return zero when the failure count is zero and one otherwise, then reran red verification.
## Prevention rule
Before freezing a test runner, verify both its expected red exit and the logical existence of a zero-exit all-green path.
## Detection check
Inspect the final return expression and run `bash research/spikes/build-frozen-red.sh`; planning stubs must yield 28/28 failing with exit 1, while zero failures maps to exit 0.
## Evidence
`research/spikes/frozen-red-exit.txt` and the frozen runner source. No implementation was added.
