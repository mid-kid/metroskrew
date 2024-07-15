.macro incbin off, len
.incbin "ds/2.0/base/mwldarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x6aba5

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

code_findexe = 0x12a40
code_findexe.end = 0x12c00

addr_envp = 0x4a92a8

addr_argc = 0x4aaa28

addr_argv = 0x4abb10

.include "patch.i"
