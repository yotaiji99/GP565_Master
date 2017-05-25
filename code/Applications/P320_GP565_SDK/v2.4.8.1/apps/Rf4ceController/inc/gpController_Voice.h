/*
 * Copyright (c) 2015, GreenPeak Technologies
 *
 * gpController_Voice.h
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
 *     0!                         $Header: //depot/main/Embedded/Applications/P857_RC_RemoteSolution_Rogers_Eclipse_GP565/vlatest/apps/Rf4ceController/inc/gpController_Voice.h#1 $
 *    M'   GreenPeak              $Change: 79988 $
 *   0'         Technologies      $DateTime: 2016/04/30 14:07:02 $
 *  F
 */


#ifndef _GPCONTROLLER_VOICE_H_
#define _GPCONTROLLER_VOICE_H_

///@file gpController_Voice.h
///

/*******************************************************************************
 *                      Include Files
 ******************************************************************************/
 #include "global.h"

/*******************************************************************************
 *                    Defines
 ******************************************************************************/


/*******************************************************************************
 *                    Type Definitions
 ******************************************************************************/

/** @name gpController_Voice_MsgId_t */
//@{
/** @brief Start an audio session */
#define gpController_Voice_MsgId_Start                   0x01
/** @brief Stop an audio session */
#define gpController_Voice_MsgId_Stop                    0x02
/** @brief Indicaten to application Audio is out of resourses */
#define gpController_Voice_MsgId_cbOutOfResources        0x03



/** @typedef gpController_Voice_MsgId_t
 *  @brief The Audio module message IDs
*/
typedef UInt8 gpController_Voice_MsgId_t;
//@}

/** @typedef gpController_Voice_Msg_t
 *  The gpController_Voice_Msg_t type specifies audio specific messages to
 *  exchange with the MAIN module.
*/
typedef union {
    UInt8 bindingId;
} gpController_Voice_Msg_t;

/******************************************************************************
 *                    Public Function Definitions
 ******************************************************************************/
 /** @ingroup AUD
 *
 *  This primitive can be used to initialize the Audio module.
 */
GP_API void gpController_Voice_Init(void);

/** @ingroup AUD
 *
 *  This primitive can be used to do a request to the Audio module.
 *
 *  The Audio module will not trigger any callbacks (input only).
 *
 *  @param msgId Identifier for the message.
 *  @param pMsg  Data passed in the message (NULL for indication without data).
 */
GP_API void gpController_Voice_Msg( gpController_Voice_MsgId_t msgId,
                                    gpController_Voice_Msg_t *pMsg);

/** @ingroup AUD
 *
 *  This primitive indicates (to the next higher layer) that an event occured.
 *
 *  @param msgId Identifier for the message.
 *  @param pMsg  Data passed in the message (NULL for indication without data).
 */
GP_API void gpController_Voice_cbMsg(    gpController_Voice_MsgId_t msgId,
                                            gpController_Voice_Msg_t *pMsg);

#endif /* _GPCONTROLLER_VOICE_H_ */
