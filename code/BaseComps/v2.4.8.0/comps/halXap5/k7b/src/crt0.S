/*
 * File: crt0.S
 *       Default C-runtime implementation.
 * Copyright (C) 2013, 2014 Greenpeak Technologies.
 */

#include "gpHal_kx_regprop_basic.h"
#include "gpHal_kx_enum.h"
#include "gpHal_kx_mm.h"

#include "rom_jump_table.h"
#include "asm_helpers.h"

/*
 * Global variable for heap management, required in __sbrk.c.
 */
#ifdef GP_KX_HEAP_SIZE
    .global  __heap    
    .data
__heap:
    .word   heap_begin
     
    .rodata
    .global  __heapsize
__heapsize:
    .word   heap_size
#endif

/*
 * Global (unitialised) variable for deep sleep.
 * We put it in a separate section, so we can put them at a fixed address.
 */
    .section .deepsleep, "aw", @nobits
    
$sp_ret:
    .skip 4

    .text
    .p2align    1

/*
 * hardreset handler
 * We only end up here from a debug session or from a WB reset. Need to perform
 * some actions otherwise done by the bootloader.
 */
    .global $hard_reset
    .type   $hard_reset, @function
$hard_reset:
    /* Init RAM */
    st.32.i     #0, @(GP_MM_RAM_MAGIC_WORD_START, 0)
    st.1.i      #1, GP_WB_ADDRESS_U1(MM_INIT_START)
    /* Stack window init on hard reset */
    mov.i       %r0, #elf_wb_stack_begin
    st.i        %r0, @(GP_WB_MM_GEN_WINDOW_VAR_0_BASE_ADDRESS_ADDRESS, 0)
    mov.i       %r0, #elf_wb_stack_sz
    st.i        %r0, @(GP_WB_MM_GEN_WINDOW_VAR_0_LEN_ADDRESS, 0)

$do_full_init:
    /*
     * we need to set/restore our stack window registers
     * if we do a partial restore, they are not restored by HW!!! 
     */
    
    /* pbm options physical address offset */
    mov.i       %r0, #elf_pbm_options_base
    st.i        %r0, @(GP_WB_MM_PBM_OPTS_BASE_ADDRESS_ADDRESS, 0)
    /* event physical address offset */
    mov.i       %r0, #elf_event_base
    st.i        %r0, @(GP_WB_MM_EVENT_BASE_ADDRESS_ADDRESS, 0)
    /* pbm physical address offsets - loop over all 8 possible PBM */
    mov.i       %r0, #GP_WB_MM_PBM_0_DATA_BASE_ADDRESS_ADDRESS
    mov.i       %r1, #elf_pbm_base
$do_pbm_init:
    st.i        %r1, @(0, %r0)
    add.i       %r0, %r0, #2
    add.i       %r1, %r1, #0x80
    cmp.i       %r0, #GP_WB_MM_PBM_7_DATA_BASE_ADDRESS_ADDRESS
    blt.u       ($do_pbm_init, %pc)
    mov.i       %r0, #elf_pbm_ack_begin
    st.i        %r0, @(GP_WB_MM_PBM_7_DATA_BASE_ADDRESS_ADDRESS, 0)
    /* gpx physical address offsets */
    mov.i       %r0, #elf_gp0_begin
    st.i        %r0, @(GP_WB_MM_GEN_WINDOW_0_BASE_ADDRESS_ADDRESS, 0)
    mov.i       %r0, #elf_gp1_begin
    st.i        %r0, @(GP_WB_MM_GEN_WINDOW_1_BASE_ADDRESS_ADDRESS, 0)
    mov.i       %r0, #elf_gp2_begin
    st.i        %r0, @(GP_WB_MM_GEN_WINDOW_2_BASE_ADDRESS_ADDRESS, 0)
    mov.i       %r0, #elf_gp3_begin
    st.i        %r0, @(GP_WB_MM_GEN_WINDOW_3_BASE_ADDRESS_ADDRESS, 0)
    bra.i       ($crt0, %pc)


/*
 * softreset handler:
 *  - entry from the ROM/FLASH bootloader, treat similar to hard_reset
 *  - other cases: allows the debugging user to note that it has occurred
 *    but also to allow the application to resume execution
 */
    .global $soft_reset
    .type   $soft_reset, @function
$soft_reset:
    /* First check if we originate from the ROM/FLASH bootloader */
    movb2r.32   %r4, %brk1
    cmp.32.i    %r4, %r5, GP_MM_ROM_START
    bne         ($do_soft_reset, %pc)

    /* Not a real softreset, init from bootloader */

    /*
     * Clear bootloader prefetch memory exception.
     */
    st.1.i      #1, GP_WB_ADDRESS_U1(XAP5_GP_CLR_LOGICAL_ADDRESS_ON_ERR)

    /* Stack window must be restored whether POR or wake-up */
    mov.i       %r0, #elf_wb_stack_begin
    st.i        %r0, @(GP_WB_MM_GEN_WINDOW_VAR_0_BASE_ADDRESS_ADDRESS, 0)
    mov.i       %r0, #elf_wb_stack_sz
    st.i        %r0, @(GP_WB_MM_GEN_WINDOW_VAR_0_LEN_ADDRESS, 0)

    /* Check if POR - perform full init or a restore */
    ld.1.i      %flags[c], GP_WB_ADDRESS_U1(STANDBY_UNMASKED_PORD_INTERRUPT)
    bcs         ($do_full_init, %pc)
    bra.i       ($sm_restore_entry, %pc)

