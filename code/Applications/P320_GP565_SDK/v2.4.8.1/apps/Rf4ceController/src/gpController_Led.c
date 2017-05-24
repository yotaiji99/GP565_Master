/*
 * Copyright (c) 2015, GreenPeak Technologies
 *
 * gpController_Led.c
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/apps/Rf4ceController/src/gpController_Led.c#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */


/*******************************************************************************
 *                      Include Files
 ******************************************************************************/
#include "gpController_Led.h"
#include "hal.h"
#include "gpSched.h"
#include "gpLog.h"
#include "gpController_Main.h"

/*******************************************************************************
 *                      Defines
 ******************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_APP

#define GP_PERIOD_10MS            10000UL           /* Constant to scale LED On/Off period to 10 ms resolution */

/* Define LED-sequence state machine events */
#define Led_EventOff                  0x00
#define Led_EventOn                   0x01
#define Led_EventSequence             0x02
#define Led_EventEndPeriod            0x03
#define Led_EventToggle               0x04
                                  
/* Define LED-sequence state machine states */
#define Led_Off                        0x00
#define Led_On                         0x01
#define Led_SequenceOff                0x03
#define Led_SequenceOn                 0x04
                                      
/* Define LED-sequence state machine led on/off */
#define Led_StatusOff                  false
#define Led_StatusOn                   true

#define GP_LED_BLINK_PATTERN_TIME_UNIT  (10000UL)
/*******************************************************************************
 *                      Type definitions
 ******************************************************************************/

/* Led sequence stat-machine parameters */
/*******************************************************************************
 *                      Static Function Declarations
 ******************************************************************************/
void gpController_Led_SetLed(gpLed_Status_t Led_Status, gpController_Led_Color_t color);
static void gpController_Led_CheckNextAction(gpController_Led_Sequence_t *sequence, UInt8 ledAction);

/* Call-back functions from scheduler, triggers next time-based state-transition*/
static void cbLedSequencePeriodRed(void);
static void cbLedSequencePeriodGreen(void);

/* State-machine functions to realize sequences and on-off behaviour without any glitches*/
static gpLed_Status_t LedSequenceStateMachine(gpController_Led_sequenceStm_t *ledsequenceStm,
                                             UInt8 ledAction,
                                             gpController_Led_Sequence_t *sequence,
                                             void_func cbLedSequence);

/*******************************************************************************
 *                      Static data Declarations
 ******************************************************************************/

/* State-Machine data declarations */
static gpController_Led_sequenceStm_t ledsequenceStmRed;
static gpController_Led_sequenceStm_t ledsequenceStmGreen;

static UInt8     Led_onTime;
static UInt8     Led_offTime;
static UInt8     Led_numberOfBlinks;
static UInt8     Led_blinkIndex = 0;
static UInt16    Led_ledPattern;
// to properly handle nesting of calls, I need seperate counters for the red and green LED
static UInt8     redLed_OnOffCounter = 0;
static UInt8     greenLed_OnOffCounter = 0;
static void_func Led_callback = NULL;

/*******************************************************************************
 *                      Public Functions
 ******************************************************************************/

void gpController_Led_Init(void)
{
    /* Init LED's at Off-level */
    HAL_LED_CLR( RED );
    HAL_LED_CLR( GRN );
    
    /* State machines in LED-Off state (all zero) */
    MEMSET(&ledsequenceStmRed, 0, sizeof(ledsequenceStmRed));
    MEMSET(&ledsequenceStmGreen, 0, sizeof(ledsequenceStmGreen));
}

void gpController_Led_Msg(  gpController_Led_MsgId_t msgId,
                            gpController_Led_Msg_t *pMsg)
{
    switch(msgId)
    {
        case gpController_Led_MsgId_OnIndication:
        {
            gpController_Led_CheckNextAction((gpController_Led_Sequence_t *)pMsg, Led_EventOn);
            break;
        }
        case gpController_Led_MsgId_OffIndication:
        {
            gpController_Led_CheckNextAction((gpController_Led_Sequence_t *)pMsg, Led_EventOff);
            break;
        }
        case gpController_Led_MsgId_SequenceIndication:
        {
            gpController_Led_CheckNextAction((gpController_Led_Sequence_t *)pMsg, Led_EventSequence);
            break;
        }
        case gpController_Led_MsgId_ToggleIndication:
        {
            gpController_Led_CheckNextAction((gpController_Led_Sequence_t *)pMsg, Led_EventToggle);
            break;
        }
        default:
        {
            break;
        }
    }
}

