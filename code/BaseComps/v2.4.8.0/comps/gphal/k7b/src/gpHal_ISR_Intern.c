/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 *   
 *  The file gpHal.h contains the general functions of the HAL (init, reset, interrupts).
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/src/gpHal_ISR_Intern.c#1 $
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
#include "gpLog.h"
#include "hal_WB.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
 
/*****************************************************************************
 *                    Inline Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/


void gpHal_Interrupt(void)
{
    while ( GP_WB_READ_INT_CTRL_UNMASKED_PHY_INTERRUPT() )
    {
        gpHal_PHYInterrupt();
    }

    while (GP_WB_READ_INT_CTRL_UNMASKED_RCI_INTERRUPT())
    {
        gpHal_RCIInterrupt();
    }
   
#if defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)
    while (GP_WB_READ_INT_CTRL_UNMASKED_ES_INTERRUPT())
    {
        GP_LOG_PRINTF("esl",0);
        gpHal_ESInterrupt();
    }
#endif //defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)
}

INTERRUPT_H void handle_trc_int(void)
{
    GP_ASSERT_SYSTEM(false); //Initial copy - SW-2025
#if 0
    hal_IntHandlerPrologue();
    GP_LOG_PRINTF("trc",0);
    gpHal_MACInterrupt();
#endif    
}

INTERRUPT_H void handle_stbc_int(void)
{
    hal_IntHandlerPrologue();
    gpHal_STBCInterrupt();
}

