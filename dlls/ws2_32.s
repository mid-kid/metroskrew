.intel_syntax noprefix
.include "_macros.i"

alias _WS2_32.dll_3, closesocket
alias _WS2_32.dll_4, connect
alias _WS2_32.dll_9, htons
alias _WS2_32.dll_b, inet_addr
alias _WS2_32.dll_c, inet_ntoa
alias _WS2_32.dll_f, ntohs
alias _WS2_32.dll_10, recv
alias _WS2_32.dll_12, select
alias _WS2_32.dll_13, send
alias _WS2_32.dll_16, shutdown
alias _WS2_32.dll_17, socket
alias _WS2_32.dll_6f, WSAGetLastError
alias _WS2_32.dll_73, WSAStartup
alias _WS2_32.dll_74, WSACleanup
alias _WS2_32.dll_97, __WSAFDIsSet

func closesocket; die closesocket
func connect; die connect
func htons; die htons
func inet_addr; die inet_addr
func inet_ntoa; die inet_ntoa
func ntohs; die ntohs
func recv; die recv
func select; die select
func send; die send
func shutdown; die shutdown
func socket; die socket
func WSAGetLastError; die WSAGetLastError
func WSAStartup; die WSAStartup

func WSACleanup
    # Nothing to do, really
    xor eax, eax
    ret

func __WSAFDIsSet; die __WSAFDIsSet
