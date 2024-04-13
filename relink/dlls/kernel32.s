.intel_syntax noprefix
.include "dlls/macros.i"

.global ExitProcess
ExitProcess:
    push [esp + 4]
    push offset 9f
    call printf
    pop eax
    call exit

9:
    .asciz "trace: ExitProcess: uExitCode=%d\n"

.global IsBadReadPtr
IsBadReadPtr:
    die IsBadReadPtr

.global RtlUnwind
RtlUnwind:
    die RtlUnwind

.global GetCurrentProcess
GetCurrentProcess: trace GetCurrentProcess
    mov eax, -1
    ret

.global DuplicateHandle
DuplicateHandle: trace DuplicateHandle
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
.ifdef TRACE
    push eax
    push offset 9f
    call printf
    pop eax
    pop eax
.endif
    ret

.ifdef TRACE
9:
    .asciz "trace: GetLastError: res=%d\n"
.endif

.global GetStdHandle
GetStdHandle: trace GetStdHandle
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
InitializeCriticalSection: #trace InitializeCriticalSection
    ret 4
DeleteCriticalSection: #trace DeleteCriticalSection
    ret 4
EnterCriticalSection: #trace EnterCriticalSection
    ret 4
LeaveCriticalSection: #trace LeaveCriticalSection
    ret 4

.global FindFirstFileA
FindFirstFileA:
    push ebp
    mov ebp, esp
.ifdef TRACE
    push [ebp + 4 + 4 * 1]
    push offset 9f
    call printf
    add esp, 4 * 2
.endif

    die FindFirstFileA
    leave
    ret 4 * 2

.ifdef TRACE
9:
    .asciz "trace: FindFirstFileA: lpFileName=%s\n"
.endif

.global GetFileAttributesA
GetFileAttributesA:
    push ebp
    mov ebp, esp
    push ebx

    mov eax, [ebp + 4 + 4]
    call path_dup_unx
.ifndef NDEBUG
    push eax
    push offset 9f
    call printf
    pop eax
    pop eax
.endif

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

.ifdef TRACE
    push [ebp + 4 + 4]
    push eax
    push offset 8f
    call printf
    pop eax
    pop eax
    add esp, 4 * 1
.endif

    pop ebx
    leave
    ret 4

.ifdef TRACE
8:
    .asciz "trace: GetFileAttributesA: res=%x lpFileName=%s\n"
.endif

.ifndef NDEBUG
9:
    .asciz "stub: GetFileAttributesA: only presence: %s\n"
.endif

.global FindNextFileA
FindNextFileA:
    die FindNextFileA

.global FindClose
FindClose:
    die FindClose

.global GetCommandLineA
GetCommandLineA:
    die GetCommandLineA

.global GetEnvironmentStrings
GetEnvironmentStrings: #trace GetEnvironmentStrings
    mov eax, environ
    ret

.global FreeEnvironmentStringsA
FreeEnvironmentStringsA: #trace FreeEnvironmentStringsA
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

.ifndef NDEBUG
    push [ebp + 4 + 4 * 2]
    push offset 9f
    call printf
    add esp, 4 * 2
.endif

    mov eax, [ebp + 4 + 4 * 2]
    call path_dos

    push [ebp + 4 + 4 * 2]
    call strlen

    leave
    ret 4 * 2

1:
    die "GetCurrentDirectoryA: Buffer too small"

.ifndef NDEBUG
9:
    .asciz "GetCurrentDirectoryA: %s\n"
.endif

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
    mov eax, [esp + 4]  # hObject
    dec eax

    # Make sure to not close stdio descriptors
    push eax
    cmp eax, 3
    jc 2f
    call close
    jmp 1f
2:
    xor eax, eax
1:
    inc eax

.ifdef TRACE
    push eax
    push offset 9f
    call printf
    pop eax
    pop eax
.endif
    add esp, 4
    ret 4

.ifdef TRACE
9:
    .asciz "trace: CloseHandle: res=%d hObject=%d\n"
