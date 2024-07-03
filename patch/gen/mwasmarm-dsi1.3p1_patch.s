.macro incbin off, len
.incbin "dsi/1.3p1/mwasmarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x5aa86

code_fs_1 = 0x2a57f
code_fs_1.end = 0x2a58d

code_fs_2 = 0x2a5a9
code_fs_2.end = 0x2a5b7

code_fs_3 = 0x302b6
code_fs_3.end = 0x302c4

code_init_args = 0x30d10
code_init_args.end = 0x31000

code_init_envp = 0x311f0
code_init_envp.end = 0x31210

code_getenv = 0x31290
code_getenv.end = 0x312e0

addr_main = 0x314f0

addr_envp = 0xc2496c

addr_argc = 0xc25d60

addr_argv = 0xc26d00

.include "patch.i"
