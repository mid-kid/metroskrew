.intel_syntax noprefix
.include "dlls/macros.i"

.global _WS2_32.dll_3
_WS2_32.dll_3:
.global closesocket
closesocket:
    die closesocket

.global _WS2_32.dll_4
_WS2_32.dll_4:
.global connect
connect:
    die connect

.global _WS2_32.dll_9
_WS2_32.dll_9:
.global htons
htons:
    die htons

.global _WS2_32.dll_b
_WS2_32.dll_b:
.global inet_addr
inet_addr:
    die inet_addr

.global _WS2_32.dll_c
_WS2_32.dll_c:
.global inet_ntoa
inet_ntoa:
    die inet_ntoa

.global _WS2_32.dll_f
_WS2_32.dll_f:
.global ntohs
ntohs:
    die ntohs

.global _WS2_32.dll_10
_WS2_32.dll_10:
.global recv
recv:
    die recv

.global _WS2_32.dll_12
_WS2_32.dll_12:
.global select
select:
    die select

.global _WS2_32.dll_13
_WS2_32.dll_13:
.global send
send:
    die send

.global _WS2_32.dll_16
_WS2_32.dll_16:
.global shutdown
shutdown:
    die shutdown

.global _WS2_32.dll_17
_WS2_32.dll_17:
.global socket
socket:
    die socket

.global _WS2_32.dll_6f
_WS2_32.dll_6f:
.global WSAGetLastError
WSAGetLastError:
    die WSAGetLastError

.global _WS2_32.dll_73
_WS2_32.dll_73:
.global WSAStartup
WSAStartup:
    die WSAStartup

.global _WS2_32.dll_74
_WS2_32.dll_74:
.global WSACleanup
WSACleanup:
    die WSACleanup

.global _WS2_32.dll_97
_WS2_32.dll_97:
.global __WSAFDIsSet
__WSAFDIsSet:
    die __WSAFDIsSet
