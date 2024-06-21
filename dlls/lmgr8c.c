void dll_lp_checkout() __asm__("_LMGR8C.dll_be")
    __attribute__((alias("lp_checkout")));
void dll_lp_checkin() __asm__("_LMGR8C.dll_bd")
    __attribute__((alias("lp_checkin")));
void dll_lp_errstring() __asm__("_LMGR8C.dll_bf")
    __attribute__((alias("lp_errstring")));

int lp_checkout() { return 0; }
void lp_checkin() {}
void lp_errstring() {}
