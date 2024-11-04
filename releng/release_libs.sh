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
libvers="
    dsi/1.6sp1 dsi/1.3 dsi/1.2 dsi/1.1p1 dsi/1.1
    ds/2.0/sp2p3 ds/2.0/sp2 ds/2.0/sp1p2 ds/2.0/sp1
    ds/2.0/p4 ds/2.0/p2 ds/2.0/base
    ds/1.2/sp4 ds/1.2/sp3 ds/1.2/sp2 ds/1.2/base_b73 ds/1.2/base
"
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
