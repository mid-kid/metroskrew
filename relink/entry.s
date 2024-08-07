.intel_syntax noprefix
.include "_port.i"

func main
    mov eax, [esp + 4]
    mov [main_argc], eax
    mov eax, [esp + 8]
    mov [main_argv], eax

    # Set up the FPU to reflect the default state in windows
    # Usually it's set to a different value due to the startfiles used
    #  in modern compilers, but this application needs lower precision.
    fldcw [cw]

    jmp offset pe_start

cw: .short 0x027f

.bss
.global main_argc
main_argc: .zero 4
.global main_argv
main_argv: .zero 4
