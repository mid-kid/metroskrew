.macro incbin off, len
.incbin "ds/1.2/sp4/mwasmarm.exe", \off, \len
.endm

pe_text_off = 0x400
pe_text_len = 0x5a000
pe_text_addr = 0xb81000

code_fs_1 = 0x2a32f
code_fs_1.end = 0x2a33d
addr_fs_1 = code_fs_1 - pe_text_off + pe_text_addr

code_fs_2 = 0x2a359
code_fs_2.end = 0x2a367
addr_fs_2 = code_fs_2 - pe_text_off + pe_text_addr

code_fs_3 = 0x30066
code_fs_3.end = 0x30074
addr_fs_3 = code_fs_3 - pe_text_off + pe_text_addr

code_init_args = 0x30ac0
code_init_args.end = 0x30db0
addr_init_args = code_init_args - pe_text_off + pe_text_addr

code_init_envp = 0x30fa0
code_init_envp.end = 0x30fc0
addr_init_envp = code_init_envp - pe_text_off + pe_text_addr

code_getenv = 0x31040
code_getenv.end = 0x31090
addr_getenv = code_getenv - pe_text_off + pe_text_addr

code_main = 0x312a0
addr_main = code_main - pe_text_off + pe_text_addr

code_depfile_get_target = 0x347d0
code_depfile_get_target.end = 0x347dd
addr_depfile_get_target = code_depfile_get_target - pe_text_off + pe_text_addr

code_findexe = 0x34b40
code_findexe.end = 0x34d00
addr_findexe = code_findexe - pe_text_off + pe_text_addr

code_depfile_get_header = 0x52310
code_depfile_get_header.end = 0x52322
addr_depfile_get_header = code_depfile_get_header - pe_text_off + pe_text_addr

code_depfile_build = 0x52bd0
code_depfile_build.end = 0x52be1
addr_depfile_build = code_depfile_build - pe_text_off + pe_text_addr

addr_envp = 0xc2396c

addr_argc = 0xc24d60

addr_argv = 0xc25d00

.include "patch.i"
