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


#define gpController_KeyBoard_CommandCodeSelect                        0x00
#define gpController_KeyBoard_CommandCodeUp                            0x01
#define gpController_KeyBoard_CommandCodeDown                          0x02
#define gpController_KeyBoard_CommandCodeLeft                          0x03
#define gpController_KeyBoard_CommandCodeRight                         0x04
#define gpController_KeyBoard_CommandCodeRightUp                       0x05
#define gpController_KeyBoard_CommandCodeRightDown                     0x06
#define gpController_KeyBoard_CommandCodeLeftUp                        0x07
#define gpController_KeyBoard_CommandCodeLeftDown                      0x08
#define gpController_KeyBoard_CommandCodeRootMenu                      0x09
#define gpController_KeyBoard_CommandCodeSetupMenu                     0x0A
#define gpController_KeyBoard_CommandCodeContentsMenu                  0x0B
#define gpController_KeyBoard_CommandCodeFavoriteMenu                  0x0C
#define gpController_KeyBoard_CommandCodeExit                          0x0D
#define gpController_KeyBoard_CommandCodeNumber0                       0x20
#define gpController_KeyBoard_CommandCodeNumber1                       0x21
#define gpController_KeyBoard_CommandCodeNumber2                       0x22
#define gpController_KeyBoard_CommandCodeNumber3                       0x23
#define gpController_KeyBoard_CommandCodeNumber4                       0x24
#define gpController_KeyBoard_CommandCodeNumber5                       0x25
#define gpController_KeyBoard_CommandCodeNumber6                       0x26
#define gpController_KeyBoard_CommandCodeNumber7                       0x27
#define gpController_KeyBoard_CommandCodeNumber8                       0x28
#define gpController_KeyBoard_CommandCodeNumber9                       0x29
#define gpController_KeyBoard_CommandCodeDot                           0x2A
#define gpController_KeyBoard_CommandCodeEnter                         0x2B
#define gpController_KeyBoard_CommandCodeClear                         0x2C
#define gpController_KeyBoard_CommandCodeNextFavorite                  0x2F
#define gpController_KeyBoard_CommandCodeChannelUp                     0x30
#define gpController_KeyBoard_CommandCodeChannelDown                   0x31
#define gpController_KeyBoard_CommandCodePreviousChannel               0x32
#define gpController_KeyBoard_CommandCodeSoundSelect                   0x33
#define gpController_KeyBoard_CommandCodeInputSelect                   0x34
#define gpController_KeyBoard_CommandCodeDisplayInformation            0x35
#define gpController_KeyBoard_CommandCodeHelp                          0x36
#define gpController_KeyBoard_CommandCodePageUp                        0x37
#define gpController_KeyBoard_CommandCodePageDown                      0x38
#define gpController_KeyBoard_CommandCodePower                         0x40
#define gpController_KeyBoard_CommandCodeVolumeUp                      0x41
#define gpController_KeyBoard_CommandCodeVolumeDown                    0x42
#define gpController_KeyBoard_CommandCodeMute                          0x43
#define gpController_KeyBoard_CommandCodePlay                          0x44
#define gpController_KeyBoard_CommandCodeStop                          0x45
#define gpController_KeyBoard_CommandCodePause                         0x46
#define gpController_KeyBoard_CommandCodeRecord                        0x47
#define gpController_KeyBoard_CommandCodeRewind                        0x48
#define gpController_KeyBoard_CommandCodeFastForward                   0x49
#define gpController_KeyBoard_CommandCodeEject                         0x4A
#define gpController_KeyBoard_CommandCodeForward                       0x4B
#define gpController_KeyBoard_CommandCodeBackward                      0x4C
#define gpController_KeyBoard_CommandCodeStopRecord                    0x4D
#define gpController_KeyBoard_CommandCodePauseRecord                   0x4E
#define gpController_KeyBoard_CommandCodeAngle                         0x50
#define gpController_KeyBoard_CommandCodeSubPicture                    0x51
#define gpController_KeyBoard_CommandCodeVideoOnDemand                 0x52
#define gpController_KeyBoard_CommandCodeElectronicProgramGuide        0x53
#define gpController_KeyBoard_CommandCodeTimerProgramming              0x54
#define gpController_KeyBoard_CommandCodeInitialConfiguration          0x55
#define gpController_KeyBoard_CommandCodePlayFunction                  0x60    /*Additional operands*/
#define gpController_KeyBoard_CommandCodePausePlayfunction             0x61
#define gpController_KeyBoard_CommandCodeRecordFunction                0x62
#define gpController_KeyBoard_CommandCodePauseRecordFunction           0x63
#define gpController_KeyBoard_CommandCodeStopFunction                  0x64
#define gpController_KeyBoard_CommandCodeMuteFunction                  0x65
#define gpController_KeyBoard_CommandCodeRestoreVolumeFunction         0x66
#define gpController_KeyBoard_CommandCodeTuneFunction                  0x67    /*Additional operands*/
#define gpController_KeyBoard_CommandCodeSelectMediaFunction           0x68    /*Additional operands*/
#define gpController_KeyBoard_CommandCodeSelectAVInputFunction         0x69    /*Additional operands*/
#define gpController_KeyBoard_CommandCodeSelectAudioInputfunction      0x6A    /*Additional operands*/
#define gpController_KeyBoard_CommandCodePowerToggleFunction           0x6B
#define gpController_KeyBoard_CommandCodePowerOffFunction              0x6C
#define gpController_KeyBoard_CommandCodePowerOnFunction               0x6D
#define gpController_KeyBoard_CommandCodeF1Blue                        0x71
#define gpController_KeyBoard_CommandCodeF2Red                         0x72
#define gpController_KeyBoard_CommandCodeF3Green                       0x73
#define gpController_KeyBoard_CommandCodeF4Yellow                      0x74
#define gpController_KeyBoard_CommandCodeF5                            0x75
#define gpController_KeyBoard_CommandCodeData                          0x76

#define gpController_KeyBoard_CommandCodeNone                          0xFE

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

GP_API void Keyboard_SetupKeyPressed( void );
#endif /* _GPCONTROLLER_KEYBOARD_H_ */
