.intel_syntax noprefix
.include "_macros.i"

func RegOpenKeyExA
.ifndef NDEBUG
    push [esp + 4 * 2]
    push offset 9f
    wcall printf
    add esp, 4 * 2
.endif

    mov eax, 2
    ret 4 * 5

.ifndef NDEBUG
9:
    .asciz "stub: RegOpenKeyExA: %s\n"
.endif

func RegQueryValueExA; die RegQueryValueExA

func RegCloseKey; die RegCloseKey
