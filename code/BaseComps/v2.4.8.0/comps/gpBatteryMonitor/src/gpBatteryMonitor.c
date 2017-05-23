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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpBatteryMonitor/src/gpBatteryMonitor.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
//#define GP_LOCAL_LOG

#include "hal.h"
#include "gpHal.h"
#include "gpBatteryMonitor.h"


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_APP

#define BATTERYMONITOR_LOAD_DURATION       (2000)  //2000us (length of applying rf-rx load before start of loaded adc measurement)
#define BATTERYMONITOR_RECOVER_DURATION    (2000)  //2000us (length of idling              before start of unloaded adc measurement)
#define BATTERYMONITOR_DUMMY_VALUE         (0xC0)  //3V

/*Avoid receiving any packets - disable pre-amble detection*/
#define BATTERYMONITOR_ENABLE_LOAD()  do{ \
    GP_WB_WRITE_PLME_DISABLE_PREAMBLE_DET_ON_CH_IDX_0(1); \
    gpHal_SetRxOnWhenIdle(0, true, 11); \
} while(false)
#define BATTERYMONITOR_DISABLE_LOAD() do{ \
    gpHal_SetRxOnWhenIdle(0, false, 0xFF); \
    GP_WB_WRITE_PLME_DISABLE_PREAMBLE_DET_ON_CH_IDX_0(0); \
} while(false)
#define BATTERYMONITOR_CHECK_LOAD()   gpHal_GetRxOnWhenIdle()

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static Bool gpBatteryMonitor_Busy = false;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpBatteryMonitor_Init(void)
{
    //Initialize needed blocks
    hal_InitADC();
}



Bool gpBatteryMonitor_Measure(gpBatteryMonitor_MeasurementType_t type, UInt8* pVoltage)
{
    if (gpBatteryMonitor_Busy)
        return false;

    gpBatteryMonitor_Busy = true;

    if(!gpHal_ChipEmulated)
    {
        UInt8 result;

        Bool previousOnStatus;

        //Enable load condition if desired
        if(type == gpBatteryMonitor_MeasurementTypeLoaded)
        {
            previousOnStatus = BATTERYMONITOR_CHECK_LOAD();
            if(!previousOnStatus)
            {
                BATTERYMONITOR_ENABLE_LOAD();
            }
            HAL_WAIT_US(BATTERYMONITOR_LOAD_DURATION);
        }
        else if (type == gpBatteryMonitor_MeasurementTypeUnloadedWithRecovery)
        {
            HAL_WAIT_US(BATTERYMONITOR_RECOVER_DURATION);
        }

        //Get battery voltage
        result = hal_MeasureADC_8(GPHAL_ENUM_ADCIF_CHANNEL_VBATT, GPHAL_ENUM_ADCIF_VOLTAGE_RANGE_0V_2V);

        //Disable load condition
        if(type == gpBatteryMonitor_MeasurementTypeLoaded)
        {
            if(!previousOnStatus)
            {
                BATTERYMONITOR_DISABLE_LOAD();
            }
        }

        *pVoltage = result; // 2 bit = V , 6 bit = 1/64 V, since voltage range is 2V and VBATT is VDDD/2
    }
    else
    {
        //Stub FPGA version
        *pVoltage =  BATTERYMONITOR_DUMMY_VALUE;
    }

    gpBatteryMonitor_Busy = false;
    return true;
}


