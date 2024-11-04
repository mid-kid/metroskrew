#!/bin/sh
set -e

MESON="${MESON:-meson}"

PROJ="${PROJ:-..}"

rm -rf install_linux
rm -rf build_linux
"$MESON" setup "$PROJ" build_linux \
    --cross-file "$PROJ/meson/i686-unknown-linux-gnu.ini" \
    --prefix / \
    --buildtype release \
    --strip
"$MESON" install -C build_linux --destdir ../install_linux
rm -rf build_linux

mv install_linux metroskrew
cp -aT install_libs metroskrew

rm -f metroskrew-linux.tar metroskrew-linux.tar.xz
tar cf metroskrew-linux.tar metroskrew
rm -rf metroskrew
xz -9v metroskrew-linux.tar
