.macro incbin off, len
.incbin "dsi/1.1/mwccarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x1b5e9a

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

code_findexe = 0x170f0
code_findexe.end = 0x172b0

code_memreuse01 = 0x113a1b
code_memreuse01.end = 0x113a28

code_memreuse01_exit = 0x113b2d
code_memreuse01_exit.end = 0x113b35

addr_envp = 0x659b28

addr_memreuse01_len = 0x659f74

addr_argc = 0x65b358

addr_argv = 0x65c448

addr_memreuse01_arr = 0x65c49c

.include "patch.i"
