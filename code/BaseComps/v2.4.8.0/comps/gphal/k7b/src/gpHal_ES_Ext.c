/*
 * Copyright (c) 2014, GreenPeak Technologies
 *   
 *  This file defines all functions for the event scheduler and sleep modes.  These functions can be used to schedule certain actions : an interrupt, TX of a packet, etc.
 *  The diffent sleep and wakeup modes can also be initialized and used with these functions.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/src/gpHal_ES_Ext.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpHal.h"
#include "gpHal_DEFS.h"
#include "gpHal_ES.h"

#include "gpHal_HW.h"
#include "gpHal_reg.h"
#include "gpAssert.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

#define GP_ES_OFFSET_STATE                                14

#define GP_ES_SYMBOL_TIME_SHIFT                           4
#define GP_ES_SYMBOL_TIME                                 (0x1 << GP_ES_SYMBOL_TIME_SHIFT) /*in us*/

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/
 
#define reEvaluateEvents() \
    { \
        /*ReevaluateEvents + Wait untill done*/ \
        GP_WB_ES_REEVALUATE_EVENTS(); \
        GP_DO_WHILE_TIMEOUT_ASSERT(GP_WB_READ_ES_NEXT_EVENT_SEARCH_BUSY(), GP_HAL_DEFAULT_TIMEOUT); \
    };

#define setEventValid(eventNbr,enable) GP_HAL_READMODIFYWRITE_REG(GPHAL_REGISTER_ES_VALID_EVENTS_0 + eventNbr/8, ( 1 << (eventNbr % 8) ), ( (enable ? 1:0) << (eventNbr % 8) ) )

/** @brief Reads a complete eventDescriptor_t structure from the corresponding Absolute Event entry.
 *
 *  Reads a complete eventDescriptor_t structure from the corresponding Absolute Event entry.
 *
 * @param pAbsoluteEventDescriptor Pointer to the AbsoluteEventDescriptor_t structure where the the Event options will be read to.
 * @param eventNbr                 The index of the Absolute Event (1..16).
*/
#define GP_ES_READ_EVENT_DESCRIPTOR(pAbsoluteEventDescriptor, eventNbr )        \
    do {                                                                        \
        COMPILE_TIME_ASSERT(sizeof(*(pAbsoluteEventDescriptor))==GP_MM_RAM_EVENT_OFFSET); \
        GP_HAL_READ_BYTE_STREAM                                                        \
            ( GP_MM_RAM_EVENT_START + (eventNbr) * GP_MM_RAM_EVENT_OFFSET,   \
              (pAbsoluteEventDescriptor),                                       \
              GP_MM_RAM_EVENT_OFFSET                                          \
              );                                                                \
                                                                                \
        /* Convert endianness (if needed) */                                    \
        RF_TO_HOST_UINT32(&(pAbsoluteEventDescriptor)->exTime);                 \
        RF_TO_HOST_UINT32(&(pAbsoluteEventDescriptor)->recPeriod);              \
        RF_TO_HOST_UINT16(&(pAbsoluteEventDescriptor)->recAmount);              \
        RF_TO_HOST_UINT16(&(pAbsoluteEventDescriptor)->customData);             \
        RF_TO_HOST_ENUM(&(pAbsoluteEventDescriptor)->control);                  \
        RF_TO_HOST_ENUM(&(pAbsoluteEventDescriptor)->type);                     \
    } while (false)

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

//-------------------------------------------------------------------------------------------------------
//  EVENT SCHEDULER FUNCTIONS
//-------------------------------------------------------------------------------------------------------
void gpHal_ResetTime(void)
{
    DISABLE_GP_GLOBAL_INT();
    
    GP_WB_ES_APPLY_EXTERNAL_ZERO_CALIBRATION();
    //Wait for application of calibration to finish
    GP_DO_WHILE_TIMEOUT_ASSERT(GP_WB_READ_ES_APPLY_EXTERNAL_ZERO_CALIBRATION_BUSY(), GP_HAL_DEFAULT_TIMEOUT);
    //Wait for symbol time to be calculated
    GP_DO_WHILE_TIMEOUT_ASSERT(!GP_WB_READ_ES_SYMBOL_COUNTER_UPDATED_SINCE_LAST_CALIBRATION(), GP_HAL_DEFAULT_TIMEOUT);    

    ENABLE_GP_GLOBAL_INT();
}

