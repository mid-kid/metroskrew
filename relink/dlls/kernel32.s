.intel_syntax noprefix
.include "macros.i"
.include "stat.i"

func IsBadReadPtr; die IsBadReadPtr

func RtlUnwind; die RtlUnwind

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

func MapViewOfFile; die MapViewOfFile

func UnmapViewOfFile; die UnmapViewOfFile

func SetFileAttributesA; die SetFileAttributesA

func OpenFileMappingA; die OpenFileMappingA

func MultiByteToWideChar; die MultiByteToWideChar

func IsValidCodePage; die IsValidCodePage

func GetACP; die GetACP

func LocalFileTimeToFileTime; die LocalFileTimeToFileTime
