#!/bin/sh
set -e

MESON="${MESON:-meson}"

PROJ="${PROJ:-..}"

rm -rf install_windows
rm -rf build_windows
"$MESON" setup "$PROJ" build_windows \
    --cross-file "$PROJ/meson/i686-w64-mingw32.ini" \
    --prefix / \
    --buildtype release \
    --strip
"$MESON" install -C build_windows --destdir ../install_windows
rm -rf build_windows

mv install_windows metroskrew
cp -aT install_libs metroskrew

rm -f metroskrew-windows.7z
7zz a -mx9 metroskrew-windows.7z metroskrew || \
    7z a -mx9 metroskrew-windows.7z metroskrew
rm -rf metroskrew
