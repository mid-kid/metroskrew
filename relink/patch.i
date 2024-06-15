.include "macros.i"

patch = 0
patch.end = 0
.macro patch, name
.fill patch.end - (. - pe_text), 1, 0x90
incbin (text_off + patch.end), (\name - patch.end)
patch = \name
patch.end = \name\().end
    \name
.endm
.macro patch_end
.fill patch.end - (. - pe_text), 1, 0x90
incbin (text_off + patch.end), (text_len - patch.end)
.endm

.macro patch_fs_1
    xor eax, eax
    push eax
.endm

.macro patch_fs_2
    xor eax, eax
    push eax
.endm

.macro patch_init_args
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

.macro patch_init_envp
    ret
.endm

.macro patch_getenv
    wjmp getenv
.endm
