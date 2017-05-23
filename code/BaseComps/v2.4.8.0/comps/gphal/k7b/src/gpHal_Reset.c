/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 * gphal_Reset.c
 *
 *  The file gpHal.h contains startup code of the gphal (init, reset).
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/src/gpHal_Reset.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#define COMPLETE_MAP
#include "gpHal.h"
#include "gpHal_DEFS.h"

//GP hardware dependent register definitions
#include "gpHal_HW.h"          //Containing all uC dependent implementations
#include "gpHal_reg.h"
#include "gpAssert.h"

#include "gpLog.h"
#include "gpReset.h"

#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GPHAL_TEST_ADDRESS        GP_WB_MACFILT_EXTENDED_ADDRESS_ADDRESS + 6
#define GPHAL_TEST_ADDRESS_VALUE  (gpHal_ChipEmulated ? 0x00 : 0x15) //Should be 0x15 from GP address range

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

#ifndef GP_HAL_BSP_INIT
//No generated GPIO initialization available - for external usage
#define GP_HAL_BSP_INIT()
#endif //GP_HAL_BSP_INIT

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

#ifndef GP_HAL_EXPECTED_CHIP_ID
UInt8 gpHal_ChipId;
#endif
#ifndef GP_HAL_EXPECTED_CHIP_VERSION
UInt8 gpHal_ChipVersion;
#endif
#ifndef GP_HAL_EXPECTED_CHIP_EMULATED
Bool  gpHal_ChipEmulated;
UInt8 gpHal_Analogue;
#else
#define gpHal_Analogue 7
#endif

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static void gpHal_InitRadioSettings(void);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

void gpHalReset_InitMacFilter(void)
{
    // init macfilt stuff, not really at it's place in fll lib....
    //Skipping false values - register starts out as 0
    GP_WB_WRITE_MACFILT_EXT_ADDR_TABLE_SIZE(7); //1 entry

    GP_WB_WRITE_MACFILT_ACCEPT_FT_BCN(true);
    GP_WB_WRITE_MACFILT_ACCEPT_FT_DATA(true);
    GP_WB_WRITE_MACFILT_ACCEPT_FT_ACK(true);
    GP_WB_WRITE_MACFILT_ACCEPT_FT_CMD(true);
    //GP_WB_WRITE_MACFILT_ACCEPT_FT_RSV_4(true);
    //GP_WB_WRITE_MACFILT_ACCEPT_FT_RSV_5(true);
    //GP_WB_WRITE_MACFILT_ACCEPT_FT_RSV_6(true);
    //GP_WB_WRITE_MACFILT_ACCEPT_FT_RSV_7(true);

    GP_WB_WRITE_MACFILT_ACCEPT_FV_2003(true);
    GP_WB_WRITE_MACFILT_ACCEPT_FV_2006(true);
    //GP_WB_WRITE_MACFILT_ACCEPT_FV_2010(false);
    //GP_WB_WRITE_MACFILT_ACCEPT_FV_RSV_3(false);
    //GP_WB_WRITE_MACFILT_ACCEPT_FV_RSV_4(false);
    //GP_WB_WRITE_MACFILT_ACCEPT_FV_RSV_5(false);
    //GP_WB_WRITE_MACFILT_ACCEPT_FV_RSV_6(false);
    //GP_WB_WRITE_MACFILT_ACCEPT_FV_RSV_7(false);

    //GP_WB_WRITE_MACFILT_ACCEPT_FT_BCN_SRC_ADDR_MODE_00(false);
    GP_WB_WRITE_MACFILT_ACCEPT_BCAST_DADDR(true);
    GP_WB_WRITE_MACFILT_ACCEPT_BCAST_PAN_ID(true);
    GP_WB_WRITE_MACFILT_SRC_PAN_ID_BEACON_CHECK_ON(true);
    GP_WB_WRITE_MACFILT_SRC_PAN_ID_DATA_COMMAND_CHECK_ON(true);
    GP_WB_WRITE_MACFILT_DST_PAN_ID_CHECK_ON(true);
    GP_WB_WRITE_MACFILT_DST_ADDR_CHECK_ON(true);
    //GP_WB_WRITE_MACFILT_CMD_TYPE_CHECK_ON(false);

    GP_WB_WRITE_MACFILT_FT_BCN_TO_QUEUE(true);
    GP_WB_WRITE_MACFILT_FT_DATA_TO_QUEUE(true);
    //GP_WB_WRITE_MACFILT_FT_ACK_TO_QUEUE(false);
    GP_WB_WRITE_MACFILT_FT_CMD_TO_QUEUE(true);
    GP_WB_WRITE_MACFILT_FT_RSV_4_TO_QUEUE(true);
    GP_WB_WRITE_MACFILT_FT_RSV_5_TO_QUEUE(true);
    GP_WB_WRITE_MACFILT_FT_RSV_6_TO_QUEUE(true);
    GP_WB_WRITE_MACFILT_FT_RSV_7_TO_QUEUE(true);

    GP_WB_WRITE_MACFILT_BROADCAST_MASK(0xFFFF);
    GP_WB_WRITE_MACFILT_SHORT_ADDRESS(0xFFFF); //Previous Kx default
}

