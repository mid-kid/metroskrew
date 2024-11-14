.macro incbin off, len
.incbin "ds/2.0/sp1p5/mwldarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x6abd5
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

code_init_args = 0x14a0
code_init_args.end = 0x1790
addr_init_args = code_init_args - pe_text_off + pe_text_addr

code_init_envp = 0x1980
code_init_envp.end = 0x19a0
addr_init_envp = code_init_envp - pe_text_off + pe_text_addr

code_getenv = 0x1a20
code_getenv.end = 0x1a70
addr_getenv = code_getenv - pe_text_off + pe_text_addr

code_main = 0x1c80
addr_main = code_main - pe_text_off + pe_text_addr

code_depfile_get_target = 0x126d0
code_depfile_get_target.end = 0x126dd
addr_depfile_get_target = code_depfile_get_target - pe_text_off + pe_text_addr

code_findexe = 0x12a40
code_findexe.end = 0x12c00
addr_findexe = code_findexe - pe_text_off + pe_text_addr

code_depfile_get_header = 0x34b80
code_depfile_get_header.end = 0x34b92
addr_depfile_get_header = code_depfile_get_header - pe_text_off + pe_text_addr

code_depfile_build = 0x35440
code_depfile_build.end = 0x35451
addr_depfile_build = code_depfile_build - pe_text_off + pe_text_addr

addr_envp = 0x4a92a8

addr_argc = 0x4aaa28

addr_argv = 0x4abb10

.include "patch.i"
