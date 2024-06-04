.intel_syntax noprefix
.include "macros.i"
.include "stat.i"

.global IsBadReadPtr
IsBadReadPtr:
    die IsBadReadPtr

.global RtlUnwind
RtlUnwind:
    die RtlUnwind

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

MAX_PATH = 260

.global FindFirstFileA
FindFirstFileA:
    push ebp
    mov ebp, esp
    push ebx

.ifndef NDEBUG
    push [ebp + 4 + 4 * 1]  # lpFileName
    push offset 4f
    call printf
    add esp, 4 * 2
.endif

    # Convert the string
    mov eax, [ebp + 4 + 4 * 1]  # lpFileName
    call path_dup_unx
    push eax  # path

    # Check if we're trying to list an actual directory
    push 0
    push eax  # path
    call strchr
    add esp, 4 * 2

    mov ebx, -2
    cmp word ptr [eax + ebx], 0x2a2f  # '/*' = Anything
    jz 2f

    mov ebx, -1
    cmp byte ptr [eax + ebx], 0x2f  # '/' = trailing slash
    jz 2f

    # Assume it's a full path to a file

    # Create hFindFile object
    push 4 * 2
    call malloc
    add esp, 4
    mov dword ptr [eax], 0
    pop ebx  # path
    mov [eax + 4], ebx
    jmp 1f

2:
    # Truncate the string
    mov byte ptr [eax + ebx], 0

    # Create a DIR stream
    call opendir
    and eax, eax
    jz 8f
    push eax  # dir

    # Create hFindFile object
    push 4 * 2
    call malloc
    add esp, 4
    pop ebx  # dir
    mov [eax], ebx
    pop ebx  # path
    mov [eax + 4], ebx

1:
    push eax
    mov ebx, [ebp + 4 + 4 * 2]  # lpFindFileData
    call FindNextFileA_do
    and eax, eax
    jz 7f
    pop eax

1:
    pop ebx
.ifdef TRACE
    push [ebp + 4 + 4 * 1]  # lpFileName
    push eax
    push offset 5f
    call printf
    pop eax
    pop eax
.endif
    leave
    ret 4 * 2

.ifndef NDEBUG
4:
    .asciz "stub: FindFirstFileA: only file name and attributes: %s\n"
.endif

.ifdef TRACE
5:
    .asciz "trace: FindFirstFileA: res=%x lpFileName=%s\n"
.endif

# FindNextFileA_do error
7:
    pop eax
    push eax
    push [eax + 4]
    mov eax, [eax]
    and eax, eax
    jnz 2f
    push eax
    call closedir
    add esp, 4
2:
    call free
    add esp, 4
# OS error
8:
    call free
    add esp, 4
    mov eax, -1
    jmp 1b

# Unimplemented error
9:
    push [ebp + 4 + 4 * 1]  # lpFileName
    push offset 1f
    call printf
    add esp, 4 * 2
    push 1
    call exit

1:
    .asciz "die: FindFirstFileA: lpFileName=%s\n"

GetFileAttributes_do:
    stat eax
    xor ebx, ebx
    and eax, eax
    jnz 2f

    # Check if the file is a dir
    stat_get mode
    and eax, 0170000  # S_IFMT
    cmp eax, 0040000  # S_IFDIR
    jnz 1f
    or ebx, 0x10  # FILE_ATTRIBUTE_DIRECTORY
    jmp 2f
1:
    or ebx, 0x20  # FILE_ATTRIBUTE_ARCHIVE
2:

    stat_pop
    ret

.global FindNextFileA
FindNextFileA:
    push ebp
    mov ebp, esp
    push ebx

.ifndef NDEBUG
    push offset 4f
    call printf
    add esp, 4 * 1
.endif

    mov eax, [ebp + 4 + 4 * 1]  # hFindFile
    mov ebx, [ebp + 4 + 4 * 2]  # lpFindFileData
    call FindNextFileA_do

    pop ebx
.ifdef TRACE
    push [ebp + 4 + 4 * 1] # lpFileName
    push eax
    push offset 5f
    call printf
    pop eax
    pop eax
.endif
    leave
    ret 4 * 2

.ifndef NDEBUG
4:
    .asciz "stub: FindNextFileA: only file name and attributes\n"
.endif

.ifdef TRACE
5:
    .asciz "trace: FindNextFileA: res=%d hFindFile=%x\n"
.endif

