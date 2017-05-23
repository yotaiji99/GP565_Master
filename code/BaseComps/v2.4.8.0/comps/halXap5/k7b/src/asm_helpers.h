/*
 * File: asm_helpers.h
 *       Assembler macros
 * Copyright (C) 2014 Greenpeak Technologies.
 */

#ifndef __ASM_HELPERS_H__
#define __ASM_HELPERS_H__

#define __L16(c)                    ((c) &  0xFFFF)
#define __U16(c)                    ((c) >> 16)
#define __ADDRESS_U1(wb_reg)        GP_WB_##wb_reg##_ADDRESS[GP_WB_##wb_reg##_LSB]
#define __SOFTPOR_REASON(reason)    __ADDRESS_U1(PMUD_SOFT_POR_##reason)

#define GP_WB_ADDRESS_U1(wb_reg)    @(__ADDRESS_U1(wb_reg), 0)

.macro  mov.32u.i   regl, regh, const
    mov.i       \regl, #__L16(\const)
    mov.i       \regh, #__U16(\const)
.endm

.macro  st.8.mbp    reg, val, addr
    mov.i       \reg, #(\val)
    st.8.i      \reg, @(\addr, 0)
.endm

.macro  ldcmp.i     reg, addr, const
    ld.i        \reg, @(\addr, 0)
    cmp.i       \reg, #\const
.endm

.macro  cmp.32.i    regl, regh, const
    cmp.i       \regl, #__L16(\const)
    cmp.c.i     \regh, #__U16(\const)
.endm

.macro  ldcmp.32.i  regl, regh, addr, const
    ld.32.i     \regl, @(\addr, 0)
    cmp.32.i    \regl, \regh, \const
.endm

.macro  softpor     addr
    st.1.i      #1, @(\addr, 0)
    halt
.endm

.macro  jump_to_rom index, opt_cmd=
    ld.i        %r0, @(GP_MM_ROM_JUMP_TABLE_START + 2 * \index, 0)
#if (GP_MM_ROM_START_ADDRESS & 0xFFFF) == 0
    mov.i       %r1, #(GP_MM_ROM_START >> 16)
#else
# error "Oook?"
    mov.32.i    %r2, #GM_MM_ROM_START
    add.r       %r0, %r0, %r2
    add.c.i     %r1, %r3, #0
#endif
    \opt_cmd
    bra.i       (0, %r0)
.endm

.macro  crcini.32.r res, oper
    clu.ddss    #0, \res, \oper
.endm

.macro  crc32.32.r  res, oper
    clu.ddss    #1, \res, \oper
.endm

.macro  crc32.8.r   res, oper
    clu.ddss    #2, \res, \oper
.endm

.macro  crc32r.32.r res, oper
    clu.ddss    #3, \res, \oper
.endm

.macro  crc32r.8.r  res, oper
    clu.ddss    #4, \res, \oper
.endm

#endif  /* __ASM_HELPERS_H__ */
