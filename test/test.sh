#!/bin/sh
set -eux

SRC="${SRC:-.}"

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

case "$1" in
    basic_c|switch_float_bug)
        mwccarm -c -o "$1.o" "$SRC/$1.c"
        cmp "$1.o" "$SRC/res/$1.o"
        ;;

    basic_s)
        mwasmarm -c -o "$1.o" "$SRC/$1.s"
        ;;
esac