#ifndef GP_HAL_EXPECTED_CHIP_EMULATED
static void gpHalReset_Calibrate(void)
{
}
#endif


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

//-------------------------------------------------------------------------------------------------------
//  INIT / RESET / INT handling
//-------------------------------------------------------------------------------------------------------

Bool gpHal_DidGPReset(void)
{
    if(GP_WB_READ_STANDBY_UNMASKED_PORD_INTERRUPT())
    {
        GP_WB_STANDBY_CLR_PORD_INTERRUPT();
        return true;
    }
    return false;
}

Bool gpHal_CheckMsi(void)
{
    return (GPHAL_TEST_ADDRESS_VALUE == GP_HAL_READ_REG(GPHAL_TEST_ADDRESS));
}

#ifdef GP_DIVERSITY_GPHAL_EXTERN
extern UInt8 gpHal_awakeCounter;
#endif //GP_DIVERSITY_GPHAL_EXTERN

UInt8 gpHal_IsRadioAccessible(void)
{
#ifdef GP_DIVERSITY_GPHAL_EXTERN
    return gpHal_awakeCounter;
#else
    return true;
#endif //GP_DIVERSITY_GPHAL_EXTERN
}

void gpHal_Init(Bool timedMAC)
{

    //Version checking
    gpHal_InitVersionInfo();


#ifdef GP_HAL_EXPECTED_PRODUCT_ID
    {
        const UInt8 id[] = {XSTRINGIFY(GP_HAL_EXPECTED_PRODUCT_ID)};
        if(gpHal_ParseAttr(1,-sizeof(id),id) < 0)
        {
            GP_ASSERT_SYSTEM(false);
        }
    }
#endif

    // Give SPI time to give returncode
    GP_WB_WRITE_STANDBY_RTGTS_THRESHOLD(0xFF);
    // Clocks for sleep mode switching measurements
    GP_WB_WRITE_ES_ENABLE_OSCILLATOR_BENCHMARK(1);
    GP_WB_WRITE_ES_ENABLE_CLK_TIME_REFERENCE_OSCILLATOR_BENCHMARK_BY_UC(1);

    // Set any radio related settings
    gpHal_InitRadioSettings();
    gpHalReset_InitMacFilter();

    //Enable PRG settings - clk will be enabled when taking a sample
    GP_WB_WRITE_PRG_ENABLE_RANDOM_SEED(1);
    GP_WB_WRITE_PRG_ENABLE_RANDOM_SOURCE(1); //Add extra random data

    gpHal_InitScan();
    gpHal_InitMAC(timedMAC);
    GP_HAL_BSP_INIT();


#ifdef GP_DIVERSITY_GPHAL_EXTERN
#ifdef GP_COMP_GPHAL_ES_EXT_EVENT
    // configure sleep/wakeup
    {
        gpHal_ExternalEventDescriptor_t externalEventDescriptor;

        externalEventDescriptor.type = gpHal_EventTypeDummy;
        gpHal_EnableExternalEventCallbackInterrupt(true);
        gpHal_ScheduleExternalEvent(&externalEventDescriptor);
        //Default we choose Event sleep mode (same as k4)
        gpHal_SetSleepMode(gpHal_SleepModeEvent);
    }
#endif
#endif //GP_DIVERSITY_GPHAL_EXTERN

#ifdef GP_DIVERSITY_STACK_DBG
    // as we overlap the stack with the retention memory, we need make sure the dig stays awake.
    //GP_WB_WRITE_STANDBY_(); //?
#endif //GP_DIVERSITY_STACK_DBG

#ifdef GP_COMP_GPHAL_ES_ABS_EVENT
    //Set too late event threshold to 2 us - prevents too late event when timebase jumps
    GP_WB_WRITE_ES_BUFFER_TIME(1); //2us
#endif //GP_COMP_GPHAL_ES_ABS_EVENT
}

void gpHal_AdvancedInit(void)
{
    gpHal_GoToSleepWhenIdle(true);
}

/*****************************************************************************
 *                    Kx Specific
 *****************************************************************************/

