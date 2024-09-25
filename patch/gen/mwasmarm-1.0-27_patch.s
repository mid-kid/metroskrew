.macro incbin off, len
.incbin "dsi/1.2/mwasmarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x5aa86
pe_text_addr = 0xb81000

code_fs_1 = 0x2a57f
code_fs_1.end = 0x2a58d
addr_fs_1 = code_fs_1 - pe_text_off + pe_text_addr

code_fs_2 = 0x2a5a9
code_fs_2.end = 0x2a5b7
addr_fs_2 = code_fs_2 - pe_text_off + pe_text_addr

code_fs_3 = 0x302b6
code_fs_3.end = 0x302c4
addr_fs_3 = code_fs_3 - pe_text_off + pe_text_addr

code_init_args = 0x30d10
code_init_args.end = 0x31000
addr_init_args = code_init_args - pe_text_off + pe_text_addr

code_init_envp = 0x311f0
code_init_envp.end = 0x31210
addr_init_envp = code_init_envp - pe_text_off + pe_text_addr

code_getenv = 0x31290
code_getenv.end = 0x312e0
addr_getenv = code_getenv - pe_text_off + pe_text_addr

code_main = 0x314f0
addr_main = code_main - pe_text_off + pe_text_addr

code_findexe = 0x34dc0
code_findexe.end = 0x34f80
addr_findexe = code_findexe - pe_text_off + pe_text_addr

addr_envp = 0xc2496c

addr_argc = 0xc25d60

addr_argv = 0xc26d00

.include "patch.i"
