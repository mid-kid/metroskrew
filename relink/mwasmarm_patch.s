.intel_syntax noprefix
.include "patch.i"

.macro incbin off, len
.incbin "bins/mwasmarm.exe", \off, \len
.endm

.section .patch_pe_text, "ax"
pe_text:

text_off = 0x400
text_len = 0x5a1f0

addr_argc = pe_text + 0x2a05a + 2
addr_argv = pe_text + 0x2a054 + 2
addr_argp = pe_text + 0x2a04e + 2

patch_fs_1 = 0x29fdf
patch_fs_1.end = 0x29fed
patch patch_fs_1

patch_fs_2 = 0x2a009
patch_fs_2.end = 0x2a017
patch patch_fs_2

patch_init_args = 0x30770
patch_init_args.end = 0x30a60
patch patch_init_args

patch_end
