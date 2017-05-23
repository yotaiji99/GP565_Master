/*
 * Copyright (c) 2014, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/inc/gpHal_kx_SEC.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GPHAL_KX_SEC_H_
#define _GPHAL_KX_SEC_H_

/* check that frame ptr register only holds the frame_ptr property as an active property*/
#define GP_HAL_CHECK_FRAME_PTR_ONLY_ACTIVE_PROP_IN_REG()                \
    do { /* no need to allow for extra inactive properties */           \
        GP_HAL_CHECK_ONLY_PROP_IN_REG(GPHAL_PROP_PBM_FORMAT_T_FRAME_PTR); \
    } while (0)

/* check that A ptr register only holds the A ptr property as an active property*/
#define GP_HAL_CHECK_A_PTR_ONLY_ACTIVE_PROP_IN_REG()                    \
    do { /* no need to allow for extra incactive properties */          \
        GP_HAL_CHECK_ONLY_PROP_IN_REG(GPHAL_PROP_PBM_FORMAT_S_A_PTR);   \
    } while (0)

#endif //_GPHAL_KX_SEC_H_
