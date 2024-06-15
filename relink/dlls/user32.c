#include <stdint.h>

// WINE headers, used for the sake of type-checking definitions
#define WINUSERAPI
#include <stdarg.h>
#include "windef.h"
#include "winbase.h"
#include "winuser.h"

// Local headers
#include "utils.h"

struct rsrc_strings {
    UINT id;
    uint16_t *data;
};
extern struct rsrc_strings pe_rsrc_strings[] __asm__("pe_rsrc_strings");

INT WINAPI LoadStringA(HINSTANCE hInstance, UINT uID, LPSTR lpBuffer, INT cchBufferMax)
{
    // Make sure it's not trying to grab strings from a different instance
    // -1 is the HACK value returned from LoadLibraryA
    if (hInstance != (HINSTANCE)0 && hInstance != (HINSTANCE)-1 &&
        hInstance != (HINSTANCE)0x00400000) goto die;

    // Find the set of 16 strings
    uint16_t *str = NULL;
    for (struct rsrc_strings *rsrc = pe_rsrc_strings;
            rsrc->id; rsrc++) {
        if (rsrc->id == (uID >> 4) + 1) {
            str = rsrc->data;
            break;
        }
    }
    if (!str) return 0;

    // Skip to the nth string in the set
    for (UINT c = uID & 0xf; c; c--) str += *str + 1;

    // Get the size, truncate if necessary
    INT res = *str++;
    if (cchBufferMax - 1 < res) res = cchBufferMax - 1;

    // Copy the string, truncating each character
    LPSTR dst = lpBuffer;
    for (INT x = res; x; x--) *dst++ = *str++;
    *dst++ = '\0';

    TRACE("LoadStringA: res=%d hInstance=%p uID=%d lpBuffer='%s'",
        res, hInstance, uID, lpBuffer);
    return res;

die:
    DIE("LoadStringA: %p %d", hInstance, uID);
}
