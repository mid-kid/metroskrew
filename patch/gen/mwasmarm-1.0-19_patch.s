.macro incbin off, len
.incbin "ds/1.2/base/mwasmarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x59fd0
pe_text_addr = 0xb81000

off_depfile_struct__source = 0x1c

off_depfile_struct__targets = 0x423

off_depfile_struct__num_headers = 0x870

off_depfile_struct__headers = 0x878

code_fs_1 = 0x2a30f
code_fs_1.end = 0x2a31d
addr_fs_1 = code_fs_1 - pe_text_off + pe_text_addr

code_fs_2 = 0x2a339
code_fs_2.end = 0x2a347
addr_fs_2 = code_fs_2 - pe_text_off + pe_text_addr

code_fs_3 = 0x30046
code_fs_3.end = 0x30054
addr_fs_3 = code_fs_3 - pe_text_off + pe_text_addr

code_init_args = 0x30aa0
code_init_args.end = 0x30d90
addr_init_args = code_init_args - pe_text_off + pe_text_addr

code_init_envp = 0x30f80
code_init_envp.end = 0x30fa0
addr_init_envp = code_init_envp - pe_text_off + pe_text_addr

code_getenv = 0x31020
code_getenv.end = 0x31070
addr_getenv = code_getenv - pe_text_off + pe_text_addr

code_main = 0x31280
addr_main = code_main - pe_text_off + pe_text_addr

code_depfile_get_target = 0x347b0
code_depfile_get_target.end = 0x347bd
addr_depfile_get_target = code_depfile_get_target - pe_text_off + pe_text_addr

code_findexe = 0x34b20
code_findexe.end = 0x34ce0
addr_findexe = code_findexe - pe_text_off + pe_text_addr

code_depfile_get_header = 0x522e0
code_depfile_get_header.end = 0x522f2
addr_depfile_get_header = code_depfile_get_header - pe_text_off + pe_text_addr

code_depfile_build = 0x52ba0
code_depfile_build.end = 0x52bb1
addr_depfile_build = code_depfile_build - pe_text_off + pe_text_addr

addr_envp = 0xc2396c

addr_argc = 0xc24d60

addr_argv = 0xc25d00

.include "patch.i"
