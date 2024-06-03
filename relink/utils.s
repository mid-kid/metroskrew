.intel_syntax noprefix

.global path_dup_unx_c
path_dup_unx_c:
    mov eax, [esp + 4]
    call path_dup_unx
    ret

.global path_dup_unx
path_dup_unx:
    push ebx
    mov ebx, eax

    cmp word ptr [ebx], 0x3a5a  # Z:
    jnz 1f
    cmp byte ptr [ebx + 2], '\\'
    jnz 1f
    add ebx, 2
1:

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

# Concatenate [ebx] with [eax], with a path separator
.global path_join
path_join:
    push eax  # ext
    call strlen
    push eax  # len_ext
    push ebx  # base
    call strlen
    mov [esp], eax  # len_base
    add eax, [esp + 4 * 2]  # len_ext
    add eax, 2  # '/' and '\0'
    push ebx  # base
    push eax
    call malloc
    mov [esp], eax  # dst
    call strcpy
    pop ebx  # dst
    add esp, 4  # base
    pop eax  # len_base
    add eax, ebx
    mov byte ptr [eax], '/'
    add eax, 1
    mov [esp], eax
    call strcpy
    add esp, 4 * 2
    mov eax, ebx
    ret
