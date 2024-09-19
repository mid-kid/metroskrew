#!/usr/bin/env -S sh -c 'GLIBC_TUNABLES=glibc.malloc.mmap_max=0 gdb -x script.gdb --args ./build/relink/mwccarm-3.0-137.exe -c test.c' --

set disassembly-flavor intel

define my1
p (unsigned)$edx >> 16
p (void*)($eax + 0x3e + 2)
c
end

define my2
set $var06 = *(uint32_t*)($ebx + 0x1c)
set $var05 = *(uint32_t*)($edx + 0xc)
set $var04 = *(uint32_t*)0x63ccf0
set $var03 = *(uint32_t*)($var05 + 0x1c)
set $var02 = *(uint32_t*)($var04 + $var03 * 4)
set $var01 = *(uint32_t*)($var02 + ($var06 >> 5) * 4)
p $var01
p (void*)($var02 + ($var06 >> 5) * 4)
p $var03
p $var06
c
end

# Half-pointer is written
b *0x5454b2
# Faulty value is read
b *0x50551a

#define my
#p $eax
#c
#end
#b *0x50553b

# 0x63ccf0

# $1 = 1
# $2 = 11
# $3 = 3
# $4 = 15
# $5 = 11
# $6 = 27
# $7 = 59
# $8 = 27
# $9 = 63412

#$1 = 128
#$2 = (void *) 0x800318
#
#$3 = 128
#$4 = (void *) 0x8003a8
#
#$5 = 128
#$6 = (void *) 0x800360
#
#$7 = 128
#$8 = (void *) 0x800438
#
#$9 = 128
#$10 = (void *) 0x8004c8
#
#$11 = 128
#$12 = (void *) 0x800480
#
#$13 = 128
#$14 = (void *) 0x8003f0
#
#$15 = 128
#$16 = (void *) 0x800510
#
#$17 = 128
#$18 = (void *) 0x800558
#
#$19 = 128
#$20 = (void *) 0x800630
#
#$21 = 128
#$22 = (void *) 0x8005e8
#
#$23 = 128
#$24 = (void *) 0x8005a0
#
#$25 = 128
#$26 = (void *) 0x800678
#
#$27 = 128
#$28 = (void *) 0x800750
#
#$29 = 128
#$30 = (void *) 0x800708
#
#$31 = 128
#$32 = (void *) 0x8006c0
#
#$33 = 128
#$34 = (void *) 0x8007f0
#
#$35 = 128
#$36 = (void *) 0x800838
#
#$37 = 128
#$38 = (void *) 0x800798
#
#$39 = 128
#$40 = (void *) 0x800910
#
#$41 = 128
#$42 = (void *) 0x8008c8
#
#$43 = 128
#$44 = (void *) 0x800880
#
#$45 = 128
#$46 = (void *) 0x800958
#
#$47 = 128
#$48 = (void *) 0x8009a0
#
#$49 = 128
#$50 = (void *) 0x800a30
#
#$51 = 128
#$52 = (void *) 0x8009e8
#
#$53 = 128
#$54 = (void *) 0x800ac0
#
#$55 = 128
#$56 = (void *) 0x800b50
#
#$57 = 128
#$58 = (void *) 0x800b08
#
#$59 = 128
#$60 = (void *) 0x800a78
#
#$61 = 128
#$62 = (void *) 0x800bf0
#
#$63 = 128
#$64 = (void *) 0x800c38
#
#$65 = 128
#$66 = (void *) 0x800b98
#
#$67 = 128
#$68 = (void *) 0x800cc8
#
#$69 = 128
#$70 = (void *) 0x800c80
#
#$71 = 128
#$72 = (void *) 0x800d10
#
#$73 = 128
#$74 = (void *) 0x800d58
#
#$75 = 128
#$76 = (void *) 0x800de8
#
#$77 = 128
#$78 = (void *) 0x800da0
#
#$79 = 128
#$80 = (void *) 0x800e30
#
#$81 = 128
#$82 = (void *) 0x800ec0
#
#$83 = 128
#$84 = (void *) 0x800e78
#
###############
#
#$1 = 1
#$2 = (void *) 0x800330
#$3 = 0
#$4 = 1
#0x005053dd      830a01         or dword [edx], 1
#
#$5 = 11
#$6 = (void *) 0x800348
#$7 = 3
#$8 = 2
#0x005817bb      891a           mov dword [edx], ebx
#
#$9 = 3
#$10 = (void *) 0x800338
#$11 = 1
#$12 = 3
#0x005817bb      891a           mov dword [edx], ebx
#
#$13 = 15
#$14 = (void *) 0x800340
#$15 = 2
#$16 = 3
#0x005817bb      891a           mov dword [edx], ebx
#
#$17 = 11
#$18 = (void *) 0x800348
#$19 = 3
#$20 = 4
#0x005817bb      891a           mov dword [edx], ebx
#
#$21 = 27
#$22 = (void *) 0x800350
#$23 = 4
#$24 = 5
#0x005817bb      891a           mov dword [edx], ebx
#
#$25 = 59
#$26 = (void *) 0x800358
#$27 = 5
#$28 = 7
#0x005817bb      891a           mov dword [edx], ebx
#
#$29 = 27
#$30 = (void *) 0x800350
#$31 = 4
#$32 = 8
#0x005817bb      891a           mov dword [edx], ebx
#
#$33 = 128
#$34 = (void *) 0x800360
#$35 = 6
#$36 = 9
#
#$37 = 187
#$38 = (void *) 0x800368
#$39 = 7
#$40 = 9
#
#$41 = 283
#$42 = (void *) 0x800370
#$43 = 8
#$44 = 9
