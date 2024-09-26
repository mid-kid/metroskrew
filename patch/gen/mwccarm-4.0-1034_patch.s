.macro incbin off, len
.incbin "dsi/1.3/mwccarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x1b6f7a

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

code_findexe = 0x17130
code_findexe.end = 0x172f0

code_memreuse01_hook = 0x11406b
code_memreuse01_hook.end = 0x114078

code_memreuse01_exit = 0x11417d
code_memreuse01_exit.end = 0x114185

addr_envp = 0x65bb94

addr_memreuse01_len = 0x65bd10

addr_argc = 0x65d0f4

addr_argv = 0x65e1e4

addr_memreuse01_arr = 0x65e238

.include "patch.i"
