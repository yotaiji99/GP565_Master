/*
 * Copyright (c) 2011-2012, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpBatteryMonitor/inc/gpBatteryMonitor.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */


#ifndef _GP_BATTERY_MONITOR_H_
#define _GP_BATTERY_MONITOR_H_

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

#define gpBatteryMonitor_MeasurementTypeUnloadedWithoutRecovery   0
#define gpBatteryMonitor_MeasurementTypeUnloadedWithRecovery      1
#define gpBatteryMonitor_MeasurementTypeLoaded                    2
typedef UInt8 gpBatteryMonitor_MeasurementType_t;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

GP_API void gpBatteryMonitor_Init(void);
GP_API Bool gpBatteryMonitor_Measure(gpBatteryMonitor_MeasurementType_t type, UInt8* pVoltage);


#ifdef __cplusplus
}
#endif


#endif //_GP_BATTERY_MONITOR_H_
