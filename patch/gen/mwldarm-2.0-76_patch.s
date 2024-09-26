.macro incbin off, len
.incbin "ds/1.2/sp2p3/mwldarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x69df5
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

code_findexe = 0x12a40
code_findexe.end = 0x12c00
addr_findexe = code_findexe - pe_text_off + pe_text_addr

addr_envp = 0x4a8298

addr_argc = 0x4a9a18

addr_argv = 0x4aab00

.include "patch.i"
