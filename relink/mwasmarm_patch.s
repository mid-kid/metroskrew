.intel_syntax noprefix

.section .patch_pe_text, "ax"
pe_text:

text_off = 0x400
text_len = 0x5a200

addr_argc = pe_text + 0x2a05a + 2
addr_argv = pe_text + 0x2a054 + 2

patch_fs_1 = 0x29fdf
patch_fs_1.end = 0x29fed
patch_fs_2 = 0x2a009
patch_fs_2.end = 0x2a017
patch_init_args = 0x30770
patch_init_args.end = 0x30a60

.incbin "bins/mwasmarm.exe", text_off, patch_fs_1

    xor eax, eax
    push eax

.fill patch_fs_1.end - (. - pe_text), 1, 0x90
.incbin "bins/mwasmarm.exe", text_off + patch_fs_1.end, patch_fs_2 - patch_fs_1.end

    xor eax, eax
    push eax

.fill patch_fs_2.end - (. - pe_text), 1, 0x90
.incbin "bins/mwasmarm.exe", text_off + patch_fs_2.end, patch_init_args - patch_fs_2.end

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
.incbin "bins/mwasmarm.exe", text_off + patch_init_args.end, text_len - patch_init_args.end
