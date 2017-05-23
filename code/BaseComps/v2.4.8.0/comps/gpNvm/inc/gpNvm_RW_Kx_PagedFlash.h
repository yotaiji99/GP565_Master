
/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 * gpNvm.h
 *
 * This file defines the Non Volatilie Memory component api
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpNvm/inc/gpNvm_RW_Kx_PagedFlash.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */


#ifndef _GPNVM_RW_KX_PAGEDFLASH_H_
#define _GPNVM_RW_KX_PAGEDFLASH_H_

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#ifndef GP_DATA_SECTION_SIZE_NVM
#error define GP_DATA_SECTION_SIZE_NVM when building
#endif //GP_DATA_SECTION_SIZE_NVM
#define NVM_SIZE          (GP_DATA_SECTION_SIZE_NVM)
#define NVM_NUMBER_PAGES  (NVM_SIZE / FLASH_PAGE_SIZE)

#endif //_GPNVM_RW_KX_PAGEDFLASH_H_
