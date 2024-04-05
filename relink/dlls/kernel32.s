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
    push ebp
    mov ebp, esp
    push ebx

    # Check if it's an stdio handle
    mov eax, [ebp + 4 + 4 * 2]
    dec eax
    cmp eax, 3
    jae 8f
    inc eax

    # Store the handle
    mov ebx, [ebp + 4 + 4 * 4]
    test ebx, ebx
    jz 8f
    mov [ebx], eax

    mov eax, 1
    pop ebx
    leave
    ret 4 * 7

# TODO: Actually duplicate non-stdio handles
8:
    push [ebp + 4 + 4 * 2]
    push offset 9f
    call printf
    add esp, 4 * 2
    push 1
    jmp exit

9:
    .asciz "die: DuplicateHandle: %d\n"

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
    inc eax
    ret 4

1:
    mov eax, -1
    ret 4

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
    push ebp
    mov ebp, esp
    push ebx

    mov eax, [ebp + 4 + 4]
    call path_dup_unx
    push eax
    push offset 9f
    call printf
    pop eax
    pop eax

    push 0  # F_OK
    push eax
    call access
    mov ebx, eax
    call free
    add esp, 4 * 2
    mov eax, ebx

    test eax, eax
    jnz 1f
    mov eax, 0x80  # FILE_ATTRIBUTE_NORMAL
1:

    pop ebx
    leave
    ret 4

9:
    .asciz "stub: GetFileAttributesA: only presence: %s\n"

.global FindNextFileA
FindNextFileA:
    die FindNextFileA

.global FindClose
FindClose:
    die FindClose

.global GetCommandLineA
GetCommandLineA:
    stub "GetCommandLineA: only program name"
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
    push offset 9f
    call printf

    mov eax, [ebp + 4 + 4 * 2]
    call path_dos

    push [ebp + 4 + 4 * 2]
    call strlen

    leave
    ret 4 * 2

# TODO: Return necessary amount of bytes if buffer too small
1:
    die "GetCurrentDirectoryA: Buffer too small"

9:
    .asciz "GetCurrentDirectoryA: %s\n"

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
    mov eax, [esp + 4]
    dec eax
    push eax
    call close
    add esp, 4
    inc eax
    ret 4

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
    push [esp + 4]
    push offset 1f
    call printf
    add esp, 4 * 2

    xor eax, eax
    ret 4

1:
    .asciz "stub: GetModuleHandleA: %s\n"

.global GetModuleFileNameA
GetModuleFileNameA:
    push [esp + 4 * 1]
    push offset 1f
    call printf
    add esp, 4 * 2

    xor eax, eax
    ret 4 * 3

1:
    .asciz "stub: GetModuleFileNameA: %d\n"

.global LoadLibraryA
LoadLibraryA:
    push [esp + 4]
    push offset 1f
    call printf
    add esp, 4 * 2

    mov eax, -1
    ret 4

1:
    .asciz "HACK: LoadLibraryA: %s\n"

.global FreeLibrary
FreeLibrary:
    push [esp + 4]
    push offset 1f
    call printf
    add esp, 4 * 2

    mov eax, 1
    ret 4

1:
    .asciz "HACK: FreeLibrary: %d\n"

.global GlobalAlloc
GlobalAlloc:
    push ebp
    mov ebp, esp
    push ebx

    # Always allocate 4 more bytes, for size
    mov eax, [ebp + 4 + 4 * 2]
    add eax, 4
    push eax
    call malloc
    add esp, 4

    # Store original size
    mov ebx, [ebp + 4 + 4 * 2]
    mov [eax], ebx
    add eax, 4
    push eax

    # Handle 0x20, initializing the memory to zero
    test dword ptr [ebp + 4 + 4 * 1], 0x40
    jz 1f

    push [ebp + 4 + 4 * 2]
    push 0
    push eax
    call memset
    add esp, 4 * 3

