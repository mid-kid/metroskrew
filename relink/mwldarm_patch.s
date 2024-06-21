.intel_syntax noprefix
.include "patch.i"

.macro incbin off, len
.incbin "bins/mwldarm.exe", \off, \len
.endm

.section .patch_pe_text, "ax"
pe_text:

pe_text_off = 0x400
pe_text_len = 0x6ac35

addr_argc = pe_text + 0x8a + 2
addr_argv = pe_text + 0x84 + 2
addr_envp = pe_text + 0x7e + 2

patch_fs_1 = pe_text_off + 0xf
patch_fs_1.end = pe_text_off + 0x1d
patch patch_fs_1, patch_fs

patch_fs_2 = pe_text_off + 0x39
patch_fs_2.end = pe_text_off + 0x47
patch patch_fs_2, patch_fs

patch_init_args = pe_text_off + 0x10a0
patch_init_args.end = pe_text_off + 0x1390
patch patch_init_args, patch_init_args_old

patch_init_envp = pe_text_off + 0x1580
patch_init_envp.end = pe_text_off + 0x15a0
patch patch_init_envp, patch_init_envp

patch_getenv = pe_text_off + 0x1620
patch_getenv.end = pe_text_off + 0x1670
patch patch_getenv, patch_getenv

patch_end
