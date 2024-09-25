# vim:ft=asm:
# This file is included from a generated patch.s
# It's in charge of applying the patches, given the offsets supplied within
#  the generated file.
.include "_port.i"

.intel_syntax noprefix

patch = pe_text_off
patch.end = pe_text_off
.macro patch, code, name
.fill patch.end - pe_text_off + pe_text - (.), 1, 0x90
incbin patch.end, (\code - patch.end)
patch = \code
patch.end = \code\().end
    \name
.endm
.macro patch_end
.fill patch.end - pe_text_off + pe_text - (.), 1, 0x90
incbin patch.end, (pe_text_off + pe_text_len - patch.end)
.endm

# This patch avoids the use of the "fs" register to access thread-local storage
# It's only used near the entrypoints, regular access is done through the
#  kernel32 functions TlsAlloc/Free() and TlsGet/SetValue()
.macro patch_fs
    xor eax, eax
    push eax
.endm

# This patch attempts to patch over the function that parses GetCommandLineA
#  to extract the argc/argv values and pass them into main().
# Since we bring in our own C runtime, the command line is already parsed and
#  can be assigned directly in this function, instead of going through an extra
#  round of potentially lossy conversion.
.macro patch_init_args
    mov eax, [main_argc]
    mov [addr_argc], eax
    mov eax, [main_argv]
    mov [addr_argv], eax
    ret
.endm

# This patch avoids a call to GetEnvironmentStrings(), which is used by the
#  target program's C runtime to initialize getenv(3) and setenv(3) data.
# We bring in our own C runtime, so we will use our own getenv(3).
.macro patch_init_envp
    ret
.endm

# Use our own getenv(3)
.macro patch_getenv
    wjmp patch_getenv
.endm

.macro patch_nop
    nop
.endm

.macro patch_memreuse01
    push eax
    wcall patch_memreuse01
    pop eax
.endm

# The actual code
.section .patch_pe_text, "ax"
pe_text:
    patch code_fs_1, patch_fs
    patch code_fs_2, patch_fs
    patch code_init_args, patch_init_args
    patch code_init_envp, patch_init_envp
    patch code_getenv, patch_getenv
.ifdef code_memreuse01
    patch code_memreuse01, patch_memreuse01
.endif
    patch_end

.section .rodata
.global memreuse01_arr; memreuse01_arr = addr_memreuse01_arr
.global memreuse01_len; memreuse01_len = addr_memreuse01_len
