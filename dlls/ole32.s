.intel_syntax noprefix
.include "_macros.i"

func CoInitialize
    stub CoInitialize
    xor eax, eax
    ret 4

func CoUninitialize; die CoUninitialize

func CoCreateInstance
    stub CoCreateInstance
    mov eax, 0x80000000
    ret 4 * 5

func CoTaskMemAlloc; die CoTaskMemAlloc
func CoTaskMemFree; die CoTaskMemFree
