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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/apps/Rf4ceController/src/gpController_BatteryMonitor.c#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */


/*******************************************************************************
 *                      Include Files
 ******************************************************************************/
#include "gpController_BatteryMonitor.h"
#include "gpBatteryMonitor.h"
#include "gpAssert.h"
#include "gpKeyscan.h"

/*******************************************************************************
 *                      Defines
 ******************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_APP

#define BATTERY_STATUS_INTERVAL_EXPONENT                (2)           /* Interval is 2^GP_STATUS_INTERVAL_EXPONENT * 1/64 V*/
#define BATTERY_EMPTY_KEYSCAN_LEVEL                     (0x80 | 0x08) /* 2.125 V*/

#define STATUS_DOUBLE_INTERVAL   (                                   (1<<(BATTERY_STATUS_INTERVAL_EXPONENT+1))) /*2^(GP_STATUS_INTERVAL_EXPONENT+1) * 1/64 V*/
#define STATUS_INTERVAL          (                                   (1<<(BATTERY_STATUS_INTERVAL_EXPONENT  ))) /*2^(GP_STATUS_INTERVAL_EXPONENT)   * 1/64 V*/
#define STATUS_HALF_INTERVAL     ((BATTERY_STATUS_INTERVAL_EXPONENT==0)?0:(1<<(BATTERY_STATUS_INTERVAL_EXPONENT-1))) /*2^(GP_STATUS_INTERVAL_EXPONENT-1) * 1/64 V*/

/*******************************************************************************
 *                      Type definitions
 ******************************************************************************/

/*******************************************************************************
 *                      Static Function Declarations
 ******************************************************************************/
static UInt8 gpController_BatteryMonitor_RoundToInterval(UInt8 unrounded);
static void gpController_BatteryMonitor_SetLoadedLevel(UInt8 batLevelLoaded);
static Bool gpController_BatteryMonitor_CheckLowVoltageCallback(void);
static void gpController_BatteryMonitor_Measure(Bool forceLoadedBatteryMeasurement);

/*******************************************************************************
 *                      Static data Declarations
 ******************************************************************************/
Bool  gpController_BatterMonitor_BatteryLevelLoadedUpdated   = false; 
UInt8 gpController_BatterMonitor_BatteryLevelUnloaded        = 0xE0; 
UInt8 gpController_BatterMonitor_BatteryLevelLoaded          = 0xE0; 
/*******************************************************************************
 *                      Public Functions
 ******************************************************************************/

void gpController_BatteryMonitor_Init(void)
{
    UInt8 newLevelLoaded, referenceLevel;   
    
    //Perform unloaded measurement
    gpBatteryMonitor_Measure( gpBatteryMonitor_MeasurementTypeUnloadedWithoutRecovery, &gpController_BatterMonitor_BatteryLevelUnloaded );
    
    //Perform loaded measurement          
    gpBatteryMonitor_Measure( gpBatteryMonitor_MeasurementTypeLoaded, &newLevelLoaded );
    
    referenceLevel = gpController_BatteryMonitor_RoundToInterval(gpController_BatterMonitor_BatteryLevelLoaded);
    
    gpController_BatteryMonitor_SetLoadedLevel(newLevelLoaded);
    
    if (newLevelLoaded <= (referenceLevel + STATUS_DOUBLE_INTERVAL))
    {
        /* Keyscan workaround on critical battery voltage */
        gpKeyScan_RegisterCheckLowVoltageCallback(gpController_BatterMonitor_BatteryLevelLoaded <= BATTERY_EMPTY_KEYSCAN_LEVEL?gpController_BatteryMonitor_CheckLowVoltageCallback:NULL);
    }
}

