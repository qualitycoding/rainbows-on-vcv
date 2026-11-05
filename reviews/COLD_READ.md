# Cold Read

## Semantic dry run — pass 2

Reading only branch artifacts, an implementing agent can determine the goal, original-identity constraint, exact DSP/public interfaces, control formulas, calibration unknowns, reference format, CI matrix, performance budget, gates, failure branches, commands, output paths and closing protocol. No human question is required outside G-LISTEN/G-002-if-requested/G-003.

Pass 1 found missing numbered decision IDs, range-only test traceability, a missing-reference path that could stall before its gate, an unverified custom test target, and a red runner without a green exit. These were corrected in the branch and recorded as lessons where Rule 11 requires. Pass 2 found zero semantic items.

## Mechanical verification

`python3 research/spikes/check_plan.py` returns `PASS: zero mechanical cold-read items`. It checks required artifacts, steps/gates, all C/D/T IDs, runner labels, freeze hashes, forbidden failure swallowing and L/K required fields/secrets.

## Limitation

The protocol-requested fresh Sonnet/Haiku subagents are unavailable. This is a best-available separated planner review plus executable mechanical check, not a claim of independent-agent review. R-010 carries the residual Medium risk; S-REVIEW and S-ADVERSARIAL still mandate fresh implementation reviewers.
