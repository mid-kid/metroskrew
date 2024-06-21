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
