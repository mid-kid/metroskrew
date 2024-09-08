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
    fninit
    fnclex
    fldcw cw_x87
    ldmxcsr cw_sse

    mov eax, 0
    mov ebx, 0
    mov ecx, 0
    mov edx, 0

    jmp offset pe_start

cw_x87: .short 0x027f
cw_sse: .long 0x1f80

.bss
.global main_argc
main_argc: .zero 4
.global main_argv
main_argv: .zero 4
