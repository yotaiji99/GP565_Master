
/*
 * Copyright (c) 2010-2013, GreenPeak Technologies
 *
 * gpKeyboard.h
 *
 * This file defines the keyboard translation component API
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
 *     0!                         $Header: //depot/main/Embedded/BaseComps/vlatest/sw/comps/gpKeyboard/inc/gpKeyboard.h#40 $
 *    M'   GreenPeak              $Change: 42733 $
 *   0'         Technologies      $DateTime: 2013/10/04 13:28:29 $
 *  F
 */


#ifndef _GPKEYBOARD_H_
#define _GPKEYBOARD_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <global.h>
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

//Bitmasks for keyInfo
#define GPKEYBOARD_REPEATABLE                0x02
#define GPKEYBOARD_VENDOR                    0x04
#define GPKEYBOARD_IR_KEY                    0x08

#define GPKEYBOARD_IS_REPEATABLE(keyInfo)   (!!((keyInfo) & GPKEYBOARD_REPEATABLE))
#define GPKEYBOARD_IS_VENDOR_CMD(keyInfo)   (!!((keyInfo) & GPKEYBOARD_VENDOR))
#define GPKEYBOARD_IS_IR_KEY(keyInfo)       (!!((keyInfo) & GPKEYBOARD_IR_KEY))


//Spare commands
#define CMD_SPARE                       0xFF
#define CMD_SPARE_0                     0xFE
#define CMD_SPARE_1                     0xFD
#define CMD_SPARE_2                     0xFC
#define CMD_SPARE_3                     0xFB
#define CMD_SPARE_4                     0xFA
#define CMD_SPARE_5                     0xF9
#define CMD_SPARE_6                     0xF8
#define CMD_SPARE_7                     0xF7
#define CMD_SPARE_8                     0xF6
#define CMD_SPARE_9                     0xF5
#define CMD_SPARE_10                    0xF4
#define CMD_SPARE_11                    0xF3
#define CMD_SPARE_12                    0xF2
#define CMD_SPARE_13                    0xF1
#define CMD_SPARE_14                    0xF0
#define CMD_SPARE_15                    0xEF
#define CMD_SPARE_16                    0xEE

#define CMD_START                       0xCF
#define CMD_TELETEXT                    0xCD
#define CMD_GUIDE                       0xCC

//Number keys
#define CMD_10                          0xDA
#define CMD_11                          0xDB
#define CMD_12                          0xDC

//#define GP_KEYBOARD_DIVERSITY_EMULATION

#define GP_KEYBOARD_MAX_KEYS_INDICATED   8
#ifndef GP_KEYBOARD_MAX_CMD_PARAM_LENGTH
#define GP_KEYBOARD_MAX_CMD_PARAM_LENGTH 5
#endif //GP_KEYBOARD_MAX_CMD_PARAM_LENGTH

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct gpKeyboard_KeyInfo {

//FIXME - update documentation

/** @brief Bitfield containing extra key information: byte = {Res|Res|Res|Res|IR_KEY|VENDOR|REPEATABLE} */
    UInt8   keyInfo;
/** @brief Key code */
    UInt8   keyCode;
} gpKeyboard_KeyInfo_t;

/** @typedef gpKeyboard_pKeyInfo_t
 *  @brief The gpKeyboard_pKeyInfo_t type defines a pointer to a gpKeyboard_KeyInfo structure
*/
typedef gpKeyboard_KeyInfo_t* gpKeyboard_pKeyInfo_t;
    
