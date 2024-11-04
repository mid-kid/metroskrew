.macro incbin off, len
.incbin "ds/2.0/p4/mwccarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x197b8a
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

code_depfile_get_target = 0x16cf0
code_depfile_get_target.end = 0x16cfd
addr_depfile_get_target = code_depfile_get_target - pe_text_off + pe_text_addr

code_findexe = 0x17060
code_findexe.end = 0x17220
addr_findexe = code_findexe - pe_text_off + pe_text_addr

code_depfile_get_header = 0x3ad80
code_depfile_get_header.end = 0x3ad92
addr_depfile_get_header = code_depfile_get_header - pe_text_off + pe_text_addr

code_depfile_build = 0x3b640
code_depfile_build.end = 0x3b651
addr_depfile_build = code_depfile_build - pe_text_off + pe_text_addr

code_prog_malloc = 0x412c0
addr_prog_malloc = code_prog_malloc - pe_text_off + pe_text_addr

code_FUN_004f8b60 = 0xf74f0
addr_FUN_004f8b60 = code_FUN_004f8b60 - pe_text_off + pe_text_addr

code_FUN_00505340 = 0x103c90
code_FUN_00505340.end = 0x103e40
addr_FUN_00505340 = code_FUN_00505340 - pe_text_off + pe_text_addr

addr_envp = 0x637320

addr_DAT_0063a798 = 0x637758

addr_DAT_0063a828 = 0x6377e8

addr_argc = 0x638b14

addr_argv = 0x639c04

addr_DAT_0063ccb0 = 0x639c20

addr_DAT_0063ccf0 = 0x639c60

.include "patch.i"
