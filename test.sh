#!/bin/sh
set -e
export LANG=C

test -f build/build.ninja || make setup
ninja -C build relink/mwccarm-3.0-137.exe

qemu-i386 ./build/relink/mwccarm-3.0-137.exe -o test_qemu.o -c test.c
./build/relink/mwccarm-3.0-137.exe -o test_host.o -c test.c

diff test_host.o test_qemu.o || true

dump=objdump
command -v arm-none-eabi-objdump 2>&1 > /dev/null && dump=arm-none-eabi-objdump || true
$dump -d test_host.o > test_host.dmp
$dump -d test_qemu.o > test_qemu.dmp
diff --color=always --unified=100 test_host.dmp test_qemu.dmp || true
