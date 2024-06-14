#!/bin/sh
set -eu

BIN="${BIN:-../bins}"

# basic_c
wine "$BIN/mwccarm.exe" -c -o res/basic_c.o basic_c.c

# switch_float_bug
wine "$BIN/mwccarm.exe" -c -o res/switch_float_bug.o switch_float_bug.c
