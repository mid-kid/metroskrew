# vim:ft=asm:
# This file is included from a generated patch.s
# It's in charge of applying the patches, given the offsets supplied within
#  the generated file.

.intel_syntax noprefix
.include "macros.i"

patch = pe_text_off
patch.end = pe_text_off
.macro patch, code, name
.fill patch.end - pe_text_off + pe_text - (.), 1, 0x90
incbin patch.end, (\code - patch.end)
patch = \code
patch.end = \code\().end
    \name
.endm
.macro patch_end
.fill patch.end - pe_text_off + pe_text - (.), 1, 0x90
incbin patch.end, (pe_text_off + pe_text_len - patch.end)
.endm

.macro patch_fs
    xor eax, eax
    push eax
.endm

.macro patch_init_args_old
    push ebx
    mov eax, [main_argc]
    mov ebx, [addr_argc]
    mov [ebx], eax
    mov eax, [main_argv]
    mov ebx, [addr_argv]
    mov [ebx], eax
    pop ebx
    ret
.endm

.macro patch_init_args
    mov eax, [main_argc]
    mov [addr_argc], eax
    mov eax, [main_argv]
    mov [addr_argv], eax
    ret
.endm

.macro patch_init_envp
    ret
.endm

.macro patch_getenv
    wjmp getenv
.endm

# The actual code
.section .patch_pe_text, "ax"
pe_text:
    patch code_fs_1, patch_fs
    patch code_fs_2, patch_fs
    patch code_init_args, patch_init_args
    patch code_init_envp, patch_init_envp
    patch code_getenv, patch_getenv
    patch_end
