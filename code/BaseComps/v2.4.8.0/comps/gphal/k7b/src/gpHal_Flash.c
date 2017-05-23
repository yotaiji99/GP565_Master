/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 *  The file contains functionality to program Kx flash
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/src/gpHal_Flash.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

#include "gpBsp.h"
#include "hal.h"

#define COMPLETE_MAP
#include "gpHal.h"
#include "gpLog.h"

#include "gpHal_kx_Flash.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

#define FLASH_START_ADDRESS             GP_MM_FLASH_LINEAR_START

#define FLASH_SIZE                      (GP_MM_FLASH_LINEAR_SIZE - GP_MM_FLASH_INF_PAGE_SIZE)

#define FLASH_FUNCTION_NONE             (GP_WB_MM_FLASH_CE_MASK)
#define FLASH_FUNCTION_PAGE_ERASE       (GP_WB_MM_FLASH_CE_MASK | GP_WB_MM_FLASH_PERASE_MASK)
#define FLASH_FUNCTION_SECTOR_ERASE     (GP_WB_MM_FLASH_CE_MASK | GP_WB_MM_FLASH_SERASE_MASK)
#define FLASH_FUNCTION_MASS_ERASE       (GP_WB_MM_FLASH_CE_MASK | GP_WB_MM_FLASH_MERASE_MASK)
#define FLASH_FUNCTION_PAGE_WRITE       (GP_WB_MM_FLASH_CE_MASK | GP_WB_MM_FLASH_WRONLY_MASK)
#define FLASH_FUNCTION_PAGE_PROG        (GP_WB_MM_FLASH_CE_MASK | GP_WB_MM_FLASH_PROG_MASK  )

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

 #define FLASH_ALIGN_SECTOR(address)     ((address) - ((address) % FLASH_SECTOR_SIZE))

#define FLASH_IN_PAGE(address, length)  ((length) <= (FLASH_PAGE_SIZE - ((address) % FLASH_PAGE_SIZE)))

#define FLASH_ADDR_TO_PHYS(address)     (((address) - FLASH_START_ADDRESS) / 4)

#define FLASH_CHECK_ADDRESS(address)        (FLASH_START_ADDRESS <= (address) && (address) < FLASH_START_ADDRESS + FLASH_SIZE)
#define FLASH_CHECK_INF_ADDRESS(address)    (GP_MM_FLASH_INF_PAGE_START <= (address) && (address) < GP_MM_FLASH_INF_PAGE_END)

#define FLASH_CHECK_ADDRESS_WITH_RETURN(address)                                \
    do {                                                                        \
        if (!FLASH_CHECK_ADDRESS(address)) return gpHal_FlashError_OutOfRange;  \
    } while(0)
#define FLASH_CHECK_INF_ADDRESS_WITH_RETURN(address)                                \
    do {                                                                        \
        if (!FLASH_CHECK_INF_ADDRESS(address)) return gpHal_FlashError_OutOfRange;  \
    } while(0)
#define FLASH_CHECK_WORD_ALIGNED_WITH_RETURN(address)                           \
    do {                                                                        \
        if (((address) & 0x3) != 0) return gpHal_FlashError_UnalignedAddress;   \
    } while(0)
#define FLASH_CHECK_PAGE_ALIGNED_WITH_RETURN(address)                           \
    do {                                                                        \
        if ((address) != FLASH_ALIGN_PAGE(address)) return gpHal_FlashError_UnalignedAddress;   \
    } while(0)
#define FLASH_CHECK_SECTOR_ALIGNED_WITH_RETURN(address)                         \
    do {                                                                        \
        if ((address) != FLASH_ALIGN_SECTOR(address)) return gpHal_FlashError_UnalignedAddress; \
    } while(0)
#define FLASH_CHECK_PAGE_ACCESS_WITH_RETURN(address, length)                    \
    do {                                                                        \
        if (!FLASH_CHECK_ADDRESS(address) || !FLASH_IN_PAGE(address, length)) return gpHal_FlashError_OutOfRange;  \
    } while(0)

 /*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

 /*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

//ROM flash routines

// Expects the number of _WORDS_ to be written
extern void flash_write_page_rom(UInt32 address, UInt32* data, UInt16 length_and_type);

extern void flash_erase_rom(UInt32 address, UInt16 type);

// Expects the number of _BYTES_ to be read
extern UInt16 flash_read_rom(UInt32 address, UInt8* data, UInt16 length);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

        //hal_UartRxPoll(); //Avoid UART Rx overrun

static gpHal_FlashError_t gpHal_FlashWritePageBasic(FlashPtr physAddress, UInt32* data, UInt8 wordLength, UInt8 type)
{
    if (wordLength != 0)
    {
        flash_write_page_rom(physAddress, data, (((UInt16)wordLength) << 8) | type);
    }
    return gpHal_FlashError_Success;
}

static gpHal_FlashError_t gpHal_FlashEraseBasic(FlashPtr physAddress, UInt16 type)
{
    flash_erase_rom(physAddress, type);
    return gpHal_FlashError_Success;
}

/*
 * Expects 32-bit aligned address and size in 32-bit words.
 */
static gpHal_FlashError_t gpHal_FlashBlankCheck(FlashPtr address, UInt16 length)
{
    UIntLoop i;

    for (i = 0; i < length; ++i)
    {
        UInt32 erased;
        flash_read_rom(address + (4*i), (UInt8*) &erased, 4);
        if (erased != 0) return gpHal_FlashError_BlankFailure;
    }

    return gpHal_FlashError_Success;
}

