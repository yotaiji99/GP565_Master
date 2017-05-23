/*
 * Copyright (c) 2011-2013, GreenPeak Technologies
 *
 *   This file contains the definitions of the public functions and enumerations of the gpRf4ceActionControl.
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/comps/gpRf4ceActionControl/inc/gpRf4ceActionControl_CommandCodes.h#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */

#ifndef _GP_RF4CE_ACTION_CONTROL_COMMANDCODES_H_
#define _GP_RF4CE_ACTION_CONTROL_COMMANDCODES_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <global.h>

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/// @name gpRf4ceActionControl_CommandCode_t
//@{
#define gpRf4ceActionControl_CommandCodeSelect                        0x00
#define gpRf4ceActionControl_CommandCodeUp                            0x01
#define gpRf4ceActionControl_CommandCodeDown                          0x02
#define gpRf4ceActionControl_CommandCodeLeft                          0x03
#define gpRf4ceActionControl_CommandCodeRight                         0x04
#define gpRf4ceActionControl_CommandCodeRightUp                       0x05
#define gpRf4ceActionControl_CommandCodeRightDown                     0x06
#define gpRf4ceActionControl_CommandCodeLeftUp                        0x07
#define gpRf4ceActionControl_CommandCodeLeftDown                      0x08
#define gpRf4ceActionControl_CommandCodeRootMenu                      0x09
#define gpRf4ceActionControl_CommandCodeSetupMenu                     0x0A
#define gpRf4ceActionControl_CommandCodeContentsMenu                  0x0B
#define gpRf4ceActionControl_CommandCodeFavoriteMenu                  0x0C
#define gpRf4ceActionControl_CommandCodeExit                          0x0D
#define gpRf4ceActionControl_CommandCodeNumber0                       0x20
#define gpRf4ceActionControl_CommandCodeNumber1                       0x21
#define gpRf4ceActionControl_CommandCodeNumber2                       0x22
#define gpRf4ceActionControl_CommandCodeNumber3                       0x23
#define gpRf4ceActionControl_CommandCodeNumber4                       0x24
#define gpRf4ceActionControl_CommandCodeNumber5                       0x25
#define gpRf4ceActionControl_CommandCodeNumber6                       0x26
#define gpRf4ceActionControl_CommandCodeNumber7                       0x27
#define gpRf4ceActionControl_CommandCodeNumber8                       0x28
#define gpRf4ceActionControl_CommandCodeNumber9                       0x29
#define gpRf4ceActionControl_CommandCodeDot                           0x2A
#define gpRf4ceActionControl_CommandCodeEnter                         0x2B
#define gpRf4ceActionControl_CommandCodeClear                         0x2C
#define gpRf4ceActionControl_CommandCodeNextFavorite                  0x2F
#define gpRf4ceActionControl_CommandCodeChannelUp                     0x30
#define gpRf4ceActionControl_CommandCodeChannelDown                   0x31
#define gpRf4ceActionControl_CommandCodePreviousChannel               0x32
#define gpRf4ceActionControl_CommandCodeSoundSelect                   0x33
#define gpRf4ceActionControl_CommandCodeInputSelect                   0x34
#define gpRf4ceActionControl_CommandCodeDisplayInformation            0x35
#define gpRf4ceActionControl_CommandCodeHelp                          0x36
#define gpRf4ceActionControl_CommandCodePageUp                        0x37
#define gpRf4ceActionControl_CommandCodePageDown                      0x38
#define gpRf4ceActionControl_CommandCodePower                         0x40
#define gpRf4ceActionControl_CommandCodeVolumeUp                      0x41
#define gpRf4ceActionControl_CommandCodeVolumeDown                    0x42
#define gpRf4ceActionControl_CommandCodeMute                          0x43
#define gpRf4ceActionControl_CommandCodePlay                          0x44
#define gpRf4ceActionControl_CommandCodeStop                          0x45
#define gpRf4ceActionControl_CommandCodePause                         0x46
#define gpRf4ceActionControl_CommandCodeRecord                        0x47
#define gpRf4ceActionControl_CommandCodeRewind                        0x48
#define gpRf4ceActionControl_CommandCodeFastForward                   0x49
#define gpRf4ceActionControl_CommandCodeEject                         0x4A
#define gpRf4ceActionControl_CommandCodeForward                       0x4B
#define gpRf4ceActionControl_CommandCodeBackward                      0x4C
#define gpRf4ceActionControl_CommandCodeStopRecord                    0x4D
#define gpRf4ceActionControl_CommandCodePauseRecord                   0x4E
#define gpRf4ceActionControl_CommandCodeAngle                         0x50
#define gpRf4ceActionControl_CommandCodeSubPicture                    0x51
#define gpRf4ceActionControl_CommandCodeVideoOnDemand                 0x52
#define gpRf4ceActionControl_CommandCodeElectronicProgramGuide        0x53
#define gpRf4ceActionControl_CommandCodeTimerProgramming              0x54
#define gpRf4ceActionControl_CommandCodeInitialConfiguration          0x55
#define gpRf4ceActionControl_CommandCodePlayFunction                  0x60    /*Additional operands*/
#define gpRf4ceActionControl_CommandCodePausePlayfunction             0x61
#define gpRf4ceActionControl_CommandCodeRecordFunction                0x62
#define gpRf4ceActionControl_CommandCodePauseRecordFunction           0x63
#define gpRf4ceActionControl_CommandCodeStopFunction                  0x64
#define gpRf4ceActionControl_CommandCodeMuteFunction                  0x65
#define gpRf4ceActionControl_CommandCodeRestoreVolumeFunction         0x66
#define gpRf4ceActionControl_CommandCodeTuneFunction                  0x67    /*Additional operands*/
#define gpRf4ceActionControl_CommandCodeSelectMediaFunction           0x68    /*Additional operands*/
#define gpRf4ceActionControl_CommandCodeSelectAVInputFunction         0x69    /*Additional operands*/
#define gpRf4ceActionControl_CommandCodeSelectAudioInputfunction      0x6A    /*Additional operands*/
#define gpRf4ceActionControl_CommandCodePowerToggleFunction           0x6B
#define gpRf4ceActionControl_CommandCodePowerOffFunction              0x6C
#define gpRf4ceActionControl_CommandCodePowerOnFunction               0x6D
#define gpRf4ceActionControl_CommandCodeF1Blue                        0x71
#define gpRf4ceActionControl_CommandCodeF2Red                         0x72
#define gpRf4ceActionControl_CommandCodeF3Green                       0x73
#define gpRf4ceActionControl_CommandCodeF4Yellow                      0x74
#define gpRf4ceActionControl_CommandCodeF5                            0x75
#define gpRf4ceActionControl_CommandCodeData                          0x76


#define gpRf4ceActionControl_CommandCodeNone                          0xFE

/// @typedef gpRf4ceActionControl_CommandCode_t
/// @brief The gpRf4ceActionControl_CommandCode_t type defines the user control command.
typedef UInt8 gpRf4ceActionControl_CommandCode_t;
//@}

#endif //_GP_RF4CE_ACTION_CONTROL_COMMANDCODES_H_


