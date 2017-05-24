/*
 * Copyright (c) 2010-2014, GreenPeak Technologies
 *
 * gpBaseComps_StackInit.c
 *
 * The file contains the function that initializes all the base components that are used.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpBaseComps/src/gpBaseComps_StackInit.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */


#define GP_COMPONENT_ID GP_COMPONENT_ID_BASECOMPS

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpBaseComps.h"

#include "hal.h"

#include "gpRace.h"

#include "gpSched.h"

#include "gpBatteryMonitor.h"

#include "gpReset.h"



#ifdef GP_COMP_COM
#include "gpCom.h"
#endif //GP_COMP_COM

#ifdef GP_DIVERSITY_LOG
#include "gpLog.h"
#endif //GP_DIVERSITY_LOG



#include "gpHal.h"



#include "gpMacCore.h"
#include "gpMacDispatcher.h"

#include "gpRandom.h"
#include "gpEncryption.h"



#include "gpPoolMem.h"

#include "gpPd.h"
#include "gpPad.h"

#include "gpRf4ce.h"
#include "gpRf4ceDispatcher.h"
#include "gpRf4ceRib.h"
#include "gpRf4ceBindValidation.h"
#include "gpRf4ceUserControl.h"
#include "gpRf4ceKickOutLru.h"
#ifdef GP_COMP_RF4CEVOICE
#include "gpRf4ceVoice.h"
#endif //GP_COMP_RF4CEVOICE
#include "gpNvm.h"

#include "gpKeyScan.h"











#include "gpRandom.h"


#include "gpVersion.h"


#include "gpAssert.h"
#include "gpRxArbiter.h"







#define MINIMAL_BATTERY_LEVEL_FOR_FGT (179) /*179/64 V = 2.8 V*/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

void BaseComps_GoToRace(void)
{
    UInt8 batteryLevel;

    gpBatteryMonitor_Measure(gpBatteryMonitor_MeasurementTypeUnloadedWithRecovery, &batteryLevel);

    if (batteryLevel >= MINIMAL_BATTERY_LEVEL_FOR_FGT)
    {
        gpReset_ResetReason_t resetReason = gpReset_GetResetReason();
        // Only start race if resetReason is from the list below
        if( resetReason == gpReset_ResetReason_UnSpecified ||
            resetReason == gpReset_ResetReason_HW_Watchdog ||
            resetReason == gpReset_ResetReason_HW_Por)
        {
            gpRace_Start(GP_RACE_STARTUP_DOWN_COUNT, NULL);
        }
    }
}


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpBaseComps_StackInit(void)
{
//--------------------------
//Initialize logging components
//--------------------------


#if defined(GP_COMP_COM) 
    gpCom_Init();
#endif

#ifdef GP_DIVERSITY_LOG
    gpLog_Init();
#endif //GP_DIVERSITY_LOG

//--------------------------
//Init low level components
//--------------------------

    gpHal_Init(false);
    gpHal_EnableInterrupts(true);



    gpSched_Init();


    gpPoolMem_Init();






    gpSched_StartTimeBase();
#ifndef GP_SCHED_FREE_CPU_TIME
    gpSched_SetGotoSleepEnable(false);
#endif //GP_SCHED_FREE_CPU_TIME

    gpBatteryMonitor_Init();


//--------------------------
// Init low level components with possible external communication
//--------------------------

    gpNvm_Init();

    gpReset_Init();

    gpRxArbiter_Init();

    gpPd_Init();
    gpPad_Init();

    //Check if code needs execution
    BaseComps_GoToRace();

    gpHal_AdvancedInit();

//--------------------------
//Init higher level components
//--------------------------
    gpVersion_Init();
    gpRandom_Init();
    gpEncryption_Init();




    gpBaseComps_MacInit();

    gpBaseComps_Rf4ceInit();

    gpKeyScan_Init();

    gpIrTx_Init();








    gpRandom_RandomizeSeed();



}

void gpBaseComps_MacInit(void)
{
    gpMacCore_Init();
    gpMacDispatcher_Init();
}


void gpBaseComps_Rf4ceInit(void)
{

    gpRf4ce_Init();
    gpRf4ceDispatcher_Init();
    gpRf4ceRib_Init();
    gpRf4ceBindValidation_Init();
    gpRf4ceKickOutLru_Init();
    gpRf4ceUserControl_Init();
#ifdef GP_COMP_RF4CEVOICE
    gpRf4ceVoice_Init();
#endif // GP_COMP_RF4CEVOICE
}
