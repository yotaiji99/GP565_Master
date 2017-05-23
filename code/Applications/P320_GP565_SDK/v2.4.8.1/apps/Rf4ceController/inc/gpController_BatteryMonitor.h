/*
 * Copyright (c) 2015, GreenPeak Technologies
 *
 * gpController_BatteryMonitor.h
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/apps/Rf4ceController/inc/gpController_BatteryMonitor.h#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */


#ifndef _GPCONTROLLER_BATTERYMONITOR_H_
#define _GPCONTROLLER_BATTERYMONITOR_H_

///@file gpController_BatteryMonitor.h
///

/*******************************************************************************
 *                      Include Files
 ******************************************************************************/
 #include "global.h"
 
/*******************************************************************************
 *                    Defines
 ******************************************************************************/
#define BATTERY_HIGH_LEVEL            (0x80 | 0x2C) /* 2.6875 V*/
#define BATTERY_MEDIUM_LEVEL          (0x80 | 0x24) /* 2.5625 V*/
#define BATTERY_LOW_LEVEL             (0x80 | 0x1C) /* 2.4375 V*/
#define BATTERY_CRITICAL_LEVEL        (0x80 | 0x0C) /* 2.1875 V*/


#define BATTERY_TRANSLATE_LEVEL_TO_ZRC20(x,y)     do{      \
                                                        if(x>BATTERY_CRITICAL_LEVEL)    \
                                                        {                               \
                                                            y = 0x04;                   \
                                                        }                               \
                                                        if(x>BATTERY_LOW_LEVEL)         \
                                                        {                               \
                                                            y = 0x08;                   \
                                                        }                               \
                                                        if(x>BATTERY_MEDIUM_LEVEL)      \
                                                        {                               \
                                                            y = 0x0C;                   \
                                                        }                               \
                                                        if(x>BATTERY_HIGH_LEVEL)         \
                                                        {                               \
                                                            y = 0x0f;                   \
                                                        }                               \
                                                }while(false)
 /* Battery status flags description */
#define STATUS_FLAGS_BATTERY_REPLACEMENT_BM                       0x01
#define STATUS_FLAGS_BATTERY_CHARGING_BM                          0x02
#define STATUS_FLAGS_IMPENDING_DOOM_BM                            0x04
 
/*******************************************************************************
 *                    Type Definitions
 ******************************************************************************/
 
/** @name gpController_BatteryMonitor_MsgId_t */
//@{
/** @brief Ask the Battery Module module to return a measurement result. The user will have to specify the measurement type via the gpController_BatteryMonitor_Msg_t parameter in the gpController_BatteryMonitor_Msg function.*/
#define gpController_BatteryMonitor_MsgId_GetBatteryLevel                   0x01


/** @typedef gpController_BatteryMonitor_MsgId_t
 *  @brief The gpController_BatteryMonitor_MsgId_t type defines the message IDs to be exchanged between MAIN module and BatteryMonitor module.
*/
typedef UInt8 gpController_BatteryMonitor_MsgId_t;
//@}

/** @name gpController_BatteryMonitor_MeasurementType_t */
//@{
/** @brief Update Unloaded voltage, i.e. with radio off. */
#define gpController_BatteryMonitor_Unloaded                                0x01
/** @brief Update Unloaded and Loaded voltage, i.e. during transmission. */
#define gpController_BatteryMonitor_LoadedAndUnloaded                       0x02

/** @typedef gpController_BatteryMonitor_MeasurementType_t
 *  @brief The gpController_BatteryMonitor_MeasurementType_t type defines the measurement that needs to be performed by the battery monitor module.
*/
typedef UInt8 gpController_BatteryMonitor_MeasurementType_t;
//@}

/** @typedef gpController_BatteryMonitor_MeasurementResult_t
 *  The gpController_BatteryMonitor_MeasurementResult_t is used to return the values of a battery monitor measurement request.
*/
typedef struct gpController_BatteryMonitor_MeasurementResult
{    
    /** The flags that go with the battery measurement. This is a bitmap. bit 0 indicates the battery was recently replaced. Bit 1 indicates the battery is charging. Bit 2 indicates the battery level is critical. All other bits are reserved. */
    UInt8 Flags;
    /** The loaded battery level. Only valid if the issued measurementType was of type gpController_BatteryMonitor_LoadedAndUnloaded. */
    UInt8 BatteryLevelLoaded;
    /** The unloaded battery level. Only valid if the issued measurementType was of type gpController_BatteryMonitor_LoadedAndUnloaded. */
    UInt8 BatteryLevelUnloaded;
}gpController_BatteryMonitor_MeasurementResult_t;

/** @typedef gpController_BatteryMonitor_Msg_t
 *  The gpController_BatteryMonitor_Msg_t type specifies battery monitor specific messages to
 *  exchange with the MAIN module.
*/
typedef union {
    gpController_BatteryMonitor_MeasurementType_t measurementType;
    gpController_BatteryMonitor_MeasurementResult_t measurementResult;
} gpController_BatteryMonitor_Msg_t;

/******************************************************************************
 *                    Public Function Definitions
 ******************************************************************************/
 /** @ingroup BM
 *
 *  This primitive can be used to initialize the Battery Monitor module.
 */
GP_API void gpController_BatteryMonitor_Init(void);

/** @ingroup BM
 *
 *  This primitive can be used to do a request to the BatteryMonitor module.
 *
 *  The BatteryMonitor module will not trigger any callbacks (input only).
 *
 *  @param msgId Identifier for the message.
 *  @param pMsg  Data passed in the message (NULL for indication without data).
 */
GP_API void gpController_BatteryMonitor_Msg(  gpController_BatteryMonitor_MsgId_t msgId,
                                   gpController_BatteryMonitor_Msg_t *pMsg);

#endif /* _GPCONTROLLER_BATTERYMONITOR_H_ */
