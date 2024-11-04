#!/bin/sh
set -e
./release_libs.sh
./release_linux.sh
./release_windows.sh
rm -rf install_libs
