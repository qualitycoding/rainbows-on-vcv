# Implementation Handoff

## Purpose and reading order

Build an original-identity, clean-room Rainbow-Machine-V2-inspired mono VCV Rack 2 effect under Apache-2.0. Active profile: `software`; `audio=true`; `software.deploys=false`. Do not implement while on the generation branch.

Read: `plan/PROFILE.md` → `plan/ASSUMPTIONS.md` → `plan/DECISIONS.md` → `plan/ENVIRONMENT.md` → `plan/GATES.md` → `plan/PLAN.md` → `plan/TRACEABILITY.md` → `research/PRIOR_KNOWLEDGE.md`.

## Start, tests and freeze

Start at **S-000**, create `impl-<run_id>`, and execute the pinned setup in `plan/ENVIRONMENT.md`. Use durable `/mnt/agents` for caches/checkpoints and executable local storage for tools. Run `make -f tests/Makefile test` for the frozen suite and `sha256sum -c tests/FROZEN_MANIFEST.sha256` before and after every step. Planning red results are in `research/spikes/frozen-red.txt`; implementation must turn all 28 tests green without editing them.

Steps: S-000 → S-001 → S-002 → S-003 → S-004 → S-005 → S-006/S-007 → S-008 → S-REVIEW → S-ADVERSARIAL → S-LISTEN → S-PACKAGE → S-RETRO → S-KNOW. Independent steps may run concurrently when dependencies and exclusive resources permit, including while halted at a gate.

Human gates: G-LISTEN (sound/reference approval), G-002 only if an external release is later requested, and mandatory G-003 (store destinations). G-LISTEN needs the user's redistribution-compatible hardware recordings and answers. Missing evidence means blocked, never an invented pass.

For an invalid frozen test, stop and write `TEST_CHALLENGE.md`; use the Phase 2 amendment and refreeze procedure. For any other halt write `BLOCKED.md`. Record every substitution immediately in `DEVIATIONS.md`; append every attempt to `EXECUTION_LOG.md`; commit and push each green chunk and verify remote HEAD. Default to the most reversible non-scope-expanding option unless frozen tests, security, data integrity, public interfaces, research integrity, or release are touched.

## Required global rules

**Rule 6 — Everything Is Committed:** No finding, decision, proof, figure spec, or artifact exists unless it is committed to the generation branch. **Push cadence:** after every successful chunk — a plan step turning green, a frozen-test amendment round closed, a review or gate record committed — immediately push the working branch to the remote, then verify with `ls-remote` that the remote matches local HEAD. Retry transient failures with backoff (on flaky networks prefer HTTP/1.1 with a raised `http.postBuffer`); report any remote/local mismatch instead of assuming the push landed. On ephemeral platforms this is the primary defense against losing work; on stable platforms it is cheap insurance and keeps collaborators and CI current.

**Rule 11 — Lessons:** Whenever the executing agent corrects its own execution, it records a lesson before continuing. A correction includes redoing/reverting work, a failed command/assumption/edit fixed by another approach, a check catching the agent's mistake, or human correction. Retrying the same transient failure and planned red-against-stubs are not corrections. Record `lessons/L-<YYYYMMDDTHHMMSSZ>-<slug>.md` using Appendix A before continuing, with trigger, mistake, correction, one imperative prevention rule, detection check, severity and taxonomy tags. Name recurrence and increment repeat occurrences. Lessons never contain secrets or personal data.

**Rule 12 — Knowledge Base:** All supporting research and derived knowledge is recorded as knowledge items, so later work can reuse it instead of researching it again. Whenever either agent discovers a new fact about the task's Subject (its technologies, formats, APIs, platforms, tools, mathematical objects, or venues), it adds an item before continuing. Use `knowledge/K-<timestamp>-<slug>.md` and Appendix B; record source/locator/version/confidence/claim and at least one `subject:` tag. Never edit away a wrong outdated fact: add a superseding item.

**Rule 13 — Lesson & Knowledge Stores:** Lessons and knowledge items persist across tasks in separate stores recorded at intake (`A-010`). During implementation both accumulate on the working branch. Read stores at research start and write only after G-003. Lessons ultimately go to `qualitycoding/Lessons`; knowledge ultimately goes to `qualitycoding/knowledge`; push separately with regenerated indexes. Never include credentials, tokens, secrets or personal data.

**Rule 9 — Integrity:** No step may fabricate, cherry-pick without disclosure, or manually alter data, test results, benchmarks, or figures.

## Audio and delivery

D-014 fixes the 12-pair blind protocol, demos, qualitative veto and three-round maximum. CI creates the orphan evidence branch only from `[gate-evidence G-LISTEN]`. Keep answer keys sealed until the listener responds. The package step produces four CI artifacts and no release. No deployment runbook applies (`software.deploys=false`).

Execution ends only after S-RETRO and S-KNOW's gate/fallback. Store pushes also include the planning lessons and knowledge carried on this generation branch.
