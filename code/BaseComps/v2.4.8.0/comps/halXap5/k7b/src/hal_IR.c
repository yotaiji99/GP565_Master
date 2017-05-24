/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 * hal_IR.c
 *   Hardware Abstraction Layer for IR on XAP device.
 *
 *
 *                ,               This software is owned by GreenPeak Technologies
 *                g               and protected under applicable copyright laws.
 *               ]&$              It is delivered under the terms of the license
 *               ;QW              and is intended and supplied for use solely and
 *               G##&             exclusively with products manufactured by
 *               N#&0,            GreenPeak Technologies.
 *              +Q*&##
 *              00#Q&&g
 *             ]M8  *&Q           THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 *             #N'   Q0&          CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 *            i0F j%  NN          IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 *           ,&#  ##, "KA         LIMITED TO, IMPLIED WARRANTIES OF
 *           4N  NQ0N  0A         MERCHANTABILITY AND FITNESS FOR A
 *          2W',^^ `48  k#        PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *         40f       ^6 [N        GREENPEAK TECHNOLOGIES B.V. SHALL NOT, IN ANY
 *        jB9         `, 0A       CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 *       ,&?             ]G       INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 *      ,NF               48      FOR ANY REASON WHATSOEVER.
 *      EF                 @
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/halXap5/k7b/src/hal_IR.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_HALXAP5

//#define GP_LOCAL_LOG
#define COMPLETE_MAP

#include "hal.h"
#include "gpHal.h"

#include "gpLog.h"
#include "gpAssert.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


#ifndef HAL_IR_MAPPING
#define HAL_IR_MAPPING GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED
#endif

#define HAL_IR_BUFFER_SIZE_IN_WORDS  200
#define HAL_IR_NUM_BUFFERS  2

#define HAL_IR_RAM_POINTER_TO_OFFSET_FROM_START(pBuf)      ((((UInt16)pBuf)-GP_MM_RAM_START)/2)

#define HAL_IR_DEFAULT_DUTY_CYCLE_TIME          33

#define HAL_IS_IR_CALLBACK_REGISTERED()   halIr_IntCallback
#define HAL_CB_IR_CALLBACK(id)            halIr_IntCallback(id)

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef UInt8 hal_IRMode_t;
/*****************************************************************************
 *                    Static Data
 *****************************************************************************/

// selected IR mode is always stored in SW
hal_IRMode_t halIr_Mode;

static halIr_IntCallback_t  halIr_IntCallback;

/*****************************************************************************
 *                    External Function Definitions
 *****************************************************************************/

#ifdef GP_DIVERSITY_GPHAL_EXTERN
extern void hal_cbIrStartOfPattern(void);
#endif //GP_DIVERSITY_GPHAL_EXTERN

/*****************************************************************************
 *                    Static Function Declarations
 *****************************************************************************/
void halIR_WaitForComplete(void);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

void halIR_StartRamSequencer(hal_IRMode_t irMode, UInt16* pPattern, UInt8 numEntries)
{
    // configure buffer if needed
    if(pPattern)
    {
        GP_WB_WRITE_IR_RAM_SEQUENCE_LEN(numEntries-1);
        GP_WB_WRITE_IR_RAM_SEQUENCE_START_PTR(HAL_IR_RAM_POINTER_TO_OFFSET_FROM_START(pPattern));
    }

    if(!GP_WB_READ_IR_BUSY())
    {
        GP_WB_WRITE_STANDBY_RTGTS_SW(false);

        //force enable clock in timed mode to avoid race condition in HW (AD-1275)
        if (irMode == GP_WB_ENUM_IR_MODULATION_MODE_TIME_BASED) {
            GP_WB_WRITE_STANDBY_FON_CLK_EPI_IR(1);
        }

        // Set-up mode and mapping
        GP_WB_WRITE_IR_MODULATION_MODE(irMode);
        GP_WB_WRITE_IR_OUTPUT_PINMAP(HAL_IR_MAPPING);

        //Startup only once
        GP_WB_WRITE_IR_REPEAT_SEQUENCE(1);

        // Start sequence
        GP_WB_IR_START_RAM_SEQUENCER();
    }
    else
    {
        //Only needed when IR is already running
        GP_WB_WRITE_IR_LATCH_RAM_SEQUENCE_START_PTR_AND_LEN_ON_REPEAT(1);
    }
}

void halIR_WaitForComplete(void)
{
    GP_WB_WRITE_IR_REPEAT_SEQUENCE(0);
    GP_DO_WHILE_TIMEOUT_ASSERT(GP_WB_READ_IR_BUSY(), 1000000UL); //1s
}

INTERRUPT_H void handle_ir_int(void)
{
    hal_IntHandlerPrologue();

    /* Notify the application about the ISR
     *  - No CLR required as done in the ISR
     *  - Update application buffer handling
     */
    if(HAL_IS_IR_CALLBACK_REGISTERED())
    {
        if(GP_WB_READ_INT_CTRL_MASKED_IR_SEQUENCE_START_INTERRUPT())
        {
            HAL_CB_IR_CALLBACK(halIr_InterruptIdStart);
        }
        else if(GP_WB_READ_INT_CTRL_MASKED_IR_SEQUENCE_REPEAT_INTERRUPT())
        {
            HAL_CB_IR_CALLBACK(halIr_InterruptIdRepeat);
        }
        else if(GP_WB_READ_INT_CTRL_MASKED_IR_SEQUENCE_DONE_INTERRUPT())
        {
            HAL_CB_IR_CALLBACK(halIr_InterruptIdDone);
        }
    }

    //Clear all sources
    GP_WB_WRITE_IR_INTERRUPT_TRIGGERS(GP_WB_IR_CLR_SEQUENCE_START_INTERRUPT_MASK  |
                                      GP_WB_IR_CLR_SEQUENCE_REPEAT_INTERRUPT_MASK |
                                      GP_WB_IR_CLR_SEQUENCE_DONE_INTERRUPT_MASK);
}

#if 0
void halIR_DumpSequence(void)
{
    UInt8 i;
    UInt16 entry[2];
    UInt8 length = GP_WB_READ_IR_RAM_SEQUENCE_LEN();
    UInt16 ramStart;


    GP_LOG_SYSTEM_PRINTF("set-clr l:%u s:0x%x",0,length/2+1,halIR_StartAddress);
    for(i = 0; i < length; i+=2)
    {
        GP_HAL_READ_REGS16(halIR_StartAddress+i*2,     &entry[0]);
        GP_HAL_READ_REGS16(halIR_StartAddress+(i+1)*2, &entry[1]);
        GP_LOG_SYSTEM_PRINTF("* s:%x:%u c:%x:%u",0,ramStart+i*2, entry[0], ramStart+(i+1)*2, entry[1]);
        gpLog_Flush();
    }
}
#endif

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void hal_InitIR(void)
{
    halIr_IntCallback = NULL;

    //Clear pin selection
    GP_WB_WRITE_IR_OUTPUT_PINMAP(GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED); //Switched during execution
    GP_WB_WRITE_IR_INPUT_PINMAP( GP_WB_ENUM_GENERIC_SINGLE_PINMAP_NOT_MAPPED);

#ifdef HAL_IR_OUTPUT_OD
    GP_WB_WRITE_IR_OUTPUT_DRIVE(GP_WB_ENUM_GENERIC_IO_DRIVE_OPEN_DRAIN);
#else
    GP_WB_WRITE_IR_OUTPUT_DRIVE(GP_WB_ENUM_GENERIC_IO_DRIVE_PUSH_PULL);
#ifdef HAL_IR_ACTIVE_LOW
    GP_WB_WRITE_IR_OUTPUT_INVERT(1);
#endif
#endif

    //Interrupt setup
    GP_WB_WRITE_INT_CTRL_MASK_IR_SEQUENCE_START_INTERRUPT(1);
    GP_WB_WRITE_INT_CTRL_MASK_IR_SEQUENCE_REPEAT_INTERRUPT(1);
    GP_WB_WRITE_INT_CTRL_MASK_IR_SEQUENCE_DONE_INTERRUPT(1);
    //GP_WB_WRITE_INT_CTRL_MASK_IR_INDEX_MATCH_INTERRUPT(1);

#ifdef GP_DIVERSITY_GPHAL_EXTERN
    gpHal_RegisterIRCallback(halIR_cbIR);
    GP_WB_WRITE_INT_CTRL_MASK_EXT_IR_INTERRUPT(1);
#else
    GP_WB_WRITE_INT_CTRL_MASK_INT_IR_INTERRUPT(1);
#endif

    //Modulation generation
    // - Register based
    // - Pattern based (RAM)
    // * Time based (RAM) -> set clr sequence (even set, odd clr) //500 ns, 1 us, 2 us and 4 us
    // - Ext based (external pin)
    // - Event based (modulation on/off events)

    // SW variable: initialize to off
    halIr_Mode = GP_WB_ENUM_IR_MODULATION_MODE_MODULATION_OFF;

    GP_WB_WRITE_IR_MODULATION_MODE(GP_WB_ENUM_IR_MODULATION_MODE_MODULATION_OFF); //Switched during execution
    GP_WB_WRITE_IR_RAM_SEQUENCE_LEN(0);

    //Configuration time based
    GP_WB_WRITE_IR_TIMER_TU(GP_WB_ENUM_IR_TIME_UNIT_TU_1US);
    GP_WB_WRITE_IR_REPEAT_SEQUENCE(0);

    //Configure latching - start always needed - repeat switched on when new pattern is written
    GP_WB_WRITE_IR_LATCH_RAM_SEQUENCE_START_PTR_AND_LEN_ON_START(1);


}

void hal_SetIRCarrierPeriod(UInt16 period) //in 125ns
{
    UInt8 divider = 0;  // 62.5ns
    //Minimum T_carrier = 125ns/8MHz - 62.5ns*2^0 = 62.5ns

    GP_ASSERT_DEV_EXT(!GP_WB_READ_IR_BUSY());

    while (period >= 128)
    {
        period = period >> 1; //Calculate adjusted wrap value
        divider += 1;
    }

    //Set timing information

    //Carrier period
    //Freq: Max 8MHz  (prescale_div=0, wrap=1)
    //      Min 488Hz (prescale_div=7, wrap=255)
    GP_WB_WRITE_IR_PRESCALE_DIV(divider); // Tprescale = 62.5ns*(2**prescale_div)

    //Set carrier period + Carrier duty cycle
    if (period !=0)
    {
        // 100% wrap = 0, threshold = 0
        // 50%  wrap = (threshold+1)*2 - 1
        UInt8 wrap      = period*2-1;
        GP_WB_WRITE_IR_WRAP(wrap); //Tcarrier = Tprescale*(wrap+1) -- 62.5ns*(wrap+1)

        //Avoid wraparound use slightly larger DC on high freq (wrap = 1 -> 8MHz)
        hal_SetIRDutyCycle(HAL_IR_DEFAULT_DUTY_CYCLE_TIME);     //33%DC - (wrap/3)-1
    }
    else
    {
        //No carrier needed - PWM functionality
        //threshold >= wrap --> 100% duty cycle
        GP_WB_WRITE_IR_WRAP(0x0); //us
        GP_WB_WRITE_IR_THRESHOLD(0x0); //us
    }
}

void hal_SetIRDutyCycle(UInt8 dutyCycle)
{
    UInt8 wrap;
    UInt8 threshold;

    GP_ASSERT_DEV_EXT(dutyCycle <= 100);    // range: [0-100]
    GP_ASSERT_DEV_EXT(!GP_WB_READ_IR_BUSY());

    wrap = GP_WB_READ_IR_WRAP();
    threshold = (UInt8)(udiv_ceil((UInt32)wrap, (100UL/dutyCycle)));

    GP_WB_WRITE_IR_THRESHOLD(threshold);
}

void hal_MarkEndOfIRCode(void)
{

}

void hal_DisableIRCarrier(void)
{
    //Wait untill done
    halIR_WaitForComplete();

    //Check Mem error
    GP_ASSERT_DEV_EXT(!GP_WB_READ_IR_MEM_ERR());

    //Unmap pinmap
    GP_WB_WRITE_IR_OUTPUT_PINMAP(GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED);

    //Stop modulation
    GP_WB_WRITE_IR_MODULATION_MODE(GP_WB_ENUM_IR_MODULATION_MODE_MODULATION_OFF);

    //undo forcing of enable clock for all modes
    GP_WB_WRITE_STANDBY_FON_CLK_EPI_IR(0);

    //IR done, chip can go to sleep again
    GP_WB_WRITE_STANDBY_RTGTS_SW(true);
}

void hal_ConfigurePatternMode(UInt8 timeUnit)
{
    halIr_Mode = GP_WB_ENUM_IR_MODULATION_MODE_PATTERN_BASED;
    GP_WB_WRITE_IR_CARRIER_TU(timeUnit);
}

void hal_ConfigureTimeMode(UInt8 timeUnit)
{
    halIr_Mode = GP_WB_ENUM_IR_MODULATION_MODE_TIME_BASED;
    GP_WB_WRITE_IR_TIMER_TU(timeUnit);
}

void hal_ConfigureRegisterMode(void)
{
    halIr_Mode = GP_WB_ENUM_IR_MODULATION_MODE_REGISTER_BASED;
    GP_WB_WRITE_IR_MODULATION_MODE(halIr_Mode);
    GP_WB_WRITE_IR_OUTPUT_PINMAP(HAL_IR_MAPPING);
}

void hal_SetIRRegisterValue(Bool on)
{
    GP_WB_WRITE_IR_MODULATION(!!on);
}

void hal_PlaySequenceFromRam(UInt16* pPattern, UInt8 numEntries)
{
    if(halIr_Mode == GP_WB_ENUM_IR_MODULATION_MODE_TIME_BASED || halIr_Mode == GP_WB_ENUM_IR_MODULATION_MODE_PATTERN_BASED)
    {
        halIR_StartRamSequencer(halIr_Mode, pPattern, numEntries);
    }
    else
    {
        GP_LOG_PRINTF("Wrong mode: exp %x or %x, curr: %x",0,GP_WB_ENUM_IR_MODULATION_MODE_TIME_BASED,GP_WB_ENUM_IR_MODULATION_MODE_PATTERN_BASED,halIr_Mode);
    }
}

//IR register callbacks
void hal_RegisterIrIntCallback(halIr_IntCallback_t callback)
{
    halIr_IntCallback = callback;
}

