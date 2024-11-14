.macro incbin off, len
.incbin "dsi/1.1/mwasmarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x5a9d0
pe_text_addr = 0xb81000

off_depfile_struct__source = 0x1c

off_depfile_struct__targets = 0x423

off_depfile_struct__num_headers = 0x870

off_depfile_struct__headers = 0x878

code_fs_1 = 0x2a55f
code_fs_1.end = 0x2a56d
addr_fs_1 = code_fs_1 - pe_text_off + pe_text_addr

code_fs_2 = 0x2a589
code_fs_2.end = 0x2a597
addr_fs_2 = code_fs_2 - pe_text_off + pe_text_addr

code_fs_3 = 0x30296
code_fs_3.end = 0x302a4
addr_fs_3 = code_fs_3 - pe_text_off + pe_text_addr

code_init_args = 0x30cf0
code_init_args.end = 0x30fe0
addr_init_args = code_init_args - pe_text_off + pe_text_addr

code_init_envp = 0x311d0
code_init_envp.end = 0x311f0
addr_init_envp = code_init_envp - pe_text_off + pe_text_addr

code_getenv = 0x31270
code_getenv.end = 0x312c0
addr_getenv = code_getenv - pe_text_off + pe_text_addr

code_main = 0x314d0
addr_main = code_main - pe_text_off + pe_text_addr

code_depfile_get_target = 0x34a00
code_depfile_get_target.end = 0x34a0d
addr_depfile_get_target = code_depfile_get_target - pe_text_off + pe_text_addr

code_findexe = 0x34d70
code_findexe.end = 0x34f30
addr_findexe = code_findexe - pe_text_off + pe_text_addr

code_depfile_get_header = 0x52ce0
code_depfile_get_header.end = 0x52cf2
addr_depfile_get_header = code_depfile_get_header - pe_text_off + pe_text_addr

code_depfile_build = 0x535a0
code_depfile_build.end = 0x535b1
addr_depfile_build = code_depfile_build - pe_text_off + pe_text_addr

addr_envp = 0xc2496c

addr_argc = 0xc25d60

addr_argv = 0xc26d00

.include "patch.i"
