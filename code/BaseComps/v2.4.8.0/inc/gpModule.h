/*
 * Copyright (c) 2010-2014, GreenPeak Technologies
 *
 * gpModule.h
 *   This file contains the MODULE ID of the components that uses the serial protocol.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/inc/gpModule.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GP_MODULE_H_
#define _GP_MODULE_H_


#define GP_MODULE_ID_APP                        1
#define GP_MODULE_ID_RXARBITERAPI               2
#define GP_MODULE_ID_GPHAL                      7
#ifdef GP_COMP_COM
#define GP_MODULE_ID_COM                        10
#endif //GP_COMP_COM
#define GP_MODULE_ID_LOG                        11
#define GP_MODULE_ID_ASSERTAPI                  29
#define GP_MODULE_ID_MACCOREAPI                 109
#define GP_MODULE_ID_NVM_API                    32
#define GP_MODULE_ID_RESETAPI                    33
#define GP_MODULE_ID_BASECOMPSAPI 35
#define GP_MODULE_ID_RF4CEMSOPROFILE            39
#define GP_MODULE_ID_RF4CEUSERCONTROLAPI        41
#define GP_MODULE_ID_RF4CEBINDVALIDATIONAPI     42
#define GP_MODULE_ID_SNIFFERAPI                 68
#define GP_MODULE_ID_KEYSCAN                    12
#define GP_MODULE_ID_IRTXAPI                    102
#define GP_MODULE_ID_RANDOMAPI                  108
#define GP_MODULE_ID_MACDISPATCHERAPI           114
#define GP_MODULE_ID_ENCRYPTIONAPI              124
#define GP_MODULE_ID_VERSIONAPI                 129
#ifdef GP_COMP_OSHS
#define GP_MODULE_ID_OSHS                       138
#endif
#ifdef GP_COMP_SCHEDEXT
#define GP_MODULE_ID_SCHEDEXT                   139
#endif
#ifdef GP_COMP_SENSORHAL
#define GP_MODULE_ID_SENSORHAL                  140
#endif
#define GP_MODULE_ID_NVMAPI 181

#endif  // _GP_MODULE_H_

