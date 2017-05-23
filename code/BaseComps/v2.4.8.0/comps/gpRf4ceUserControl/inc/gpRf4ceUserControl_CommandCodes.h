/*
 * Copyright (c) 2011-2013, GreenPeak Technologies
 *
 *   This file contains the definitions of the public functions and enumerations of the gpRf4ceUserControl.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpRf4ceUserControl/inc/gpRf4ceUserControl_CommandCodes.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GP_RF4CE_USER_CONTROL_COMMANDCODES_H_
#define _GP_RF4CE_USER_CONTROL_COMMANDCODES_H_

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

/// @name gpRf4ceUserControl_CommandCode_t
//@{
#define gpRf4ceUserControl_CommandCodeSelect                        0x00
#define gpRf4ceUserControl_CommandCodeUp                            0x01
#define gpRf4ceUserControl_CommandCodeDown                          0x02
#define gpRf4ceUserControl_CommandCodeLeft                          0x03
#define gpRf4ceUserControl_CommandCodeRight                         0x04
#define gpRf4ceUserControl_CommandCodeRightUp                       0x05
#define gpRf4ceUserControl_CommandCodeRightDown                     0x06
#define gpRf4ceUserControl_CommandCodeLeftUp                        0x07
#define gpRf4ceUserControl_CommandCodeLeftDown                      0x08
#define gpRf4ceUserControl_CommandCodeRootMenu                      0x09
#define gpRf4ceUserControl_CommandCodeSetupMenu                     0x0A
#define gpRf4ceUserControl_CommandCodeContentsMenu                  0x0B
#define gpRf4ceUserControl_CommandCodeFavoriteMenu                  0x0C
#define gpRf4ceUserControl_CommandCodeExit                          0x0D
#define gpRf4ceUserControl_CommandCodeNumber0                       0x20
#define gpRf4ceUserControl_CommandCodeNumber1                       0x21
#define gpRf4ceUserControl_CommandCodeNumber2                       0x22
#define gpRf4ceUserControl_CommandCodeNumber3                       0x23
#define gpRf4ceUserControl_CommandCodeNumber4                       0x24
#define gpRf4ceUserControl_CommandCodeNumber5                       0x25
#define gpRf4ceUserControl_CommandCodeNumber6                       0x26
#define gpRf4ceUserControl_CommandCodeNumber7                       0x27
#define gpRf4ceUserControl_CommandCodeNumber8                       0x28
#define gpRf4ceUserControl_CommandCodeNumber9                       0x29
#define gpRf4ceUserControl_CommandCodeDot                           0x2A
#define gpRf4ceUserControl_CommandCodeEnter                         0x2B
#define gpRf4ceUserControl_CommandCodeClear                         0x2C
#define gpRf4ceUserControl_CommandCodeNextFavorite                  0x2F
#define gpRf4ceUserControl_CommandCodeChannelUp                     0x30
#define gpRf4ceUserControl_CommandCodeChannelDown                   0x31
#define gpRf4ceUserControl_CommandCodePreviousChannel               0x32
#define gpRf4ceUserControl_CommandCodeSoundSelect                   0x33
#define gpRf4ceUserControl_CommandCodeInputSelect                   0x34
#define gpRf4ceUserControl_CommandCodeDisplayInformation            0x35
#define gpRf4ceUserControl_CommandCodeHelp                          0x36
#define gpRf4ceUserControl_CommandCodePageUp                        0x37
#define gpRf4ceUserControl_CommandCodePageDown                      0x38
#define gpRf4ceUserControl_CommandCodePower                         0x40
#define gpRf4ceUserControl_CommandCodeVolumeUp                      0x41
#define gpRf4ceUserControl_CommandCodeVolumeDown                    0x42
#define gpRf4ceUserControl_CommandCodeMute                          0x43
#define gpRf4ceUserControl_CommandCodePlay                          0x44
#define gpRf4ceUserControl_CommandCodeStop                          0x45
#define gpRf4ceUserControl_CommandCodePause                         0x46
#define gpRf4ceUserControl_CommandCodeRecord                        0x47
#define gpRf4ceUserControl_CommandCodeRewind                        0x48
#define gpRf4ceUserControl_CommandCodeFastForward                   0x49
#define gpRf4ceUserControl_CommandCodeEject                         0x4A
#define gpRf4ceUserControl_CommandCodeForward                       0x4B
#define gpRf4ceUserControl_CommandCodeBackward                      0x4C
#define gpRf4ceUserControl_CommandCodeStopRecord                    0x4D
#define gpRf4ceUserControl_CommandCodePauseRecord                   0x4E
#define gpRf4ceUserControl_CommandCodeAngle                         0x50
#define gpRf4ceUserControl_CommandCodeSubPicture                    0x51
#define gpRf4ceUserControl_CommandCodeVideoOnDemand                 0x52
#define gpRf4ceUserControl_CommandCodeElectronicProgramGuide        0x53
#define gpRf4ceUserControl_CommandCodeTimerProgramming              0x54
#define gpRf4ceUserControl_CommandCodeInitialConfiguration          0x55
#define gpRf4ceUserControl_CommandCodePlayFunction                  0x60    /*Additional operands*/
#define gpRf4ceUserControl_CommandCodePausePlayfunction             0x61
#define gpRf4ceUserControl_CommandCodeRecordFunction                0x62
#define gpRf4ceUserControl_CommandCodePauseRecordFunction           0x63
#define gpRf4ceUserControl_CommandCodeStopFunction                  0x64
#define gpRf4ceUserControl_CommandCodeMuteFunction                  0x65
#define gpRf4ceUserControl_CommandCodeRestoreVolumeFunction         0x66
#define gpRf4ceUserControl_CommandCodeTuneFunction                  0x67    /*Additional operands*/
#define gpRf4ceUserControl_CommandCodeSelectMediaFunction           0x68    /*Additional operands*/
#define gpRf4ceUserControl_CommandCodeSelectAVInputFunction         0x69    /*Additional operands*/
#define gpRf4ceUserControl_CommandCodeSelectAudioInputfunction      0x6A    /*Additional operands*/
#define gpRf4ceUserControl_CommandCodePowerToggleFunction           0x6B
#define gpRf4ceUserControl_CommandCodePowerOffFunction              0x6C
#define gpRf4ceUserControl_CommandCodePowerOnFunction               0x6D
#define gpRf4ceUserControl_CommandCodeF1Blue                        0x71
#define gpRf4ceUserControl_CommandCodeF2Red                         0x72
#define gpRf4ceUserControl_CommandCodeF3Green                       0x73
#define gpRf4ceUserControl_CommandCodeF4Yellow                      0x74
#define gpRf4ceUserControl_CommandCodeF5                            0x75
#define gpRf4ceUserControl_CommandCodeData                          0x76


#define gpRf4ceUserControl_CommandCodeNone                          0xFE

/// @typedef gpRf4ceUserControl_CommandCode_t
/// @brief The gpRf4ceUserControl_CommandCode_t type defines the user control command.
typedef UInt8 gpRf4ceUserControl_CommandCode_t;
//@}

#endif //_GP_RF4CE_USER_CONTROL_COMMANDCODES_H_