1:
    # Handle the rest
    test dword ptr [ebp + 4 + 4 * 1], ~0x40
    jnz 8f

    pop eax
    pop ebx
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
    mov eax, [esp + 4]
    sub eax, 4
    push eax
    call free
    pop eax
    xor eax, eax
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
    mov ebx, 0
    jz 1f

    # If it's relative, add current directory
    push [ebp + 4 + 4 * 3]
    push [ebp + 4 + 4 * 2]
    call GetCurrentDirectoryA

    # Append slash
    mov ebx, eax
    mov eax, [ebp + 4 + 4 * 3]
    mov byte ptr [eax + ebx], '\\'
    inc ebx
1:

    # Calculate full string length
    push ebx
    push [ebp + 4 + 4 * 1]
    call strlen
    add esp, 4 * 2
    add eax, ebx
    push eax  # Return value

    # Compare it to the buffer size
    inc eax
    cmp [ebp + 4 + 4 * 2], eax
    jc 1f

    # Set the file part pointer
    mov eax, ebx
    add eax, [ebp + 4 + 4 * 3]
    mov ebx, [ebp + 4 + 4 * 4]
    mov [ebx], eax

    # Append the filename
    push [ebp + 4 + 4 * 1]
    push eax
    call strcpy
    add esp, 4 * 2

    pop eax
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
    dec eax
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
    push ebp
    mov ebp, esp
    push ebx

    push [ebp + 4 + 4 * 1]  # lpFileName
    push offset 8f
    call printf
    add esp, 4 * 2

    # Cover for unsupported functionality
    mov eax, [ebp + 4 + 4 * 7]  # hTemplateFile
    and eax, eax
    jnz 9f
    mov eax, [ebp + 4 + 4 * 6]  # dwFlagsAndAttributes
    and eax, ~0x80  # FILE_ATTRIBUTE_NORMAL
    jnz 9f
    mov eax, [ebp + 4 + 4 * 4]  # lpSecurityAttributes
    and eax, eax
    jnz 9f

    # Ignored, can't action on this
    #mov eax, [ebp + 4 + 4 * 3]  # dwShareMode

    mov eax, [ebp + 4 + 4 * 5]  # dwCreationDisposition
    and eax, eax
    jz 9f
    cmp eax, 6
    jnc 9f
    mov eax, [offset 5f + (eax - 1) * 4]
    push eax

    mov eax, [ebp + 4 + 4 * 2]  # dwDesiredAccess
    mov ebx, 1
    cmp eax, 0x40000000
    jz 1f
    inc ebx
    cmp eax, 0x80000000
    jz 1f
    inc ebx
    cmp eax, 0xc0000000
    jnz 9f
1:
    mov ebx, [offset 6f + (ebx - 1) * 4]
    pop eax
    or eax, ebx

    pop ebx

    push 0777  # mode
    push eax  # flags
    push [ebp + 4 + 4 * 1]  # lpFileName
    call open
    leave
    ret 4 * 7

5:
    # dwCreationDisposition:
    .long 00300  # CREATE_NEW = O_CREAT | O_EXCL
    .long 01100  # CREATE_ALWAYS = O_CREAT | O_TRUNC
    .long 00000  # OPEN_EXISTING = 0
    .long 00100  # OPEN_ALWAYS = O_CREAT
    .long 01000  # TRUNCATE_EXISTING = O_TRUNC

6:
    # dwDesiredAccess
    .long 01  # GENERIC_WRITE = O_WRONLY
    .long 00  # GENERIC_READ = O_RDONLY
    .long 02  # GENERIC_WRITE | GENERIC_READ = O_RDWR

8:
    .asciz "CreateFileA: %s\n"

9:
    push [ebp + 4 + 4 * 7]  # hTemplateFile
    push [ebp + 4 + 4 * 6]  # dwFlagsAndAttributes
    push [ebp + 4 + 4 * 5]  # dwCreationDisposition
    push [ebp + 4 + 4 * 4]  # lpSecurityAttributes
    push [ebp + 4 + 4 * 3]  # dwShareMode
    push [ebp + 4 + 4 * 2]  # dwDesiredAccess
    push [ebp + 4 + 4 * 1]  # lpFileName
    push offset 1f
    call printf
    push 1
    jmp exit

