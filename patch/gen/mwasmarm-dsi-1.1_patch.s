.macro incbin off, len
.incbin "mwccarm/dsi/1.1/mwasmarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x5a9d0

code_fs_1 = 0x2a55f
code_fs_1.end = 0x2a56d

code_fs_2 = 0x2a589
code_fs_2.end = 0x2a597

code_fs_3 = 0x30296
code_fs_3.end = 0x302a4

code_init_args = 0x30cf0
code_init_args.end = 0x30fe0

code_init_envp = 0x311d0
code_init_envp.end = 0x311f0

code_getenv = 0x31270
code_getenv.end = 0x312c0

addr_main = 0x314d0

addr_envp = 0xc2496c

addr_argc = 0xc25d60

addr_argv = 0xc26d00

.include "patch.i"
