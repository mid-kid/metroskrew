.intel_syntax noprefix

.global main
main:
    mov eax, [esp + 4]
    mov [main_argc], eax
    mov eax, [esp + 8]
    mov [main_argv], eax

    jmp dword ptr [_pe_start]
_pe_start: .long pe_start

.bss
.global main_argc
main_argc: .zero 4
.global main_argv
main_argv: .zero 4
