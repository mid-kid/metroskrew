.intel_syntax noprefix

.global path_unx
path_unx:
1:
    inc eax
    cmp byte ptr [eax - 1], 0
    jz 1f
    cmp byte ptr [eax - 1], '\\'
    jnz 1b
    mov byte ptr [eax - 1], '/'
    jmp 1b
1:
    ret

.global path_dos
path_dos:
1:
    inc eax
    cmp byte ptr [eax - 1], 0
    jz 1f
    cmp byte ptr [eax - 1], '/'
    jnz 1b
    mov byte ptr [eax - 1], '\\'
    jmp 1b
1:
    ret
