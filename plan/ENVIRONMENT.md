# Environment and Verified Pins

## Pins and existence checks

| Component | Pin | Existence / command verification |
|---|---|---|
| VCV Rack SDK | 2.6.6 | Official download archive lists all four targets; Linux archive SHA-256 `420da2452def7b195f98e3a0650c35f1e0f058a98a71840b4347d3d203618532`; extracted and inspected. |
| C++ | C++17 | `g++ -std=c++17 -Wall -Wextra -Werror -pedantic -Isrc tests/frozen_tests.cpp -o /tmp/rainbows-frozen-red` executed; output in `research/spikes/frozen-red.txt`. |
| Planning compiler | GCC 12.2.0 | `g++ --version` executed. CI uses each SDK runner's declared compiler and prints its version. |
| GNU Make | 4.3 planning baseline | `make --version` executed; `plugin.mk` 2.6.6 defines `dist` and `install`. |
| Python | 3.12.12 | `python3 --version` executed; `python3 $RACK_DIR/helper.py` printed command help. |
| NumPy / SciPy / PyYAML | 2.2.5 / 1.16.2 / 6.0.2 | Imported versions queried in planning environment. |
| SoundFile | 0.13.1 | PyPI pin installation recorded in `research/spikes/python-deps.txt`. |
| pip-audit | 2.9.0 | PyPI pin checked; implementation scans `requirements-audio.lock`. |
| actions/checkout | v4.2.2 | Remote tag lookup recorded in `research/spikes/action-pins.txt`. |
| actions/upload-artifact / download-artifact | v4.6.2 / v4.3.0 | Remote tag lookup recorded in same file. |
| actions/setup-python | v5.6.0 | Remote tag lookup recorded in same file. |
| msys2/setup-msys2 | v2.22.0 | Remote tag lookup recorded in same file. |
| CI runners | ubuntu-24.04; windows-2025; macos-15-intel; macos-15 | GitHub hosted-runner reference and runner-images repository, C-013. No `-latest`. |

## Setup commands

Run from repository root; all commands were executed against these pins or verified in Rack SDK 2.6.6 files. Use executable local storage for binaries and scripts; invoke durable-mount scripts through their interpreter.

```sh
mkdir -p .toolchain
curl -fL https://vcvrack.com/downloads/Rack-SDK-2.6.6-lin-x64.zip -o .toolchain/Rack-SDK.zip
printf '%s  %s\n' '420da2452def7b195f98e3a0650c35f1e0f058a98a71840b4347d3d203618532' '.toolchain/Rack-SDK.zip' | sha256sum -c -
unzip -q -o .toolchain/Rack-SDK.zip -d .toolchain
export RACK_DIR="$PWD/.toolchain/Rack-SDK"
python3 "$RACK_DIR/helper.py"
python3 -m pip install -r requirements-audio.lock
python3 -m pip_audit -r requirements-audio.lock
make -f tests/Makefile test
RACK_DIR="$RACK_DIR" make -j2
RACK_DIR="$RACK_DIR" make dist
sha256sum -c tests/FROZEN_MANIFEST.sha256
```

For Windows use the `Rack-SDK-2.6.6-win-x64.zip` archive in the pinned MSYS2 environment; for each macOS job use its matching `mac-x64` or `mac-arm64` archive. The implementation step must record those archives' SHA-256 values before use and must not substitute an SDK. `make dist` produces `dist/qualitycoding-rainbows-2.0.0-<arch>.vcvplugin`.

`requirements-audio.lock` contains exactly:

```text
numpy==2.2.5
scipy==1.16.2
soundfile==0.13.1
PyYAML==6.0.2
pip-audit==2.9.0
```

## CI matrix artifact

`.github/workflows/build.yml` must have `strategy.fail-fast:false`, one native job per runner above, artifact sanity checks, Ubuntu ASan/UBSan and 20× performance jobs, a dependency scan, and no release job. Performance gate: `ubuntu-24.04` public-runner class (documented 4 vCPU/16 GB at planning), one pinned benchmark thread, 48 kHz, block 256, 60 audio seconds; median of five runs ≥20× and every run ≥18×; peak RSS ≤64 MiB. A miss follows D-performance; no automatic relaxation. Long sanitizers/soak run on `ubuntu-24.04`; no local ephemeral result may be reported in place of CI.

## Command verification locators

- Rack `process`, parameters/ports, `configBypass`, JSON, sample-rate event: Rack SDK 2.6.6 `include/engine/Module.hpp` lines 125–233 and 339–463.
- Mono sum: `include/engine/Port.hpp` `getVoltageSum()` line 99; VCV-VOLTS.
- `RACK_DIR`, `make`, `make dist`, `make install`: `plugin.mk` lines 1–24 and 71–105; VCV-TUTORIAL.
- `helper.py`: `python3 "$RACK_DIR/helper.py"` help output verified.
- Frozen compile/run: `research/spikes/build-frozen-red.sh` and `frozen-red-exit.txt` (28 clean stub failures, expected red only during planning).
- Network retries: five attempts and bounded backoff per D-dependency; final failure is visible. No load-bearing command may use failure-swallowing idioms.
