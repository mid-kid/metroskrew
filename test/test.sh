#!/bin/sh
set -eu

SRC="${SRC:-.}"

wine() {
    test -n "${MESON_EXE_WRAPPER:-}" && set -- "$MESON_EXE_WRAPPER" "$@"
    "$@"
}

mwccarm() {
    wine "$MWCCARM" "$@"
}
mwldarm() {
    wine "$MWLDARM" "$@"
}
mwasmarm() {
    wine "$MWASMARM" "$@"
}

unset variant
test_cc() {
    name="$1"; shift
    oname="${name}${variant:+_}${variant:-}"
    mwccarm "$@" -c -o "$MWCCARM_VER-$oname.o" "$SRC/$name.c"
    cmp "$MWCCARM_VER-$oname.o" "$SRC/res/$MWCCARM_VER-$oname.o"
}
test_ld() {
    name="$1"; shift
    mwldarm "$@" -nostdlib -o "$MWLDARM_VER-$name.elf"
}
test_as() {
    name="$1"; shift
    mwasmarm "$@" -c -o "$MWASMARM_VER-$name.o" "$SRC/$name.s"
}

case "$1" in
    basic_c) test_cc "$1" ;;
    include) test_cc "$1" -gccinc ;;
    include_dir) test_cc "$1" -gccinc -I"$SRC" ;;
    include_sys) export MWCIncludes="$SRC"; test_cc "$1" -gccinc ;;
    switch_float_bug) test_cc "$1" ;;
    memreuse01_bug)
        SKREW_HACK01=00000000 variant=00 test_cc "$1"
        SKREW_HACK01=ffffffff variant=ff test_cc "$1"
        ;;
    basic_ld) test_ld "$1" "$SRC/link.lcf" \
        "$SRC/res/mwccarm-4.0-1051-basic_c.o" ;;
    basic_s) test_as "$1" ;;
    *) exit 1 ;;
esac