1:
    .asciz "die: CreateFileA: '%s' %x %x %x %x %x %d\n"

.global GetTickCount
GetTickCount:
    stub GetTickCount
    xor eax, eax
    ret

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
    stub GetSystemTime

    mov eax, [esp + 4]
    mov word ptr [eax + 2 * 0], 0  # wYear
    mov word ptr [eax + 2 * 1], 0  # wMonth
    mov word ptr [eax + 2 * 2], 0  # wDayOfWeek
    mov word ptr [eax + 2 * 3], 0  # wDay
    mov word ptr [eax + 2 * 4], 0  # wHour
    mov word ptr [eax + 2 * 5], 0  # wMinute
    mov word ptr [eax + 2 * 6], 0  # wSecond
    mov word ptr [eax + 2 * 7], 0  # wMilliseconds
    ret 4

.global SystemTimeToFileTime
SystemTimeToFileTime:
    stub SystemTimeToFileTime

    mov eax, [esp + 4 * 2]
    mov dword ptr [eax + 4 * 0], 0  # dwLowDateTime
    mov dword ptr [eax + 4 * 1], 0  # dwHighDateTime
    ret 4 * 2

.global CompareFileTime
CompareFileTime:
    die CompareFileTime

.global GlobalReAlloc
GlobalReAlloc:
    push ebp
    mov ebp, esp
    push ebx
    push ecx

    # Account for the extra bytes
    mov eax, [ebp + 4 + 4 * 2]
    add eax, 4
    push eax
    mov eax, [ebp + 4 + 4 * 1]
    sub eax, 4
    push eax
    call realloc
    add esp, 4 * 2

    # Store old size in ecx
    mov ecx, [eax]
    mov ebx, [ebp + 4 + 4 * 2]
    mov [eax], ebx
    add eax, 4
    push eax

    # Handle 0x20, initializing the memory to zero
    test dword ptr [ebp + 4 + 4 * 3], 0x40
    jz 1f

    mov ebx, [ebp + 4 + 4 * 2]
    sub ebx, ecx
    jc 1f
    add eax, ecx

    push ebx
    push 0
    push eax
    call memset
    add esp, 4 * 3

1:
    # Handle the rest
    test dword ptr [ebp + 4 + 4 * 3], ~0x42
    jnz 8f

    pop eax
    pop ecx
    pop ebx
    leave
    ret 4 * 3

8:
    push [ebp + 4 + 4 * 3]
    push offset 9f
    call printf
    push 1
    jmp exit

9:
    .asciz "die: GlobalReAlloc %04x\n"

.global GlobalFlags
GlobalFlags:
    xor eax, eax
    ret 4

.global FileTimeToSystemTime
FileTimeToSystemTime:
    die FileTimeToSystemTime

.global FindResourceA
FindResourceA:
    push ebp
    mov ebp, esp

    push [ebp + 4 + 4 * 3]
    push [ebp + 4 + 4 * 2]
    push [ebp + 4 + 4 * 1]
    push offset 1f
    call printf

    xor eax, eax
    leave
    ret 4 * 3

1:
    .asciz "stub: FindResourceA: %d '%s' '%s'\n"

.global LoadResource
LoadResource:
    push ebp
    mov ebp, esp

    push [ebp + 4 + 4 * 2]
    push [ebp + 4 + 4 * 1]
    push offset 1f
    call printf

    xor eax, eax
    leave
    ret 4 * 2

1:
    .asciz "stub: LoadResource: %d %d\n"

.global LockResource
LockResource:
    push ebp
    mov ebp, esp

    push [ebp + 4 + 4]
    push offset 1f
    call printf

    xor eax, eax
    leave
    ret 4

1:
    .asciz "stub: LockResource: %d\n"

.global SizeofResource
SizeofResource:
    push ebp
    mov ebp, esp

    push [ebp + 4 + 4 * 2]
    push [ebp + 4 + 4 * 1]
    push offset 1f
    call printf

    xor eax, eax
    leave
    ret 4 * 2

1:
    .asciz "stub: SizeofResource: %d %d\n"

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