/*******************************************************************************
 *                      Static Functions
 ******************************************************************************/
static void gpController_Led_CheckNextAction(gpController_Led_Sequence_t *sequence, UInt8 ledAction)
{
    gpLed_Status_t Led_Status = Led_StatusOff;
    
    if(sequence->color == gpController_Led_ColorRed)
    {
        /* If not evEndPeriod, then remove pending period ending */
        if(ledAction != Led_EventEndPeriod)
        {
            ledsequenceStmRed.sequence2Steady = false;
        }
        Led_Status = LedSequenceStateMachine(&ledsequenceStmRed, ledAction, sequence, cbLedSequencePeriodRed);
    }

    if(sequence->color == gpController_Led_ColorGreen)
    {
        /* If not evEndPeriod, then remove pending period ending */
        if(ledAction != Led_EventEndPeriod)
        {
            ledsequenceStmGreen.sequence2Steady = false;
        }

		//GP_LOG_SYSTEM_PRINTF("sequenceStm = %d , Blinks = %d , Steady = %d",0,ledsequenceStmGreen.sequenceStm,ledsequenceStmGreen.cntnumOfBlinks,ledsequenceStmGreen.sequence2Steady);
        Led_Status = LedSequenceStateMachine(&ledsequenceStmGreen, ledAction, sequence, cbLedSequencePeriodGreen);
    }

    gpController_Led_SetLed(Led_Status, sequence->color);

} /* End of gpController_Led_CheckNextAction */


static void cbLedSequencePeriodRed(void)
{
    gpController_Led_Sequence_t sequence;
    gpLed_Status_t Led_Status = Led_StatusOff;

    Led_Status = LedSequenceStateMachine(&ledsequenceStmRed, Led_EventEndPeriod, &sequence, cbLedSequencePeriodRed);
    gpController_Led_SetLed(Led_Status, gpController_Led_ColorRed);
} /* End of cbLedSequencePeriodRed*/

static void cbLedSequencePeriodGreen(void)
{
    gpController_Led_Sequence_t sequence;
    gpLed_Status_t Led_Status = Led_StatusOff;

    Led_Status = LedSequenceStateMachine(&ledsequenceStmGreen, Led_EventEndPeriod, &sequence, cbLedSequencePeriodGreen);
    gpController_Led_SetLed(Led_Status, gpController_Led_ColorGreen);
} /* End of cbLedSequencePeriodGreen*/

