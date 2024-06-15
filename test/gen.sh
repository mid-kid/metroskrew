#!/bin/sh
set -eu

BIN="${BIN:-../bins}"

# basic_c
wine "$BIN/mwccarm.exe" -c -o res/basic_c.o basic_c.c

# include
wine "$BIN/mwccarm.exe" -gccinc -c -o res/include.o include.c

# include_dir
wine "$BIN/mwccarm.exe" -gccinc -I. -c -o res/include_dir.o include_dir.c

# include_sys
MWCIncludes=. wine "$BIN/mwccarm.exe" -gccinc -c -o res/include_sys.o include_sys.c

# switch_float_bug
wine "$BIN/mwccarm.exe" -c -o res/switch_float_bug.o switch_float_bug.c
