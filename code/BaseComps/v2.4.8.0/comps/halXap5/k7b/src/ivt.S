// XAP5 ivt.S Interrupt/Exception Vector Table

    .section    .vectors,"a",@progbits
    .p2align    1

    .org 0x0000
$ivt:
    // 0x00 Reserved entry, but used as VPP
    .addr ($ivt, 0)

    /* Reset handlers
     * Enter supervisor mode
     */
    .org 0x04

    // 0x04 HardReset
    .addr ($hard_reset, 0)

    // 0x08 SoftReset
    .addr ($soft_reset, 0)

    /*
     * User/Trusted mode Exception Handlers
     * Enter supervisor mode
     * We don't use User/Trusted mode, hence NA
     */
    .org 0x0c

    // InstructionError_S
    .addr (unexpected_int, 0)

    // 0x10 NullPointer_S
    .addr (unexpected_int, 0)

    // DivideByZero_S
    .addr (unexpected_int, 0)

    // UnknownInstruction_S
    .addr (unexpected_int, 0)

    // AlignError_S
    .addr ($anonymous_exception_hdlr, 0)

    // 0x20 MMUDataError_S
    .addr ($anonymous_exception_hdlr, 0)

    // MMUProgError_S
    .addr ($anonymous_exception_hdlr, 0)

    // MMUUserDataError_S (From User Mode)
    .addr ($anonymous_exception_hdlr, 0)

    // MMUUserProgError_S (From User Mode)
    .addr ($anonymous_exception_hdlr, 0)

    /*
     * User/Trusted mode System Calls/Services
     * Enter Trusted mode
     * We don't use User/Trusted mode, hence NA
     */
    .org 0x30

    // 0x30 SysCall0_T
    .addr (unexpected_int, 0)

    // SysCall1_T
    .addr (unexpected_int, 0)

    // SysCall2_T
    .addr (unexpected_int, 0)

    // SysCall3_T
    .addr (unexpected_int, 0)

    // 0x40 SingleStep_T
    .addr (unexpected_int, 0)

    // Break_T
    .addr (unexpected_int, 0)

    // PrivInstruction_S, see User/Trusted Exceptions
    .addr ($anonymous_exception_hdlr, 0)

    /*
     * Supervisor Mode/Interrupt Mode Exception Handlers
     * enter Recovery mode
     */
    .org 0x4c

    // 0x4C InstructionError_R
    .addr (handle_instruction_error_exception, 0)

    // 0x50 NullPointer_R
    .addr (handle_nullpointer_exception, 0)

    // 0x54 DivideByZero_R
    .addr (handle_div_by_zero_exception, 0)

    // 0x58 UnknownInstruction_R
    .addr (handle_unknown_instruction_exception, 0)

    // 0x5C AlignError_R
    .addr (handle_align_error, 0)

    // 0x60 MMUDataError_R
    .addr (handle_mmu_data_exception, 0)

    // 0x64 MMUProgError_R
    .addr (handle_mmu_prog_exception, 0)

    /*
     * Supervisor/Interrupt Mode System Calls
     * stay in current mode
     */
    .org 0x70

    // 0x70 SysCall0_SI
    .addr (unexpected_int, 0)

    // 0x74 SysCall1_SI
    .addr (unexpected_int, 0)

    // 0x78 SysCall2_SI
    .addr (unexpected_int, 0)

    // 0x7C SysCall3_SI
    .addr (unexpected_int, 0)

    /*
     * Non Maskable Interrupt (NMI) Handlers
     * Enter NMI mode
     * Our Interrupt logic does not produce NMI's
     */
    .org 0x80

     // 0x80 Int00
    .addr (unexpected_int, 0)

     // 0x84 Int01
    .addr (unexpected_int, 0)

     // 0x88 Int02
    .addr ($handleContextBackup, 0)

     // 0x8C Int03
    .addr (unexpected_int, 0)

     /*
      * Maskable Interrupt Handlers
      * Enter Interrupt mode
      */
      // 0x90 Interrupt4, vhdl idx 0
    .addr (handle_adcif_int, 0)

      // Interrupt5, vhdl idx 1
    .addr (handle_dma_int, 0)

     // Interrupt6, vhdl idx 2
    .addr (handleESInt, 0)

     // Interrupt7, vhdl idx 3
    .addr (handleGpioInt, 0)

    // Interrupt8, vhdl idx 4
    .addr (handle_i2csl_int, 0)

     // Interrupt9, vhdl idx 5
    .addr (handle_ipc_int, 0)

     // Interrupt10, vhdl idx 6
    .addr (handle_ir_int, 0)

     // 0xa0 Interrupt11, vhdl idx 7
    .addr (handleKeypInt, 0)

     // Interrupt12, vhdl idx 8
    .addr (handle_macfilt_int, 0)

     // Interrupt13, vhdl idx 9
    .addr (handle_mri_int, 0)

     // Interrupt14, vhdl idx 10
    .addr (handle_phy_int, 0)

    // Interrupt15, vhdl idx 11
    .addr (handle_pwm_int, 0)

     // 0xb0 Interrupt16, vhdl idx 12
    .addr (handle_qta_int, 0)

     // Interrupt17, vhdl idx 13
    .addr (handle_rci_int, 0)

     // Interrupt18, vhdl idx 14
    .addr (handle_spim_int, 0)

    // Interrupt19, vhdl idx 15
    .addr (handle_spisl_int, 0)

     // Interrupt20, vhdl idx 16
    .addr (handle_ssp_int, 0)

     // 0xc0 Interrupt21, vhdl idx 17
    .addr (handle_stbc_int, 0)

     // Interrupt22, vhdl idx 18
    .addr (handle_trc_int, 0)

     // Interrupt23, vhdl idx 19
    .addr (handle_twims_int,  0)

     // Interrupt24, vhdl idx 20
    .addr (handleUartInt, 0)

     // 0xd0 Interrupt25, vhdl idx 21
    .addr (handle_watchdog_int, 0)

     // 0xe0 Interrupt26
    .addr (unexpected_int, 0)

     // Interrupt27
    .addr (unexpected_int, 0)

     // Interrupt28
    .addr (unexpected_int, 0)

     // Interrupt29
    .addr (unexpected_int, 0)

     // 0xF0 Interrupt30
    .addr (unexpected_int, 0)

    // 0xF0 Interrupt31
    .addr (unexpected_int, 0)

    .text
