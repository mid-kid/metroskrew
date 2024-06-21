.intel_syntax noprefix
.include "port.i"

func main
    mov eax, [esp + 4]
    mov [main_argc], eax
    mov eax, [esp + 8]
    mov [main_argv], eax

    # Set up the FPU to reflect the default state in windows
    # Usually it's set to a different value due to the startfiles used
    #  in modern compilers, but this application needs lower precision.
    fldcw [cw]

    jmp dword ptr [_pe_start]

cw: .short 0x027f
_pe_start: .long pe_start

.bss
.global main_argc
main_argc: .zero 4
.global main_argv
main_argv: .zero 4