/** @name gpKeyboard_CmdType_t */
//@{
/** @brief TBD */
#define gpKeyboard_CmdTypeExternParsed      0x00
/** @brief Key command enters setup mode */
#define gpKeyboard_CmdTypeSetup             0x01
/** @brief Setup mode timed out*/
#define gpKeyboard_CmdTypeSetupTimeout      0x02
/** @brief Key command is a Reset command */
#define gpKeyboard_CmdTypeReset             0x10
/** @brief Key command is a Bind command using the BindValidation block*/
#define gpKeyboard_CmdTypeBindValidation    0x20
/** @brief Key command is a Bind command using the BindAutoDisc block*/
#define gpKeyboard_CmdTypeBindAutoDisc      0x21
/** @brief Key command start the IR configuration*/
#define gpKeyboard_CmdTypeIrConfig          0x30
/** @brief Key command is Unknown or there is a pending command parsing */
#define gpKeyboard_CmdTypeInvalid           0xFE
/** @brief Key command is Nothing */
#define gpKeyboard_CmdTypeNone              0xFF

/** @typedef gpKeyboard_CmdType_t
 *  @brief The gpKeyboard_CmdType_t type defines the different commands a key combination can define
*/
typedef UInt8 gpKeyboard_CmdType_t;
//@}

