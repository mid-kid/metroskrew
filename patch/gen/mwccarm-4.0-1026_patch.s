.macro incbin off, len
.incbin "dsi/1.2/mwccarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x1b65aa

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

code_findexe = 0x17120
code_findexe.end = 0x172e0

addr_envp = 0x65a8d4

addr_argc = 0x65c108

addr_argv = 0x65d1f8

.include "patch.i"
