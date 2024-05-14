.intel_syntax noprefix
.include "macros.i"

.global MessageBoxA
MessageBoxA:
    push ebp
    mov	ebp, esp
    push [ebp + 4 + 4 * 2]
    push [ebp + 4 + 4 * 3]
    push offset 1f
    call printf
    mov	eax, 1
    leave
    ret	4 * 4
1:
    .asciz "%s: %s\n"

.global LoadStringA
LoadStringA:
    push ebp
    mov ebp, esp
    push ebx

    # Make sure it's not trying to grab strings from a different instance
    mov eax, [ebp + 4 + 4 * 1]  # hInstance
    cmp eax, -1  # Bypass hack return value from LoadLibraryA
    jnz 1f
    inc eax
1:
    and eax, eax
    jnz 9f

    # Get the ID, divided by 16
    mov eax, [ebp + 4 + 4 * 2]  # uID
    shr eax, 4
    inc eax

    # Look it up in the table
    mov ebx, offset pe_rsrc_strings
1:
    cmp dword ptr [ebx], 0
    jz 5f

    cmp [ebx], eax
    jz 1f
    add ebx, 4 * 2
    jmp 1b
1:
    mov ebx, [ebx + 4]

    # Find the Nth string in the 16-string bundle
    mov eax, [ebp + 4 + 4 * 2]  # uID
    and eax, 0xf
1:
    and eax, eax
    jz 1f
    push ebx
    movzx ebx, word ptr [ebx]
    inc ebx
    add ebx, ebx
    add ebx, [esp]
    add esp, 4
    dec eax
    jmp 1b
1:

    # Pick the smaller of the two sizes (string size, buffer size)
    push ebx
    mov eax, [ebp + 4 + 4 * 4]  # cchBufferMax
    dec eax  # leave space for zero terminator
    jc 5f
    movzx ebx, word ptr [ebx]
    cmp eax, ebx
    jc 1f
    mov eax, ebx
1:
    pop ebx

    # Store the return value
    push eax

    # "Convert" the resulting string into ASCII by truncation
    inc eax
    push eax  # Counter value
    push ebx  # src buffer
    mov ebx, [ebp + 4 + 4 * 3]  # lpBuffer

1:
    # Check if we're done
    dec dword ptr [esp + 4]
    jz 1f

    # Copy one byte and advance
    add word ptr [esp], 2
    mov eax, [esp]
    mov ax, [eax]
    mov [ebx], al
    inc ebx
    jmp 1b
1:
    # Append zero byte
    mov byte ptr [ebx], 0

    # Get return value
    add esp, 4 * 2
    pop eax

1:
    pop ebx
.ifdef TRACE
    push [ebp + 4 + 4 * 3]  # lpBuffer
    push [ebp + 4 + 4 * 2]  # uID
    push eax
    push offset 8f
    call printf
    add esp, 4
    pop eax
.endif

    leave
    ret 4 * 4

# Generic error
5:
    xor eax, eax
    jmp 1b

.ifdef TRACE
8:
    .asciz "trace: LoadStringA: res=%d uID=%d lpBuffer='%s'\n"
.endif

9:
    push [ebp + 4 + 4 * 2]  # uID
    push [ebp + 4 + 4 * 1]  # hInstance
    push offset 1f
    call printf
    add esp, 4 * 3
    push 1
    call exit

1:
    .asciz "die: LoadStringA: hInstance=%d uID=%d\n"
