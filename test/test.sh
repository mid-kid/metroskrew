#!/bin/sh
set -eu

SRC="${SRC:-.}"

mwccarm() {
    set -- "$MWCCARM" "$@"
    test -n "${MESON_EXE_WRAPPER:-}" && set -- "$MESON_EXE_WRAPPER" "$@"
    "$@"
}

case "$1" in
    basic_c|switch_float_bug)
        mwccarm -c -o "$1.o" "$SRC/$1.c"
        cmp "$1.o" "$SRC/res/$1.o"
        ;;
esac
