.intel_syntax noprefix

# This file references the exact stdcall symbols for windows libraries.
# It causes the linker to find the right object files in the libraries,
#  causing any references to symbols without the `@` number to fall back
#  to the original implementations if not available.
# Only used for windows builds.

.macro import name, num
.set _\name, _\name\()@\num
.endm

import CloseHandle, 4
import CoCreateInstance, 20
import CoInitialize, 4
import CoTaskMemAlloc, 4
import CoTaskMemFree, 4
import CoUninitialize, 0
import CompareFileTime, 8
import CreateDirectoryA, 8
import CreateFileA, 28
import CreateFileMappingA, 24
import CreateProcessA, 40
import DeleteCriticalSection, 4
import DeleteFileA, 4
import DuplicateHandle, 28
import EnterCriticalSection, 4
import ExitProcess, 4
import FileTimeToSystemTime, 8
import FindClose, 4
import FindFirstFileA, 8
import FindNextFileA, 8
import FindResourceA, 12
import FormatMessageA, 28
import FreeEnvironmentStringsA, 4
import FreeLibrary, 4
import GetACP, 0
import GetCommandLineA, 0
import GetConsoleScreenBufferInfo, 8
import GetCurrentDirectoryA, 8
import GetCurrentProcess, 0
import GetEnvironmentStrings, 0
import GetExitCodeProcess, 8
import GetFileAttributesA, 4
import GetFileSize, 8
import GetFileTime, 16
import GetFileVersionInfoA, 16
import GetFileVersionInfoSizeA, 8
import GetFullPathNameA, 16
import GetLastError, 0
import GetLocalTime, 4
import GetModuleFileNameA, 12
import GetModuleHandleA, 4
import GetStdHandle, 4
import GetSystemDirectoryA, 8
import GetSystemTime, 4
import GetTickCount, 0
import GetTimeZoneInformation, 4
import GetWindowsDirectoryA, 8
import GlobalAlloc, 8
import GlobalFlags, 4
import GlobalFree, 4
import GlobalReAlloc, 12
import InitializeCriticalSection, 4
import IsBadReadPtr, 8
import IsValidCodePage, 4
import LeaveCriticalSection, 4
import LoadLibraryA, 4
import LoadResource, 8
import LoadStringA, 16
import LockResource, 4
import MapViewOfFile, 20
import MessageBoxA, 16
import MoveFileA, 8
import MultiByteToWideChar, 24
import OpenFileMappingA, 12
import ReadFile, 20
import RegCloseKey, 4
import RegOpenKeyExA, 20
import RegQueryValueExA, 24
import RemoveDirectoryA, 4
import RtlUnwind, 16
import SetConsoleCtrlHandler, 8
import SetEndOfFile, 4
import SetFileAttributesA, 8
import SetFilePointer, 16
import SetFileTime, 16
import SetStdHandle, 8
import SizeofResource, 8
import SystemTimeToFileTime, 8
import TlsAlloc, 0
import TlsFree, 4
import TlsGetValue, 4
import TlsSetValue, 8
import UnmapViewOfFile, 4
import VerQueryValueA, 16
import WaitForSingleObject, 8
import WriteFile, 20

import closesocket, 4
import connect, 12
import htons, 4
import inet_addr, 4
import inet_ntoa, 4
import ntohs, 4
import recv, 16
import select, 20
import send, 16
import shutdown, 8
import socket, 12
import WSAGetLastError, 0
import WSAStartup, 8
import WSACleanup, 0
import __WSAFDIsSet, 8
