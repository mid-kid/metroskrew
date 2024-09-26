#!/bin/sh
set -e
export LANG=C

test -f build/build.ninja || make setup
ninja -C build/opt.release relink/mwccarm-3.0-137.exe

GLIBC_TUNABLES=glibc.malloc.mmap_max=0 \
setarch -R ./build/opt.release/relink/mwccarm-3.0-137.exe -o test_qemu.o -c test.c
setarch -R ./build/opt.release/relink/mwccarm-3.0-137.exe -o test_host.o -c test.c

diff test_qemu.o test_host.o || true

dump=objdump
command -v arm-none-eabi-objdump 2>&1 > /dev/null && dump=arm-none-eabi-objdump || true
$dump -d test_qemu.o > test_qemu.dmp
$dump -d test_host.o > test_host.dmp
diff --color=always --unified=100 test_qemu.dmp test_host.dmp || true
