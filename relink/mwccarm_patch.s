.intel_syntax noprefix
.include "patch.i"

.macro incbin off, len
.incbin "bins/mwccarm.exe", \off, \len
.endm

.section .patch_pe_text, "ax"
pe_text:

text_off = 0x400
text_len = 0x19898a

addr_argc = pe_text + 0x8a + 2
addr_argv = pe_text + 0x84 + 2
addr_envp = pe_text + 0x7e + 2

patch_fs_1 = 0xf
patch_fs_1.end = 0x1d
patch patch_fs_1

patch_fs_2 = 0x39
patch_fs_2.end = 0x47
patch patch_fs_2

patch_init_args = 0x21b0
patch_init_args.end = 0x24a0
patch patch_init_args

patch_init_envp = 0x2690
patch_init_envp.end = 0x26b0
patch patch_init_envp

patch_getenv = 0x2730
patch_getenv.end = 0x2780
patch patch_getenv

patch_end