//we define all interrupt handlers above here as weak aliasses for wr_unexpected_int.
//this way, you don't need to define them all, undefined ones are treated as unexpected.

    .equ handle_adcif_int, wr_unexpected_int
    .weak handle_adcif_int

    .equ handle_dma_int, wr_unexpected_int
    .weak handle_dma_int

    .equ handle_pwm_int, wr_unexpected_int
    .weak handle_pwm_int

    .equ handle_spisl_int, wr_unexpected_int
    .weak handle_spisl_int

    .equ handleESInt, wr_unexpected_int
    .weak handleESInt

    .equ  handle_macfilt_int, wr_unexpected_int
    .weak handle_macfilt_int

    .equ  handle_stbc_int, wr_unexpected_int
    .weak handle_stbc_int

    .equ  handle_trc_int, wr_unexpected_int
    .weak handle_trc_int

    .equ  handle_twims_int, wr_unexpected_int
    .weak handle_twims_int

    .equ handle_mri_int, wr_unexpected_int
    .weak handle_mri_int

    .equ handleGpioInt, wr_unexpected_int
    .weak handleGpioInt

    .equ handle_i2csl_int, wr_unexpected_int
    .weak handle_i2csl_int

    .equ handle_twi_ms_int, wr_unexpected_int
    .weak handle_twi_ms_int

    .equ handle_watchdog_int, wr_unexpected_int
    .weak handle_watchdog_int

    .equ handle_ssp_int, wr_unexpected_int
    .weak handle_ssp_int

    .equ handle_phy_int, wr_unexpected_int
    .weak handle_phy_int

    .equ handle_qta_int, wr_unexpected_int
    .weak handle_qta_int

    .equ handle_rci_int, wr_unexpected_int
    .weak handle_rci_int

    .equ handleUartInt, wr_unexpected_int
    .weak handleUartInt

    .equ handleKeypInt, wr_unexpected_int
    .weak handleKeypInt

    .equ handle_spim_int, wr_unexpected_int
    .weak handle_spim_int

    .equ handle_ir_int, wr_unexpected_int
    .weak handle_ir_int

    .equ handle_ipc_int, wr_unexpected_int
    .weak handle_ipc_int

/* exception handlers
 * code (address in vp or 0xFF) is put in r2
 * ext jump to WrGenericException
 * since we need many of these handlers,
 * this code is designed to generate short instructions (2x16bit)
 */
$anonymous_exception_hdlr:
    //r2 will be destroyed anyway by softreset
    mov.i %r2, # 0xFF
    bra.i (wr_generic_exception, %pc)

    .weak handle_instruction_error_exception
handle_instruction_error_exception:
    mov.i %r2, # 0x4C
    bra.i (wr_generic_exception, %pc)

    .weak handle_nullpointer_exception
handle_nullpointer_exception:
    mov.i %r2, # 0x50
    bra.i (wr_generic_exception, %pc)

    .weak handle_div_by_zero_exception
handle_div_by_zero_exception:
    mov.i %r2, # 0x54
    bra.i (wr_generic_exception, %pc)
    
    .weak handle_unknown_instruction_exception
handle_unknown_instruction_exception:
    mov.i %r2, # 0x58
    bra.i (wr_generic_exception, %pc)

    .weak handle_align_error
handle_align_error:
    mov.i %r2, # 0x5C
    bra.i (wr_generic_exception, %pc)

    .weak handle_mmu_data_exception
handle_mmu_data_exception:
    mov.i %r2, # 0x60
    bra.i (wr_generic_exception, %pc)

    .weak handle_mmu_prog_exception
handle_mmu_prog_exception:
    mov.i %r2, # 0x64
    bra.i (wr_generic_exception, %pc)

// a wrapper around unexpected_int (so unexpected_int can be weak)
    .global wr_unexpected_int
wr_unexpected_int:
    bra.i (unexpected_int, %pc)


// a wrapper around handle_generic_exception (so handle_generic_exception can be weak)
wr_generic_exception:
    bra.i (handle_generic_exception, %pc)
