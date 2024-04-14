.intel_syntax noprefix
.include "macros.i"

.global CoInitialize
CoInitialize:
    stub CoInitialize
    xor eax, eax
    ret 4

.global CoUninitialize
CoUninitialize:
    die CoUninitialize

.global CoCreateInstance
CoCreateInstance:
    stub CoCreateInstance
    mov eax, -1
    ret 4 * 5

.global CoTaskMemAlloc
CoTaskMemAlloc:
    die CoTaskMemAlloc

.global CoTaskMemFree
CoTaskMemFree:
    die CoTaskMemFree
