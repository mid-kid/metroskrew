.intel_syntax noprefix
.include "macros.i"
.include "stat.i"

.weak ExitProcess
.global ExitProcess
ExitProcess:
    push [esp + 4]
.ifdef TRACE
    push offset 9f
    call printf
    pop eax
.endif
    call exit

.ifdef TRACE
9:
    .asciz "trace: ExitProcess: uExitCode=%d\n"
.endif

.global IsBadReadPtr
IsBadReadPtr:
    die IsBadReadPtr

.global RtlUnwind
RtlUnwind:
    die RtlUnwind

.weak GetCurrentProcess
.global GetCurrentProcess
GetCurrentProcess: trace GetCurrentProcess
    mov eax, -1
    ret

.weak DuplicateHandle
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
    call exit

9:
    .asciz "die: DuplicateHandle: %d\n"

.weak GetLastError
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

.weak GetStdHandle
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

.weak GetFileAttributesA
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
    add esp, 4
    pop eax
.endif

    push eax
    call GetFileAttributes_do
    call free
    add esp, 4

    and ebx, ebx
    jnz 1f
    mov ebx, -1
1:

.ifdef TRACE
    push [ebp + 4 + 4]
    push ebx
    push offset 8f
    call printf
    add esp, 4 * 3
.endif

    mov eax, ebx
    pop ebx
    leave
    ret 4

.ifdef TRACE
8:
    .asciz "trace: GetFileAttributesA: res=%x lpFileName=%s\n"
.endif

.ifndef NDEBUG
9:
    .asciz "stub: GetFileAttributesA: only presence and directory: %s\n"
.endif

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

.weak GetCurrentDirectoryA
.global GetCurrentDirectoryA
GetCurrentDirectoryA:
    push ebp
    mov ebp, esp

    push [ebp + 4 + 4 * 1]
    push [ebp + 4 + 4 * 2]

    mov eax, [esp + 4]
    sub eax, 2
    jc 1f
    mov [esp + 4], eax
    mov eax, [esp]
    mov word ptr [eax], 0x3a5a  # Z:
    add eax, 2
    mov [esp], eax

    call getcwd
    add esp, 4 * 2
    test eax, eax
    jz 1f

    mov eax, [ebp + 4 + 4 * 2]
    call path_dos

.ifndef NDEBUG
    push [ebp + 4 + 4 * 2]
    push offset 9f
    call printf
    add esp, 4 * 2
.endif

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

.weak CloseHandle
.global CloseHandle
CloseHandle:
    mov eax, [esp + 4]  # hObject
    dec eax

    # Make sure to not close stdio descriptors
    cmp eax, 3
    jc 2f
    push eax
    call close
    add esp, 4
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

.weak GetModuleHandleA
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

.weak GetModuleFileNameA
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

.weak FreeLibrary
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

.weak GetFullPathNameA
.global GetFullPathNameA
GetFullPathNameA:
    push ebp
    mov ebp, esp
    push ebx

    # Check if the string starts at the root
    mov eax, [ebp + 4 + 4 * 1]  # lpFileName
    xor ebx, ebx
    cmp byte ptr [eax], '\\'
    jz 1f
    cmp word ptr [eax], 0x3a5a  # Z:
    jnz 2f
    cmp byte ptr [eax + 2], '\\'
    jz 1f
2:

    # If it's relative, add current directory
    push [ebp + 4 + 4 * 3]  # lpBuffer
    push [ebp + 4 + 4 * 2]  # nBufferLength
    call GetCurrentDirectoryA

    # Append slash
    mov ebx, eax
    mov eax, [ebp + 4 + 4 * 3]  # lpBuffer
    mov byte ptr [eax + ebx], '\\'
    inc ebx
1:

    # Calculate full string length
    push ebx
    push [ebp + 4 + 4 * 1]  # lpFileName
    call strlen
    add esp, 4 * 2
    add eax, ebx
    push eax  # Return value

    # Compare it to the buffer size
    inc eax
    cmp [ebp + 4 + 4 * 2], eax  # nBufferLength
    jc 8f

    # Append the filename
    mov eax, ebx
    add eax, [ebp + 4 + 4 * 3]  # lpBuffer
    push [ebp + 4 + 4 * 1]  # lpFileName
    push eax
    call strcpy

    # Set the file part pointer
    mov dword ptr [esp + 4], '\\'
    call strrchr
    inc eax
    cmp eax, 1
    jnz 1f
    mov eax, [esp]
