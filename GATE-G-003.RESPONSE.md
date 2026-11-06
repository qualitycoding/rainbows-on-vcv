# GATE G-003 — Response

- Response: `push-to-proposed`
- Targets: `qualitycoding/Lessons` (lessons), `qualitycoding/knowledge` (knowledge)
- Payload: 5 lessons (L-20261105T213000Z, L-20261105T214500Z, L-20261106T120000Z,
  L-20261106T123000Z, L-20261106T063000Z) and 16 knowledge items
  (K-20261106T120000Z..1213Z-c-001..c-014, K-20261106T063000Z-sdk-block-api,
  K-20261106T063001Z-helper-createplugin-empty-dir).
- Procedure: copy each store separately, regenerate indexes, schema/secret
  scan, push each store separately, verify remote heads.