//-------------------------------------------------------------------------------------------------------
//  ABSOLUTE EVENT SCHEDULER FUNCTIONS
//-------------------------------------------------------------------------------------------------------

#ifdef GP_COMP_GPHAL_ES_ABS_EVENT

gpHal_Result_t gpHal_MonitorAbsoluteEvent(UInt8 eventNbr, gpHal_AbsoluteEventDescriptor_t* pAbsoluteEvent)
{
    GP_ASSERT_SYSTEM(false); //Initial copy - SW-2025
#if 0
    gpHal_Result_t result;
    UInt16         validEvents;
    UInt16         checkEvent;

    DISABLE_GP_GLOBAL_INT();

    GP_HAL_READ_REGS16(GPHAL_REGISTER_ES_VALID_EVENTS_0 , &validEvents );

    checkEvent = validEvents & (((UInt16)(1)) << eventNbr);

    if (checkEvent != 0x00)
    {
        setEventValid(eventNbr, false);     //Mark event invalid
        reEvaluateEvents();                 //ReevaluateEvents + Wait until done
        GP_ES_READ_EVENT_DESCRIPTOR( pAbsoluteEvent , eventNbr );  //Read Absolute Event
        
        pAbsoluteEvent->exTime    = pAbsoluteEvent->exTime    << GP_ES_TIME_UNIT_SHIFT; //Give time back in us
        pAbsoluteEvent->recPeriod = pAbsoluteEvent->recPeriod << GP_ES_TIME_UNIT_SHIFT;

        setEventValid(eventNbr, true);  //MarkEventValid
        reEvaluateEvents();             //ReevaluateEvents

        result = gpHal_ResultSuccess;
    }
    else
    {
        result = gpHal_ResultInvalidHandle;
    }
    ENABLE_GP_GLOBAL_INT();

    return result;
#endif
    return gpHal_ResultInvalidHandle;
}
#endif //GP_COMP_GPHAL_ES_ABS_EVENT

//-------------------------------------------------------------------------------------------------------
//  RELATIVE AND EXTERNAL EVENT SCHEDULER FUNCTIONS
//-------------------------------------------------------------------------------------------------------

#ifdef GP_COMP_GPHAL_ES_EXT_EVENT
#ifdef GP_DIVERSITY_GPHAL_EXTERN
gpHal_Result_t gpHal_UnscheduleExternalEvent(void)
{
    GP_ASSERT_SYSTEM(false); //Initial copy - SW-2025
#if 0
    DISABLE_GP_GLOBAL_INT();

    //Mark invalid
    GP_WB_WRITE_ES_EXTERNAL_EVENT_VALID(false);
    //Disable wake up through external Event
    GPHAL_ES_DISABLE_EXT_WAKEUP();
    //Update PSM
    GP_WB_WRITE_STANDBY_MCB_CLK_MODE(true);

    ENABLE_GP_GLOBAL_INT();
#endif
    return gpHal_ResultSuccess;
}

gpHal_Result_t gpHal_MonitorExternalEvent(gpHal_ExternalEventDescriptor_t* pExternalEventDescriptor)
{
    gpHal_Result_t result=gpHal_ResultSuccess;
    
    GP_ASSERT_SYSTEM(false); //Initial copy - SW-2025
#if 0
    if(GP_WB_READ_ES_EXTERNAL_EVENT_VALID()) 
    {
        pExternalEventDescriptor->type = (gpHal_EventType_t)GP_WB_READ_ES_EXTERNAL_EVENT_TYPE_TO_BE_EXECUTED();
    } 
    else //No valid External event present
    {
        result = gpHal_ResultInvalidRequest;
    }
#endif
    return result;
}
#endif //GP_DIVERSITY_GPHAL_EXTERN
#endif //GP_COMP_GPHAL_ES_EXT_EVENT