/** @name gpKeyboard_LogicalId_t */
//@{
#define gpKeyboard_LogicalId_Select                        0x00
#define gpKeyboard_LogicalId_Up                            0x01
#define gpKeyboard_LogicalId_Down                          0x02
#define gpKeyboard_LogicalId_Left                          0x03
#define gpKeyboard_LogicalId_Right                         0x04
#define gpKeyboard_LogicalId_RightUp                       0x05
#define gpKeyboard_LogicalId_RightDown                     0x06
#define gpKeyboard_LogicalId_LeftUp                        0x07
#define gpKeyboard_LogicalId_LeftDown                      0x08
#define gpKeyboard_LogicalId_RootMenu                      0x09
#define gpKeyboard_LogicalId_SetupMenu                     0x0A
#define gpKeyboard_LogicalId_ContentsMenu                  0x0B
#define gpKeyboard_LogicalId_FavoriteMenu                  0x0C
#define gpKeyboard_LogicalId_Exit                          0x0D
#define gpKeyboard_LogicalId_Number0                       0x20
#define gpKeyboard_LogicalId_Number1                       0x21
#define gpKeyboard_LogicalId_Number2                       0x22
#define gpKeyboard_LogicalId_Number3                       0x23
#define gpKeyboard_LogicalId_Number4                       0x24
#define gpKeyboard_LogicalId_Number5                       0x25
#define gpKeyboard_LogicalId_Number6                       0x26
#define gpKeyboard_LogicalId_Number7                       0x27
#define gpKeyboard_LogicalId_Number8                       0x28
#define gpKeyboard_LogicalId_Number9                       0x29
#define gpKeyboard_LogicalId_Dot                           0x2A
#define gpKeyboard_LogicalId_Enter                         0x2B
#define gpKeyboard_LogicalId_Clear                         0x2C
#define gpKeyboard_LogicalId_NextFavorite                  0x2F
#define gpKeyboard_LogicalId_ChannelUp                     0x30
#define gpKeyboard_LogicalId_ChannelDown                   0x31
#define gpKeyboard_LogicalId_PreviousChannel               0x32
#define gpKeyboard_LogicalId_SoundSelect                   0x33
#define gpKeyboard_LogicalId_InputSelect                   0x34
#define gpKeyboard_LogicalId_DisplayInformation            0x35
#define gpKeyboard_LogicalId_Help                          0x36
#define gpKeyboard_LogicalId_PageUp                        0x37
#define gpKeyboard_LogicalId_PageDown                      0x38
#define gpKeyboard_LogicalId_Power                         0x40
#define gpKeyboard_LogicalId_VolumeUp                      0x41
#define gpKeyboard_LogicalId_VolumeDown                    0x42
#define gpKeyboard_LogicalId_Mute                          0x43
#define gpKeyboard_LogicalId_Play                          0x44
#define gpKeyboard_LogicalId_Stop                          0x45
#define gpKeyboard_LogicalId_Pause                         0x46
#define gpKeyboard_LogicalId_Record                        0x47
#define gpKeyboard_LogicalId_Rewind                        0x48
#define gpKeyboard_LogicalId_FastForward                   0x49
#define gpKeyboard_LogicalId_Eject                         0x4A
#define gpKeyboard_LogicalId_Forward                       0x4B
#define gpKeyboard_LogicalId_Backward                      0x4C
#define gpKeyboard_LogicalId_StopRecord                    0x4D
#define gpKeyboard_LogicalId_PauseRecord                   0x4E
#define gpKeyboard_LogicalId_Angle                         0x50
#define gpKeyboard_LogicalId_SubPicture                    0x51
#define gpKeyboard_LogicalId_VideoOnDemand                 0x52
#define gpKeyboard_LogicalId_ElectronicProgramGuide        0x53
#define gpKeyboard_LogicalId_TimerProgramming              0x54
#define gpKeyboard_LogicalId_InitialConfiguration          0x55
#define gpKeyboard_LogicalId_PlayFunction                  0x60
#define gpKeyboard_LogicalId_PausePlayFunction             0x61
#define gpKeyboard_LogicalId_RecordFunction                0x62
#define gpKeyboard_LogicalId_PauseRecordFunction           0x63
#define gpKeyboard_LogicalId_StopFunction                  0x64
#define gpKeyboard_LogicalId_MuteFunction                  0x65
#define gpKeyboard_LogicalId_RestoreVolumeFunction         0x66
#define gpKeyboard_LogicalId_TuneFunction                  0x67
#define gpKeyboard_LogicalId_SelectMediaFunction           0x68
#define gpKeyboard_LogicalId_SelectAVInputFunction         0x69
#define gpKeyboard_LogicalId_SelectAudioInputfunction      0x6A
#define gpKeyboard_LogicalId_PowerToggleFunction           0x6B
#define gpKeyboard_LogicalId_PowerOffFunction              0x6C
#define gpKeyboard_LogicalId_PowerOnFunction               0x6D
#define gpKeyboard_LogicalId_F1Blue                        0x71
#define gpKeyboard_LogicalId_F2Red                         0x72
#define gpKeyboard_LogicalId_F3Green                       0x73
#define gpKeyboard_LogicalId_F4Yellow                      0x74
#define gpKeyboard_LogicalId_F5                            0x75
#define gpKeyboard_LogicalId_Data                          0x76
#define gpKeyboard_LogicalId_A                             0xCA
#define gpKeyboard_LogicalId_B                             0xCB
#define gpKeyboard_LogicalId_C                             0xCC
#define gpKeyboard_LogicalId_D                             0xCD
#define gpKeyboard_LogicalId_E                             0xCE
#define gpKeyboard_LogicalId_F                             0xCF
#define gpKeyboard_LogicalId_G                             0xD0
#define gpKeyboard_LogicalId_H                             0xD1
#define gpKeyboard_LogicalId_I                             0xD2
#define gpKeyboard_LogicalId_J                             0xD3
#define gpKeyboard_LogicalId_K                             0xD4
#define gpKeyboard_LogicalId_L                             0xD5
#define gpKeyboard_LogicalId_M                             0xD6
#define gpKeyboard_LogicalId_N                             0xD7
#define gpKeyboard_LogicalId_O                             0xD8
#define gpKeyboard_LogicalId_P                             0xD9
#define gpKeyboard_LogicalId_Q                             0xDA
#define gpKeyboard_LogicalId_R                             0xDB
#define gpKeyboard_LogicalId_S                             0xDC
#define gpKeyboard_LogicalId_T                             0xDD
#define gpKeyboard_LogicalId_U                             0xDE
#define gpKeyboard_LogicalId_V                             0xDF
#define gpKeyboard_LogicalId_W                             0xE0
#define gpKeyboard_LogicalId_X                             0xE1
#define gpKeyboard_LogicalId_Y                             0xE2
#define gpKeyboard_LogicalId_Z                             0xE3
#define gpKeyboard_LogicalId_Lock                          0xA9
#define gpKeyboard_LogicalId_CAPS                          0xAA
#define gpKeyboard_LogicalId_ALT                           0xAB
#define gpKeyboard_LogicalId_Space                         0xAC
#define gpKeyboard_LogicalId_Comma                         0xA0
#define gpKeyboard_LogicalId_Home                          0x10
#define gpKeyboard_LogicalId_Profile                       0xA0
#define gpKeyboard_LogicalId_Call                          0xA1
#define gpKeyboard_LogicalId_Hold                          0xA2
#define gpKeyboard_LogicalId_End                           0xA3
#define gpKeyboard_LogicalId_Views                         0xA4
#define gpKeyboard_LogicalId_SelfView                      0xA5
#define gpKeyboard_LogicalId_Zoom_In                       0xA6
#define gpKeyboard_LogicalId_Zoom_Out                      0xA7
#define gpKeyboard_LogicalId_Mute_Mic                      0xB0
#define gpKeyboard_LogicalId_Stop_Video                    0xB1
#define gpKeyboard_LogicalId_WWW                           0xB2
#define gpKeyboard_LogicalId_At                            0xB3
#define gpKeyboard_LogicalId_COM                           0xB4
#define gpKeyboard_LogicalId_Share                         0xB5
#define gpKeyboard_LogicalId_Search                        0xB6
#define gpKeyboard_LogicalId_Lang                          0xB7
#define gpKeyboard_LogicalId_ClosedCaptioning              0xB8

