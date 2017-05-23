/*
 * Copyright (c) 2015, GreenPeak Technologies
 *
 * gpController_Led.h
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/apps/Rf4ceController/inc/gpController_Led.h#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */


#ifndef _GPCONTROLLER_LED_H_
#define _GPCONTROLLER_LED_H_

///@file gpController_Led.h
///

/*******************************************************************************
 *                      Include Files
 ******************************************************************************/
 #include "global.h"
/*******************************************************************************
 *                    Type Definitions
 ******************************************************************************/
/** @name gpController_LED_MsgId_t */
//@{
/** @brief Indicate to the LED module a LED needs to be switched on. */
#define gpController_Led_MsgId_OnIndication                     0x01
/** @brief Indicate to the LED module a LED needs to be switched off or a
    sequence needs to be stopped. */
#define gpController_Led_MsgId_OffIndication                    0x02
/** @brief Indicate to the LED module a LED sequence needs to be started. */
#define gpController_Led_MsgId_SequenceIndication               0x03
/** @brief Indicate to the LED module a LED needs to toggle. Will not be done if a sequence is pending. */
#define gpController_Led_MsgId_ToggleIndication                 0x04

/** @typedef gpController_Led_MsgId_t
 *  @brief The gpController_Led_MsgId_t type defines the LED message IDs.
*/
typedef UInt8 gpController_Led_MsgId_t;
//@}

/** @name gpController_Led_Color_t */
//@{
/** @brief Red LED. */
#define gpController_Led_ColorRed                              0x01
/** @brief Green LED. */
#define gpController_Led_ColorGreen                            0x02

/** @typedef gpController_Led_Color_t
 *  @brief The gpController_Led_Color_t type defines the LED color IDs.
*/
typedef UInt8 gpController_Led_Color_t;
//@}

/** @typedef gpController_Led_Sequence_t
 *  The gpController_Led_Sequence_t type defines paramaters that need to be
 *  passed to the LED module when a LED change is required.
*/
typedef struct gpController_Led_Sequence{
    /** The LED color */
    gpController_Led_Color_t color;
    /** Delay until the sequence is started (resolution 10 ms) */
    UInt8 startTime;
    /** Time the LED is enabled per blink (resolution 10 ms) */
    UInt8 onTime;
    /** Time the LED is disabled per blink (resolution 10 ms) */
    UInt8 offTime;
    /** The number of blinks (0 is undefined / 255 is infinite) */
    UInt8 numOfBlinks;
} gpController_Led_Sequence_t;

/** @typedef gpController_Led_Msg_t
 *  The gpController_Led_Msg_t type specifies LED specific messages to
 *  exchange with the MAIN module.
*/
typedef union {
    gpController_Led_Sequence_t sequence;
} gpController_Led_Msg_t;

/******************************************************************************
 *                    Public Function Definitions
 ******************************************************************************/
 /** @ingroup LED
 *
 *  This primitive can be used to initialize the LED module.
 */
GP_API void gpController_Led_Init(void);

/** @ingroup LED
 *
 *  This primitive can be used to do a request to the LED module.
 *
 *  The LED module will not trigger any callbacks (input only).
 *
 *  @param msgId Identifier for the message.
 *  @param pMsg  Data passed in the message (NULL for indication without data).
 */
GP_API void gpController_Led_Msg(  gpController_Led_MsgId_t msgId,
                                   gpController_Led_Msg_t *pMsg);
                                   
/** @ingroup LED
 *
 *  This primitive can be used to a request LED squence running.
 *
 *  The module will return true, if a sequence is running.
 *
 *  @param color collor of requested LED
 */
Bool gpController_Led_SequenceActive(gpController_Led_Color_t color);

#endif /* _GPCONTROLLER_LED_H_ */
