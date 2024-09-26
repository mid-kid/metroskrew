.macro incbin off, len
.incbin "ds/2.0/p2/mwasmarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x5a1f0
pe_text_addr = 0xb81000

code_fs_1 = 0x2a3df
code_fs_1.end = 0x2a3ed
addr_fs_1 = code_fs_1 - pe_text_off + pe_text_addr

code_fs_2 = 0x2a409
code_fs_2.end = 0x2a417
addr_fs_2 = code_fs_2 - pe_text_off + pe_text_addr

code_fs_3 = 0x30116
code_fs_3.end = 0x30124
addr_fs_3 = code_fs_3 - pe_text_off + pe_text_addr

code_init_args = 0x30b70
code_init_args.end = 0x30e60
addr_init_args = code_init_args - pe_text_off + pe_text_addr

code_init_envp = 0x31050
code_init_envp.end = 0x31070
addr_init_envp = code_init_envp - pe_text_off + pe_text_addr

code_getenv = 0x310f0
code_getenv.end = 0x31140
addr_getenv = code_getenv - pe_text_off + pe_text_addr

code_main = 0x31350
addr_main = code_main - pe_text_off + pe_text_addr

code_findexe = 0x34bf0
code_findexe.end = 0x34db0
addr_findexe = code_findexe - pe_text_off + pe_text_addr

addr_envp = 0xc2496c

addr_argc = 0xc25d60

addr_argv = 0xc26d00

.include "patch.i"
