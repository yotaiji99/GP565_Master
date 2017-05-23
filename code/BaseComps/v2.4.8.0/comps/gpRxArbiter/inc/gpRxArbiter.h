/*
 * Copyright (c) 2013-2014, GreenPeak Technologies
 *
 *   gpRxArbiter.h
 *   This file contains the definitions of the public functions and enumerations of the gpRxArbiter.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpRxArbiter/inc/gpRxArbiter.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */


#ifndef _GP_RX_ARBITER_H_
#define _GP_RX_ARBITER_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include "global.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#ifdef GP_DIVERSITY_NR_OF_STACKS
#define GP_RX_ARBITER_NUMBER_OF_STACKS  GP_DIVERSITY_NR_OF_STACKS
#else//GP_DIVERSITY_NR_OF_STACKS
#define GP_RX_ARBITER_NUMBER_OF_STACKS  1
#endif //GP_DIVERSITY_NR_OF_STACKS

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/** @name gpRxArbiter_FaMode_t*/
//@{
/** @ingroup GEN_FA
 *  @brief The frequency agility is disabled.
*/
#define gpRxArbiter_FaModeDisabled       0x00
/** @ingroup GEN_FA
 *  @brief The frequency agility is enabled.
*/
#define gpRxArbiter_FaModeEnabled        0x01
/** @ingroup GEN_FA
 *  @brief The frequency agility is running in power save mode.
*/
typedef UInt8 gpRxArbiter_FaMode_t ;
//@}

typedef UInt8 gpRxArbiter_StackId_t;
#define gpRxArbiter_StackIdRf4ce        0x0
#define gpRxArbiter_StackIdZigBeePro    0x1
#define gpRxArbiter_StackIdZigBeeIp     0x2
#define gpRxArbiter_StackIdInvalid      0xFF

typedef UInt8 gpRxArbiter_Result_t;
#define gpRxArbiter_ResultSuccess               0x0
#define gpRxArbiter_ResultInvalidArgument       0x1

typedef UInt8 gpRxArbiter_RadioState_t;
#define gpRxArbiter_RadioStateOff           0
#define gpRxArbiter_RadioStateOn            1
#define gpRxArbiter_RadioStateDutyCycle     2
#define gpRxArbiter_RadioStateInvalid       0xff
#define gpRxArbiter_RadioStateActive(state) (state==gpRxArbiter_RadioStateOn || state==gpRxArbiter_RadioStateDutyCycle)

typedef void (* gpRxArbiter_cbSetFaMode_t)(gpRxArbiter_StackId_t stackId, gpRxArbiter_FaMode_t mode) ;
typedef void (* gpRxArbiter_cbChannelUpdate_t)(gpRxArbiter_StackId_t stackId, UInt8 channel) ;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

void gpRxArbiter_Init( void );
void gpRxArbiter_DeInit( void );

gpRxArbiter_Result_t gpRxArbiter_ResetStack( gpRxArbiter_StackId_t stackId );
gpRxArbiter_Result_t gpRxArbiter_SetStackChannel( UInt8 channel , gpRxArbiter_StackId_t stackId );
UInt8                gpRxArbiter_GetStackChannel( gpRxArbiter_StackId_t stackId );
UInt8                gpRxArbiter_GetCurrentRxChannel( void );

gpRxArbiter_Result_t gpRxArbiter_SetStackRxOn( Bool enable , gpRxArbiter_StackId_t stackId );
Bool                 gpRxArbiter_GetStackRxOn( gpRxArbiter_StackId_t stackId );
gpRxArbiter_RadioState_t gpRxArbiter_GetCurrentRxOnState( void );

gpRxArbiter_Result_t gpRxArbiter_SetDutyCycleOnOffTimes(UInt32 onTimeUs , UInt32 offTimeUs , gpRxArbiter_StackId_t stackId );
gpRxArbiter_Result_t gpRxArbiter_EnableDutyCycling( Bool enable , gpRxArbiter_StackId_t stackId );
Bool                 gpRxArbiter_GetDutyCycleEnabled( gpRxArbiter_StackId_t stackId );



#if (GP_RX_ARBITER_NUMBER_OF_STACKS == 1)
#define gpRxArbiter_SetStackPriority( priority , stackId )      (gpRxArbiter_ResultSuccess)

#define gpRxArbiter_RegisterSetFaModeCallback( stackId, cb)     (gpRxArbiter_ResultSuccess)
#define gpRxArbiter_RegisterChannelUpdateCallback( stackId, cb) (gpRxArbiter_ResultSuccess)
#define gpRxArbiter_DeRegisterChannelUpdateCallback( stackId ) (gpRxArbiter_ResultSuccess)

#else

gpRxArbiter_Result_t gpRxArbiter_SetStackPriority( UInt8 priority , gpRxArbiter_StackId_t stackId );

// register for notifications from gpRxArbiter
gpRxArbiter_Result_t gpRxArbiter_RegisterSetFaModeCallback(gpRxArbiter_StackId_t stackId, gpRxArbiter_cbSetFaMode_t cb) ;
gpRxArbiter_Result_t gpRxArbiter_RegisterChannelUpdateCallback(gpRxArbiter_StackId_t stackId, gpRxArbiter_cbChannelUpdate_t cb) ;
gpRxArbiter_Result_t gpRxArbiter_DeRegisterChannelUpdateCallback(gpRxArbiter_StackId_t stackId);

#endif // #if GP_RX_ARBITER_NUMBER_OF_STACKS == 1

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GP_RX_ARBITER_H_