UInt16 gpHal_GetHWVersionId(void)
{
    return GP_WB_READ_STANDBY_VERSION();
}

void gpHal_InitVersionInfo(void)
{
    UInt16 HWversion = gpHal_GetHWVersionId();

    UInt8 chipId;
    UInt8 analogue = 6;
    UInt8 chipVersion  = (HWversion >> 0)  & (BM(6)-1);
    Bool chipEmulated  = (HWversion >> 11) & (BM(1)-1);

#ifdef GP_HAL_EXPECTED_CHIP_EMULATED
    //JAVM: emulated, dev_int
    GP_ASSERT_DEV_INT(chipEmulated == GP_HAL_EXPECTED_CHIP_EMULATED);
    chipId = GPHAL_CHIP_ID_K7B;
#else
    gpHal_ChipEmulated = chipEmulated;
#endif
    //GP_LOG_SYSTEM_PRINTF("%x", 0, HWversion);

    if (gpHal_ChipEmulated)
    {
        //chipId: bit 0..10.
        //but fpga uses upper three for colibri version.
        //uses different bit numbering than chip version
        UInt8 analogueId = (HWversion >> 8) & (BM(3)-1);
        chipId = ((HWversion >> 6)  & (BM(2)-1)) | ((HWversion >> (12-2))  & ((BM(4)-1)<<2));

        switch (analogueId)
        {
        case 0:
            analogue = 8; break; //colibri 8
        case 7:
            analogue = 7; break; //colibri 7
        default:
            GP_ASSERT_DEV_INT(false);
        }

        // with the new definition of chipid and revisions, the chipid of kiwi7 == 0
        if (chipId == 0)
            chipId = GPHAL_CHIP_ID_K7B;
    }
    else //silicon
    {
        chipId = GPHAL_CHIP_ID_K7B;
        analogue = 9;
    }
#ifndef GP_HAL_EXPECTED_CHIP_EMULATED
    gpHal_Analogue = analogue;
#else
    NOT_USED(analogue);
#endif

    switch (chipVersion)
    {
        case 1:
            chipVersion = 0;
            break;
        case 5: //FPGA version at the moment
            chipVersion = 0;
            break;
        default:
#ifndef GP_HAL_MM_UNKNOWN_CHIP_VERSION_HANDLING
            chipVersion = 0xFF;
#else

            gpHal_FlashReadInf(GP_HAL_MM_UNKNOWN_CHIP_VERSION_HANDLING, 1, &chipVersion);
            if (chipVersion == 0) //chipVersion is not written
            {
                //treat as last known chipVersion (all checks should use <= 2, instead of == 2)
                chipVersion = 0xFF;
            }
#endif
    }

    //Checks
#ifdef GP_HAL_EXPECTED_CHIP_ID
    GP_ASSERT_SYSTEM(chipId == GP_HAL_EXPECTED_CHIP_ID);
#else
    gpHal_ChipId = chipId;
#endif


#ifdef GP_HAL_EXPECTED_CHIP_VERSION
    GP_ASSERT_SYSTEM(chipVersion == GP_HAL_EXPECTED_CHIP_VERSION);
#else
    gpHal_ChipVersion = chipVersion;
#endif
}


#if defined(GP_DIVERSITY_GPHAL_EXTERN)
void gpHal_Reset(void)
{
    DISABLE_GP_GLOBAL_INT();
    gpHal_InitSleep();

    GP_ASSERT_DEV_EXT(GP_BSP_RESET_PIN_USED());

//To be replaced with rap routine
    HAL_GP_SET_RESET(0);
    HAL_WAIT_MS(5);  /*5ms wait time*/
    HAL_GP_SET_RESET(1);
    HAL_WAIT_MS(5);
    HAL_GP_CLR_RESET();
    {
        //Check startup interrupt
        UInt16 pollLoop = 800; //200 ms wait
        while(--pollLoop)
        {
            if(!GP_INT_IS_1)
            {
                if(GP_WB_READ_INT_CTRL_MASKED_STBC_ACTIVE_INTERRUPT())
                {
                    GP_WB_STANDBY_CLR_ACTIVE_INTERRUPT();
                    break;
                }
            }
            HAL_WAIT_US(250);
        }
        if( pollLoop == 0 )
        {
           gpReset_ResetSystem();
        }
    }

    GP_HAL_MAC_STATE_INIT(); //Reset local copies of chip settings

    ENABLE_GP_GLOBAL_INT();
}
#endif

void gpHal_InitRadioSettings(void)
{
#ifndef GP_HAL_EXPECTED_CHIP_EMULATED
    gpHalReset_Calibrate();
#endif
}
