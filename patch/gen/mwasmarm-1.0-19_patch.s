.macro incbin off, len
.incbin "ds/1.2/base/mwasmarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x59fd0

code_fs_1 = 0x2a30f
code_fs_1.end = 0x2a31d

code_fs_2 = 0x2a339
code_fs_2.end = 0x2a347

code_fs_3 = 0x30046
code_fs_3.end = 0x30054

code_init_args = 0x30aa0
code_init_args.end = 0x30d90

code_init_envp = 0x30f80
code_init_envp.end = 0x30fa0

code_getenv = 0x31020
code_getenv.end = 0x31070

addr_main = 0x31280

addr_envp = 0xc2396c

addr_argc = 0xc24d60

addr_argv = 0xc25d00

.include "patch.i"
