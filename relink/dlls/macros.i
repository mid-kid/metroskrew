.macro stub name
.ifndef NDEBUG
    push offset stub_str\@
    call puts
    pop eax
    jmp stub_end\@
stub_str\@:
    .asciz "stub: \name"
stub_end\@:
.endif
.endm

.macro trace name
.ifdef TRACE
    push offset stub_str\@
    call puts
    pop eax
    jmp stub_end\@
stub_str\@:
    .asciz "trace: \name"
stub_end\@:
.endif
.endm

.macro die name
    push offset die_str\@
    call puts
    push 1
    jmp exit
die_str\@:
    .asciz "die: \name"
.endm
