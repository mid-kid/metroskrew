.macro incbin off, len
.incbin "dsi/1.1p1/mwccarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x1b601a
pe_text_addr = 0x401000

off_depfile_struct__source = 0x1c

off_depfile_struct__targets = 0x423

off_depfile_struct__num_headers = 0x870

off_depfile_struct__headers = 0x878

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

code_depfile_get_target = 0x16d80
code_depfile_get_target.end = 0x16d8d
addr_depfile_get_target = code_depfile_get_target - pe_text_off + pe_text_addr

code_findexe = 0x170f0
code_findexe.end = 0x172b0
addr_findexe = code_findexe - pe_text_off + pe_text_addr

code_depfile_get_header = 0x3bf60
code_depfile_get_header.end = 0x3bf72
addr_depfile_get_header = code_depfile_get_header - pe_text_off + pe_text_addr

code_depfile_build = 0x3c820
code_depfile_build.end = 0x3c831
addr_depfile_build = code_depfile_build - pe_text_off + pe_text_addr

code_prog_malloc = 0x42550
addr_prog_malloc = code_prog_malloc - pe_text_off + pe_text_addr

code_FUN_004f8b60 = 0x103c30
addr_FUN_004f8b60 = code_FUN_004f8b60 - pe_text_off + pe_text_addr

code_FUN_00505340 = 0x113af0
code_FUN_00505340.end = 0x113ca0
addr_FUN_00505340 = code_FUN_00505340 - pe_text_off + pe_text_addr

addr_envp = 0x65ab54

addr_DAT_0063a798 = 0x65afa0

addr_DAT_0063a828 = 0x65b038

addr_argc = 0x65c384

addr_argv = 0x65d474

addr_DAT_0063ccb0 = 0x65d494

addr_DAT_0063ccf0 = 0x65d4c8

.include "patch.i"
