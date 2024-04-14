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

.ifndef NDEBUG
    push [ebp + 4 + 4 * 2]
    push [ebp + 4 + 4 * 1]
    push offset 9f
    call printf
.endif

    mov eax, [ebp + 4 + 4 * 3]
    mov byte ptr [eax], 0
    mov eax, 1

    leave
    ret 4 * 4

.ifndef NDEBUG
9:
    .asciz "HACK: LoadStringA: %d %d\n"
.endif
