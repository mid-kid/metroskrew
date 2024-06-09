.intel_syntax noprefix

.section .patch_pe_text, "ax"
pe_text:

text_off = 0x400
text_len = 0x19898a

addr_argc = pe_text + 0x8a + 2
addr_argv = pe_text + 0x84 + 2

patch_fs_1 = 0xf
patch_fs_1.end = 0x1d
patch_fs_2 = 0x39
patch_fs_2.end = 0x47
patch_init_args = 0x21b0
patch_init_args.end = 0x24a0

.incbin "bins/mwccarm.exe", text_off, patch_fs_1

    xor eax, eax
    push eax

.fill patch_fs_1.end - (. - pe_text), 1, 0x90
.incbin "bins/mwccarm.exe", text_off + patch_fs_1.end, patch_fs_2 - patch_fs_1.end

    xor eax, eax
    push eax

.fill patch_fs_2.end - (. - pe_text), 1, 0x90
.incbin "bins/mwccarm.exe", text_off + patch_fs_2.end, patch_init_args - patch_fs_2.end

# void init_args()
    push ebx
    mov eax, [main_argc]
    mov ebx, [addr_argc]
    mov [ebx], eax
    mov eax, [main_argv]
    mov ebx, [addr_argv]
    mov [ebx], eax
    pop ebx
    ret

.fill patch_init_args.end - (. - pe_text), 1, 0x90
.incbin "bins/mwccarm.exe", text_off + patch_init_args.end, text_len - patch_init_args.end
