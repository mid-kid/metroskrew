#!/bin/sh
set -eu

SRC="${SRC:-.}"
VER="${VER:-}"

wine() {
    test -n "${MESON_EXE_WRAPPER:-}" && set -- "$MESON_EXE_WRAPPER" "$@"
    "$@"
}

mwccarm() {
    wine "$MWCCARM" "$@"
}
mwasmarm() {
    wine "$MWASMARM" "$@"
}
mwldarm() {
    wine "$MWLDARM" "$@"
}

test_cc() {
    name="$1"; shift
    mwccarm "$@" -c -o "$MWCCARM_VER-$name.o" "$SRC/$name.c"
    cmp "$MWCCARM_VER-$name.o" "$SRC/res/$MWCCARM_VER-$name.o"
}
test_as() {
    name="$1"; shift
    mwasmarm "$@" -c -o "$MWASMARM_VER-$name.o" "$SRC/$name.s"
}

case "$1" in
    basic_c) test_cc "$1" ;;
    include) test_cc "$1" -gccinc ;;
    include_dir) test_cc "$1" -gccinc -I"$SRC" ;;
    include_sys) MWCIncludes="$SRC" test_cc "$1" -gccinc ;;
    switch_float_bug) test_cc "$1" ;;
    basic_s) test_as "$1" ;;
    *) exit 1 ;;
esac
