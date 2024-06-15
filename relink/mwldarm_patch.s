.intel_syntax noprefix
.include "patch.i"

.macro incbin off, len
.incbin "bins/mwldarm.exe", \off, \len
.endm

.section .patch_pe_text, "ax"
pe_text:

text_off = 0x400
text_len = 0x6ac35

addr_argc = pe_text + 0x8a + 2
addr_argv = pe_text + 0x84 + 2
addr_envp = pe_text + 0x7e + 2

patch_fs_1 = 0xf
patch_fs_1.end = 0x1d
patch patch_fs_1

patch_fs_2 = 0x39
patch_fs_2.end = 0x47
patch patch_fs_2

patch_init_args = 0x10a0
patch_init_args.end = 0x1390
patch patch_init_args

patch_init_envp = 0x1580
patch_init_envp.end = 0x15a0
patch patch_init_envp

patch_getenv = 0x1620
patch_getenv.end = 0x1670
patch patch_getenv

patch_end