FindNextFileA_do:
    push eax
    push ebx

    # If there's no DIR, assume the path is full
    cmp dword ptr [eax], 0
    jnz 2f

    # Find the filename component
    push '/'
    push [eax + 4]
    call strrchr
    and eax, eax
    jnz 1f
    mov eax, [esp]
    dec eax
1:
    add esp, 4 * 2
    inc eax

    jmp 3f

2:
    # Read the next entry
    push [eax]
    call readdir
    add esp, 4
    and eax, eax
    jz 7f
    add eax, [dirent_name_offsetof]

3:
    # Prepare strncpy
    push MAX_PATH - 1
    push eax  # dirent_name

    # Initialize unused fields of WIN32_FIND_DATAA
    push 4 * 11 + MAX_PATH + 14
    push 0
    push ebx
    call memset
    add esp, 4 * 3

    # Create full path if we don't already have one
    mov ebx, [esp + 4 * 3]  # hFindFile
    mov eax, [ebx]  # dir
    mov ebx, [ebx + 4]  # path
    and eax, eax
    jz 1f

    # Create path and get the file attributes
    mov eax, [esp]  # dirent_name
    call path_join
    push eax
    call GetFileAttributes_do
    call free
    add esp, 4

    jmp 2f

1:
    # Get file attributes directly
    mov eax, ebx
    call GetFileAttributes_do

2:
    # Set the file attributes
    # If the file doesn't exist anymore, attributes will be 0
    mov eax, [esp + 4 * 2]  # lpFindFileData.dwFileAttributes
    mov [eax], ebx

    # Copy the filename
    mov eax, [esp + 4 * 2]  # lpFindFileData
    add eax, 4 * 11  # cFileName
    push eax
    call strncpy
    add esp, 4 * 3

.ifndef NDEBUG
    mov eax, [esp]
    push [eax]
    add eax, 4 * 11
    push eax
    push offset 9f
    call printf
    add esp, 4 * 3
.endif

    mov eax, 1
7:
    add esp, 4 * 2
    ret

8:
    xor eax, eax
    add esp, 4 * 4
    ret

.ifndef NDEBUG
9:
    .asciz "FindFile: '%s' (0x%x)\n"
.endif

.global FindClose
FindClose:
    mov eax, [esp + 4]  # hFindFile
    push eax
    push [eax + 4]

    mov eax, [eax]
    and eax, eax
    jz 1f
    push eax
    call closedir
    add esp, 4
1:

    call free
    add esp, 4
    call free

.ifdef TRACE
    push eax
    push offset 9f
    call printf
    add esp, 4 * 2
.endif
    mov eax, 1
    add esp, 4
    ret 4

.ifdef TRACE
9:
    .asciz "trace: FindClose: res=%d hFindFile=%x\n"
.endif

.global GetCommandLineA
GetCommandLineA:
    die GetCommandLineA

.global GetEnvironmentStrings
GetEnvironmentStrings: #trace GetEnvironmentStrings
    mov eax, environ
    mov eax, [eax]
    ret

.global FreeEnvironmentStringsA
FreeEnvironmentStringsA: #trace FreeEnvironmentStringsA
    ret 4

.global CreateProcessA
CreateProcessA:
    die CreateProcessA

.global WaitForSingleObject
WaitForSingleObject:
    die WaitForSingleObject

.global GetExitCodeProcess
GetExitCodeProcess:
    die GetExitCodeProcess

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
    call exit

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

.global SetFileTime
SetFileTime:
    die SetFileTime

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

.global CompareFileTime
CompareFileTime:
    stub CompareFileTime
    xor eax, eax
    ret 4 * 2

.global GlobalReAlloc
GlobalReAlloc: #trace GlobalReAlloc
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
    call exit

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
.ifndef NDEBUG
    mov eax, esp
    push [eax + 4 * 3]
    push [eax + 4 * 2]
    push [eax + 4 * 1]
    push offset 1f
    call printf
.endif

    xor eax, eax
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
    stub CreateFileMappingA
    xor eax, eax
    ret 4 * 6

.global MapViewOfFile
MapViewOfFile:
    die MapViewOfFile

.global UnmapViewOfFile
UnmapViewOfFile:
    die UnmapViewOfFile

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

.global GetTimeZoneInformation
GetTimeZoneInformation:
    stub GetTimeZoneInformation

    mov eax, [esp + 4]
    push 172
    push 0
    push eax
    call memset
    add esp, 4 * 3

    xor eax, eax
    ret 4
