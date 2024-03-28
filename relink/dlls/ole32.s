.intel_syntax noprefix
.include "dlls/macros.i"

.global CoInitialize
CoInitialize:
    stub CoInitialize
    mov eax, 0
    ret 4

.global CoUninitialize
CoUninitialize:
    die CoUninitialize

.global CoCreateInstance
CoCreateInstance:
    die CoCreateInstance

.global CoTaskMemAlloc
CoTaskMemAlloc:
    die CoTaskMemAlloc

.global CoTaskMemFree
CoTaskMemFree:
    die CoTaskMemFree
