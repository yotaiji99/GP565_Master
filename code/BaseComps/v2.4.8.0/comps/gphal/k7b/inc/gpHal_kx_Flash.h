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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/inc/gpHal_kx_Flash.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GPHAL_KX_FLASH_H_
#define _GPHAL_KX_FLASH_H_

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef UInt32  FlashPtr;


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define FLASH_ALIGN_PAGE(address)       ((address) - ((address) % FLASH_PAGE_SIZE))

#define FLASH_WORD_SIZE             (4)
#define FLASH_PAGE_SIZE              128
#define FLASH_PAGES_PER_SECTOR       32
#define FLASH_SECTOR_SIZE           (FLASH_PAGES_PER_SECTOR * FLASH_PAGE_SIZE) //4096

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

typedef enum {
    gpHal_FlashError_Success = 0,
    gpHal_FlashError_OutOfRange,
    gpHal_FlashError_UnalignedAddress,
    gpHal_FlashError_BlankFailure,
    gpHal_FlashError_VerifyFailure
} gpHal_FlashError_t;

/*
 * Addresses are absolute - ie. according to the Kx memory map (not relative).
 */
gpHal_FlashError_t gpHal_FlashRead       (FlashPtr address, UInt16 length, UInt8*  data);

gpHal_FlashError_t gpHal_FlashReadInf    (FlashPtr address, UInt16 length, UInt8*  data);

gpHal_FlashError_t gpHal_FlashErasePage  (FlashPtr address);
gpHal_FlashError_t gpHal_FlashEraseSector(FlashPtr address);
gpHal_FlashError_t gpHal_FlashWritePage  (FlashPtr address, UInt8  length, UInt32* data);
gpHal_FlashError_t gpHal_FlashProgramPage(FlashPtr address, UInt8  length, UInt8*  data);

#endif //_GPHAL_KX_FLASH_H_
