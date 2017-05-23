/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 * gpHal_HW_SPI.c
 *
 * This file contains the functions dependent on the choice of MCU : SPI, Interrupts,...
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/src/gpHal_Sleep.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "hal.h"
#include "gpHal.h"
#include "gpHal_DEFS.h"
#include "gpAssert.h"


#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

#define GP_HAL_STANDBYMODEDURINGSLEEP 0xFF

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

#ifdef GP_DIVERSITY_GPHAL_EXTERN
UInt8 gpHal_awakeCounter=1;
#endif //GP_DIVERSITY_GPHAL_EXTERN

STATIC UInt8 gpHal_StandbyModeToBeUsedDuringSleep = GP_WB_ENUM_STANDBY_MODE_RC_MODE;

void gpHal_WriteSleepMode(gpHal_SleepMode_t mode);

/*****************************************************************************
 *                    static Function Definitions
 *****************************************************************************/
/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpHal_InitSleep(void)
{
#ifdef GP_DIVERSITY_GPHAL_EXTERN
    gpHal_awakeCounter=1;
#endif //GP_DIVERSITY_GPHAL_EXTERN
    gpHal_StandbyModeToBeUsedDuringSleep = GP_WB_ENUM_STANDBY_MODE_RC_MODE;
}

void gpHal_GoToSleepWhenIdle(Bool enable)
{
}

#if defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)
#if !defined(GP_DIVERSITY_GPHAL_EXTERN)
void gpHal_SetSleepMode(gpHal_SleepMode_t mode)
{
    gpHal_StandbyModeToBeUsedDuringSleep = mode;
}

void gpHal_WriteSleepMode(gpHal_SleepMode_t mode)
{
    if(mode == GP_HAL_STANDBYMODEDURINGSLEEP)
    {
        mode = gpHal_StandbyModeToBeUsedDuringSleep;
    }

    DISABLE_GP_GLOBAL_INT();
    
    if( mode == gpHal_SleepMode32kHz )
    {
        GP_ASSERT_SYSTEM( GP_BSP_32KHZ_CRYSTAL_AVAILABLE() );
    }

    //Set mode
    GP_WB_WRITE_PMUD_STBY_MODE(mode);
    //Wait until sleep clock is apply by performing a dummy calibration
    gpHalES_ApplySimpleCalibration();
    //Measure and apply benchmark
    gpHalES_ApplyBenchmarkCalibration();

    ENABLE_GP_GLOBAL_INT();
}

gpHal_SleepMode_t gpHal_GetSleepMode(void)
{
    return GP_WB_READ_PMUD_STBY_MODE();
}
#else
void gpHal_SetSleepMode(gpHal_SleepMode_t mode)
{

}

gpHal_SleepMode_t gpHal_GetSleepMode(void)
{
    return gpHal_SleepMode16MHz;
}
#endif

#endif // defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)

