#!/bin/sh
set -e

MESON="${MESON:-meson}"

PROJ="${PROJ:-..}"

rm -rf install_wine
rm -rf build_wine
"$MESON" setup "$PROJ" build_wine \
    --cross-file "$PROJ/meson/i686-w64-mingw32.ini" \
    --prefix / \
    --buildtype release \
    --strip \
    -Dwine_wrap=true
"$MESON" install -C build_wine --destdir ../install_wine
rm -rf build_wine

mv install_wine metroskrew
cp -aT install_libs metroskrew

rm -f metroskrew-wine.tar metroskrew-wine.tar.xz
tar cf metroskrew-wine.tar metroskrew
rm -rf metroskrew
xz -9v metroskrew-wine.tar