static gpHal_FlashError_t gpHal_FlashVerify(FlashPtr address, UInt32* data, UInt16 length)
{
    UIntLoop i;

    for (i = 0; i < length; ++i)
    {
        UInt32 written;
        flash_read_rom(address + (4*i), (UInt8*) &written, 4);
        //GP_LOG_PRINTF("FlashVerif %d %lx %lx",0, i, written, data[i]);
        if (written != data[i]) return gpHal_FlashError_VerifyFailure;
    }

    return gpHal_FlashError_Success;
}

static void gpHal_FlashEraseRemappedPageInSector(FlashPtr sectorAddress)
{
    UIntLoop i;
    UInt8 validRemaps;

    validRemaps = GP_HAL_READ_REG(GP_WB_STANDBY_LOCK_NVM_PAGE_REMAP_ENTRY_0_ADDRESS);

    //Erase any remapped page in sector with page erase
    for (i = 0; i < 8; i++)
    {
        if(BM(i) & validRemaps)
        {
            UInt16 remapPageIndex;
            FlashPtr pageAddress;

            GP_HAL_READ_REGS16(GP_WB_STANDBY_NVM_PAGE_REMAP_ENTRY_0_INP_ADDR_ADDRESS + i*4, &remapPageIndex);
            pageAddress = ((FlashPtr)remapPageIndex * FLASH_PAGE_SIZE) + FLASH_START_ADDRESS; //address stored as page index

            if ((pageAddress >= sectorAddress) && (pageAddress < (sectorAddress+FLASH_SECTOR_SIZE)))
            {
                gpHal_FlashErasePage(pageAddress);
            }
        }
    }
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

gpHal_FlashError_t gpHal_FlashRead(FlashPtr address, UInt16 length, UInt8* data)
{
    FLASH_CHECK_ADDRESS_WITH_RETURN(address);

    //GP_LOG_PRINTF("R %d %lx %lx", 0, length, FLASH_ADDR_TO_PHYS(address), data) ;

    flash_read_rom(address, data, length);

    return gpHal_FlashError_Success;
}

gpHal_FlashError_t gpHal_FlashReadInf(FlashPtr address, UInt16 length, UInt8*  data)
{
    FLASH_CHECK_INF_ADDRESS_WITH_RETURN(address);

    //GP_LOG_PRINTF("R %d %lx %lx", 0, length, FLASH_ADDR_TO_PHYS(address), data) ;

    flash_read_rom(address, data, length);

    return gpHal_FlashError_Success;
}

/*
 * For now, make sure the INF page is not erase by accident.
 * Cannot really do an ASSERT here, as we cannot execute from FLASH.
 */
gpHal_FlashError_t gpHal_FlashEraseSector(FlashPtr address)
{
    gpHal_FlashError_t result;

    FLASH_CHECK_ADDRESS_WITH_RETURN(address);

    GP_LOG_PRINTF("ES %lx", 0, FLASH_ADDR_TO_PHYS(address)) ;

    result = gpHal_FlashEraseBasic(FLASH_ADDR_TO_PHYS(address), FLASH_FUNCTION_SECTOR_ERASE);

    //Erase any remapped page with Page Erase
    gpHal_FlashEraseRemappedPageInSector(address);

    return result ? result : gpHal_FlashBlankCheck(address, FLASH_SECTOR_SIZE / 4);
}

gpHal_FlashError_t gpHal_FlashErasePage(FlashPtr address)
{
    gpHal_FlashError_t result;

    FLASH_CHECK_ADDRESS_WITH_RETURN(address);

    GP_LOG_PRINTF("EP %lx", 0, FLASH_ADDR_TO_PHYS(address)) ;

    result = gpHal_FlashEraseBasic(FLASH_ADDR_TO_PHYS(address), FLASH_FUNCTION_PAGE_ERASE);

    return result ? result : gpHal_FlashBlankCheck(address, FLASH_PAGE_SIZE / 4);
}

gpHal_FlashError_t gpHal_FlashWritePage(FlashPtr address, UInt8 length, UInt32* data)
{
    gpHal_FlashError_t result;

    FLASH_CHECK_PAGE_ACCESS_WITH_RETURN(address, length);
    FLASH_CHECK_WORD_ALIGNED_WITH_RETURN(address);

    GP_LOG_PRINTF("WP %lx %lx %d", 0, FLASH_ADDR_TO_PHYS(address), data, (length + 3)/ 4) ;

    result = gpHal_FlashWritePageBasic(FLASH_ADDR_TO_PHYS(address), data, (length + 3)/ 4, FLASH_FUNCTION_PAGE_WRITE);

    return result ? result : gpHal_FlashVerify(address, data, (length + 3)/ 4);
}

gpHal_FlashError_t gpHal_FlashProgramPage(FlashPtr address, UInt8 length, UInt8* data)
{
    gpHal_FlashError_t result;
    FlashPtr           pageAligned = FLASH_ALIGN_PAGE(address);
    UInt32             page[FLASH_PAGE_SIZE / 4];

    GP_LOG_PRINTF("P1 %lx < %lx < %lx",0, FLASH_START_ADDRESS, address , FLASH_START_ADDRESS + FLASH_SIZE);
    FLASH_CHECK_PAGE_ACCESS_WITH_RETURN(address, length);

    gpHal_FlashRead(pageAligned, FLASH_PAGE_SIZE, (UInt8*) page);
    MEMCPY((UInt8*)page + (address - pageAligned), data, length);

    result = gpHal_FlashWritePageBasic(FLASH_ADDR_TO_PHYS(pageAligned), page, FLASH_PAGE_SIZE / 4, FLASH_FUNCTION_PAGE_PROG);

    return result ? result : gpHal_FlashVerify(pageAligned, page, FLASH_PAGE_SIZE / 4);
}