extern LedSequenceBlink;
static gpLed_Status_t LedSequenceStateMachine(gpController_Led_sequenceStm_t *ledsequenceStm,
                                             UInt8 ledAction,
                                             gpController_Led_Sequence_t *sequence,
                                             void_func cbLedSequence)
{
    gpController_Led_sequenceStm_t localLedsequenceStm = *ledsequenceStm;
    Bool Led_Status = Led_StatusOff;
    if(ledAction == Led_EventSequence)
    {
        localLedsequenceStm.sequence = *sequence;
        localLedsequenceStm.cntnumOfBlinks = 1;
    }

    switch (localLedsequenceStm.sequenceStm)
    {
        case Led_Off:
        {
			//GP_LOG_SYSTEM_PRINTF("LedSequenceStateMachine = Led_Off",0);
            Led_Status = Led_StatusOff;
            if ((ledAction == Led_EventOn) || (ledAction == Led_EventToggle))
            {
                if(sequence->startTime)
                {
                    gpSched_ScheduleEvent(GP_PERIOD_10MS * sequence->startTime, cbLedSequence);
                }
                else
                {
                    Led_Status = Led_StatusOn;
                    localLedsequenceStm.sequenceStm = Led_On;
                }
            }
            if (ledAction == Led_EventSequence)
            {
                if(localLedsequenceStm.sequence.startTime)
                {
                    gpSched_ScheduleEvent(GP_PERIOD_10MS * localLedsequenceStm.sequence.startTime, cbLedSequence);
                    localLedsequenceStm.sequenceStm = Led_SequenceOff;
                }
                else
                {
                    Led_Status = Led_StatusOn;
                    gpSched_ScheduleEvent(GP_PERIOD_10MS * localLedsequenceStm.sequence.onTime, cbLedSequence);
                    localLedsequenceStm.sequenceStm = Led_SequenceOn;
                }
            }
            if (ledAction == Led_EventEndPeriod)
            {
                    Led_Status = Led_StatusOn;
                    localLedsequenceStm.sequenceStm = Led_On;
            }
            break;
        } /* End of State Led_Off */

        case Led_On:
        {
			//GP_LOG_SYSTEM_PRINTF("LedSequenceStateMachine = Led_On",0);
            Led_Status = Led_StatusOn;
            if ((ledAction == Led_EventOff) || (ledAction == Led_EventToggle))
            {
                if(sequence->startTime)
                {
                    gpSched_ScheduleEvent(GP_PERIOD_10MS * sequence->startTime, cbLedSequence);
                }
                else
                {
                    Led_Status = Led_StatusOff;
                    localLedsequenceStm.sequenceStm = Led_Off;
                }
            }
            if (ledAction == Led_EventSequence)
            {
                if(localLedsequenceStm.sequence.startTime)
                {
                    gpSched_ScheduleEvent(GP_PERIOD_10MS * localLedsequenceStm.sequence.startTime, cbLedSequence);
                    localLedsequenceStm.sequenceStm = Led_SequenceOn;
                }
                else
                {
                    Led_Status = Led_StatusOff;
                    gpSched_ScheduleEvent(GP_PERIOD_10MS * localLedsequenceStm.sequence.offTime, cbLedSequence);
                    localLedsequenceStm.sequenceStm = Led_SequenceOff;
                }
            }
            if (ledAction == Led_EventEndPeriod)
            {
                Led_Status = Led_StatusOff;
                localLedsequenceStm.sequenceStm = Led_Off;
            }
            break;
        } /* End of State Led_On */

        case Led_SequenceOff:
        {
			//GP_LOG_SYSTEM_PRINTF("LedSequenceStateMachine = Led_SequenceOff",0);
            Led_Status = Led_StatusOff;
            if (ledAction == Led_EventEndPeriod)
            {
                Led_Status = Led_StatusOn;
                gpSched_ScheduleEvent(GP_PERIOD_10MS * localLedsequenceStm.sequence.onTime, cbLedSequence);
                localLedsequenceStm.sequenceStm = Led_SequenceOn;
            }
            if (ledAction == Led_EventOff)
            {
                gpSched_UnscheduleEvent(cbLedSequence);
                localLedsequenceStm.sequenceStm = Led_Off;
            }
            if (ledAction == Led_EventOn)
            {
                localLedsequenceStm.sequence2Steady = true;
            }
            if ((localLedsequenceStm.sequence2Steady) && (ledAction == Led_EventEndPeriod))
            {
                localLedsequenceStm.sequence2Steady = false;
                gpSched_UnscheduleEvent(cbLedSequence);
                localLedsequenceStm.sequenceStm = Led_On;
            }
            break;
        } /* End of State Led_SequenceOff */

        case Led_SequenceOn:
        {
			//GP_LOG_SYSTEM_PRINTF("LedSequenceStateMachine = Led_SequenceOn",0);
            Led_Status = Led_StatusOn;
            if (ledAction == Led_EventEndPeriod)
            {
                Led_Status = Led_StatusOff;
                gpSched_ScheduleEvent(GP_PERIOD_10MS * localLedsequenceStm.sequence.offTime, cbLedSequence);
                localLedsequenceStm.sequenceStm = Led_SequenceOff;
                localLedsequenceStm.cntnumOfBlinks++;
                if(localLedsequenceStm.cntnumOfBlinks > localLedsequenceStm.sequence.numOfBlinks)
                {
					LedSequenceBlink=false;
                    localLedsequenceStm.sequenceStm = Led_Off;
                    gpSched_UnscheduleEvent(cbLedSequence);
                }
            }
            if (ledAction == Led_EventOn)
            {
                gpSched_UnscheduleEvent(cbLedSequence);
                localLedsequenceStm.sequenceStm = Led_On;
            }
            if (ledAction == Led_EventOff)
            {
                localLedsequenceStm.sequence2Steady = true;
            }
            if ((localLedsequenceStm.sequence2Steady) && (ledAction == Led_EventEndPeriod))
            {
                localLedsequenceStm.sequence2Steady = false;
                gpSched_UnscheduleEvent(cbLedSequence);
                localLedsequenceStm.sequenceStm = Led_Off;
            }
            break;
        } /* End of State Led_SequenceOn */
        default:
        {
            /* Code should never come here, if so Statmachine is undifined */
            /* Trigger statetransition to Led_Off */
            Led_Status = Led_StatusOff;
            gpSched_UnscheduleEvent(cbLedSequence);
            localLedsequenceStm.sequenceStm = Led_Off;
        }
    } /* End of LedSequenceStateMachine*/
    
    *ledsequenceStm = localLedsequenceStm;
    return Led_Status;
} /* End of LedSequenceStateMachine*/

