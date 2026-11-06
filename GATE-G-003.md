# GATE G-003 — Lessons & Knowledge Stores

**Status: APPROVED (`push-to-proposed`) and completed.**

## Proposed payload

Lessons on this branch (`lessons/`):
- L-20261105T213000Z-mount-credential-helper-exec (carried)
- L-20261105T214500Z-sdk-source-channel (carried)
- L-20261106T120000Z-red-runner-green-path (carried)
- L-20261106T123000Z-cold-read-explicit-references (carried)
- L-20261106T063000Z-zstd-cli-shim (new this run)

Knowledge on this branch (`knowledge/`):
- K-20261106T120000Z..1213Z-c-001..c-014 (planning, carried)
- K-20261106T063000Z-sdk-block-api (new)
- K-20261106T063001Z-helper-createplugin-empty-dir (new)

## Questions

Confirm store destinations. Allowed responses:
`push-to-proposed` (targets `qualitycoding/Lessons` and `qualitycoding/knowledge`),
`push-to: <lesson-target> [knowledge-target]`, or `do-not-push`.

On approval each store is pushed separately with regenerated indexes. If
rejected/unavailable/no response, `lessons-impl-20261106T063000Z.bundle` and
`knowledge-impl-20261106T063000Z.bundle` will be created and their paths
reported instead.

## Outcome (recorded on completion)

- Response recorded in `GATE-G-003.RESPONSE.md`: `push-to-proposed`.
- `qualitycoding/Lessons`: 5 lessons pushed; remote `main` verified at
  `6fba6ce` (local HEAD); `lessons/INDEX.md` regenerated (312 entries);
  `phase:build` added to `TAXONOMY.md`; schema/secret scan: all entries valid.
- `qualitycoding/knowledge`: 16 items pushed (14 carried research claims under
  `subject:rainbow-machine-vcv`, 2 new SDK facts under `subject:vcv-rack`);
  remote `main` verified at `a00c332` (local HEAD); `knowledge/INDEX.md`
  regenerated (470 entries); two new subjects added to `TAXONOMY.md`;
  schema/secret scan: all entries valid.
