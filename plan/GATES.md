# Human Gates

## G-LISTEN — Hardware similarity and demos

- **Trigger:** S-LISTEN, after frozen metrics, CI, traditional review and adversarial review pass.
- **Evidence:** `GATE-G-LISTEN.md`; CI green commit; metric table; `evidence/G-LISTEN` orphan branch ZIP URL `https://github.com/qualitycoding/rainbows-on-vcv/archive/refs/heads/evidence/G-LISTEN.zip`; raw per-file URLs; 12 start-window pairs; sealed key; all D-014 demos; `SHA256SUMS`; attribution/licenses; seed and round log.
- **Questions:** (1) For each pair, which side is the hardware reference, letters in order? (2) Do the demos produce the intended Rainbow-Machine-like effects; which sound off and how? (3) Are unison chorus, direction-sensitive Secondary, Tracking, ascending/descending Magic, maximum-Magic recovery and momentary/latching behavior convincing? (4) Did any passing pair still reveal an obvious difference or reversed preference?
- **Allowed responses:** `proceed`; `iterate: <notes>`; `proceed-with-rescope: <text>`; `stop`.
- **Branches:** `proceed` requires ≤8/12 and no qualitative veto, then package. `iterate` follows D-014, at most 3 rounds. `proceed-with-rescope` records accepted limitation in README and risk register, then package. `stop` ends feature work. After round 3 only rescope or stop. Missing recordings produce a blocked gate, never fabricated scores.
- **Phone instructions:** Play pairs in order on the listener's own headphones, A then B, once each; do not inspect filenames/key before answering.

## G-002 — External release (defined, not scheduled)

- **Trigger:** Only if a human later adds a public GitHub release, VCV Library submission, commercial license, or other publication step. CI artifacts and the required evidence branch are not public releases under this plan.
- **Evidence:** Four package checksums, source diff, Apache/VCV exception notices, original-identity review, CI/listening records, version/changelog, proposed destination, and any EarthQuaker Devices permission if protected identity is used.
- **Questions:** Authorize the exact release and destination? Allowed: `release`, `release-with-rescope: <text>`, `stop`.
- **Branches:** Only `release` permits the specified external action; any other response leaves packages unreleased. VCV Library submission remains forbidden if C-011 requirements are unmet.

## G-003 — Lessons and knowledge stores

- **Trigger:** S-KNOW after S-RETRO; implementation is otherwise complete.
- **Evidence:** `GATE-G-003.md` lists lesson/knowledge IDs, titles and tags; proposed targets `qualitycoding/Lessons` and `qualitycoding/knowledge`.
- **Questions:** Confirm store destinations. Allowed: `push-to-proposed`; `push-to: <lesson-target> [knowledge-target]`; `do-not-push`.
- **Branches:** On approval push each store separately and regenerate indexes. On rejection sync and retry at most 3 times per store. If unavailable/no response, create `lessons-<run_id>.bundle` and/or `knowledge-<run_id>.bundle` and report paths. `do-not-push` completes without store pushes.

While waiting, only independent steps may continue. No external release may occur without G-002.
