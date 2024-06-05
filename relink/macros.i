.macro func name
.ifndef _WIN32
.global \name
\name:
.else
.global _\name
_\name:
.endif
.endm

.macro wcall name
.ifndef _WIN32
    call \name
.else
    call _\name
.endif
.endm

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

.macro trace name
.ifdef TRACE
    push offset trace_str\@
    wcall puts
    pop eax
    jmp trace_end\@
trace_str\@:
    .asciz "trace: \name"
trace_end\@:
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
