/*
 * Copyright (c) 2010-2014, GreenPeak Technologies
 *
 * gpBaseComps.h
 *
 * Initialization function of the BaseComps components.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpBaseComps/inc/gpBaseComps.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */
#ifndef _GPBASECOMPS_H_
#define _GPBASECOMPS_H_

#include "global.h"

#ifdef __cplusplus
extern "C" {
#endif

GP_API void gpBaseComps_StackInit(void);
GP_API void gpBaseComps_StackDeInit(void);

GP_API void gpBaseComps_StackReInit(void);



GP_API void gpBaseComps_MacDeInit(void);
GP_API void gpBaseComps_MacInit(void);
GP_API void gpBaseComps_Rf4ceInit(void);
GP_API void gpBaseComps_Rf4ceDeInit(void);

#ifdef __cplusplus
}
#endif

#endif // _GPBASECOMPS_H_

