---
id: L-20261105T213000Z-mount-credential-helper-exec
title: Credential helpers on the durable mount cannot be executed
status: active
supersedes: []
recurrence_of: null
occurrences: 2
severity: Medium
tags: [tech:github, phase:research, kind:environment]
recorded_by: planner
run: gen-20261105T212027Z-rainbow-machine-vcv
recorded_at: 2026-11-05T21:30:00Z
---
## Trigger
A Git askpass or push helper is stored on `/mnt/agents` and invoked directly.
## What went wrong
The mount rejected execution despite chmod, so authenticated clone/push failed.
## Correction
Copied the helper to local `/tmp`, restored its executable bit, and invoked scripts through Bash.
## Prevention rule
Stage every executable helper onto executable local storage and run a trivial invocation before using it for Git.
## Detection check
The credential helper responds to a username prompt and `git ls-remote` succeeds without exposing the token.
## Evidence
Initial push later verified branch head 4295fee9a609ef4e18c42d68636def133cfa60e0. No secret recorded.
