#!/bin/sh
set -eu

wine() {
    test -n "${MESON_EXE_WRAPPER:-}" && set -- "$MESON_EXE_WRAPPER" "$@"
    "$@"
}

PROGRAM_NAME="${PROGRAM##*/}"

DIR="./test_$1"
mkdir -p "$DIR"

case "$1" in
    exec_argv)
        cp -av "$PROGRAM" "$MWCCARM" "$DIR"
        wine "$DIR/$PROGRAM_NAME" mwccarm -help -wrap:dbg
        ;;

    exec_path)
        cp -av "$PROGRAM" "$MWCCARM" "$DIR"
        ( mkdir -p "$DIR/empty" && cd "$DIR/empty"
            PATH=".." WINEPATH=".." \
                wine "$PROGRAM_NAME" mwccarm -help -wrap:dbg
        )
        ;;
esac
