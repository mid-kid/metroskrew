.macro stat arg, fun=stat
    sub esp, [sys_stat_sizeof]
    push esp
    push \arg
    wcall \fun
    add esp, 4 * 2
.endm

.macro stat_get name, struct=esp
    mov eax, [sys_stat_\name\()_offsetof]
    mov eax, [\struct + eax]
.endm

.macro stat_pop
    add esp, [sys_stat_sizeof]
.endm
