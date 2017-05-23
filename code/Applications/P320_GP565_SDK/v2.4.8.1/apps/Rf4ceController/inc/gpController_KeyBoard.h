/*
 * Copyright (c) 2015, GreenPeak Technologies
 *
 * gpController_KeyBoard.h
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/apps/Rf4ceController/inc/gpController_KeyBoard.h#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */


#ifndef _GPCONTROLLER_KEYBOARD_H_
#define _GPCONTROLLER_KEYBOARD_H_

///@file gpController_KeyBoard.h
///

/*******************************************************************************
 *                      Include Files
 ******************************************************************************/
#include "global.h"
#include "gpController.h"

/*******************************************************************************
 *                    Type Definitions
 ******************************************************************************/
/** @name gpController_KeyBoard_MsgId_t */
//@{
/** @brief Indicate to the KeyBoard module the keyboard needs enabling. */
#define gpController_KeyBoard_MsgId_KeyScanEnable                   0x01
/** @brief Indicate to the KeyBoard module the keyboard needs disabling. */
#define gpController_KeyBoard_MsgId_KeyScanDisable                  0x02
/** @brief Indicate to the controller a key is pressed. */
#define gpController_KeyBoard_MsgId_KeysPressedIndication           0x03
/** @brief Indicate to the controller a key is released. */
#define gpController_KeyBoard_MsgId_KeysReleasedIndication          0x04
/** @brief Indicate to the controller the setup key is pressed. */
#define gpController_KeyBoard_MsgId_SetupKeysPressedIndication      0x05
/** @brief Indicate to the controller the setup key is released. */
#define gpController_KeyBoard_MsgId_SetupKeysReleasedIndication     0x06
/** @brief Indicate to the controller the setup mode is triggered. */
#define gpController_KeyBoard_MsgId_SetupEnteredIndication          0x07



/** @typedef gpController_KeyBoard_MsgId_t
 *  @brief The Keyboard module message IDs
*/
typedef UInt8 gpController_KeyBoard_MsgId_t;
//@}


/** @typedef gpController_KeyBoard_Keys_t
 *  @brief The Keyboard key indices.
*/
typedef struct gpController_KeyBoard_Keys
{
    /** number of keys pressed */
    UInt8 count;
    /** key indeces of the pressed keys */
    UInt8 indices[GP_RF4CECMD_DIVERSITY_CONCURRENT_ACTIONS];
} gpController_KeyBoard_Keys_t;

/** @typedef gpController_KeyBoard_Msg_t
 *  The gpController_KeyBoard_Msg_t type specifies LED specific messages to
 *  exchange with the MAIN module.
*/
typedef union {
    gpController_KeyBoard_Keys_t keys;
} gpController_KeyBoard_Msg_t;

/** @name  Keyboard_Attr_Type_t */
//@{
#define ATTR_KB_SIZE                0
#define ATTR_KB_MAX_KEY_INDICATION  1
#define ATTR_KB_STUCK_KEY_TIMEOUT   2

/** @typedef Keyboard_Attr_Type_t
 *  @brief The Keyboard_Attr_Type_t type defines keyboard attributes
*/
typedef UInt8 gpKeyboardBasic_Attr_Type_t;
//@}

typedef struct{
/** @name Keyboard_Attr_t */

/** @brief The size of the keyboard matrix. This should be adequately set in the BSP file */
    UInt8 size;
/** @brief The maximum key indication requested. The maximum key indication should be atleast 1 and less than the keyboard size. */
    UInt8 maxKeyIndication;
/** @brief The stuck key timeout is used to handle stuck key. If the keyboard is not released (all keys released) after this time then the keyboard will indicate a pseudo key release to the application keyboard handler. This can allow the device to go to sleep hence avoiding battery drainout.  */
    UInt8 stuckKeyTimoutSecs;
} gpKeyboardBasic_Attr_t;


typedef struct gpKeyboardBasic_KeyInfo{
/** @name gpKeyboardBasic_KeyInfo_t */
/** @brief The valid key index in the key mapping */
    UInt8   keyIndex;
} gpKeyboardBasic_KeyInfo_t;

/** @typedef gpKeyboardBasic_pKeyInfo_t
 *  @brief The gpKeyboardBasic_pKeyInfo_t type defines a pointer to a gpKeyboardBasic_KeyInfo structure
*/
typedef gpKeyboardBasic_KeyInfo_t* gpKeyboardBasic_pKeyInfo_t;


/******************************************************************************
 *                    Public Function Definitions
 ******************************************************************************/
 /** @ingroup KEYBOARD
 *
 *  This primitive can be used to initialize the KeyBoard module.
 */
GP_API void gpController_KeyBoard_Init(void);

/** @ingroup KEYBOARD
 *
 *  This primitive can be used to do a request to the KeyBoard module.
 *
 *  The LED module will trigger an indication callbacks on a key press or release.
 *
 *  @param msgId Identifier for the message.
 *  @param pMsg  Data passed in the message (NULL for indication without data).
 */
GP_API void gpController_KeyBoard_Msg(  gpController_KeyBoard_MsgId_t msgId,
                                        gpController_KeyBoard_Msg_t *pMsg);


/** @ingroup KEYBOARD
 *
 *  This primitive indicates (to the next higher layer) that an event occured.
 *
 *  @param msgId Identifier for the message.
 *  @param pMsg  Data passed in the message (NULL for indication without data).
 */
GP_API void gpController_KeyBoard_cbMsg(    gpController_KeyBoard_MsgId_t msgId,
                                            gpController_KeyBoard_Msg_t *pMsg);

#endif /* _GPCONTROLLER_KEYBOARD_H_ */