.endif

# Thread local storage
# Assuming no multithreading
.global TlsAlloc
.global TlsFree
.global TlsGetValue
.global TlsSetValue
TlsAlloc: #trace TlsAlloc
    push ebx
    push 4
    call malloc
    pop ebx
    pop ebx
    ret
TlsFree: #trace TlsFree
    push [esp + 4]
    call free
    pop eax
    mov eax, 1
    ret 4
TlsGetValue: #trace TlsGetValue
    mov eax, [esp + 4]
    mov eax, [eax]
    ret 4
TlsSetValue: #trace TlsSetValue
    mov eax, [esp + 4 * 1]
    mov ebx, [esp + 4 * 2]
    mov [eax], ebx
    mov eax, 1
    ret 4 * 2

.global GetModuleHandleA
GetModuleHandleA:
.ifndef NDEBUG
    push [esp + 4]
    push offset 9f
    call printf
    add esp, 4 * 2
.endif

    xor eax, eax
    ret 4

.ifndef NDEBUG
9:
    .asciz "stub: GetModuleHandleA: %s\n"
.endif

.global GetModuleFileNameA
GetModuleFileNameA:
.ifndef NDEBUG
    push [esp + 4 * 1]
    push offset 9f
    call printf
    add esp, 4 * 2
.endif

    xor eax, eax
    ret 4 * 3

.ifndef NDEBUG
9:
    .asciz "stub: GetModuleFileNameA: %d\n"
.endif

.global LoadLibraryA
LoadLibraryA:
.ifndef NDEBUG
    push [esp + 4]
    push offset 1f
    call printf
    add esp, 4 * 2
.endif

    mov eax, -1
    ret 4

.ifndef NDEBUG
1:
    .asciz "HACK: LoadLibraryA: %s\n"
.endif

.global FreeLibrary
FreeLibrary:
.ifndef NDEBUG
    push [esp + 4]
    push offset 1f
    call printf
    add esp, 4 * 2
.endif

    mov eax, 1
    ret 4

.ifndef NDEBUG
1:
    .asciz "HACK: FreeLibrary: %d\n"
.endif

.global GlobalAlloc
GlobalAlloc: #trace GlobalAlloc
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
GlobalFree: #trace GlobalFree
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

.ifndef NDEBUG
    push [ebp + 4 + 4 * 1]
    push offset 9f
    call printf
    add esp, 4 * 2
.endif

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

.ifndef NDEBUG
9:
    .asciz "GetFullPathNameA: %s\n"
.endif

.global SetFilePointer
SetFilePointer:
    mov eax, [esp + 4 * 3]
    and eax, eax
    jnz 9f

    push [esp + 4 * 0 + 4 * 4]  # whence = dwMoveMethod (the values match)
    push [esp + 4 * 1 + 4 * 2]  # offset = lDistanceToMove
    mov eax, [esp + 4 * 2 + 4 * 1]  # fd = hFile
    dec eax
    push eax  # fd
    call lseek
.ifdef TRACE
    push eax
    push offset 8f
    call printf
    pop eax
    pop eax
.endif
    add esp, 4 * 3
    # Return value is exactly the same
    ret 4 * 4

.ifdef TRACE
8:
    .asciz "trace: SetFilePointer: res=%d hFile=%d lDistanceToMove=%d dwMoveMethod=%d\n"
.endif

9:
    die "SetFilePointer: Only 32 bits"

.global WriteFile
WriteFile:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 4 + 4 * 5]  # lpOverlapped
    and eax, eax
    jnz 9f

    mov eax, [ebp + 4 + 4 * 4]  # lpNumberOfBytesWritten
    and eax, eax
    jz 1f
    mov dword ptr [eax], 0
