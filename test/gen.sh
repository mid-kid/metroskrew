#!/bin/sh
set -eu

BIN="${BIN:-../bins}"

# switch_float_bug
wine "$BIN/mwccarm.exe" -c -o res/switch_float_bug.o switch_float_bug.c
