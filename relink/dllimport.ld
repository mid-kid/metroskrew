/* Linker script used exclusively for windows builds */

/* Rename the DLL imports referenced by ordinals.
 * This works in part thanks to dllimport.s */
_WS2_32.dll_3 = _closesocket;
_WS2_32.dll_4 = _connect;
_WS2_32.dll_9 = _htons;
_WS2_32.dll_b = _inet_addr;
_WS2_32.dll_c = _inet_ntoa;
_WS2_32.dll_f = _ntohs;
_WS2_32.dll_10 = _recv;
_WS2_32.dll_12 = _select;
_WS2_32.dll_13 = _send;
_WS2_32.dll_16 = _shutdown;
_WS2_32.dll_17 = _socket;
_WS2_32.dll_6f = _WSAGetLastError;
_WS2_32.dll_73 = _WSAStartup;
_WS2_32.dll_74 = _WSACleanup;
_WS2_32.dll_97 = ___WSAFDIsSet;

/* Put all the metroskrew code right after the original exe.
 * If this is not done, windows will reject the executable */
__image_base__ = ALIGN(__section_alignment__) - 0x1000;

INSERT BEFORE .text
