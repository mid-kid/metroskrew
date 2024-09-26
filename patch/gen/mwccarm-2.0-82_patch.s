.macro incbin off, len
.incbin "ds/1.2/sp2p3/mwccarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x15adfa
pe_text_addr = 0x401000

code_fs_1 = 0x40f
code_fs_1.end = 0x41d
addr_fs_1 = code_fs_1 - pe_text_off + pe_text_addr

code_fs_2 = 0x439
code_fs_2.end = 0x447
addr_fs_2 = code_fs_2 - pe_text_off + pe_text_addr

code_fs_3 = 0x526
code_fs_3.end = 0x534
addr_fs_3 = code_fs_3 - pe_text_off + pe_text_addr

code_init_args = 0x25b0
code_init_args.end = 0x28a0
addr_init_args = code_init_args - pe_text_off + pe_text_addr

code_init_envp = 0x2a90
code_init_envp.end = 0x2ab0
addr_init_envp = code_init_envp - pe_text_off + pe_text_addr

code_getenv = 0x2b30
code_getenv.end = 0x2b80
addr_getenv = code_getenv - pe_text_off + pe_text_addr

code_main = 0x2d90
addr_main = code_main - pe_text_off + pe_text_addr

code_findexe = 0x1f6f0
code_findexe.end = 0x1f880
addr_findexe = code_findexe - pe_text_off + pe_text_addr

code_prog_malloc = 0x3a000
addr_prog_malloc = code_prog_malloc - pe_text_off + pe_text_addr

code_FUN_004f8b60 = 0xdf880
addr_FUN_004f8b60 = code_FUN_004f8b60 - pe_text_off + pe_text_addr

code_FUN_00505340 = 0x14ecc0
code_FUN_00505340.end = 0x14ee70
addr_FUN_00505340 = code_FUN_00505340 - pe_text_off + pe_text_addr

code_memreuse01_hook = 0x14ed1b
code_memreuse01_hook.end = 0x14ed28
addr_memreuse01_hook = code_memreuse01_hook - pe_text_off + pe_text_addr

code_memreuse01_exit = 0x14ee2d
code_memreuse01_exit.end = 0x14ee35
addr_memreuse01_exit = code_memreuse01_exit - pe_text_off + pe_text_addr

addr_DAT_0063ccf0 = 0x5e2500

addr_envp = 0x5ecdec

addr_DAT_0063a798 = 0x5ed1b4

addr_DAT_0063a828 = 0x5ed230

addr_argc = 0x5ed58c

addr_argv = 0x5ed6e0

addr_DAT_0063ccb0 = 0x5ed6f0

.include "patch.i"
