.macro stat arg, fun=stat
    sub esp, [sys_stat_sizeof]
    push esp
    push \arg
    call \fun
    add esp, 4 * 2
.endm

.macro stat_pop
    add esp, [sys_stat_sizeof]
.endm