1:

    push [ebp + 4 + 4 * 3]  # nNumberOfBytesToWrite
    push [ebp + 4 + 4 * 2]  # lpBuffer
    mov eax, [ebp + 4 + 4 * 1]  # hFile
    dec eax
    push eax
    call write

    cmp eax, -1
    jz 2f
    push ebx
    mov ebx, [ebp + 4 + 4 * 4]  # lpNumberOfBytesWritten
    and ebx, ebx
    jz 1f
    mov [ebx], eax
1:
    pop ebx
    xor eax, eax
2:
    inc eax

.ifdef TRACE
    push eax
    push [ebp + 4 + 4 * 3]  # nNumberOfBytesToWrite
    mov eax, [ebp + 4 + 4 * 1]  # hFile
    dec eax
    push eax
    push [esp + 4 * 2]  # res
    push offset 8f
    call printf
    add esp, 4 * 4
    pop eax
.endif

    leave
    ret 4 * 5

.ifdef TRACE
8:
    .asciz "trace: WriteFile: res=%d hFile=%d nNumberOfBytesToWrite=%d\n"
.endif

9:
    die WriteFile

.global ReadFile
ReadFile: trace ReadFile
    push ebp
    mov ebp, esp

    mov eax, [ebp + 4 + 4 * 5]
    and eax, eax
    jnz 9f

    mov eax, [ebp + 4 + 4 * 4]
    and eax, eax
    jz 1f
    mov dword ptr [eax], 0
1:

    push [ebp + 4 + 4 * 3]
    push [ebp + 4 + 4 * 2]
    mov eax, [ebp + 4 + 4 * 1]
    dec eax
    push eax
    call read

    cmp eax, -1
    jz 2f
    push ebx
    mov ebx, [ebp + 4 + 4 * 4]
    and ebx, ebx
    jz 1f
    mov [ebx], eax
1:
    pop ebx
2:

    inc eax
    leave
    ret 4 * 5

9:
    die ReadFile

.global CreateFileA
CreateFileA:
    push ebp
    mov ebp, esp
    push ebx

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

    push 0666  # mode
    push eax  # flags
    mov eax, [ebp + 4 + 4 * 1]  # lpFileName
    call path_dup_unx
    push eax
    call open
    inc eax
    push eax

    push [esp + 4]
.ifndef NDEBUG
    push offset 8f
    call printf
    pop eax
.endif
    call free
    pop eax

    pop eax
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

.ifndef NDEBUG
8:
    .asciz "CreateFileA: %s\n"
.endif

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
    mov eax, [esp + 4]
    call path_dup_unx
    push eax
.ifndef NDEBUG
    push offset 9f
    call printf
    pop eax
.endif
    call unlink
    push eax
    push [esp + 4]
    call free
    pop eax
    pop eax
    add esp, 4
    inc eax
    ret 4

.ifndef NDEBUG
9:
    .asciz "DeleteFileA: %s\n"
.endif

.global MoveFileA
MoveFileA:
    die MoveFileA

.global FormatMessageA
FormatMessageA:
    push ebp
    mov ebp, esp

.ifndef NDEBUG
    push [ebp + 4 + 4 * 7]  # Arguments
    push [ebp + 4 + 4 * 6]  # nSize
    push [ebp + 4 + 4 * 5]  # lpBuffer
    push [ebp + 4 + 4 * 4]  # dwLanguageId
    push [ebp + 4 + 4 * 3]  # dwMessageId
    push [ebp + 4 + 4 * 2]  # lpSource
    push [ebp + 4 + 4 * 1]  # dwFlags
    push offset 9f
    call printf
    add esp, 4 * 8
.endif

    xor eax, eax
    leave
    ret 4 * 7

.ifndef NDEBUG
9:
    .asciz "stub: FormatMessageA: %x %x %d %d %x %d %x\n"
.endif

.global GetFileTime
GetFileTime:
    stub GetFileTime

    mov eax, [esp + 4 * 2]  # lpCreationTime
    and eax, eax
    jz 1f
    mov dword ptr [eax + 4 * 0], 0  # dwLowDateTime
    mov dword ptr [eax + 4 * 1], 0  # dwHighDateTime
