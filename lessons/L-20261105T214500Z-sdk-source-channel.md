---
id: L-20261105T214500Z-sdk-source-channel
title: Verify Rack SDK channel instead of assuming repository contents
status: active
supersedes: []
recurrence_of: null
occurrences: 1
severity: Low
tags: [tech:vcv-rack, phase:research, kind:tooling]
recorded_by: planner
run: gen-20261105T212027Z-rainbow-machine-vcv
recorded_at: 2026-11-05T21:45:00Z
---
## Trigger
A versioned Rack SDK source clone has no HEAD or headers.
## What went wrong
The assumed source-repository channel did not provide the needed SDK files.
## Correction
Switched to the official versioned SDK ZIP from the VCV downloads page.
## Prevention rule
Verify that the selected SDK channel contains `plugin.mk` and API headers before recording a setup command.
## Detection check
The extracted archive has `Rack-SDK/plugin.mk` and `Rack-SDK/include/engine/Module.hpp`.
## Evidence
Official download URL is recorded in research/SOURCES.md as VCV-DOWNLOAD.
