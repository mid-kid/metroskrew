.macro incbin off, len
.incbin "ds/2.0/sp1p5/mwccarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x19837a
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

code_depfile_get_target = 0x16cf0
code_depfile_get_target.end = 0x16cfd
addr_depfile_get_target = code_depfile_get_target - pe_text_off + pe_text_addr

code_findexe = 0x17060
code_findexe.end = 0x17220
addr_findexe = code_findexe - pe_text_off + pe_text_addr

code_depfile_get_header = 0x3b3f0
code_depfile_get_header.end = 0x3b402
addr_depfile_get_header = code_depfile_get_header - pe_text_off + pe_text_addr

code_depfile_build = 0x3bcb0
code_depfile_build.end = 0x3bcc1
addr_depfile_build = code_depfile_build - pe_text_off + pe_text_addr

code_prog_malloc = 0x41930
addr_prog_malloc = code_prog_malloc - pe_text_off + pe_text_addr

code_FUN_004f8b60 = 0xf7df0
addr_FUN_004f8b60 = code_FUN_004f8b60 - pe_text_off + pe_text_addr

code_FUN_00505340 = 0x1045c0
code_FUN_00505340.end = 0x104770
addr_FUN_00505340 = code_FUN_00505340 - pe_text_off + pe_text_addr

addr_envp = 0x63a360

addr_DAT_0063a798 = 0x63a798

addr_DAT_0063a828 = 0x63a828

addr_argc = 0x63bb54

addr_argv = 0x63cc44

addr_DAT_0063ccb0 = 0x63cc64

addr_DAT_0063ccf0 = 0x63cca4

.include "patch.i"
