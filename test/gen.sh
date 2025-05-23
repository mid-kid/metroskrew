#!/bin/sh
set -eu

BINS="${BINS:-$(echo ../subprojects/mw-executables-*/)}"
SKREW="${SKREW:-../build/relink/}"
mkdir -p res

versions="
    mwccarm-2.0-72:ds/1.2/base/mwccarm.exe
    mwccarm-2.0-73:ds/1.2/base_b73/mwccarm.exe
    mwccarm-2.0-79:ds/1.2/sp2/mwccarm.exe
    mwccarm-2.0-82:ds/1.2/sp2p3/mwccarm.exe
    mwccarm-2.0-84:ds/1.2/sp3/mwccarm.exe
    mwccarm-2.0-87:ds/1.2/sp4/mwccarm.exe
    mwccarm-3.0-114:ds/2.0/base/mwccarm.exe
    mwccarm-3.0-118:ds/2.0/p2/mwccarm.exe
    mwccarm-3.0-122:ds/2.0/p4/mwccarm.exe
    mwccarm-3.0-123:ds/2.0/sp1/mwccarm.exe
    mwccarm-3.0-126:ds/2.0/sp1p2/mwccarm.exe
    mwccarm-3.0-131:ds/2.0/sp1p5/mwccarm.exe
    mwccarm-3.0-133:ds/2.0/sp1p6/mwccarm.exe
    mwccarm-3.0-134:ds/2.0/sp1p7/mwccarm.exe
    mwccarm-3.0-136:ds/2.0/sp2/mwccarm.exe
    mwccarm-3.0-137:ds/2.0/sp2p2/mwccarm.exe
    mwccarm-3.0-138:ds/2.0/sp2p3/mwccarm.exe
    mwccarm-3.0-139:ds/2.0/sp2p4/mwccarm.exe
    mwccarm-4.0-1018:dsi/1.1/mwccarm.exe
    mwccarm-4.0-1024:dsi/1.1p1/mwccarm.exe
    mwccarm-4.0-1026:dsi/1.2/mwccarm.exe
    mwccarm-4.0-1027:dsi/1.2p1/mwccarm.exe
    mwccarm-4.0-1028:dsi/1.2p2/mwccarm.exe
    mwccarm-4.0-1034:dsi/1.3/mwccarm.exe
    mwccarm-4.0-1036:dsi/1.3p1/mwccarm.exe
    mwccarm-4.0-1051:dsi/1.6sp1/mwccarm.exe
"

mwccarm() {
    out="$1"; shift
    for version in $versions; do
        wine "$BINS/${version#*:}" -o \
            "res/${version%%:*}-$out" "$@"
    done
}

mwccarm_skrew() {
    out="$1"; shift
    for version in $versions; do
        "$SKREW/${version%%:*}.exe" -o \
            "res/${version%%:*}-$out" "$@" > /dev/null
    done
}

# basic_c
mwccarm basic_c.o -c basic_c.c

# include
mwccarm include.o -gccinc -c include.c

# include_dir
mwccarm include_dir.o -gccinc -I. -c include_dir.c

# include_sys
MWCIncludes=. mwccarm include_sys.o -gccinc -c include_sys.c

# switch_float_bug
mwccarm switch_float_bug.o -c switch_float_bug.c

# memreuse01_bug
# Can't reliably generate this file with wine, as it depends on UB.
SKREW_HACK01=00000000 mwccarm_skrew memreuse01_bug_00.o -c memreuse01_bug.c
SKREW_HACK01=ffffffff mwccarm_skrew memreuse01_bug_ff.o -c memreuse01_bug.c
