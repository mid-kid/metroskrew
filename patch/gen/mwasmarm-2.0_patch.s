.macro incbin off, len
.incbin "ds/2.0/base/mwasmarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x5a000

code_fs_1 = 0x2a32f
code_fs_1.end = 0x2a33d

code_fs_2 = 0x2a359
code_fs_2.end = 0x2a367

code_fs_3 = 0x30066
code_fs_3.end = 0x30074

code_init_args = 0x30ac0
code_init_args.end = 0x30db0

code_init_envp = 0x30fa0
code_init_envp.end = 0x30fc0

code_getenv = 0x31040
code_getenv.end = 0x31090

addr_main = 0x312a0

addr_envp = 0xc2396c

addr_argc = 0xc24d60

addr_argv = 0xc25d00

.include "patch.i"
