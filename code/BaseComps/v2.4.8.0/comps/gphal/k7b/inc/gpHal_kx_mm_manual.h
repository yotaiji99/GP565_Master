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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/inc/gpHal_kx_mm_manual.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*
* This file should contain all non-hardware dependent defines from the normal mm file.
* Most of the inf page sections that are only used in software should be described here.
*/

 #define GP_HAL_MM_UNKNOWN_CHIP_VERSION_HANDLING 0x4810B
/** @brief Fixed location in OTP*/

/** @brief Packet Buffer Memory entries*/
#define GPHAL_MM_PBMS_START             GP_MM_RAM_PBM_0_DATA_START
#define GPHAL_MM_PBM_NR_OF             (GP_MM_RAM_PBM_OPTS_NR_OF - 1) /*ACK buffer always claimed*/
#define GPHAL_MM_PBM_OFFSET             GP_MM_RAM_PBM_0_DATA_SIZE

#define GP_MM_FLASH_USER_KEY_0_START    0x48180
#define GP_MM_FLASH_USER_KEY_0_END      0x48190
#define GP_MM_FLASH_USER_KEY_0_SIZE     0x10


