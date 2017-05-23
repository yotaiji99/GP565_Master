/*
 * Copyright (c) 2011-2014, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpReset/src/gpReset.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_RESET

#include "gpReset.h"
#include "hal.h"
#include "gpNvm.h"
#include "gpLog.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define RESET_REASON_MAX        5
/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/
#define RESET_HAL_TO_GPRESET_REASON(mapping, halReason)              ((halReason < RESET_REASON_MAX)? mapping[halReason] : gpReset_ResetReason_UnSpecified)
/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
gpReset_ResetReason_t gpReset_Reason;
#define NVM_TAG_RESET_REASON    0
const gpNvm_Tag_t ROM gpReset_NvmSection[] FLASH_PROGMEM = {
    {&gpReset_Reason , sizeof(gpReset_ResetReason_t), gpNvm_UpdateFrequencyInitOnly, NULL}
};

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpReset_Init(void)
{
    // local mapping from hal reset reasons to gpReset reset reasons
    gpReset_ResetReason_t resetReasonMapping[RESET_REASON_MAX] = { 
            gpReset_ResetReason_UnSpecified, gpReset_ResetReason_HW_Por, gpReset_ResetReason_SW_Por,
            gpReset_ResetReason_HW_BrownOutDetected, gpReset_ResetReason_HW_Watchdog
    };

// fixme: should only be kept for backwards compatibility
    gpNvm_RegisterSection(GP_COMPONENT_ID, gpReset_NvmSection, number_of_elements(gpReset_NvmSection), NULL);


    // get hal reset reason and convert to gpReset_ResetReason
    gpReset_Reason = RESET_HAL_TO_GPRESET_REASON(resetReasonMapping, HAL_GET_RESET_REASON());
}

void gpReset_ResetBySwPor(void)
{
    HAL_RESET_UC();
}

void gpReset_ResetByWatchdog(void)
{
    HAL_WDT_FORCE_TRIGGER();
}

gpReset_ResetReason_t gpReset_GetResetReason(void)
{
    return gpReset_Reason;
}

