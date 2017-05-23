/*
 * Copyright (c) 2014, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/inc/gpHal_kx_defs.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GPHAL_KX_DEFS_H_
#define _GPHAL_KX_DEFS_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define COMPLETE_MAP //Development

#define GPHAL_CHIP_ID_K7B           7
#ifndef GP_HAL_EXPECTED_CHIP_ID
#define GP_HAL_EXPECTED_CHIP_ID     GPHAL_CHIP_ID_K7B
#endif


#include "gpHal_kx_regprop.h"
#include "gpHal_kx_regprop_basic.h"

#include "gpHal_kx_enum.h"
#include "gpHal_kx_enum_manual.h"

#include "gpHal_kx_mm.h"
#include "gpHal_kx_mm_manual.h"
#include "gpHal_kx_MSI_basic.h"


#include "gpHal_kx_public.h"

#include "gpHal_kx_Flash.h"

#endif //_GPHAL_KX_DEFS_H_
