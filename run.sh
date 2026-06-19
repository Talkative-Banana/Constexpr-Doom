#!/usr/bin/env bash
# run.sh — build the interpreter, capture its (constexpr-error) output
# to dump.txt, then extract the ASCII frame into frame.txt.
#
# Usage:
#   ./run.sh [SCREENWIDTH]
#
# If SCREENWIDTH is omitted, display.py's default (320) is used.
# Make sure this matches whatever SCREENWIDTH you set in types.hpp.

set -uo pipefail

WIDTH="${1:-320}"

echo "[run.sh] make clean && make (capturing to dump.txt)..."
make clean > /dev/null 2>&1

# Capture both stdout and stderr — the ShowChars dump comes through
# as a compiler error on stderr, so we need both streams merged.
make > dump.txt 2>&1
MAKE_STATUS=$?

if [ $MAKE_STATUS -eq 0 ]; then
    echo "[run.sh] WARNING: make succeeded with no errors — no ShowChars dump expected."
    echo "[run.sh] If you intended to trigger a ShowValue/ShowChars compile error,"
    echo "[run.sh] check that the debug halt is still in place."
else
    echo "[run.sh] make exited non-zero (expected, since we're using the ShowChars trick)."
fi

echo "[run.sh] dump.txt written ($(wc -l < dump.txt) lines, $(wc -c < dump.txt) bytes)."

echo "[run.sh] running display.py dump.txt frame.txt ${WIDTH}..."
python3 display.py dump.txt frame.txt "${WIDTH}"

echo "[run.sh] done. View with: cat frame.txt"