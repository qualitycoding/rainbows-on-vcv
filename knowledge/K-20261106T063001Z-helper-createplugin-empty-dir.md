---
id: K-20261106T063001Z-helper-createplugin-createplugin-empty-dir
source: Rack-SDK-2.6.6 helper.py createplugin command
locator: helper.py CLI; observed behavior
version: Rack-SDK-2.6.6
confidence: verified
subject: tech:vcv-rack
---
## Claim
`python3 $RACK_DIR/helper.py createplugin <slug> [dir]` refuses to initialize
into an existing non-empty directory ("Directory <dir> already exists" /
stalls). To scaffold inside an existing repository, generate into a fresh temp
directory and copy files in, or use the checked-in Makefile/plugin.json path.
