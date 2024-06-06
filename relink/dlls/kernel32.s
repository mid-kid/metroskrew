.intel_syntax noprefix
.include "macros.i"
.include "stat.i"

func IsBadReadPtr; die IsBadReadPtr

func RtlUnwind; die RtlUnwind

MAX_PATH = 260

func FindFirstFileA; die FindFirstFileA
    push ebp
    mov ebp, esp
    push ebx

.ifndef NDEBUG
    push [ebp + 4 + 4 * 1]  # lpFileName
    push offset 4f
    wcall printf
    add esp, 4 * 2
.endif

    # Convert the string
    mov eax, [ebp + 4 + 4 * 1]  # lpFileName
    call path_dup_unx
    push eax  # path

    # Check if we're trying to list an actual directory
    push 0
    push eax  # path
    wcall strchr
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
    wcall malloc
    add esp, 4
    mov dword ptr [eax], 0
    pop ebx  # path
    mov [eax + 4], ebx
    jmp 1f

2:
    # Truncate the string
    mov byte ptr [eax + ebx], 0

    # Create a DIR stream
    wcall opendir
    and eax, eax
    jz 8f
    push eax  # dir

    # Create hFindFile object
    push 4 * 2
    wcall malloc
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
    wcall printf
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
    wcall closedir
    add esp, 4
2:
    wcall free
    add esp, 4
# OS error
8:
    wcall free
    add esp, 4
    mov eax, -1
    jmp 1b

# Unimplemented error
9:
    push [ebp + 4 + 4 * 1]  # lpFileName
    push offset 1f
    wcall printf
    add esp, 4 * 2
    push 1
    wcall exit

1:
    .asciz "die: FindFirstFileA: lpFileName=%s\n"

GetFileAttributes_do: die GetFileAttributes_do
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

func FindNextFileA; die FindNextFileA
    push ebp
    mov ebp, esp
    push ebx

.ifndef NDEBUG
    push offset 4f
    wcall printf
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
    wcall printf
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

FindNextFileA_do: die FindNextFileA_do
    push eax
    push ebx

    # If there's no DIR, assume the path is full
    cmp dword ptr [eax], 0
    jnz 2f

    # Find the filename component
    push '/'
    push [eax + 4]
    wcall strrchr
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
    wcall readdir
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
    wcall memset
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
    wcall free
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
    wcall strncpy
    add esp, 4 * 3

.ifndef NDEBUG
    mov eax, [esp]
    push [eax]
    add eax, 4 * 11
    push eax
    push offset 9f
    wcall printf
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

func FindClose; die FindClose
    mov eax, [esp + 4]  # hFindFile
    push eax
    push [eax + 4]

    mov eax, [eax]
    and eax, eax
    jz 1f
    push eax
    wcall closedir
    add esp, 4
1:

    wcall free
    add esp, 4
    wcall free

.ifdef TRACE
    push eax
    push offset 9f
    wcall printf
    add esp, 4 * 2
.endif
    mov eax, 1
    add esp, 4
    ret 4

.ifdef TRACE
9:
    .asciz "trace: FindClose: res=%d hFindFile=%x\n"
.endif

func GetCommandLineA; die GetCommandLineA

func CreateProcessA; die CreateProcessA

func WaitForSingleObject; die WaitForSingleObject

func GetExitCodeProcess; die GetExitCodeProcess

func MoveFileA; die MoveFileA

func FormatMessageA; die FormatMessageA
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
    wcall printf
    add esp, 4 * 8
.endif

    xor eax, eax
    leave
    ret 4 * 7

.ifndef NDEBUG
9:
    .asciz "stub: FormatMessageA: %x %x %d %d %x %d %x\n"
.endif

func SetFileTime; die SetFileTime

func SetEndOfFile; die SetEndOfFile

func CreateDirectoryA; die CreateDirectoryA

func RemoveDirectoryA; die RemoveDirectoryA

func SetStdHandle; die SetStdHandle

func CompareFileTime; die CompareFileTime
    stub CompareFileTime
    xor eax, eax
    ret 4 * 2

func FileTimeToSystemTime; die FileTimeToSystemTime

func FindResourceA; die FindResourceA
.ifndef NDEBUG
    mov eax, esp
    push [eax + 4 * 3]
    push [eax + 4 * 2]
    push [eax + 4 * 1]
    push offset 1f
    wcall printf
.endif

    xor eax, eax
    ret 4 * 3

.ifndef NDEBUG
1:
    .asciz "stub: FindResourceA: %d '%s' '%s'\n"
.endif

func LoadResource; die LoadResource
    push ebp
    mov ebp, esp

.ifndef NDEBUG
    push [ebp + 4 + 4 * 2]
    push [ebp + 4 + 4 * 1]
    push offset 1f
    wcall printf
.endif

    xor eax, eax
    leave
    ret 4 * 2

.ifndef NDEBUG
1:
    .asciz "stub: LoadResource: %d %d\n"
.endif

func LockResource; die LockResource
    push ebp
    mov ebp, esp

.ifndef NDEBUG
    push [ebp + 4 + 4]
    push offset 1f
    wcall printf
.endif

    xor eax, eax
    leave
    ret 4

.ifndef NDEBUG
1:
    .asciz "stub: LockResource: %d\n"
.endif

func SizeofResource; die SizeofResource
    push ebp
    mov ebp, esp

.ifndef NDEBUG
    push [ebp + 4 + 4 * 2]
    push [ebp + 4 + 4 * 1]
    push offset 1f
    wcall printf
.endif

    xor eax, eax
    leave
    ret 4 * 2

.ifndef NDEBUG
1:
    .asciz "stub: SizeofResource: %d %d\n"
.endif

func CreateFileMappingA; die CreateFileMappingA
    stub CreateFileMappingA
    xor eax, eax
    ret 4 * 6

func MapViewOfFile; die MapViewOfFile

func UnmapViewOfFile; die UnmapViewOfFile

func SetFileAttributesA; die SetFileAttributesA

func OpenFileMappingA; die OpenFileMappingA

func MultiByteToWideChar; die MultiByteToWideChar

func IsValidCodePage; die IsValidCodePage

func GetACP; die GetACP

func GetTimeZoneInformation; die GetTimeZoneInformation
    stub GetTimeZoneInformation

    mov eax, [esp + 4]
    push 172
    push 0
    push eax
    wcall memset
    add esp, 4 * 3

    xor eax, eax
    ret 4
