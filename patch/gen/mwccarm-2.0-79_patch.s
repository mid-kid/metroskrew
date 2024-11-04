.macro incbin off, len
.incbin "ds/1.2/sp2/mwccarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x15aaaa
pe_text_addr = 0x401000

code_fs_1 = 0x40f
code_fs_1.end = 0x41d
addr_fs_1 = code_fs_1 - pe_text_off + pe_text_addr

code_fs_2 = 0x439
code_fs_2.end = 0x447
addr_fs_2 = code_fs_2 - pe_text_off + pe_text_addr

code_fs_3 = 0x526
code_fs_3.end = 0x534
addr_fs_3 = code_fs_3 - pe_text_off + pe_text_addr

code_init_args = 0x25b0
code_init_args.end = 0x28a0
addr_init_args = code_init_args - pe_text_off + pe_text_addr

code_init_envp = 0x2a90
code_init_envp.end = 0x2ab0
addr_init_envp = code_init_envp - pe_text_off + pe_text_addr

code_getenv = 0x2b30
code_getenv.end = 0x2b80
addr_getenv = code_getenv - pe_text_off + pe_text_addr

code_main = 0x2d90
addr_main = code_main - pe_text_off + pe_text_addr

code_depfile_get_target = 0x1f380
code_depfile_get_target.end = 0x1f38d
addr_depfile_get_target = code_depfile_get_target - pe_text_off + pe_text_addr

code_findexe = 0x1f6f0
code_findexe.end = 0x1f880
addr_findexe = code_findexe - pe_text_off + pe_text_addr

code_depfile_get_header = 0x34250
code_depfile_get_header.end = 0x34262
addr_depfile_get_header = code_depfile_get_header - pe_text_off + pe_text_addr

code_depfile_build = 0x34af0
code_depfile_build.end = 0x34b01
addr_depfile_build = code_depfile_build - pe_text_off + pe_text_addr

code_prog_malloc = 0x3a000
addr_prog_malloc = code_prog_malloc - pe_text_off + pe_text_addr

code_FUN_004f8b60 = 0xdf7a0
addr_FUN_004f8b60 = code_FUN_004f8b60 - pe_text_off + pe_text_addr

code_FUN_00505340 = 0x14e980
code_FUN_00505340.end = 0x14eb30
addr_FUN_00505340 = code_FUN_00505340 - pe_text_off + pe_text_addr

addr_DAT_0063ccf0 = 0x5e24f8

addr_envp = 0x5ecde4

addr_DAT_0063a798 = 0x5ed1ac

addr_DAT_0063a828 = 0x5ed228

addr_argc = 0x5ed584

addr_argv = 0x5ed6d8

addr_DAT_0063ccb0 = 0x5ed6e8

.include "patch.i"
