.intel_syntax noprefix
.include "dlls/macros.i"

.global ExitProcess
ExitProcess:
    push [esp + 4]
    jmp exit

.global IsBadReadPtr
IsBadReadPtr:
    die IsBadReadPtr

.global RtlUnwind
RtlUnwind:
    die RtlUnwind

.global GetCurrentProcess
GetCurrentProcess:
    mov eax, -1
    ret

.global DuplicateHandle
DuplicateHandle:
    stub DuplicateHandle
    mov eax, 1
    ret 4 * 7

.global GetLastError
GetLastError:
    call __errno_location
    mov eax, [eax]
    ret

.global GetStdHandle
GetStdHandle:
    mov eax, [esp + 4]
    neg eax
    sub eax, 10
    cmp eax, 3
    jae 1f
    ret 4

1:
    die GetStdHandle

# Multithread locks
.global InitializeCriticalSection
.global DeleteCriticalSection
.global EnterCriticalSection
.global LeaveCriticalSection
InitializeCriticalSection:
    ret 4
DeleteCriticalSection:
    ret 4
EnterCriticalSection:
    ret 4
LeaveCriticalSection:
    ret 4

.global FindFirstFileA
FindFirstFileA:
    die FindFirstFileA

.global GetFileAttributesA
GetFileAttributesA:
    push [esp + 4]
    push offset 1f
    call printf
    add esp, 4 * 2

    call __errno_location
    mov dword ptr [eax], 2
    mov eax, -1
    ret 4

1:
    .asciz "GetFileAttributesA: %s\n"

.global FindNextFileA
FindNextFileA:
    die FindNextFileA

.global FindClose
FindClose:
    die FindClose

.global GetCommandLineA
GetCommandLineA:
    stub GetCommandLineA
    mov eax, [main_argv]
    mov eax, [eax]
    ret

.global GetEnvironmentStrings
GetEnvironmentStrings:
    mov eax, environ
    ret

.global FreeEnvironmentStringsA
FreeEnvironmentStringsA:
    ret 4

.global GetCurrentDirectoryA
GetCurrentDirectoryA:
    push ebp
    mov ebp, esp

    push [ebp + 4 + 4 * 1]
    push [ebp + 4 + 4 * 2]
    call getcwd
    add esp, 4 * 2
    test eax, eax
    jz 1f

    push [ebp + 4 + 4 * 2]
    call strlen

    leave
    ret 4 * 2

# TODO: Return necessary amount of bytes if buffer too small
1:
    die GetCurrentDirectoryA

.global CreateProcessA
CreateProcessA:
    die CreateProcessA

.global WaitForSingleObject
WaitForSingleObject:
    die WaitForSingleObject

.global GetExitCodeProcess
GetExitCodeProcess:
    die GetExitCodeProcess

.global CloseHandle
CloseHandle:
    die CloseHandle

# Thread local storage
# Assuming no multithreading
.global TlsAlloc
.global TlsFree
.global TlsGetValue
.global TlsSetValue
TlsAlloc:
    push ebx
    push 4
    call malloc
    pop ebx
    pop ebx
    ret
TlsFree:
    push [esp + 4]
    call free
    pop eax
    mov eax, 1
    ret 4
TlsGetValue:
    mov eax, [esp + 4]
    mov eax, [eax]
    ret 4
TlsSetValue:
    mov eax, [esp + 4 * 1]
    mov ebx, [esp + 4 * 2]
    mov [eax], ebx
    mov eax, 1
    ret 4 * 2

.global GetModuleHandleA
GetModuleHandleA:
    die GetModuleHandleA

.global GetModuleFileNameA
GetModuleFileNameA:
    die GetModuleFileNameA

.global LoadLibraryA
LoadLibraryA:
    die LoadLibraryA

.global FreeLibrary
FreeLibrary:
    die FreeLibrary

.global GlobalAlloc
GlobalAlloc:
    push ebp
    mov ebp, esp

    push [ebp + 4 + 4 * 2]
    call malloc
    add esp, 4
    push eax

    # Handle 0x20, initializing the memory to zero
    test dword ptr [ebp + 4 + 4 * 1], 0x40
    jz 1f
    push [ebp + 4 + 4 * 2]
    push 0
    push [ebp - 4]
    call memset
    add esp, 4 * 3

1:
    # Handle the rest
    test dword ptr [ebp + 4 + 4 * 1], ~0x40
    jnz 8f

    pop eax
    leave
    ret 4 * 2

8:
    push [ebp + 4 + 4 * 1]
    push offset 9f
    call printf
    push 1
    jmp exit

9:
    .asciz "die: GlobalAlloc %04x\n"

.global GlobalFree
GlobalFree:
    push [esp + 4 * 1]
    call free
    pop eax
    mov eax, 0
    ret 4

