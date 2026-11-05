# Traceability

| Requirement | Evidence/tests | Producing steps |
|---|---|---|
| A-001 clean-room V2 clone | C-001…C-007; T001–T014; G-LISTEN | S-002–S-006, S-LISTEN |
| A-002 mono Rack controls | C-008/C-009; T015/T016 | S-005 |
| A-003 four desktop platforms | C-012/C-013; T024 | S-001, S-007, S-PACKAGE |
| A-004 pitch/Secondary/Tracking/Tone/Magic/bypass/rates | T001–T016/T018 | S-002–S-005 |
| A-005 20×/memory/denormals | T022/T023/T027 | S-007 |
| A-006 references/blind listening/3 rounds | T025/T028; G-LISTEN | S-006, S-008, S-LISTEN |
| A-007 Apache-2.0/original identity | C-010/C-011; T021 | S-001, S-REVIEW |
| A-008 CI packages/no release | T024; G-002 defined but unscheduled | S-007, S-PACKAGE |
| A-009 security/malformed state/no services | T017/T019–T022/T026 | S-005, S-007, S-ADVERSARIAL |
| A-010 persistent stores | G-003 | S-KNOW |
| A-011 no proprietary/bit-identical claim | C-001…C-007; G-LISTEN | S-006, S-LISTEN |

## Platform map

| Intake platform | CI job | Runner | Waiver |
|---|---|---|---|
| Windows x64 | build-win-x64 | windows-2025 | none |
| Linux x64 | build-lin-x64, sanitize, performance | ubuntu-24.04 | none |
| macOS x64 | build-mac-x64 | macos-15-intel | none |
| macOS ARM64 | build-mac-arm64 | macos-15 | none |

Linux ARM64 is outside intake. Alpine and oldest-toolchain waivers: D-013.

## Test categories

- Unit: T001–T008 (8).
- Integration: T009–T016, T024–T025 (10).
- Operational: T017–T018, T027–T028 (4).
- Security: T019–T022, T026 (5).
- Performance: T023 (1).
- Total: 28. Every numeric threshold is asserted by its producing step and fails CI when missed.

## Individual test map

T001→S-003; T002→S-003; T003→S-003; T004→S-003; T005→S-003; T006→S-004; T007→S-004; T008→S-003; T009→S-002/S-003; T010→S-002/S-003; T011→S-003/S-005; T012→S-003/S-004; T013→S-003/S-004; T014→S-002/S-004; T015→S-005; T016→S-004/S-005; T017→S-005; T018→S-002/S-003; T019→S-002/S-005; T020→S-002/S-004; T021→S-001/S-005; T022→S-002/S-007; T023→S-007; T024→S-001/S-007; T025→S-006/S-008/S-LISTEN; T026→S-007; T027→S-004/S-007; T028→S-008.

## Claim and decision coverage

C-001…C-014 and D-001…D-025 map to the tests and steps above. A-001…A-011 all have evidence. No N/A math, computational, publication, courseware, deployment, figure, or manuscript artifact is required.
