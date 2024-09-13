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
set $var03 = *(uint32_t*)($var05+0x1c)
set $var02 = *(uint32_t*)($var04 + $var03 * 4)
set $var01 = *(uint32_t*)($var02 + ($var06 >> 5) * 4)
p $var01
p (void*)($var02 + ($var06 >> 5) * 4)
#p $var03
#p (void*)$var04
#p (void*)$var05
#p $var06
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

# $1 = 1
# $2 = 11
# $3 = 3
# $4 = 15
# $5 = 11
# $6 = 27
# $7 = 59
# $8 = 27
# $9 = 63412

#$1 = 2079
#$2 = (void *) 0x81fc268
#
#$3 = 2079
#$4 = (void *) 0x81fc2f8
#
#$5 = 2079
#$6 = (void *) 0x81fc2b0
#
#$7 = 2079
#$8 = (void *) 0x81fc388
#
#$9 = 2079
#$10 = (void *) 0x81fc418
#
#$11 = 2079
#$12 = (void *) 0x81fc3d0
#
#$13 = 2079
#$14 = (void *) 0x81fc340
#
#$15 = 2079
#$16 = (void *) 0x81fc460
#
#$17 = 2079
#$18 = (void *) 0x81fc4a8
#
#$19 = 2079
#$20 = (void *) 0x81fc580
#
#$21 = 2079
#$22 = (void *) 0x81fc538
#
#$23 = 2079
#$24 = (void *) 0x81fc4f0
#
#$25 = 2079
#$26 = (void *) 0x81fc5c8
#
#$27 = 2079
#$28 = (void *) 0x81fc6a0
#
#$29 = 2079
#$30 = (void *) 0x81fc658
#
#$31 = 2079
#$32 = (void *) 0x81fc610
#
#$33 = 2079
#$34 = (void *) 0x81fc740
#
#$35 = 2079
#$36 = (void *) 0x81fc788
#
#$37 = 2079
#$38 = (void *) 0x81fc6e8
#
#$39 = 2079
#$40 = (void *) 0x81fc860
#
#$41 = 2079
#$42 = (void *) 0x81fc818
#
#$43 = 2079
#$44 = (void *) 0x81fc7d0
#
#$45 = 2079
#$46 = (void *) 0x81fc8a8
#
#$47 = 2079
#$48 = (void *) 0x81fc8f0
#
#$49 = 2079
#$50 = (void *) 0x81fc980
#
#$51 = 2079
#$52 = (void *) 0x81fc938
#
#$53 = 2079
#$54 = (void *) 0x81fca10
#
#$55 = 2079
#$56 = (void *) 0x81fcaa0
#
#$57 = 2079
#$58 = (void *) 0x81fca58
#
#$59 = 2079
#$60 = (void *) 0x81fc9c8
#
#$61 = 2079
#$62 = (void *) 0x81fcb40
#
#$63 = 2079
#$64 = (void *) 0x81fcb88
#
#$65 = 2079
#$66 = (void *) 0x81fcae8
#
#$67 = 2079
#$68 = (void *) 0x81fcc18
#
#$69 = 2079
#$70 = (void *) 0x81fcbd0
#
#$71 = 2079
#$72 = (void *) 0x81fcc60
#
#$73 = 2079
#$74 = (void *) 0x81fcca8
#
#$75 = 2079
#$76 = (void *) 0x81fcd38
#
#$77 = 2079
#$78 = (void *) 0x81fccf0
#
#$79 = 2079
#$80 = (void *) 0x81fcd80
#
#$81 = 2079
#$82 = (void *) 0x81fce10
#
#$83 = 2079
#$84 = (void *) 0x81fcdc8
#
###############
#
#$85 = 1
#$86 = (void *) 0x81fc280
#0x005053dd      830a01         or dword [edx], 1
#
#$87 = 11
#$88 = (void *) 0x81fc298
#0x005817bb      891a           mov dword [edx], ebx
#
#$89 = 3
#$90 = (void *) 0x81fc288
#0x005817bb      891a           mov dword [edx], ebx
#
#$91 = 15
#$92 = (void *) 0x81fc290
#0x005817bb      891a           mov dword [edx], ebx
#
#$93 = 11
#$94 = (void *) 0x81fc298
#0x005817bb      891a           mov dword [edx], ebx
#
#$95 = 27
#$96 = (void *) 0x81fc2a0
#0x005817bb      891a           mov dword [edx], ebx
#
#$97 = 59
#$98 = (void *) 0x81fc2a8
#0x005817bb      891a           mov dword [edx], ebx
#
#$99 = 27
#$100 = (void *) 0x81fc2a0
#0x005817bb      891a           mov dword [edx], ebx
#
#$101 = 2079
#$102 = (void *) 0x81fc2b0
#
#$103 = 187
#$104 = (void *) 0x81fc2b8
#
#$105 = 283
#$106 = (void *) 0x81fc2c0