.global GetFullPathNameA
GetFullPathNameA:
    push ebp
    mov ebp, esp
    push ebx

    push [ebp + 4 + 4 * 1]
    push offset 5f
    call printf
    add esp, 4 * 2

    # Check if the string starts at the root
    mov eax, [ebp + 4 + 4 * 1]
    mov al, [eax]
    cmp al, '\\'
    mov eax, 0
    jz 1f

    # If it's relative, add current directory
    push [ebp + 4 + 4 * 3]
    push [ebp + 4 + 4 * 2]
    call GetCurrentDirectoryA

    # Append slash
    mov ebx, [ebp + 4 + 4 * 3]
    mov byte ptr [ebx + eax], '/'
    inc eax
1:

    # Calculate full string length
    push eax
    push [ebp + 4 + 4 * 1]
    call strlen
    add esp, 4
    add eax, [esp]
    inc eax

    # Compare it to the buffer size
    cmp [ebp + 4 + 4 * 2], eax
    pop eax
    jc 1f

    # Set the file part pointer
    add eax, [ebp + 4 + 4 * 3]
    mov ebx, [ebp + 4 + 4 * 4]
    mov [ebx], eax

    # Append the filename
    push [ebp + 4 + 4 * 1]
    push eax
    call strcpy
    add esp, 4 * 2

2:
    mov bl, [eax]
    inc eax
    cmp bl, '\0'
    jz 2f
    cmp bl, '\\'
    jnz 2b
    mov byte ptr [eax - 1], '/'
    jmp 2b
2:
    sub eax, [ebp + 4 + 4 * 3]
    dec eax

1:
    pop ebx
    leave
    ret 4 * 4

5:
    .asciz "GetFullPathNameA: %s\n"

.global SetFilePointer
SetFilePointer:
    die SetFilePointer

.global WriteFile
WriteFile:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 4 + 4 * 1]
    neg eax
    push [ebp + 4 + 4 * 3]
    push [ebp + 4 + 4 * 2]
    push eax
    push offset 1f
    call printf
    pop eax
    call write
    leave
    ret 4 * 5

1:
    .asciz "WriteFile: %d %x %d\n"

.global ReadFile
ReadFile:
    die ReadFile

.global CreateFileA
CreateFileA:
    die CreateFileA

.global GetTickCount
GetTickCount:
    die GetTickCount

.global DeleteFileA
DeleteFileA:
    die DeleteFileA

.global MoveFileA
MoveFileA:
    die MoveFileA

.global FormatMessageA
FormatMessageA:
    die FormatMessageA

.global GetFileTime
GetFileTime:
    die GetFileTime

.global SetFileTime
SetFileTime:
    die SetFileTime

.global GetFileSize
GetFileSize:
    die GetFileSize

.global SetEndOfFile
SetEndOfFile:
    die SetEndOfFile

.global CreateDirectoryA
CreateDirectoryA:
    die CreateDirectoryA

.global RemoveDirectoryA
RemoveDirectoryA:
    die RemoveDirectoryA

.global SetStdHandle
SetStdHandle:
    die SetStdHandle

.global GetSystemTime
GetSystemTime:
    die GetSystemTime

.global SystemTimeToFileTime
SystemTimeToFileTime:
    die SystemTimeToFileTime

.global CompareFileTime
CompareFileTime:
    die CompareFileTime

.global GlobalReAlloc
GlobalReAlloc:
    die GlobalReAlloc

.global GlobalFlags
GlobalFlags:
    die GlobalFlags

.global FileTimeToSystemTime
FileTimeToSystemTime:
    die FileTimeToSystemTime

.global FindResourceA
FindResourceA:
    die FindResourceA

.global LoadResource
LoadResource:
    die LoadResource

.global LockResource
LockResource:
    die LockResource

.global SizeofResource
SizeofResource:
    die SizeofResource

.global CreateFileMappingA
CreateFileMappingA:
    die CreateFileMappingA

.global MapViewOfFile
MapViewOfFile:
    die MapViewOfFile

.global UnmapViewOfFile
UnmapViewOfFile:
    die UnmapViewOfFile

.global GetSystemDirectoryA
GetSystemDirectoryA:
    stub GetSystemDirectoryA
    mov eax, 0
    ret 4 * 2

.global GetWindowsDirectoryA
GetWindowsDirectoryA:
    stub GetWindowsDirectoryA
    mov eax, 0
    ret 4 * 2

.global SetConsoleCtrlHandler
SetConsoleCtrlHandler:
    stub SetConsoleCtrlHandler
    mov eax, 1
    ret 4 * 2

.global GetConsoleScreenBufferInfo
GetConsoleScreenBufferInfo:
    stub GetConsoleScreenBufferInfo
    mov eax, 0
    ret 4 * 2

.global SetFileAttributesA
SetFileAttributesA:
    die SetFileAttributesA

.global OpenFileMappingA
OpenFileMappingA:
    die OpenFileMappingA

.global MultiByteToWideChar
MultiByteToWideChar:
    die MultiByteToWideChar

.global IsValidCodePage
IsValidCodePage:
    die IsValidCodePage

.global GetACP
GetACP:
    die GetACP

.global GetLocalTime
GetLocalTime:
    die GetLocalTime

.global GetTimeZoneInformation
GetTimeZoneInformation:
    die GetTimeZoneInformation
