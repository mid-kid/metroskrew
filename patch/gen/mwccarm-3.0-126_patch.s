.macro incbin off, len
.incbin "ds/2.0/sp1p2/mwccarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x197fca
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

code_findexe = 0x17060
code_findexe.end = 0x17220
addr_findexe = code_findexe - pe_text_off + pe_text_addr

code_prog_malloc = 0x41920
addr_prog_malloc = code_prog_malloc - pe_text_off + pe_text_addr

code_FUN_004f8b60 = 0xf7cb0
addr_FUN_004f8b60 = code_FUN_004f8b60 - pe_text_off + pe_text_addr

code_FUN_00505340 = 0x104460
code_FUN_00505340.end = 0x104610
addr_FUN_00505340 = code_FUN_00505340 - pe_text_off + pe_text_addr

code_memreuse01_hook = 0x1044bb
code_memreuse01_hook.end = 0x1044c8
addr_memreuse01_hook = code_memreuse01_hook - pe_text_off + pe_text_addr

code_memreuse01_exit = 0x1045cd
code_memreuse01_exit.end = 0x1045d5
addr_memreuse01_exit = code_memreuse01_exit - pe_text_off + pe_text_addr

addr_envp = 0x639260

addr_DAT_0063a798 = 0x639698

addr_DAT_0063a828 = 0x639728

addr_argc = 0x63aa54

addr_argv = 0x63bb44

addr_DAT_0063ccb0 = 0x63bb64

addr_DAT_0063ccf0 = 0x63bba4

.include "patch.i"