$do_soft_reset:
    /*
     * we destroy r2 and r3.
     * r2: old value of brke
     * r3: (register value of user code)
     * since sp points to ram, it will fit into 16 bit.
     */

    /* save the stack pointer, and re-init stack (might be corrupt) */
    mova2r.16   %r2, %sp0
    mov.i       %r3, #stack_end
    movr2a.16   %sp0, %r3

    /* jump to C code:    handle_soft_reset(flags, errorcode, sp) */
    bsr.i (handle_soft_reset, %pc)
    /* fall-through */

    .global $crt0
    .type   $crt0, @function
$crt0:
    /* init sp with top of virtual stack*/
    mov.i       %r0, #elf_virt_stack_top
    movr2a.16   %sp0, %r0

    /* Init data segments */
#ifndef __XAP5_NEAR__
# error "This start-up code is only valid for NEAR mode execution."
#endif

    /* zero .bss */
    mov.i       %r0, (bss_begin, 0)
    mov.i       %r2, #bss_size
    blkst.8.r   #0, @(0, %r0), %r2

    /* .data - data_src is in FLASH, use FAR access to be independent of position. */
    mov.32.i    %r0, (data_begin, 0)
    mov.32.i    %r2, (data_src, 0)
    /* If data_size is too large - should cause a link error - no special check necessary */
    mov.i       %r4, #data_size
    blkcp.fr    @(0, %r0), @(0, %r2), %r4

    bsr.i       (main, %pc)
    /* fall-through */


/* ANSI C exit() function */
    .global exit
    .type   exit, @function
exit:
$exit_loop:
    halt
    bra.i       ($exit_loop, %pc)



/*BACKUP*/
// handleContextBackup
    .global $handleContextBackup

$handleContextBackup:
    .global $context_backup

$context_backup:

    /* save context. stack will contain:
         <TOP>
        pcH (*),
        pcL (*),
        r1,
        r0,
        flag, <- up to here done by hardware/interrupt entry
        r7,
        r6,
        r5,
        r4,
        r3,
        r2,
        brke,
        vpH,
        vpL,
        brk3H,
        brk3L,
        brk2H,
        brk2L,
        brk1H,
        brk1L,
        brk0H,
        brk0L
        <BOT>
        sp_ret = sp0,
        (*) pc is fixed:
            decremented with 2 so as to
            re-execute the xap_sleep after context restore.
    */
    /* only use r0 and/or r1 before backing up the other registers */
    ld.8z.i    %r0, @(GP_WB_STANDBY_STANDBY_CONTROLLER_STATE_ADDRESS, 0)
    and.i      %r0, %r0, #GP_WB_STANDBY_STANDBY_CONTROLLER_STATE_MASK
    cmp.i      %r0, #(GP_WB_ENUM_STANDBY_STATE_ACTIVE \
                << GP_WB_STANDBY_STANDBY_CONTROLLER_STATE_LSB)
    bne        ($unexp_state, %pc)

    /* xap context save part */
    push        {%r7-%r2}, #0
    mova2r.32   %r0, %vp
    movs2r      %r2, %brke
    push        {%r2-%r0}, #0
    movb2r.32   %r0, %brk0
    movb2r.32   %r2, %brk1
    movb2r.32   %r4, %brk2
    movb2r.32   %r6, %brk3
    push        {%r7-%r0}, #0
    mova2r.16   %r2, %sp0
    /* fix pc value */
    st.i        %r2, @($sp_ret + 0x0, 0)
    ld.32.i     %r0, @(+40, %r2)
    add.32.i    %r0, %r0, #-2
    st.32.i     %r0, @(+40, %r2)

    /*sample retention !!!*/
    st.1.i      #1, GP_WB_ADDRESS_U1(STANDBY_SAMPLE_RET)

$sm_backup:
    bsr.i       (sm_backup_custom, %pc)
    /* Clear: do backup interrupt */
    st.1.i      #1, GP_WB_ADDRESS_U1(XAP5_GP_FLAG_BACKUP_DONE)

    
    /*** Unmask all internal interrupts, 
    if interrupt triggered during the going to sleep proces we fall through the rom routine 
    We will end up in unknow state ! */
    st.i       #0, @(GP_WB_INT_CTRL_MASK_INT_ES_INTERRUPT_ADDRESS, 0)      //24bit register
    st.8.i     #0, @(GP_WB_INT_CTRL_MASK_INT_ES_INTERRUPT_ADDRESS + 2, 0)
    
    /*** ROM_go_to_sleep(GP_WB_READ_U8(GP_WB_STANDBY_CRC_ENABLE_ADDRESS); */
    ld.8z.i     %r2, @(GP_WB_STANDBY_CRC_ENABLE_ADDRESS, 0)
    jump_to_rom GO_TO_SLEEP
    /* does not return */

    /* we use a specific softreset insn,
     * handler will be able to find back softreset pc
     */