#define gpKeyboard_LogicalId_InvalidId                     0xFF

/** @typedef gpKeyboard_LogicalId_t
 *  @brief The gpKeyboard_LogicalId_t   TBD
*/
typedef UInt8 gpKeyboard_LogicalId_t;
//@}

/** @name gpKeyboard_Mode_t */
//@{
/** @brief Keyboard don't decodes the keys, but uses an id to an external keymapping or a logical-id*/
#define gpKeyboard_ModeExternMap    0x00
/** @brief Keyboard decodes ZRC keys */
#define gpKeyboard_ModeZrc          0x01
/** @brief Keyboard decodes ZID keys */
#define gpKeyboard_ModeZid          0x02
/** @typedef gpKeyboard_Mode_t
 *  @brief The gpKeyboard_Mode_t type defines the different keyboard modes
*/
typedef UInt8 gpKeyboard_Mode_t;
//@}

/** @name gpKeyboard_SetupType_t */
//@{
/** @brief Initial press of the SETUP key */
#define gpKeyboard_SetupTypeSetupKeyPressed     0x00
/** @brief Release of SETUP key without entering setup mode */
#define gpKeyboard_SetupTypeSetupKeyReleased    0x01
/** @brief Key press during Setup mode */
#define gpKeyboard_SetupTypeKeyPressed          0x02
/** @brief SETUP is pressed long enough to enter setup mode */
#define gpKeyboard_SetupTypeEntered             0x03
/** @brief Setup mode has completed */
#define gpKeyboard_SetupTypeComplete            0x04
/** @brief Test mode has been triggered */
#define gpKeyboard_SetupTypeTestMode            0xFE
/** @brief Setup mode has timed out */
#define gpKeyboard_SetupTypeTimeOut             0xFF
/** @typedef gpKeyboard_SetupType_t
 *  @brief The gpKeyboard_SetupType_t type defines the different setup mode phases
*/
typedef UInt8 gpKeyboard_SetupType_t;
//@}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void    gpKeyboard_SetMode(gpKeyboard_Mode_t mode);
UInt8   gpKeyboard_GetNumOfKeysPressed(UInt8* bitmap);

void    gpKeyboard_cbKeyIndication(gpKeyboard_Mode_t mode, UInt8 numKeys, gpKeyboard_pKeyInfo_t* pKeys);
void    gpKeyboard_cbCommandIndication(gpKeyboard_CmdType_t cmd, UInt8 paramLength, UInt8* params);

void    gpKeyboard_cbSetupIndication(gpKeyboard_SetupType_t type, UInt8 paramLength, UInt8* params);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // _GPKEYBOARD_H_
