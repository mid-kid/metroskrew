#!/bin/sh
set -e

MESON="${MESON:-meson}"

PROJ="${PROJ:-..}"

rm -rf install_libs
rm -rf install_native
rm -rf build_native
CC="${CC:-cc} -m32" CXX="${CXX:-c++} -m32" AS="${AS:-as} --32" \
"$MESON" setup "$PROJ" build_native \
    --prefix / \
    --buildtype release
"$MESON" install -C build_native --destdir ../install_native
rm -rf build_native

export PATH="$PWD/install_native/bin:$PATH"

rm -rf build_libs
proj_libs="$PROJ/subprojects/mw-libraries"
libvers="$(sed 's/#.*$//;/^\s*$/d' "$proj_libs/libvers.txt")"
for libver in $libvers; do
    build="build_libs/$(printf %s $libver | tr / _)"
    "$MESON" setup "$proj_libs" "$build" \
        --cross "$proj_libs/meson/mwccarm.ini" \
        --prefix / \
        -Dlibver="$libver"
    "$MESON" install -C "$build" --destdir ../../install_libs
    rm -rf "$build"
done
rm -rf build_libs
rm -rf install_native