void gpController_BatteryMonitor_Msg(  gpController_BatteryMonitor_MsgId_t msgId,
                            gpController_BatteryMonitor_Msg_t *pMsg)
{
    switch(msgId)
    {
        case gpController_BatteryMonitor_MsgId_GetBatteryLevel:
        {
            gpController_BatteryMonitor_MeasurementResult_t* pMeasurementResult;
            GP_ASSERT_DEV_EXT(pMsg != NULL);
            if(pMsg->measurementType == gpController_BatteryMonitor_LoadedAndUnloaded)
            {
                gpController_BatteryMonitor_Measure(true);
            }
            else
            {
                gpController_BatteryMonitor_Measure(false);
            }
            pMeasurementResult = (gpController_BatteryMonitor_MeasurementResult_t*)pMsg;
            
            pMeasurementResult->Flags = 0;
            if (gpController_BatterMonitor_BatteryLevelLoaded <= BATTERY_CRITICAL_LEVEL)
            {
                pMeasurementResult->Flags |= STATUS_FLAGS_IMPENDING_DOOM_BM; 
            }            
            pMeasurementResult->BatteryLevelLoaded = gpController_BatterMonitor_BatteryLevelLoaded;
            pMeasurementResult->BatteryLevelUnloaded = gpController_BatterMonitor_BatteryLevelUnloaded;            
            break;
        }
        default:
        {
            GP_ASSERT_DEV_EXT(false);
            break;
        }
    }
}

/*******************************************************************************
 *                      Static Functions
 ******************************************************************************/
static UInt8 gpController_BatteryMonitor_RoundToInterval(UInt8 unrounded)
{
    UInt8 rounded;
    
    //Add half an INTERVAL 
    rounded = unrounded + STATUS_HALF_INTERVAL;
    //Floor, by setting the STATUS_INTERVAL_EXPONENT LSB's to zero. 
    rounded >>= BATTERY_STATUS_INTERVAL_EXPONENT;
    rounded <<= BATTERY_STATUS_INTERVAL_EXPONENT;
    
    return rounded;
}

static void gpController_BatteryMonitor_SetLoadedLevel(UInt8 batLevelLoaded)
{
    UInt8 referenceLevel = gpController_BatteryMonitor_RoundToInterval(gpController_BatterMonitor_BatteryLevelLoaded);

    if (batLevelLoaded < (referenceLevel - STATUS_INTERVAL))        /*significant drop detected*/
    {
        gpController_BatterMonitor_BatteryLevelLoaded = batLevelLoaded;
        gpController_BatterMonitor_BatteryLevelLoadedUpdated = true;
    }
}

static Bool gpController_BatteryMonitor_CheckLowVoltageCallback(void)
{
    UInt8 voltage;

    if (gpBatteryMonitor_Measure(gpBatteryMonitor_MeasurementTypeUnloadedWithoutRecovery, &voltage))
    {
        //valid measurement
        return (voltage <= BATTERY_EMPTY_KEYSCAN_LEVEL);
    }
    else
    {
        //invalid measurement
        return false;
    }
}

static void gpController_BatteryMonitor_Measure(Bool forceLoadedBatteryMeasurement)
{
    UInt8 newLevelUnloaded, newLevelLoaded;     

    /* Battery unloaded measurement */
    gpBatteryMonitor_Measure( gpBatteryMonitor_MeasurementTypeUnloadedWithoutRecovery, &newLevelUnloaded );

    if (forceLoadedBatteryMeasurement || (newLevelUnloaded < (gpController_BatterMonitor_BatteryLevelUnloaded - STATUS_INTERVAL)))  /*significant drop detected*/
    {
//        Battery_SetUnloadedLevel(newLevelUnloaded);
        gpController_BatterMonitor_BatteryLevelUnloaded = newLevelUnloaded;
        
        /* Battery loaded measurement */
        gpBatteryMonitor_Measure( gpBatteryMonitor_MeasurementTypeLoaded, &newLevelLoaded );

        gpController_BatteryMonitor_SetLoadedLevel(newLevelLoaded);
        
    }
    
    /* Keyscan workaround at critical battery voltage */
    gpKeyScan_RegisterCheckLowVoltageCallback(gpController_BatterMonitor_BatteryLevelLoaded <= BATTERY_EMPTY_KEYSCAN_LEVEL?gpController_BatteryMonitor_CheckLowVoltageCallback:NULL);
}

