.intel_syntax noprefix
.include "patch.i"

.macro incbin off, len
.incbin "bins/mwasmarm.exe", \off, \len
.endm

.section .patch_pe_text, "ax"
pe_text:

pe_text_off = 0x400
pe_text_len = 0x5a1f0

code_fs_1 = 0x2a3df
code_fs_1.end = 0x2a3ed

code_fs_2 = 0x2a409
code_fs_2.end = 0x2a417

code_fs_3 = 0x30116
code_fs_3.end = 0x30124

code_init_args = 0x30b70
code_init_args.end = 0x30e60

code_init_envp = 0x31050
code_init_envp.end = 0x31070

code_getenv = 0x310f0
code_getenv.end = 0x31140

addr_main = 0x31350

addr_envp = 0xc2496c

addr_argc = 0xc25d60

addr_argv = 0xc26d00

patch code_fs_1, patch_fs
patch code_fs_2, patch_fs
patch code_init_args, patch_init_args
patch code_init_envp, patch_init_envp
patch code_getenv, patch_getenv
patch_end
