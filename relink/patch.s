.intel_syntax noprefix

.section .patch_pe_text, "ax"
pe_text:
.incbin "mwccarm.exe", 0x400, 0xf

    mov eax, 0
    push eax
.fill 0x1d - (. - pe_text), 1, 0x90
.incbin "mwccarm.exe", 0x400 + 0x1d, 0x39 - 0x1d

    mov eax, 0
    push eax
.fill 0x47 - (. - pe_text), 1, 0x90
.incbin "mwccarm.exe", 0x400 + 0x47, 0x198a00 - 0x47
