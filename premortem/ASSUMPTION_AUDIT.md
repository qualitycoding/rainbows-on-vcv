# Assumption and Premise Audit

Method: attempted to falsify each intake assumption, decision premise, single-source/inferred claim and frozen-test premise against user confirmation, official sources, SDK inspection and planning spikes. No subagent was available; this is a disclosed best-available separated audit.

| Item | Verdict | Evidence checked | Result |
|---|---|---|---|
| A-001/A-002/A-003/A-004/A-005/A-007/A-008/A-009/A-010 | confirmed | Explicit user “Use defaults except license which is apache v2”; intake record | Fixed scope, platforms, tests, Apache-2.0, no release, stores |
| A-006 | unverifiable | User accepted future listener/reference role, but recordings do not yet exist | R-001/R-008; S-LISTEN blocks without files |
| A-011 | unverifiable | Public documentation exposes behavior, not proprietary topology | R-001/R-002; clean-room similarity only, decided by captures/listening |
| D-001/D-012 identity and license premises | confirmed | VCV Plugin Licensing and Manifest | Original identity and VCV exception required |
| D-002 pitch endpoints | confirmed | Official V2 page and official guide corroborate −5/+4 st | T001 |
| D-003 dual delay mechanism | confirmed | MathWorks algorithm and DAFx feedback-pitch paper | Suitable implementation choice, not hardware-source claim |
| D-004/D-005/D-006/D-009 exact curves | unverifiable | Initial maps are engineering defaults; no hardware measurements yet | Calibration and G-LISTEN; no claims of exactness |
| D-007/D-008 Rack/Flexi wrapper | confirmed | SDK 2.6.6 headers plus official V2 Flexi description; 300 ms UI threshold is declared design | T011/T015–T017 |
| D-010 numeric thresholds | confirmed | Exact formula tolerance and deterministic block invariants; 10-cent criterion is a declared acceptance target | Gates CI; amendment required if test itself invalid |
| D-011 reference format | confirmed | User default and listening skill requirements | Importer validates provenance |
| D-013 SDK/runners | confirmed | Official 2.6.6 archive downloaded, hash/header/plugin.mk checked; GitHub runner docs | Four jobs mapped |
| D-014 listening criterion | confirmed | Skill plus prior reversed-discrimination record | Qualitative veto added |
| D-015 mutation opt-out | confirmed | Small 28-test suite and separate native/listening gates | Revisit at 5,000 lines |
| C-004/C-006 single-source claims | confirmed as official V2 declarations | Current official V2 product page | R-002 remains for audible details |
| C-010 single-source license claim | confirmed | VCV official licensing is authoritative for its own policy | T021/G-002 |

Verdict totals: **20 confirmed, 3 unverifiable, 0 falsified** across 23 grouped premises. Unverifiable premises map to R-001/R-002/R-004/R-008 and explicit capture/listening gates. No falsified item requires L/K amendment.