$unexp_state:
    softreset


$sm_restore_entry:
    /* TODO: do we need this? st.1.i      #1, GP_WB_ADDRESS_U1(STANDBY_MW_VALID) */

/*
 * Now check here if we need to restore our state
 * From the standby controller unmasked interrupts we can derive:
 *    (GPHAL_REGISTER_STANDBY_UNMASKED_INTERRUPT_STATUS_1)
 *      - was it a POR? -> GP_WB_STANDBY_UNMASKED_PORD_INTERRUPT
 *      - was the mw valid and CRC ok (if calculated)
 *         -> GP_WB_STANDBY_UNMASKED_SM_INVALID_INTERRUPT
 * From the snapshot status we can derive
 *    (GPHAL_REGISTER_STANDBY_SNAPSHOT_STATUS)
 *      - actual mw, actual crc ok status
 *      - if crc check was needed
 *      - if sm restore was needed.
 * Since the standby controller itself already invalidates the sm_restore needed
 * when CRC fails. It should be sufficient to only check this field.
 * But there is a lot more to check if that would turn you on ...
 */

/* the actual context restore + sm restore */
    .global $context_restore
$context_restore:
    /* Restore context, see context_backup for data layout. */
    ld.i        %r2, @($sp_ret + 0x0, 0)
    movr2a.16   %sp0, %r2
    pop         {%r0-%r7}, #0
    movr2b.32   %brk0, %r0
    movr2b.32   %brk1, %r2
    movr2b.32   %brk2, %r4
    movr2b.32   %brk3, %r6
    pop         {%r0-%r2}, #0
    movr2a.32   %vp, %r0
    movr2s      %brke, %r2

    .global $handleSmRestore
$handleSmRestore:
    bsr.i       (sm_restore_custom, %pc)
    /* preload restored registers! */
    st.1.i      #1, GP_WB_ADDRESS_U1(STANDBY_PRELOAD_RET)
    pop         {%r2-%r7}, #0
$restore_return:
    /* TODO: WVHO - do we need all this? Already restored - or PMUD not necessary? */
    /* TODO: clock enable strategy*/
    st.1.i      #1, GP_WB_ADDRESS_U1(STANDBY_ENABLE_MAIN_CLOCKS)
    /* Enable io-ring */
    st.1.i      #1, GP_WB_ADDRESS_U1(STANDBY_ACTIVATE_IORING)
    st.1.i      #1, GP_WB_ADDRESS_U1(PMUD_GPIO_FUNCTION_ENABLE)

    // return from interrupt into the xap_sleep_nop restore flags, r0, r1, pc
    rtie

   .equ sm_backup_custom, default_sm_backup
   .weak sm_backup_custom
   .equ sm_restore_custom, default_sm_restore
   .weak sm_restore_custom

   .global default_sm_backup
default_sm_backup:
    /* Set the CLU in the correct mode */
    mov.i       %r0, #(GP_WB_ENUM_CLU_MODE_BACKUP << GP_WB_XAP5_GP_CLU_MODE_LSB)
    st.8.i      %r0, @(GP_WB_XAP5_GP_CLU_MODE_ADDRESS, 0)
    /* Call the backup restore function */
    bsr.i       ($sm_backup_restore_default, %pc)
    pop.ret     {}, #0

   .global default_sm_restore
default_sm_restore:
    /* Set the CLU in the correct mode */
    mov.i       %r0, #(GP_WB_ENUM_CLU_MODE_RESTORE << GP_WB_XAP5_GP_CLU_MODE_LSB)
    st.8.i      %r0, @(GP_WB_XAP5_GP_CLU_MODE_ADDRESS, 0)
    /* Before calling the restore function, open the change window for the watchdog */
    mov.i       %r0, #(0x55)
    st.8.i      %r0, @(GP_WB_WATCHDOG_KEY_ADDRESS, 0)
    /* call the backup restore function */
    bsr.i       ($sm_backup_restore_default, %pc)
    /* let the outside world know that we did sm restore */
    pop.ret     {}, #0

    /* pException and ExceptionId 
       need to be declared in code. */
    .weak handle_generic_exception
    .type handle_generic_exception, @function
handle_generic_exception:
    /* {r1,r0} holds address of faulty instruction, store it for later usage */
    st.32.i     %r0, @(pException,0)
    st.8.i      %r2, @(ExceptionId,0)
    softreset

    /* Define your own function:
      void unexpected_int(void)
      if desired. */
    .weak unexpected_int
    .type unexpected_int, @function
unexpected_int:
    /*
     * Version suitable for development:
     * Stop everything with a breakpoint.
     */
    brk
    halt

    /* Define your own function: 
       void handle_soft_reset(flags, errorcode, sp) 
       if desired. */
    .weak handle_soft_reset
    .type handle_soft_reset, @function
handle_soft_reset:
    bra.i       (unexpected_int, %pc)
