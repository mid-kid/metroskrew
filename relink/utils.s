.intel_syntax noprefix

.global path_dup_unx
path_dup_unx:
    push ebx
    mov ebx, eax

    push ebx
    call strlen
    add esp, 4
    inc eax

    push eax
    call malloc
    add esp, 4

    push ebx
    push eax
    call strcpy
    add esp, 4 * 2

    mov ebx, eax
    call path_unx
    mov eax, ebx
    pop ebx
    ret

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