1:
    mov eax, [esp + 4 * 2]  # lpLastAccessTime
    and eax, eax
    jz 1f
    mov dword ptr [eax + 4 * 0], 0  # dwLowDateTime
    mov dword ptr [eax + 4 * 1], 0  # dwHighDateTime
1:
    mov eax, [esp + 4 * 2]  # lpLastWriteTime
    and eax, eax
    jz 1f
    mov dword ptr [eax + 4 * 0], 0  # dwLowDateTime
    mov dword ptr [eax + 4 * 1], 0  # dwHighDateTime
1:
    ret 4 * 4

.global SetFileTime
SetFileTime:
    die SetFileTime

.global GetFileSize
GetFileSize:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 4 + 4 * 2]  # lpFileSizeHigh
    and eax, eax
    jnz 9f

    mov eax, [ebp + 4 + 4 * 1]
    dec eax
    push eax  # fd

    # Get cur position
    push 1  # SEEK_CUR
    push 0
    push [ebp - 4 * 1]  # fd
    call lseek
    add esp, 4 * 3
    push eax  # cur_pos

    # Get end position
    push 2  # SEEK_END
    push 0
    push [ebp - 4 * 1]  # fd
    call lseek
    add esp, 4 * 3
    push eax  # end_pos

    # Restore position
    push 0  # SEEK_SET
    push [ebp - 4 * 2]  # cur_pos
    push [ebp - 4 * 1]  # fd
    call lseek
    add esp, 4 * 3

.ifdef TRACE
    push [ebp - 4 * 1]  # fd
    push [ebp - 4 * 3]  # end_pos
    push offset 8f
    call printf
    add esp, 4 * 3
.endif

    pop eax  # end_pos

    leave
    ret 4 * 2

.ifdef TRACE
8:
    .asciz "trace: GetFileSize: res=%d hFile=%d\n"
.endif

9:
    die "GetFileSize: Only 32 bits"

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
GlobalReAlloc: trace GlobalReAlloc
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
GlobalFlags: #trace GlobalFlags
    xor eax, eax
    ret 4

.global FileTimeToSystemTime
FileTimeToSystemTime:
    die FileTimeToSystemTime

.global FindResourceA
FindResourceA:
    push ebp
    mov ebp, esp

.ifndef NDEBUG
    push [ebp + 4 + 4 * 3]
    push [ebp + 4 + 4 * 2]
    push [ebp + 4 + 4 * 1]
    push offset 1f
    call printf
.endif

    xor eax, eax
    leave
    ret 4 * 3

.ifndef NDEBUG
1:
    .asciz "stub: FindResourceA: %d '%s' '%s'\n"
.endif

.global LoadResource
LoadResource:
    push ebp
    mov ebp, esp

.ifndef NDEBUG
    push [ebp + 4 + 4 * 2]
    push [ebp + 4 + 4 * 1]
    push offset 1f
    call printf
.endif

    xor eax, eax
    leave
    ret 4 * 2

.ifndef NDEBUG
1:
    .asciz "stub: LoadResource: %d %d\n"
.endif

.global LockResource
LockResource:
    push ebp
    mov ebp, esp

.ifndef NDEBUG
    push [ebp + 4 + 4]
    push offset 1f
    call printf
.endif

    xor eax, eax
    leave
    ret 4

.ifndef NDEBUG
1:
    .asciz "stub: LockResource: %d\n"
.endif

.global SizeofResource
SizeofResource:
    push ebp
    mov ebp, esp

.ifndef NDEBUG
    push [ebp + 4 + 4 * 2]
    push [ebp + 4 + 4 * 1]
    push offset 1f
    call printf
.endif

    xor eax, eax
    leave
    ret 4 * 2

.ifndef NDEBUG
1:
    .asciz "stub: SizeofResource: %d %d\n"
.endif

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
    stub GetLocalTime

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

.global GetTimeZoneInformation
GetTimeZoneInformation:
    die GetTimeZoneInformation
