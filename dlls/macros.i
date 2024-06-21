# vim:ft=asm:
.include "port.i"

.macro stub name
.ifndef NDEBUG
    push offset stub_str\@
    wcall puts
    pop eax
    jmp stub_end\@
stub_str\@:
    .asciz "stub: \name"
stub_end\@:
.endif
.endm

.macro die name
    push offset die_str\@
    wcall puts
    push 1
    wcall exit
die_str\@:
    .asciz "die: \name"
.endm
