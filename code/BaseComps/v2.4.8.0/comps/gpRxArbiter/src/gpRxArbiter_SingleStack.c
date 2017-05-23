/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 * gpRxArbiter.c
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpRxArbiter/src/gpRxArbiter_SingleStack.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_RXARBITER

//#define GP_LOCAL_LOG

#include "hal.h"
#include "gpLog.h"
#include "gpRxArbiter.h"
#include "gpHal.h"
#include "gpHal_ES.h"


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_RX_ARBITER_INVALID_CHANNEL   0xff


/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct{
    UInt8 rxOnCounter;
}gpRxArbiter_StackDesc_t;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

 /*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

gpRxArbiter_StackDesc_t gpRxArbiter_StackDesc;
UInt8 gpRxArbiter_CurrentRxChannel;
gpRxArbiter_RadioState_t gpRxArbiter_CurrentRadioState;

#define RX_ARBITER_DUTY_CYCLE_ENABLED(stackId)  false

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/


static void RxArbiter_SetRxOn( Bool rxOn )
{
    gpRxArbiter_CurrentRadioState = rxOn ? gpRxArbiter_RadioStateOn : gpRxArbiter_RadioStateOff;

    GP_LOG_PRINTF("Radio On:%u ch:%u",0, rxOn, gpRxArbiter_CurrentRxChannel);
    gpHal_SetRxOnWhenIdle( gpHal_SourceIdentifier_0, rxOn, gpRxArbiter_CurrentRxChannel);
}


static void RxArbiter_UpdateRadio(gpRxArbiter_StackId_t requestedStackId)
{
    // we have found the highest priority stack, now set channel and RxOn,
    // set rx on/rxof/duty cycle
    
    GP_LOG_PRINTF("RxArbiter_UpdateRadio gpRxArbiter_CurrentRadioState %d ",2, gpRxArbiter_CurrentRadioState);
    switch( gpRxArbiter_CurrentRadioState )
    {
        case gpRxArbiter_RadioStateOff:
        {
            if( gpRxArbiter_StackDesc.rxOnCounter )
            {
                RxArbiter_SetRxOn(true);
            }
            break;
        }
        case gpRxArbiter_RadioStateOn:
        {
            if( gpRxArbiter_StackDesc.rxOnCounter == 0 )
            {
                RxArbiter_SetRxOn(false);
            }
            else
            {
                //Channel update through RxOn
                RxArbiter_SetRxOn(false);
                RxArbiter_SetRxOn(true);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpRxArbiter_Init( void )
{
    gpRxArbiter_CurrentRxChannel = GP_RX_ARBITER_INVALID_CHANNEL;
    gpRxArbiter_CurrentRadioState = gpRxArbiter_RadioStateOff;

}

void gpRxArbiter_DeInit( void )
{

    gpRxArbiter_ResetStack(gpHal_SourceIdentifier_0);
    gpRxArbiter_CurrentRxChannel = GP_RX_ARBITER_INVALID_CHANNEL;
}

gpRxArbiter_Result_t gpRxArbiter_ResetStack( gpRxArbiter_StackId_t stackId )
{
    GP_LOG_PRINTF("Reset Stack StId %i",0,stackId);

    if( gpRxArbiter_CurrentRadioState == gpRxArbiter_RadioStateOn )
    {
        gpHal_SetRxOnWhenIdle(gpHal_SourceIdentifier_0, false, gpRxArbiter_CurrentRxChannel);
    }
    gpRxArbiter_CurrentRadioState = gpRxArbiter_RadioStateOff;
    gpRxArbiter_StackDesc.rxOnCounter = 0;
    
    if( gpRxArbiter_CurrentRxChannel == GP_RX_ARBITER_INVALID_CHANNEL )
    {
        gpRxArbiter_CurrentRxChannel = gpHal_GetRxChannel(gpHal_SourceIdentifier_0);
    }

    return gpRxArbiter_ResultSuccess;
}

gpRxArbiter_Result_t gpRxArbiter_SetStackChannel( UInt8 channel , gpRxArbiter_StackId_t stackId )
{
    GP_LOG_PRINTF("Set Ch %i, StId %i",0,channel,stackId);
    gpRxArbiter_CurrentRxChannel = channel;

    RxArbiter_UpdateRadio(stackId);

    return gpRxArbiter_ResultSuccess;
}

UInt8 gpRxArbiter_GetStackChannel( gpRxArbiter_StackId_t stackId )
{
    return gpRxArbiter_CurrentRxChannel;
}

gpRxArbiter_RadioState_t gpRxArbiter_GetCurrentRxOnState( void )
{
    return gpRxArbiter_CurrentRadioState;
}

UInt8 gpRxArbiter_GetCurrentRxChannel( void )
{
    return gpRxArbiter_CurrentRxChannel;
}

gpRxArbiter_Result_t gpRxArbiter_SetStackRxOn( Bool enable , gpRxArbiter_StackId_t stackId )
{
    GP_LOG_PRINTF("gpRxArbiter_SetStackRxOn enable %x RxOn counter %x",0,enable, gpRxArbiter_StackDesc.rxOnCounter);

    if( !RX_ARBITER_DUTY_CYCLE_ENABLED(stackId) )
    {
        if(enable)
        {
            gpRxArbiter_StackDesc.rxOnCounter++;
            if (gpRxArbiter_StackDesc.rxOnCounter == 1) // going from 0 -> 1
            {
                RxArbiter_UpdateRadio(stackId);
            }
        }
        else
        {
            if( gpRxArbiter_StackDesc.rxOnCounter )
            {
                gpRxArbiter_StackDesc.rxOnCounter--;
                if (gpRxArbiter_StackDesc.rxOnCounter == 0)
                {
                    RxArbiter_UpdateRadio(stackId);
                }
            }
            else
            {
                GP_LOG_PRINTF("RxOn invalid param negative 1",0);
                return gpRxArbiter_ResultInvalidArgument;
            }
        }
        return gpRxArbiter_ResultSuccess;
    }
    else
    {
        GP_LOG_PRINTF("RxOn invalid param Duty cycle enabled 2",0);
        return gpRxArbiter_ResultInvalidArgument;
    }
}

Bool gpRxArbiter_GetStackRxOn( gpRxArbiter_StackId_t stackId )
{
    return gpRxArbiter_StackDesc.rxOnCounter;
}


Bool gpRxArbiter_GetDutyCycleEnabled( gpRxArbiter_StackId_t stackId )
{
    return RX_ARBITER_DUTY_CYCLE_ENABLED(stackId);
}
