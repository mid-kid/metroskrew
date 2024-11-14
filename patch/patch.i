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

.macro patch_FUN_00505340
    wjmp FUN_00505340
.endm

.macro patch_depfile_build
    wjmp depfile_build
.endm

# The actual code
.section .patch_pe_text, "ax"
pe_text:
    patch code_fs_1, patch_fs
    patch code_fs_2, patch_fs
    patch code_init_args, patch_init_args
    patch code_init_envp, patch_init_envp
    patch code_getenv, patch_getenv
.ifdef code_depfile_build
    patch code_depfile_build, patch_depfile_build
.endif
.ifdef code_FUN_00505340
    patch code_FUN_00505340, patch_FUN_00505340
.endif
    patch_end

var prog_malloc addr_prog_malloc
var FUN_004f8b60 addr_FUN_004f8b60
var depfile_get_target addr_depfile_get_target
var depfile_get_header addr_depfile_get_header

var DAT_0063a798 addr_DAT_0063a798
var DAT_0063a828 addr_DAT_0063a828
var DAT_0063ccb0 addr_DAT_0063ccb0
var DAT_0063ccf0 addr_DAT_0063ccf0

.section .text
.ifdef code_depfile_build
    func depfile_struct__source;.int off_depfile_struct__source
    func depfile_struct__targets;.int off_depfile_struct__targets
    func depfile_struct__num_headers;.int off_depfile_struct__num_headers
    func depfile_struct__headers;.int off_depfile_struct__headers
.endif
