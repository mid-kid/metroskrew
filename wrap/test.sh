#!/bin/sh
set -eu

wine() {
    test -n "${MESON_EXE_WRAPPER:-}" && set -- "$MESON_EXE_WRAPPER" "$@"
    "$@"
}

PROGRAM_NAME="${PROGRAM##*/}"

DIR="./test_$1"
rm -r "$DIR"; mkdir "$DIR"

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

    sdk)
        export SKREW_DATADIR="$DIR"
        cp -av "$PROGRAM" "$DIR"
        cp -av "$MWCCARM" "$DIR/test.exe"
        mkdir -p "$DIR/sdk/test"
        echo test.exe > "$DIR/sdk/test/mwccarm.exe.txt"
        wine "$DIR/$PROGRAM_NAME" mwccarm -wrap:sdk test -help
esac
