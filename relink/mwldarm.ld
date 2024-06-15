SECTIONS {
    /DISCARD/ : { *(.pe_text) }
    .pe_text pe_text_addr : { *(.patch_pe_text) }

    .pe_exc pe_exc_addr : { *(.pe_exc) }
    .pe_rdata pe_rdata_addr : { *(.pe_rdata) }
    .pe_data pe_data_addr : { *(.pe_data) }
    .rsrc pe_rsrc_addr : { *(.pe_rsrc) }
    .pe_CRT pe_CRT_addr : { *(.pe_CRT) }
    .pe_idata pe_idata_addr : { *(.pe_idata) }
    .pe_bss pe_bss_addr : { *(.pe_bss) }
    .pe_edata pe_edata_addr : { *(.pe_edata) }
    .pe_reloc pe_reloc_addr : { *(.pe_reloc) }
}
INSERT BEFORE .text
