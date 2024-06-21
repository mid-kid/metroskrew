.intel_syntax noprefix
.include "_macros.i"

.global _WS2_32.dll_3
_WS2_32.dll_3:
func closesocket; die closesocket

.global _WS2_32.dll_4
_WS2_32.dll_4:
func connect; die connect

.global _WS2_32.dll_9
_WS2_32.dll_9:
func htons; die htons

.global _WS2_32.dll_b
_WS2_32.dll_b:
func inet_addr; die inet_addr

.global _WS2_32.dll_c
_WS2_32.dll_c:
func inet_ntoa; die inet_ntoa

.global _WS2_32.dll_f
_WS2_32.dll_f:
func ntohs; die ntohs

.global _WS2_32.dll_10
_WS2_32.dll_10:
func recv; die recv

.global _WS2_32.dll_12
_WS2_32.dll_12:
func select; die select

.global _WS2_32.dll_13
_WS2_32.dll_13:
func send; die send

.global _WS2_32.dll_16
_WS2_32.dll_16:
func shutdown; die shutdown

.global _WS2_32.dll_17
_WS2_32.dll_17:
func socket; die socket

.global _WS2_32.dll_6f
_WS2_32.dll_6f:
func WSAGetLastError; die WSAGetLastError

.global _WS2_32.dll_73
_WS2_32.dll_73:
func WSAStartup; die WSAStartup

.global _WS2_32.dll_74
_WS2_32.dll_74:
func WSACleanup
    # Nothing to do, really
    xor eax, eax
    ret

.global _WS2_32.dll_97
_WS2_32.dll_97:
func __WSAFDIsSet; die __WSAFDIsSet