1:
    mov ebx, [ebp + 4 + 4 * 4]  # lpFilePart
    and ebx, ebx
    jz 1f
    mov [ebx], eax
1:
    add esp, 4 * 2

.ifndef NDEBUG
    mov eax, [ebp + 4 + 4 * 4]  # lpFilePart
    and eax, eax
    jz 1f
    mov eax, [eax]
1:
    push eax
    push [ebp + 4 + 4 * 3]  # lpBuffer
    push [ebp + 4 + 4 * 1]  # lpFileName
    push offset 9f
    call printf
    add esp, 4 * 4
.endif

    pop eax
    pop ebx
    leave
    ret 4 * 4

8:
    die "GetFullPathNameA: Buffer too small"

.ifndef NDEBUG
9:
    .asciz "GetFullPathNameA: '%s' = '%s' (%s)\n"
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

.weak CreateFileA
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
    mov eax, [esp + 4]
    dec eax
    push eax
    push offset 8f
    call printf
    add esp, 4 * 2
.endif
    call free
    add esp, 4

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
    .asciz "CreateFileA: res=%d lpFileName=%s\n"
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
    call exit

1:
    .asciz "die: CreateFileA: '%s' %x %x %x %x %x %d\n"

.weak GetTickCount
.global GetTickCount
GetTickCount:
    stub GetTickCount
    xor eax, eax
    ret

.weak DeleteFileA
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

.weak GetFileTime
.global GetFileTime
GetFileTime:
    stub GetFileTime

    mov eax, [esp + 4 * 2]  # lpCreationTime
    and eax, eax
    jz 1f
    mov dword ptr [eax + 4 * 0], 0  # dwLowDateTime
    mov dword ptr [eax + 4 * 1], 0  # dwHighDateTime
1:
    mov eax, [esp + 4 * 3]  # lpLastAccessTime
    and eax, eax
    jz 1f
    mov dword ptr [eax + 4 * 0], 0  # dwLowDateTime
    mov dword ptr [eax + 4 * 1], 0  # dwHighDateTime
1:
    mov eax, [esp + 4 * 4]  # lpLastWriteTime
    and eax, eax
    jz 1f
    mov dword ptr [eax + 4 * 0], 0  # dwLowDateTime
    mov dword ptr [eax + 4 * 1], 0  # dwHighDateTime
1:
    mov eax, 1
    ret 4 * 4

.global SetFileTime
SetFileTime:
    die SetFileTime

.weak GetFileSize
.global GetFileSize
GetFileSize:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 4 + 4 * 2]  # lpFileSizeHigh
    and eax, eax
    jnz 9f

    mov eax, [ebp + 4 + 4 * 1]
    dec eax  # fd
.ifdef TRACE
    push eax
.endif
    stat eax, fstat
    cmp eax, -1
    jz 1f
    stat_get size
1:
    stat_pop

.ifdef TRACE
    push eax
    push offset 8f
    call printf
    pop eax
    pop eax
.endif

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

.weak GetSystemTime
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

.weak GetSystemDirectoryA
.global GetSystemDirectoryA
GetSystemDirectoryA:
    stub GetSystemDirectoryA
    xor eax, eax
    ret 4 * 2

.global GetWindowsDirectoryA
GetWindowsDirectoryA:
    stub GetWindowsDirectoryA
    xor eax, eax
    ret 4 * 2

.global SetConsoleCtrlHandler
SetConsoleCtrlHandler:
    stub SetConsoleCtrlHandler
    mov eax, 1
    ret 4 * 2

.weak GetConsoleScreenBufferInfo
.global GetConsoleScreenBufferInfo
GetConsoleScreenBufferInfo:
    stub GetConsoleScreenBufferInfo
    xor eax, eax
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

.weak GetLocalTime
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
    stub GetTimeZoneInformation

    mov eax, [esp + 4]
    push 172
    push 0
    push eax
    call memset
    add esp, 4 * 3

    xor eax, eax
    ret 4
