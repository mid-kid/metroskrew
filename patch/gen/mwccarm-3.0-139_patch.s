.macro incbin off, len
.incbin "ds/2.0/sp2p4/mwccarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x1984da

code_fs_1 = 0x40f
code_fs_1.end = 0x41d

code_fs_2 = 0x439
code_fs_2.end = 0x447

code_fs_3 = 0x526
code_fs_3.end = 0x534

code_init_args = 0x25b0
code_init_args.end = 0x28a0

code_init_envp = 0x2a90
code_init_envp.end = 0x2ab0

code_getenv = 0x2b30
code_getenv.end = 0x2b80

addr_main = 0x2d90

code_findexe = 0x17060
code_findexe.end = 0x17220

code_memreuse01_hook = 0x1047cb
code_memreuse01_hook.end = 0x1047d8

code_memreuse01_exit = 0x1048dd
code_memreuse01_exit.end = 0x1048e5

addr_envp = 0x63a368

addr_memreuse01_len = 0x63a7a4

addr_argc = 0x63be18

addr_argv = 0x63cf08

addr_memreuse01_arr = 0x63cf68

.include "patch.i"
