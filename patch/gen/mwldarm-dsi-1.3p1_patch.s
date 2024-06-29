.macro incbin off, len
.incbin "mwccarm/dsi/1.3p1/mwldarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x6c065

code_fs_1 = 0x40f
code_fs_1.end = 0x41d

code_fs_2 = 0x439
code_fs_2.end = 0x447

code_fs_3 = 0x526
code_fs_3.end = 0x534

code_init_args = 0x14a0
code_init_args.end = 0x1790

code_init_envp = 0x1980
code_init_envp.end = 0x19a0

code_getenv = 0x1a20
code_getenv.end = 0x1a70

addr_main = 0x1c80

addr_envp = 0x4abe40

addr_argc = 0x4ad6cc

addr_argv = 0x4ae7bc

.include "patch.i"