void gpController_Led_SetLed(gpLed_Status_t Led_Status, gpController_Led_Color_t color)
{
    if(color == gpController_Led_ColorRed)
    {
        if(Led_Status)
        {
            HAL_LED_SET( RED );
        }
        else
        {
            HAL_LED_CLR( RED );
        }
    }

    if(color == gpController_Led_ColorGreen)
    {
        if(Led_Status)
        {
            HAL_LED_SET( GRN );
        }
        else
        {
            HAL_LED_CLR( GRN );
        }
    }
}

Bool gpController_Led_SequenceActive(gpController_Led_Color_t color)
{
    Bool active = false;
    
    if(color == gpController_Led_ColorGreen)
    {
        if((ledsequenceStmGreen.sequenceStm == Led_SequenceOff)||(ledsequenceStmGreen.sequenceStm == Led_SequenceOn))
        {
            active = true;
        }       
    }

    if(color == gpController_Led_ColorRed)
    {
        if((ledsequenceStmRed.sequenceStm == Led_SequenceOff)||(ledsequenceStmRed.sequenceStm == Led_SequenceOn))
        {
            active = true;
        }       
    }
    return active;    
}

void Led_GenerateNextToggle(void)
{
    UInt32 scheduleOffset;
    if (Led_blinkIndex & 0x1)
    {
        // LED off
        HAL_LED_CLR(GRN);
        HAL_LED_CLR(RED);

        scheduleOffset = GP_LED_BLINK_PATTERN_TIME_UNIT * Led_offTime;
    }
    else
    {
        // LED on
        if((Led_ledPattern & (1 << (Led_blinkIndex / 2) & 0xFFFF)) != 0)
        {
            // RED LED
            HAL_LED_SET(RED);
        }
        else
        {
            // GRN LED
            HAL_LED_SET(GRN);
        }
        scheduleOffset = GP_LED_BLINK_PATTERN_TIME_UNIT * Led_onTime;
    }

    Led_blinkIndex++;

    if ((Led_blinkIndex/2) < Led_numberOfBlinks)
    {
        gpSched_ScheduleEvent(scheduleOffset, Led_GenerateNextToggle);
    }
    else
    {
        //blinking finished
        Led_blinkIndex = 0;
        Led_numberOfBlinks = 0;
        if (Led_callback != NULL)
        {
            gpSched_ScheduleEvent(GP_LED_BLINK_PATTERN_TIME_UNIT, Led_callback);
        }
    }
}

    gpController_Led_Color_t color;
    /** Delay until the sequence is started (resolution 10 ms) */
    UInt8 startTime;
    /** Time the LED is enabled per blink (resolution 10 ms) */
    UInt8 onTime;
    /** Time the LED is disabled per blink (resolution 10 ms) */
    UInt8 offTime;
    /** The number of blinks (0 is undefined / 255 is infinite) */
    UInt8 numOfBlinks;

void gpLed_GenerateBlinkSequence(
                        UInt8     color,
                        UInt8     startTime,
                        UInt16     onTime,
                        UInt16     offTime,
                        UInt8    numOfBlinks
                    )
{
    Controller_LedSequenceBlinkIR.color = color;
    Controller_LedSequenceBlinkIR.startTime = startTime;
    Controller_LedSequenceBlinkIR.onTime = onTime;
    Controller_LedSequenceBlinkIR.offTime = offTime;
    Controller_LedSequenceBlinkIR.numOfBlinks = numOfBlinks;

}


