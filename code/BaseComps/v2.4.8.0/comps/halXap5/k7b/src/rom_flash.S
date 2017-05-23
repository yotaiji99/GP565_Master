/*
 * Copyright (C) 2014 Greenpeak Technologies.
 */

#include "gpHal_kx_regprop_basic.h"
#include "gpHal_kx_enum.h"
#include "gpHal_kx_mm.h"

#include "rom_jump_table.h"
#include "asm_helpers.h"

    .global flash_erase_rom
flash_erase_rom:
    st.i        %r0, @(GP_WB_MM_MRI_NVM_ADDRESS_ADDRESS, 0)
    jump_to_rom FLASH_ERASE

    .global flash_write_page_rom
flash_write_page_rom:
    st.i        %r0, @(GP_WB_MM_MRI_NVM_ADDRESS_ADDRESS, 0)
    jump_to_rom FLASH_WRITE_PAGE

    .global flash_read_rom
flash_read_rom:
    push        {%r4}, #0
    mov.r       %r4, %r3
    mov.i       %r3, #0
    blkcp.fr    @(0, %r2), @(0, %r0), %r4
    pop.ret     {%r4}, #0
