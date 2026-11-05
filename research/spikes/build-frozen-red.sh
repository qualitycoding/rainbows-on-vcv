#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -Isrc tests/frozen_tests.cpp -o /tmp/rainbows-frozen-red
/tmp/rainbows-frozen-red > research/spikes/frozen-red.txt
