.macro stub name
    mov eax, offset stub_str\@
    push eax
    call puts
    pop eax
    jmp stub_end\@
stub_str\@:
    .asciz "stub: \name"
stub_end\@:
.endm

.macro die name
    mov eax, offset die_str\@
    push eax
    call puts
    pop eax
    mov eax, 1
    jmp exit
die_str\@:
    .asciz "die: \name"
.endm
