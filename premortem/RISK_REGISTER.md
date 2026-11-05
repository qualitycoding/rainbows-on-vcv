# Risk Register

| ID | Severity | Likelihood | Root cause / failure | Mitigation and trace | Residual |
|---|---|---|---|---|---|
| R-001 | High | Medium | Generic pitch shifter does not reproduce hardware character | D-003–D-011; capture matrix; T001–T014; G-LISTEN; max 3 mechanism-led rounds | Medium |
| R-002 | Medium | Medium | V2 Tone/Flexi details have one official source; exact curves proprietary | Official V2 governs; user captures falsify mappings; TEST_CHALLENGE amendment; no proprietary claim | Medium |
| R-003 | High | Medium | Delay wrap/NaN/native crash corrupts Rack audio thread | D-010; T018–T022/T027; ASan/UBSan; pure-C++ harness; adversarial review | Low |
| R-004 | High | Medium | Magic feedback runaway or lifeless after limiting | D-009 global bounded matrix; T006/T007/T020/T027; listening rescope branch | Medium |
| R-005 | High | Low | Apache/VCV/identity noncompliance blocks distribution | D-001/D-012; T021; original name/layout; exception notice; G-002 for release | Low |
| R-006 | High | Low | One platform silently untested or CI failure hidden | D-013; fail-fast false; T024; artifact postconditions; no failure swallowing | Low |
| R-007 | Medium | Medium | Shared CI runner misses 20× or denormals stall | Five-run median plus floor; FTZ/DAZ; D-performance blocks relaxation; T023/T027 | Medium |
| R-008 | Medium | Medium | ≤8/12 masks audible or reversed discrimination | D-014 qualitative veto; remarks verbatim; new seeds; S-LISTEN iteration | Medium |
| R-009 | Medium | Medium | Ephemeral sandbox loses tools or long-run evidence | Durable checkpoints; local staging; push cadence; CI for soak/perf; blocked honesty | Low |
| R-010 | Medium | Medium | No fresh subagent means review blind spots | Mechanical cold-read, adversarial test plan, risk/audit disclosure; implementation still requires fresh reviewers | Medium |
| R-011 | Low | Medium | macOS Intel label retires in August 2027 | D-013 scheduled maintenance; D-CI human amendment rather than silent platform deletion | Low |

Severity uses protocol Rule 4. No unmitigated Critical/High residual remains.
