/*
 * Copyright (c) 2014, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/inc/gpHal_kx_ES.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GPHAL_KX_ES_H_
#define _GPHAL_KX_ES_H_

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#if defined(GP_COMP_GPHAL_ES_ABS_EVENT) 
#ifndef GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS
#error GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS not defined
#endif
extern const UInt8 total_nr_of_events_as_an_address;
#define TOTAL_NR_OF_ABSOLUTE_EVENTS_ALLOCATED ((UInt8)&total_nr_of_events_as_an_address)
#endif //defined(GP_COMP_GPHAL_ES_ABS_EVENT) 

/** @brief Default value for the calibration of the RC oscilator */
#define GPHAL_ES_RC_SLEEP_DEFAULT_CALIB                   0x3E800000 /*FIXME  - To be checked*/

/** @brief Event will trigger scheduled Packet queue.*/
#define gpHal_EventTypeTXPacket     GP_WB_ENUM_EVENT_TYPE_MAC_TX_QUEUE0
/** @brief Event will trigger RX on.*/
#define gpHal_EventTypeRXOn         GP_WB_ENUM_EVENT_TYPE_MAC_RX_ON0
/** @brief Event will trigger RX off.*/
#define gpHal_EventTypeRXOff        GP_WB_ENUM_EVENT_TYPE_MAC_RX_OFF0
/** @brief Event will trigger a dummy event, it only generates an interrupt.*/
#define gpHal_EventTypeDummy        GP_WB_ENUM_EVENT_TYPE_DUMMY

/** @brief The GP chip will be in event mode during standby and can only be woken up by an external event.
    The event timer is stopped and can only be restarted by a full reset of the GP chip.*/
#define gpHal_SleepModeEvent            GP_WB_ENUM_STANDBY_MODE_RC_MODE

/** @brief The GP chip will run a RC timer during standby.*/
#define gpHal_SleepModeRC               GP_WB_ENUM_STANDBY_MODE_RC_MODE

/** @brief The GP chip will run a 32kHz clock during standby.*/
#define gpHal_SleepMode32kHz            GP_WB_ENUM_STANDBY_MODE_XTAL_32KHZ_MODE

/** @brief The GP chip will run a 16MHz clock during standby.*/
#define gpHal_SleepMode16MHz            GP_WB_ENUM_STANDBY_MODE_XTAL_16MHZ_MODE

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

/*pin and edge are provided by BSP
 */
#define GPHAL_ES_ENABLE_EXT_WAKEUP()                    \
     GP_BSP_KX_WRITE_WAKEUP_PIN_MODE(GP_BSP_KX_WAKEUP_EDGE)
#define GPHAL_ES_DISABLE_EXT_WAKEUP()                   \
     GP_BSP_KX_WRITE_WAKEUP_PIN_MODE(GP_WB_ENUM_WAKEUP_PIN_MODE_NO_EDGE)

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/
 
void gpHalES_ApplySimpleCalibration(void);
void gpHalES_ApplyBenchmarkCalibration(void);

#endif //_GPHAL_KX_ES_H_

