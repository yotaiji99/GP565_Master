/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 * gpHal_ES.c
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/src/gpHal_ES.c#1 $
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

#define setEventValid(eventNbr,enable) GP_HAL_READMODIFYWRITE_REG(GP_WB_ES_VALID_EVENTS_ADDRESS + eventNbr/8, ( 1 << (eventNbr % 8) ), ( (enable ? 1:0) << (eventNbr % 8) ) )

/** @brief Writes a complete eventDescriptor_t structure to the corresponding Absolute Event entry.
 *
 *  Writes a complete eventDescriptor_t structure to the corresponding Absolute Event entry.
 *
 * @param pAbsoluteEventDescriptor Pointer to the AbsoluteEventDescriptor_t structure containing the Event options.
 * @param eventNbr                 The index of the Absolute Event (1..16).
*/
#define GP_ES_WRITE_EVENT_DESCRIPTOR(pAbsoluteEventDescriptor, eventNbr )       \
    do {                                                                        \
        /* Convert endianness (if needed) */                                    \
        HOST_TO_RF_UINT32(&(pAbsoluteEventDescriptor)->exTime);                 \
        HOST_TO_RF_UINT32(&(pAbsoluteEventDescriptor)->recPeriod);              \
        HOST_TO_RF_UINT16(&(pAbsoluteEventDescriptor)->recAmount);              \
        HOST_TO_RF_UINT16(&(pAbsoluteEventDescriptor)->customData);             \
        HOST_TO_RF_ENUM(&(pAbsoluteEventDescriptor)->control);                  \
        HOST_TO_RF_ENUM(&(pAbsoluteEventDescriptor)->type);                     \
                                                                                \
        GP_HAL_WRITE_BYTE_STREAM                                                       \
            (                                                                   \
             (gpHal_Address_t)(GP_MM_RAM_EVENT_START + (eventNbr) * GP_MM_RAM_EVENT_OFFSET),    \
             (pAbsoluteEventDescriptor),                                        \
             GP_MM_RAM_EVENT_OFFSET                                           \
             );                                                                 \
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
 *                    Static Data Definitions
 *****************************************************************************/

#ifdef GP_COMP_GPHAL_ES_ABS_EVENT
static UInt16 gpHal_ES_AbsoluteEventsInUse = 0;
#endif //GP_COMP_GPHAL_ES_ABS_EVENT

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpHalES_ApplySimpleCalibration(void)
{
    GP_WB_ES_APPLY_EXTERNAL_SIMPLE_CALIBRATION();
    GP_DO_WHILE_TIMEOUT_ASSERT(GP_WB_READ_ES_APPLY_EXTERNAL_SIMPLE_CALIBRATION_BUSY(), GP_HAL_DEFAULT_TIMEOUT);
    GP_DO_WHILE_TIMEOUT_ASSERT(!GP_WB_READ_ES_SYMBOL_COUNTER_UPDATED_SINCE_LAST_CALIBRATION(), GP_HAL_DEFAULT_TIMEOUT);    
}

//Calculated from relation in 10.1.6 + table 10.1.2
#define GP_WB_ES_BENCHMARK_CALIBRATION_FACTOR 0x200000UL
void gpHalES_ApplyBenchmarkCalibration(void)
{
    UInt32 benchmark;

    //Perform benchmark measurement (procedure from databook 10.1.6)
    GP_WB_ES_TRIGGER_OSCILLATOR_BENCHMARK();
    GP_DO_WHILE_TIMEOUT_ASSERT(!GP_WB_READ_ES_OSCILLATOR_BENCHMARK_SECOND_EDGE_SEEN(), GP_HAL_DEFAULT_TIMEOUT);
    benchmark = GP_WB_READ_ES_OSCILLATOR_BENCHMARK_COUNTER();
    
    //Apply calibration
    GP_WB_WRITE_ES_CALIBRATION_FACTOR(benchmark*GP_WB_ES_BENCHMARK_CALIBRATION_FACTOR);
    gpHalES_ApplySimpleCalibration();
}

//-------------------------------------------------------------------------------------------------------
//  ENABLE / DISABLE CALLBACK FUNCTIONS
//-------------------------------------------------------------------------------------------------------

//ES interrupts
#ifdef GP_COMP_GPHAL_ES_ABS_EVENT
#define GPHAL_REGISTER_INT_CTRL_MASK_ES_EVENT_INTERRUPT_0 GP_WB_INT_CTRL_MASK_ES_EVENT_INTERRUPT_ADDRESS
void gpHal_EnableAbsoluteEventCallbackInterrupt(UInt8 eventNbr, Bool enable)
{
    //Enable interrupt mask for the selected absolute interrupt
    DISABLE_GP_GLOBAL_INT();
    GP_HAL_READMODIFYWRITE_REG(GPHAL_REGISTER_INT_CTRL_MASK_ES_EVENT_INTERRUPT_0 + eventNbr/8, ( 1 << (eventNbr % 8) ), ( (enable ? 1:0) << (eventNbr % 8) ) );
    ENABLE_GP_GLOBAL_INT();
}
#endif
//-------------------------------------------------------------------------------------------------------
//  EVENT SCHEDULER FUNCTIONS
//-------------------------------------------------------------------------------------------------------

void gpHal_GetTime(UInt32* pTime)
{
    DISABLE_GP_GLOBAL_INT();
    *pTime = GP_WB_READ_ES_AUTO_SAMPLED_SYMBOL_COUNTER();
    ENABLE_GP_GLOBAL_INT();
    (*pTime) <<= GP_ES_TIME_UNIT_SHIFT;
}

void gpHal_ApplyCalibration(Int32 phaseAdjustment,UInt32 frequency)
{
    //int->uint and >>=1 this works because phase_comp is really a 31-bit property, not 32.
    UInt32 phase_comp = phaseAdjustment;
    
    phase_comp >>= GP_ES_TIME_UNIT_SHIFT;

    DISABLE_GP_GLOBAL_INT();

    //Update calibration factor + phase
    GP_WB_WRITE_ES_CALIBRATION_FACTOR(frequency);
    GP_WB_WRITE_ES_PHASE_COMPENSATION(phase_comp);

    //Trigger write of new factor
    gpHalES_ApplySimpleCalibration();
    
    ENABLE_GP_GLOBAL_INT();
}

//-------------------------------------------------------------------------------------------------------
//  ABSOLUTE EVENT SCHEDULER FUNCTIONS
//-------------------------------------------------------------------------------------------------------

#ifdef GP_COMP_GPHAL_ES_ABS_EVENT
gpHal_AbsoluteEventId_t gpHal_GetAbsoluteEvent(void)
{
    UIntLoop i;
    for (i = 0; i < GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS; i++)
    {
        if (!BIT_TST(gpHal_ES_AbsoluteEventsInUse, i)) {
            BIT_SET(gpHal_ES_AbsoluteEventsInUse, i);
            return i;
        }
    }

    /* Assert instead of returning */
    GP_ASSERT_DEV_EXT(false);

    return GPHAL_ES_ABSOLUTE_EVENT_ID_INVALID;
}

void gpHal_FreeAbsoluteEvent(gpHal_AbsoluteEventId_t eventId)
{
    GP_ASSERT_DEV_EXT(eventId < GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS);
    GP_ASSERT_DEV_EXT(BIT_TST(gpHal_ES_AbsoluteEventsInUse, eventId));
    BIT_CLR(gpHal_ES_AbsoluteEventsInUse, eventId);
}

void gpHal_RefreshAbsoluteEvent(UInt32 absTime, UInt8 control, UInt8 eventNbr)
{
    GP_ASSERT_DEV_EXT( GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS > eventNbr );

    absTime >>= GP_ES_TIME_UNIT_SHIFT;

    DISABLE_GP_GLOBAL_INT();
    setEventValid(eventNbr, false);

    HAL_WAIT_US(5);

    //Update Execution Time
    GP_HAL_WRITE_REGS32(GP_MM_RAM_EVENT_START + (eventNbr) * GP_MM_RAM_EVENT_OFFSET + offsetof(gpHal_AbsoluteEventDescriptor_t,exTime), &absTime);
    //Update Control field
    GP_HAL_WRITE_REG(GP_MM_RAM_EVENT_START + (eventNbr) * GP_MM_RAM_EVENT_OFFSET + offsetof(gpHal_AbsoluteEventDescriptor_t,control), control);

    setEventValid(eventNbr, true);
    ENABLE_GP_GLOBAL_INT();

    GP_WB_ES_REEVALUATE_EVENTS();    //refresh
}

void gpHal_ScheduleAbsoluteEvent(gpHal_AbsoluteEventDescriptor_t* pAbsoluteEventDescriptor, UInt8 eventNbr)
{
    GP_ASSERT_DEV_EXT( GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS > eventNbr );

    // Do shifting outside atomic section
    pAbsoluteEventDescriptor->exTime    = pAbsoluteEventDescriptor->exTime    >> GP_ES_TIME_UNIT_SHIFT;  // Write time in /2 us
    pAbsoluteEventDescriptor->recPeriod = pAbsoluteEventDescriptor->recPeriod >> GP_ES_TIME_UNIT_SHIFT;  // Write time in /2 us

    DISABLE_GP_GLOBAL_INT();

    setEventValid(eventNbr, false);     // Mark event Invalid
    reEvaluateEvents();                 // ReevaluateEvents + Wait untill done
    GP_ASSERT_SYSTEM((GP_WB_READ_ES_CURRENT_EVENT_NUMBER() != eventNbr)
              || (GP_WB_READ_ES_CURRENT_EVENT_PENDING() == false));
    
    GP_ES_WRITE_EVENT_DESCRIPTOR( pAbsoluteEventDescriptor , eventNbr );  //Write Absolute Event

    setEventValid(eventNbr, true);  // MarkEventValid
    reEvaluateEvents();             // ReevaluateEvents + Wait until done

    ENABLE_GP_GLOBAL_INT();

    //Leave descriptor intact
    pAbsoluteEventDescriptor->exTime    = pAbsoluteEventDescriptor->exTime    << GP_ES_TIME_UNIT_SHIFT;
    pAbsoluteEventDescriptor->recPeriod = pAbsoluteEventDescriptor->recPeriod << GP_ES_TIME_UNIT_SHIFT;
}

gpHal_EventState_t gpHal_UnscheduleAbsoluteEvent(UInt8 eventNbr)
{
    gpHal_EventState_t state;

    GP_ASSERT_DEV_EXT( GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS > eventNbr );

    DISABLE_GP_GLOBAL_INT();
    state = (gpHal_EventState_t) (GP_ES_GET_EVENT_STATE( GP_HAL_READ_REG(GP_MM_RAM_EVENT_START + (eventNbr) * GP_MM_RAM_EVENT_OFFSET + offsetof(gpHal_AbsoluteEventDescriptor_t,control)) ));

    if(state != gpHal_EventStateDone)
    {
        setEventValid(eventNbr, false);     // Mark event Invalid        
        reEvaluateEvents();                 // ReevaluateEvents + Wait untill done
        GP_ASSERT_SYSTEM((GP_WB_READ_ES_CURRENT_EVENT_NUMBER() != eventNbr)
                  || (GP_WB_READ_ES_CURRENT_EVENT_PENDING() == false));         // to be verified
    }

    ENABLE_GP_GLOBAL_INT();
    
    return state;
}
#endif //GP_COMP_GPHAL_ES_ABS_EVENT

//-------------------------------------------------------------------------------------------------------
//  RELATIVE AND EXTERNAL EVENT SCHEDULER FUNCTIONS
//-------------------------------------------------------------------------------------------------------
#ifdef GP_COMP_GPHAL_ES_REL_EVENT
void gpHal_ScheduleImmediateEvent(gpHal_EventType_t type)
{
    DISABLE_GP_GLOBAL_INT();

    GP_WB_WRITE_ES_RELATIVE_EVENT_TYPE_TO_BE_EXECUTED(type); //Write Relative event
    GP_WB_WRITE_ES_RELATIVE_EVENT_TIME_DELAY(0);             //Schedule immediate
    GP_WB_ES_RELATIVE_EVENT_EXECUTE();                       //Execute Event

    ENABLE_GP_GLOBAL_INT();
}
#endif //GP_COMP_GPHAL_ES_REL_EVENT

#ifdef GP_COMP_GPHAL_ES_EXT_EVENT
#ifdef GP_DIVERSITY_GPHAL_EXTERN
void gpHal_ScheduleExternalEvent(gpHal_ExternalEventDescriptor_t* pExternalEventDescriptor)
{
    DISABLE_GP_GLOBAL_INT();

    // Set Type
    GP_WB_WRITE_ES_EXTERNAL_EVENT_TYPE_TO_BE_EXECUTED(pExternalEventDescriptor->type);
    // Mark Valid
    GP_WB_WRITE_ES_EXTERNAL_EVENT_VALID(true);
    //Enable wake up through external Event
    GPHAL_ES_ENABLE_EXT_WAKEUP();
    ENABLE_GP_GLOBAL_INT();
}

#endif //GP_DIVERSITY_GPHAL_EXTERN
#endif //GP_COMP_GPHAL_ES_EXT_EVENT

