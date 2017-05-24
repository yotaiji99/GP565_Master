/*
 * Copyright (c) 2013-2014, GreenPeak Technologies
 *
 * This file gives an implementation of the Non Volatile Memory component for internal FLASH on Kx chips.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpNvm/src/gpNvm_RW_Kx_PagedFlash.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
//#define GP_LOCAL_LOG

#include <limits.h>
#include "global.h"
#include "hal.h"
#include "gpNvm.h"
#include "gpNvm_defs.h"
#include "gpLog.h"
#include "gpAssert.h"
#include "gpHal.h"
#include "gpUtils.h"
#include "gpNvm_RW_Kx_PagedFlash.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
                    
#define GP_COMPONENT_ID GP_COMPONENT_ID_NVM

#define NVM_START_ADDR                 0

#define FLASH_ADDRESS(page)  (nvmFlashBase + page * FLASH_PAGE_SIZE)
#define RESOLVE_TO_PHYSICAL_PAGE(page) (nvmRemapTable[page])

// ############################################################################
// #                                                                          #
// # WARNING: PAGE CRC _MUST_ BE THE FIRST ELEMENT IN A PAGE HEADER, TO ALLOW #
// # THE CORRECT CALCULATION OF PAGE CRC                                      #
// #                                                                          #
// ############################################################################
#define NVM_PAGE_HEADER_OFFSET_TO_PAGE_CRC                    (0) // # DO NOT CHANGE LOCATION OF THIS PROPERTY, PAGE CRC CALCULATION DEPENDS ON IT
#define NVM_PAGE_HEADER_CRC_BYTES                             (2) // # DO NOT CHANGE LOCATION OF THIS PROPERTY, PAGE CRC CALCULATION DEPENDS ON IT
#define NVM_PAGE_HEADER_OFFSET_TO_PAGE_MEMORY_RANGE_SPECIFIER (NVM_PAGE_HEADER_OFFSET_TO_PAGE_CRC + NVM_PAGE_HEADER_CRC_BYTES) // 1 byte to determine NVM page address range
#define NVM_PAGE_HEADER_MEMORY_RANGE_SPECIFIER_BYTES          (1)
#define NVM_PAGE_HEADER_OFFSET_TO_PAGE_COUNTER                (NVM_PAGE_HEADER_OFFSET_TO_PAGE_MEMORY_RANGE_SPECIFIER + NVM_PAGE_HEADER_MEMORY_RANGE_SPECIFIER_BYTES)
#define NVM_PAGE_HEADER_PAGE_COUNTER_BYTES                    (2)
#define NVM_PAGE_HEADER_SIZE                                  (NVM_PAGE_HEADER_CRC_BYTES + NVM_PAGE_HEADER_MEMORY_RANGE_SPECIFIER_BYTES + NVM_PAGE_HEADER_PAGE_COUNTER_BYTES)

#define NUMBER_OF_PRECEDING_HEADERS(address) ( (address / (FLASH_PAGE_SIZE - NVM_PAGE_HEADER_SIZE)) + 1)

#define FLASH_BROKEN_PAGE_LABEL  {0x44, 0x45, 0x41, 0x44}

#define FLASH_FREE_LABEL         {0x0, 0x0, 0x0, 0x0, 0x0}

// link bit in page counter is enabled only if data written by a write block operation
// is to spill to the next page
#define NVM_PAGE_COUNTER_RESET_THRESHOLD      (SHRT_MAX - NVM_NUMBER_PAGES - 1) // limit range so that bit 15 is disabled. also prevent theoretical counter overflow _during_ block write (worst case block write programs entire NVM).
#define NVM_PAGE_ENABLE_LINK(x)               (x | (1<<15))
#define IS_NVM_PAGE_COUNTER_LINK_ENABLED(x)   ((x>>15) & 1)
#define NVM_PAGE_COUNTER_WITHOUT_LINK(x)      (x & ~(1<<15))

#define NOT_AVAILABLE   (0xFF)

//Bitmask accessing macro's
#define NVM_REMAP_TABLE_IS_VALID(virtualPage)   BIT_TST(isNvmRemapTableValid[(virtualPage)/8], (virtualPage)%8)
#define NVM_REMAP_TABLE_SET_VALID(virtualPage)  BIT_SET(isNvmRemapTableValid[(virtualPage)/8], (virtualPage)%8)
#define NVM_REMAP_TABLE_CLR_VALID(virtualPage)  BIT_CLR(isNvmRemapTableValid[(virtualPage)/8], (virtualPage)%8)

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct
{
    UInt8  physicalPageCopy1 ; // slot in nvmRemapTable
    UInt8  physicalPageCopy2 ; // logicalPage of memory range copy2, if such exists
    UInt8  refCounter ; // how many pages refer to this address range
} memoryRangeInfo_t ;

typedef struct
{
    UInt8  virtualPage;
    UInt16 pageCounter;
} pageInfoSummary_t ;

typedef struct
{
    FlashPtr  address ;
    UInt8    *data ;
    Int16     length ;
    UInt8    *bytesWritten ;
    UInt8     virtualPage ;
    UInt8     virtualPageResetCounterTrigger ;
    
} preparePage_t ;

/*****************************************************************************
 *                    Checks
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

STATIC UInt8 numberOfDeletedPages ;

STATIC UInt8 deletedPages[NVM_NUMBER_PAGES] ;

STATIC UInt16 globalNvmPageCounter ;

STATIC UInt8 freePageSearchStart ;

STATIC const UInt8 pageBrokenLabel[] = FLASH_BROKEN_PAGE_LABEL ;

STATIC const UInt8 pageFreeLabel[] = FLASH_FREE_LABEL ;

STATIC FlashPtr nvmFlashBase ;

typedef union nvmFlashGlobal_u
{
    struct nvmFlashRuntimeVariables_t
    {
        UInt32 devicePage[FLASH_PAGE_SIZE / FLASH_WORD_SIZE];
    } nvmFlashRuntimeVariables;
    struct nvmFlashBootVariables_t
    {
        memoryRangeInfo_t memoryRangeInfo[NVM_NUMBER_PAGES];
        pageInfoSummary_t pageInfoSummary[NVM_NUMBER_PAGES];
    } nvmFlashBootVariables;
} nvmFlashGlobal_t;

STATIC nvmFlashGlobal_t nvmFlashGlobal;


STATIC UInt32 pageBackup[FLASH_PAGE_SIZE / FLASH_WORD_SIZE];

// Map address range to physical address (with the same size)
// Entry 0 maps resolves accesses to NVM address range 0x0   - 0x80
// Entry 1 maps resolves accesses to NVM address range 0x80  - 0x100
// Entry 2 maps resolves accesses to NVM address range 0x100 - 0x180
// ...
STATIC UInt8 nvmRemapTable[NVM_NUMBER_PAGES] ;

// make sure that read operations of mapped pages don't return invalid data due to a double reference,
// and that write operations don't delete data that does not belong to the logical page addressed 
// (also because of double references).
// data is moved around the NVM, and this will lead to double references if a default mapping is 
// provided (consider: because writing logical page 0 can map it to the physical page of logical 
// page 1 if that one is empty! the consequences are that a read operation of logical page 1 will 
// yield the wrong data, and when logical page 1 is written, it will destroy the physical data of 
// logical page 0 because they are mapped to the same physical address and old data is always delete
// (after writing has been verified). So pages that are not in use (memoryRangeSpecifier == 0) are 
// mapped only during a write operation, and read operations that for pages that are not mapped
// yet will have no side effects in terms of accessing the NVM.
// logical pages are mapped when either we load a valid page from NVM (memoryRangeSpecifier != 0) or
// when we perform a write operation for a memory range that has not been mapped yet.
STATIC UInt8 isNvmRemapTableValid[(NVM_NUMBER_PAGES-1)/8 + 1] ; //Bitmask

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

const UIntPtr gpNvm_NvmBaseAddr = (NVM_START_ADDR);

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

static gpHal_FlashError_t Nvm_FlashReadPage(UInt8 page, UInt16 length, UInt8* data)
{
    return gpHal_FlashRead(FLASH_ADDRESS(page), length, data);
}

static gpHal_FlashError_t Nvm_FlashWritePage(UInt8 page, UInt8 length, UInt32* data)
{
    return gpHal_FlashWritePage(FLASH_ADDRESS(page), length, data);
}

static gpHal_FlashError_t Nvm_FlashErasePage(UInt8 page)
{
    return gpHal_FlashErasePage(FLASH_ADDRESS(page));
}

static void disableBrokenPage(UInt8 brokenPage)
{
    UInt8 index ;

    GP_LOG_PRINTF("disableBrokenPage->", 0) ;

    GP_LOG_PRINTF("Disabling page 0x%lx", 0, brokenPage) ;
    
    COMPILE_TIME_ASSERT(sizeof(pageBrokenLabel) == FLASH_WORD_SIZE) ;
    
    // disable the broken page - delete magic word
    Nvm_FlashErasePage(brokenPage);

    index = 0;

    while (index < FLASH_PAGE_SIZE)
    {
        MEMCPY((UInt8 *)nvmFlashGlobal.nvmFlashRuntimeVariables.devicePage + index, pageBrokenLabel, sizeof(pageBrokenLabel));

        index += (sizeof(pageBrokenLabel)<<1) ;
    }

    // Note: we write to the page without taking into account failure - no point in remapping here as there is no data to write
    Nvm_FlashWritePage(brokenPage, FLASH_PAGE_SIZE, nvmFlashGlobal.nvmFlashRuntimeVariables.devicePage);
    
    GP_LOG_PRINTF("<-disableBrokenPage", 0) ;
}

static Bool selectFreePage(UInt8 virtualPageToRemap)
{    
    UInt8 found = 0 ;
    UInt8 cycles = 0 ;
    UInt8 physicalPage ;
    UInt8 memoryRangeSpecifier ; // memoryRangeSpecifier starts at 1
    
    GP_LOG_PRINTF("selectFreePage->", 0) ;

    // move on at least one page beyond the previous search starting point
    if (++freePageSearchStart >= NVM_NUMBER_PAGES)
    {
        freePageSearchStart = 0 ;
    }
    
    physicalPage = freePageSearchStart ;
    
    // potentially, scan the entire NVM
    while (cycles < NVM_NUMBER_PAGES)
    {
        UInt8 header[NVM_PAGE_HEADER_SIZE] ;

        Nvm_FlashReadPage(physicalPage, NVM_PAGE_HEADER_SIZE, header) ;

        MEMCPY(&memoryRangeSpecifier, header + NVM_PAGE_HEADER_OFFSET_TO_PAGE_MEMORY_RANGE_SPECIFIER, NVM_PAGE_HEADER_MEMORY_RANGE_SPECIFIER_BYTES) ;

        if (memoryRangeSpecifier == 0)
        {
            // free page
            
            found = 1 ;
            
            // remap to new page
            nvmRemapTable[virtualPageToRemap] = physicalPage;

            GP_LOG_PRINTF("nvmRemapTable[%d]=0x%lx", 0, virtualPageToRemap, physicalPage); 
            
            break ;
        }
        
        ++cycles ;
        
        ++physicalPage ;
        
        if (physicalPage >= NVM_NUMBER_PAGES)
        {
            // we use FLASH_ADDRESS so logicalPage is translated into a physical address.
            // since we launch our search where we last stopped, we must be able to role-over.
            physicalPage = 0 ;
        }
    }
    
    if (!found)
    {
        // failed to allocate a free page - power failure robustness compromised but this is still allowed, meaning that new data page = old data page (one copy of data, if power failure occurs while writing data is lost).
        
        GP_LOG_PRINTF("Failed to allocate page replacement for virtual page %d", 0, virtualPageToRemap) ;
    }
    
    GP_LOG_PRINTF("<-selectFreePage", 0) ;
    
    return (found == 1) ;
}

static void writePage(UInt8 virtualPage)
{    
    UInt8 memoryRangeRecentlyAllocated = 0 ;    
    Bool newPageSelected ;
    UInt8 originalPhysicalPage ;
    
    GP_LOG_PRINTF("writePage->", 0) ;
    
    originalPhysicalPage = RESOLVE_TO_PHYSICAL_PAGE(virtualPage);
    
    // select another page to contain the data so that we can recover from power failures
    newPageSelected = selectFreePage(virtualPage) ;

    // target page was already deleted in as part of a previous write operation
    // but if an alternative page could not be allocated, we must delete the currently
    // mapped physical page and try write to it - power failure robustness lost
    if (!newPageSelected)
    {
        GP_LOG_PRINTF("Failed to find alternative page for data, using current page", 0) ;
        
        // physical address of logicalPage did not change because selectFreePage did not succeed
        // so that everything below targets the old page
        Nvm_FlashErasePage(RESOLVE_TO_PHYSICAL_PAGE(virtualPage)) ;
    }

    while (1)
    {
        UInt8 result ;

        result = Nvm_FlashWritePage(RESOLVE_TO_PHYSICAL_PAGE(virtualPage), FLASH_PAGE_SIZE, pageBackup);

        if (result == gpHal_FlashError_Success)
        {
            GP_LOG_PRINTF("Write successful to logical page %d physical page 0x%lx", 0, virtualPage, RESOLVE_TO_PHYSICAL_PAGE(virtualPage));
            
            if (!NVM_REMAP_TABLE_IS_VALID(virtualPage))
            {
                // don't delete old data if there is no such data !
                memoryRangeRecentlyAllocated = 1 ;
            }
            NVM_REMAP_TABLE_SET_VALID(virtualPage);
            break ;
        }
        else if (result == gpHal_FlashError_VerifyFailure)
        {
            disableBrokenPage(RESOLVE_TO_PHYSICAL_PAGE(virtualPage));

            // try to find an alternative page for the data
            newPageSelected = selectFreePage(virtualPage) ;
                
            if (!newPageSelected)
            {
                /* failing to find an alternative page is the end of the call - write failed */

                break ;
            }
                
            // alternative found, make another write attempt
        }
        else
        {
            /* we only replace a page if a verification error occurred */

            GP_LOG_PRINTF("Write unsuccessful, unexpected error", 0) ;
            
            break;
        }
    }
    
    // delete the original page only if an alternative page was allocated
    if (newPageSelected && !memoryRangeRecentlyAllocated)
    {
        // Queue deleted page. We don't want to delete old data until the entire block has been
        // written, to allow recovery from power failure (remapping logical pages to old data if
        // it is determined that the new data is corrupt or incomplete)

        deletedPages[numberOfDeletedPages] = originalPhysicalPage ;
        ++numberOfDeletedPages ;
        
        GP_ASSERT_SYSTEM(numberOfDeletedPages < NVM_NUMBER_PAGES) ;
    }
    
    GP_LOG_PRINTF("<-writePage", 0) ;
}

static void deleteOldData(void)
{
    UIntLoop index ;
    
    GP_LOG_PRINTF("deleteOldData->", 0) ;
    
    // Delete old data - no power failure occurred, fresh copy most likely to be valid
    // Deleting old data is deferred slightly so that it is still available in the event
    // a power failure occurs while writing new data is in progress - we want the old data 
    // to be available upon boot if writing of the new data is not valid
    for (index = 0 ; index < numberOfDeletedPages ; index++)
    {
        GP_LOG_PRINTF("Deleting physical page 0x%lx", 0, deletedPages[index]) ;
        
        Nvm_FlashErasePage(deletedPages[index]);
    }
    
    numberOfDeletedPages = 0 ;
    
    GP_LOG_PRINTF("<-deleteOldData", 0) ;
}

// we regularly reset page counters to prevent page counter rollover which could
// make sure choose the wrong page upon started if we encounter a copy2
// memory range specifier (in case the more recent copy of the data had its
// counter rolled-over to 0, while the old copy, which was not deleted because
// of a power failure, maintains page counter value 2^15-1)
// this function must be executed only if we are 100% certain that a write operation
// (read: block write) has been successful, so that it is safe to remove the links
// embedded in the page counters.
static void considerResettingPageCounters(UInt8 virtualPageResetCounterTrigger)
{
    if (globalNvmPageCounter >= NVM_PAGE_COUNTER_RESET_THRESHOLD)
    {
        UInt8   virtualPage ;
        UInt8   header[NVM_PAGE_HEADER_SIZE] ;
        UInt16 crc ;

        // reset global counter
        globalNvmPageCounter = 0 ;
    
        COMPILE_TIME_ASSERT(sizeof(header) == NVM_PAGE_HEADER_SIZE) ;
        
        // traverse NVM pages, reset copy of global counter per page
        // reset the counter of the page that triggered this procedure last
        // so that we can recover from a power failure in the midst of this procedure
        // by repeating it during the next power up, making sure all counters are 0
        
        // Reset all counters except the page that contains the counter than triggered counter maintenance
        // (resetting that counter is kept for very end) because we want to trigger this
        // procedure again if a power failure occurs while it is busy
        for (virtualPage = 0 ; virtualPage < NVM_NUMBER_PAGES ; virtualPage++)
        {
            if (virtualPage != virtualPageResetCounterTrigger)
            {
                // read entire page
                Nvm_FlashReadPage(RESOLVE_TO_PHYSICAL_PAGE(virtualPage), FLASH_PAGE_SIZE, (UInt8 *)pageBackup) ;
                
                // only reset counter if page is not empty and not damaged
                if ( (MEMCMP( (UInt8 *)pageBackup, pageFreeLabel, sizeof(pageFreeLabel) ) ) &&
                     (MEMCMP( (UInt8 *)pageBackup, pageBrokenLabel, sizeof(pageBrokenLabel) ) ) )
                {
                    // reset counter
                    MEMCPY( (UInt8 *)pageBackup + NVM_PAGE_HEADER_OFFSET_TO_PAGE_COUNTER, &globalNvmPageCounter, NVM_PAGE_HEADER_PAGE_COUNTER_BYTES) ;

                    // All data in page. Now calculate and store CRC - header (excluding CRC field) and data
                    crc = gpUtils_CalculateCrc( (UInt8 *)pageBackup + NVM_PAGE_HEADER_CRC_BYTES, FLASH_PAGE_SIZE - NVM_PAGE_HEADER_CRC_BYTES) ;
                    MEMCPY( (UInt8 *)pageBackup + NVM_PAGE_HEADER_OFFSET_TO_PAGE_CRC, &crc, NVM_PAGE_HEADER_CRC_BYTES) ;

                    // we don't a writePage here because we are unlikely to have enough replacement pages! Note: this
                    // loop traverses all of the Flash.
                    Nvm_FlashWritePage(RESOLVE_TO_PHYSICAL_PAGE(virtualPage), FLASH_PAGE_SIZE, pageBackup);
                    
                    GP_LOG_PRINTF("Virtual page %d counter reset", 0, virtualPage) ;
                }
                else
                {
                    GP_LOG_PRINTF("Virtual page %d free or damaged; ignoring", 0, virtualPage) ;
                }
            }
        }
        
        // Finally, reset counter of the page that triggered the reset
                
        // read page
        Nvm_FlashReadPage(RESOLVE_TO_PHYSICAL_PAGE(virtualPageResetCounterTrigger), FLASH_PAGE_SIZE, (UInt8 *)pageBackup) ;
                
        // reset counter
        MEMCPY( (UInt8 *)pageBackup + NVM_PAGE_HEADER_OFFSET_TO_PAGE_COUNTER, &globalNvmPageCounter, NVM_PAGE_HEADER_PAGE_COUNTER_BYTES) ;

        // All data in page. Now calculate and store CRC - header (excluding CRC field) and data
        crc = gpUtils_CalculateCrc( (UInt8 *)pageBackup + NVM_PAGE_HEADER_CRC_BYTES, FLASH_PAGE_SIZE - NVM_PAGE_HEADER_CRC_BYTES) ;
        MEMCPY( (UInt8 *)pageBackup + NVM_PAGE_HEADER_OFFSET_TO_PAGE_CRC, &crc, NVM_PAGE_HEADER_CRC_BYTES) ;

        Nvm_FlashWritePage(RESOLVE_TO_PHYSICAL_PAGE(virtualPageResetCounterTrigger), FLASH_PAGE_SIZE, pageBackup);
        
        GP_LOG_PRINTF("Virtual page %d (triggered counter reset) counter reset", 0, virtualPageResetCounterTrigger) ;
    }
}

// insert data into page at the right location, update page header and calculate page CRC
gpHal_FlashError_t preparePage(preparePage_t *pageInfo)
{
    gpHal_FlashError_t result = gpHal_FlashError_OutOfRange ;

    GP_LOG_PRINTF("preparePage->", 0);

    if (pageInfo->length)
    {
        UInt16 crc ;
        UInt16 pageCounter ;
        UInt8 memoryRangeSpecifier = pageInfo->virtualPage + 1 ;

        COMPILE_TIME_ASSERT(sizeof(globalNvmPageCounter) == NVM_PAGE_HEADER_PAGE_COUNTER_BYTES) ;
        COMPILE_TIME_ASSERT(sizeof(crc) == NVM_PAGE_HEADER_CRC_BYTES) ;

        // CRC calculation will fail unless CRC property is located at the beginning of page header
        COMPILE_TIME_ASSERT(NVM_PAGE_HEADER_OFFSET_TO_PAGE_CRC == 0) ;
        COMPILE_TIME_ASSERT(NVM_PAGE_HEADER_CRC_BYTES == sizeof(UInt16) ) ;

        FlashPtr offset = pageInfo->address - FLASH_ALIGN_PAGE(pageInfo->address) ;

        *pageInfo->bytesWritten = ( (UInt32)pageInfo->length < FLASH_PAGE_SIZE - offset) ? pageInfo->length : FLASH_PAGE_SIZE - offset;

        // insert data
        MEMCPY((UInt8 *)pageBackup + offset, pageInfo->data, *pageInfo->bytesWritten);

        // update page counter        
        // trigger counter update in case we are about to hit an counter roll-over
        if (++globalNvmPageCounter >= NVM_PAGE_COUNTER_RESET_THRESHOLD)
        {
            pageInfo->virtualPageResetCounterTrigger = pageInfo->virtualPage ;
        }
        
        // Is page linked to next page (due to data sharing) or not ?
        // Does data not fit entirely into the addressed page?
        // If so, enable the link bit so that we can restore correctly
        // from incomplete data storage during next power start-up, assuming
        // an earlier copy of the data exists
        if (offset + pageInfo->length > FLASH_PAGE_SIZE)
        {
            // This must never happen for a write byte operation...!
            pageCounter = NVM_PAGE_ENABLE_LINK(globalNvmPageCounter) ;
            GP_LOG_PRINTF("Page %d is linked to next page", 0, pageInfo->virtualPage);
        }
        else
        {
            pageCounter = globalNvmPageCounter ;
            GP_LOG_PRINTF("Page %d is not linked to next page", 0, pageInfo->virtualPage);
        }
        
        // store in frame
        MEMCPY((UInt8 *)pageBackup + NVM_PAGE_HEADER_OFFSET_TO_PAGE_COUNTER, &pageCounter, NVM_PAGE_HEADER_PAGE_COUNTER_BYTES) ;

        // update memory range
        MEMCPY((UInt8 *)pageBackup + NVM_PAGE_HEADER_OFFSET_TO_PAGE_MEMORY_RANGE_SPECIFIER, &memoryRangeSpecifier, NVM_PAGE_HEADER_MEMORY_RANGE_SPECIFIER_BYTES) ;

        // All data in page. Now calculate and store CRC - header (excluding CRC field) and data
        crc = gpUtils_CalculateCrc( (UInt8 *)pageBackup + NVM_PAGE_HEADER_CRC_BYTES, FLASH_PAGE_SIZE - NVM_PAGE_HEADER_CRC_BYTES) ;
        MEMCPY( (UInt8 *)pageBackup + NVM_PAGE_HEADER_OFFSET_TO_PAGE_CRC, &crc, NVM_PAGE_HEADER_CRC_BYTES) ;
        
        result = gpHal_FlashError_Success ;
        
        GP_LOG_PRINTF("preparePage: memoryRangeSpecifier = %d, counter = %d", 0, memoryRangeSpecifier, pageCounter) ;
    }

    GP_LOG_PRINTF("preparePage<-", 0);

    return result ;
}

static void Nvm_FlashCollectPageInfo(void)
{
    UInt8  physicalPage;
    UInt8  memoryRangeSpecifier;
    UInt16 pageCounter;
    UInt16 crc;

    for (physicalPage = 0 ; physicalPage < NVM_NUMBER_PAGES ; physicalPage++)
    {
        nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].virtualPage = NOT_AVAILABLE;
        nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].pageCounter = 0;

        Nvm_FlashReadPage(physicalPage, FLASH_PAGE_SIZE, (UInt8 *)pageBackup) ;
        
        if ( (MEMCMP(pageBackup, pageFreeLabel, sizeof(pageFreeLabel) ) ) &&
             (MEMCMP(pageBackup, pageBrokenLabel, sizeof(pageBrokenLabel) ) ) )
        {
            MEMCPY(&memoryRangeSpecifier, (UInt8*)pageBackup + NVM_PAGE_HEADER_OFFSET_TO_PAGE_MEMORY_RANGE_SPECIFIER, NVM_PAGE_HEADER_MEMORY_RANGE_SPECIFIER_BYTES) ;
            MEMCPY(&pageCounter, (UInt8*)pageBackup + NVM_PAGE_HEADER_OFFSET_TO_PAGE_COUNTER, NVM_PAGE_HEADER_PAGE_COUNTER_BYTES) ;
            //pageCounter = NVM_PAGE_COUNTER_WITHOUT_LINK(pageCounter) ;
            MEMCPY( &crc, (UInt8 *)pageBackup + NVM_PAGE_HEADER_OFFSET_TO_PAGE_CRC, NVM_PAGE_HEADER_CRC_BYTES) ;

            // calculate page content CRC
            crc -= gpUtils_CalculateCrc( (UInt8 *)pageBackup + NVM_PAGE_HEADER_CRC_BYTES, FLASH_PAGE_SIZE - NVM_PAGE_HEADER_CRC_BYTES) ;

            if ((crc == 0) && (memoryRangeSpecifier))
            {
                nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].virtualPage = memoryRangeSpecifier - 1;
                nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].pageCounter = pageCounter;
                // first, collect reference counts
                nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].virtualPage].refCounter++;
            }

            GP_LOG_SYSTEM_PRINTF("Flash p:%d/%d vp:%d c:%d l:%d r:%d crc:%d",0, physicalPage, NVM_NUMBER_PAGES, memoryRangeSpecifier - 1, NVM_PAGE_COUNTER_WITHOUT_LINK(pageCounter),IS_NVM_PAGE_COUNTER_LINK_ENABLED(pageCounter),
                nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].virtualPage].refCounter,
                crc );
#ifdef GP_DIVERSITY_LOG
            gpLog_Flush();
#endif
        }
    }
}

// it is believed that copy1 is always valid - therefore, there is no reason for check the CRC of pages here as this
// function only processes pages related to copy1
static void resolveNvmTopology(UInt8 *pNumberOfMemoryRangesCopy1, UInt8 *pCopy1StartPhysicalPage, UInt8 *pCopy1StartMemoryRangeSpecifier)
{
    UInt8  physicalPage ;
    UInt8  memoryRangeSpecifier ;
    UInt8  maxSteps ;
    UInt8  duplicateDataFound = 0 ;
    
    UInt16 pageCounter ;
    UInt16 smallestPageCounter = USHRT_MAX ;
    
    GP_LOG_PRINTF("resolveNvmTopology->", 0) ;

    // find beginning of duplicated pages block - first page has the smallest page counter
    // (beginning of copy1)

    // find where the duplicate data starts (copy1 has the smaller counters than copy2)
    // a duplicate memory range has reference counter > 1
    for (physicalPage = 0 ; physicalPage < NVM_NUMBER_PAGES ; physicalPage++)
    {
        if (nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].virtualPage != NOT_AVAILABLE)
        {
            pageCounter = NVM_PAGE_COUNTER_WITHOUT_LINK(nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].pageCounter) ;
            // duplicate data?
            if (nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].virtualPage].refCounter > 1)
            {
                duplicateDataFound = 1 ;
                if (smallestPageCounter > pageCounter)
                {
                    smallestPageCounter = pageCounter ;
                    *pCopy1StartPhysicalPage = physicalPage ;
                    *pCopy1StartMemoryRangeSpecifier = nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].virtualPage + 1 ;
                }
            }
        }
    }
    
    if (duplicateDataFound)
    {
        *pNumberOfMemoryRangesCopy1 = 1 ;

        // measure the length of the copy1 section (number of pages we expect to find in copy2 as well to consider it as a
        // replacement for copy1)
        
        if (*pCopy1StartPhysicalPage + 1 >= NVM_NUMBER_PAGES)
        {
            // we use FLASH_ADDRESS so logicalPage is translated into a physical address.
            // since a physical page is not strongly related to the address range it
            // represents, we must consider going back to the beginning looking for
            // what we're after.
            physicalPage = 0 ;
        }
        else
        {
            physicalPage = *pCopy1StartPhysicalPage + 1 ;
        }
        
        maxSteps = 0 ;
        
        // the page counter of the first page of copy1
        pageCounter = nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[*pCopy1StartPhysicalPage].pageCounter;
        memoryRangeSpecifier = *pCopy1StartMemoryRangeSpecifier ;
        
        while (1)
        {
            // search for the next page in the block by comparing page counter (larger by 1) and reference counter (=2)
            
            // Remember: logicalPage is initialized to have the value of the first
            // logical page of the after first page of the block of pages (or a single page)
            // that has more than one reference to them, and that is why it is safe to scan from 
            // it on, looking for the end of the block of duplicate pages
            
            // we always scan the entire NVM, because pages can be placed anywhere.
            // at this stage, logicalPageCopy1 and logicalPageCopy2 are not known (we
            // try to determine them after resolveNvmTopology returns), so we must rely
            // on what we do know: the page counter and the memory range specifier
            // of each page. logicalPageCopy1 and logicalPageCopy2 are used later to
            // delete duplicate data before the final, remaining set is scanned, logical
            // pages are remapped and the controller allowed to continue with its start-up.
            if (nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].virtualPage != NOT_AVAILABLE)
            {
                UInt16 temporalPageCounter ;
                UInt8  temporalMemoryRangeSpecifier ;

                temporalMemoryRangeSpecifier = nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].virtualPage + 1;
                temporalPageCounter = nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].pageCounter;

                if(memoryRangeSpecifier + 1 == temporalMemoryRangeSpecifier)
                {
                    if (NVM_PAGE_COUNTER_WITHOUT_LINK(pageCounter) + 1 == NVM_PAGE_COUNTER_WITHOUT_LINK(temporalPageCounter))
                    {
                        GP_LOG_SYSTEM_PRINTF("PC:0x%04x",0, pageCounter);
                        if(IS_NVM_PAGE_COUNTER_LINK_ENABLED(pageCounter))
                        {
                            // this must have been the following page

                            // increment copy1 length
                            *pNumberOfMemoryRangesCopy1 += 1 ;

                            // prepare the expect memory range and page counter of the
                            // next page in the block, if available

                            pageCounter = temporalPageCounter ;
                            memoryRangeSpecifier = temporalMemoryRangeSpecifier ;
                        }
                    }
                }
            }
            
            if (++physicalPage >= NVM_NUMBER_PAGES)
            {
                // same reasoning here. see above comment.
                physicalPage = 0 ;
            }

            if (++maxSteps >= NVM_NUMBER_PAGES)
            {
                break ;
            }
        }
    }
    
    GP_LOG_PRINTF("<-resolveNvmTopology", 0) ;
}

// A NVM turns invalid when a broken page cannot be replaced (replacing a broken page requires having at least one page
// with a magic word and a memory range specifier = 0
static void determineNvmOperationalStatus(void)
{
    UIntLoop physicalPage ;

    UInt8  memoryRangeSpecifier ; // memoryRangeSpecifier starts at 1
    UInt8  firstPhysicalPageCopy1 = 0 ;
    UInt8  firstPhysicalPageCopy2 = 0;
    UInt8  firstMemoryRangeSpecifierCopy1 = 0 ;
    UInt8  numberOfMemoryRangesCopy1 = 0 ;
    UInt8  maxSteps = 0 ;
    
    UInt16 crc ;
    UInt16 pageCounter ;
    
    Bool copy2Valid;

    GP_LOG_PRINTF("determineNvmOperationalStatus->", 0) ;

    COMPILE_TIME_ASSERT(sizeof(globalNvmPageCounter) == NVM_PAGE_HEADER_PAGE_COUNTER_BYTES) ;
    COMPILE_TIME_ASSERT(sizeof(memoryRangeSpecifier) == NVM_PAGE_HEADER_MEMORY_RANGE_SPECIFIER_BYTES) ;
    COMPILE_TIME_ASSERT(sizeof(crc) == NVM_PAGE_HEADER_CRC_BYTES) ;
    COMPILE_TIME_ASSERT(sizeof(pageCounter) == NVM_PAGE_HEADER_PAGE_COUNTER_BYTES) ;
    
    MEMSET(nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo, 0, sizeof(nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo) ) ;
    
    // We start by dealing of some NVM topology related issues involving double references to equal memory ranges upon start-up,
    // if deletion of old data was either not complete or performed:
    // |X|-------|Y|Y|X|
    // |X|Y|Y|---|X|
    // |X|X|-----|Y|Y|Y|X|
    // |X|Y|-----|X|Y|
    // etc.
    // "X" or "Y" indicate correspondence with copy1 ("X") or copy2 ("Y")
    // All those cases include a "roll-over" - data starts actually at the end of the NVM (thus, on the right-hand side)
    // and continues at the beginning. So in order to correctly collect data (that is, associating the right pages with copy1
    // and copy2 WITHOUT mixing them!) we must start processing where copy1 starts.
    // this situation is the result of an memory range being decoupled from a particular physical page, thus any
    // address range can end up everywhere.
    
    // this call only has impact on situations when there is duplicate data in NVM and it is spread across 
    // NVM end and start (see above)
    
    Nvm_FlashCollectPageInfo();

    resolveNvmTopology(&numberOfMemoryRangesCopy1, &firstPhysicalPageCopy1, &firstMemoryRangeSpecifierCopy1) ;

    UIntLoop virtualPage;
    UInt8 duplicateLength = 0;
    for(virtualPage = 0; virtualPage < NVM_NUMBER_PAGES; virtualPage++)
    {
        nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[virtualPage].physicalPageCopy1 = NOT_AVAILABLE ;
        nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[virtualPage].physicalPageCopy2 = NOT_AVAILABLE ;
        if (nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[virtualPage].refCounter == 2)
        {
            UIntLoop physicalPage;
            UInt8    physPage1 = NOT_AVAILABLE;
            UInt8    physPage2 = NOT_AVAILABLE;
            UInt8    pageCounter1;
            UInt8    pageCounter2;
            for(physicalPage = 0; physicalPage < NVM_NUMBER_PAGES; physicalPage++)
            {
                if(nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].virtualPage == virtualPage)
                {
                    if(physPage1 == NOT_AVAILABLE)
                    {
                        physPage1 = physicalPage ;
                        pageCounter1 = nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].pageCounter;
                    }
                    else if (physPage2 == NOT_AVAILABLE)
                    {
                        physPage2 = physicalPage ;
                        pageCounter2 = nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].pageCounter;
                    }
                    else
                    {
                        GP_ASSERT_DEV_INT(false);
                    }
                }
            }
            if(NVM_PAGE_COUNTER_WITHOUT_LINK(pageCounter1) < NVM_PAGE_COUNTER_WITHOUT_LINK(pageCounter2))
            {
                nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[virtualPage].physicalPageCopy1 = physPage1;
                nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[virtualPage].physicalPageCopy2 = physPage2;
            }
            else
            {
                nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[virtualPage].physicalPageCopy1 = physPage2;
                nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[virtualPage].physicalPageCopy2 = physPage1;
            }
            GP_LOG_PRINTF("Duplicate for virt page %d : %d and %d",0, virtualPage, nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[virtualPage].physicalPageCopy1, nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[virtualPage].physicalPageCopy2);
            duplicateLength++;
        }
        if (nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[virtualPage].refCounter == 1)
        {
            UIntLoop physicalPage;
            for(physicalPage = 0; physicalPage < NVM_NUMBER_PAGES; physicalPage++)
            {
                if(nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].virtualPage == virtualPage)
                {
                    nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[virtualPage].physicalPageCopy1 = physicalPage;
                    nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[virtualPage].physicalPageCopy2 = NOT_AVAILABLE;
                }
            }
            GP_LOG_PRINTF("Non-Duplicate for virt page %d : %d",0, virtualPage, nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[virtualPage].physicalPageCopy1);
        }
    }

    for (physicalPage = 0 ; physicalPage < NVM_NUMBER_PAGES ; physicalPage++)
    {
        if( (nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].virtualPage + 1 ) == firstMemoryRangeSpecifierCopy1)
        {
            // it's start of the range
            if(physicalPage != firstPhysicalPageCopy1)
            {
                // this is the second copy
                GP_ASSERT_DEV_INT(NVM_PAGE_COUNTER_WITHOUT_LINK(nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].pageCounter) > NVM_PAGE_COUNTER_WITHOUT_LINK(nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[firstPhysicalPageCopy1].pageCounter));
                GP_ASSERT_DEV_INT (nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].virtualPage].refCounter > 1);
                firstPhysicalPageCopy2 = physicalPage;
            }
        }
    }

    // working assumption: if there is a duplicity in records, the first copy ("copy1") is
    // always valid, and the secondary copy ("copy2") could be preferred only based upon 
    // its page counters being all bigger than their respective counterparts ("copy1").
    // blocks in copy2 can be independent of one another.

    if(duplicateLength > 0)
    {
        if (firstPhysicalPageCopy2 + 1 >= NVM_NUMBER_PAGES)
        {
            // we use FLASH_ADDRESS so logicalPage is translated into a physical address.
            // since a physical page is not strongly related to the address range it
            // represents, we must consider going back to the beginning looking for
            // what we're after.
            physicalPage = 0 ;
        }
        else
        {
            physicalPage = firstPhysicalPageCopy2 + 1 ;
        }
        
        maxSteps = 0 ;
        
        // the page counter of the first page of copy2
        pageCounter = nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[firstPhysicalPageCopy2].pageCounter;
        memoryRangeSpecifier = firstMemoryRangeSpecifierCopy1 ; // same as in copy2 because it's duplicate.

        copy2Valid = true;
        if(IS_NVM_PAGE_COUNTER_LINK_ENABLED(pageCounter))
        {
            copy2Valid = false;
        }

        while (1)
        {
            if (nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].virtualPage != NOT_AVAILABLE)
            {
                UInt16 temporalPageCounter ;
                UInt8  temporalMemoryRangeSpecifier ;

                temporalMemoryRangeSpecifier = nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].virtualPage + 1;
                temporalPageCounter = nvmFlashGlobal.nvmFlashBootVariables.pageInfoSummary[physicalPage].pageCounter;
        
                if(memoryRangeSpecifier + 1 == temporalMemoryRangeSpecifier)
                {
                    if (NVM_PAGE_COUNTER_WITHOUT_LINK(pageCounter) + 1 == NVM_PAGE_COUNTER_WITHOUT_LINK(temporalPageCounter))
                    {
                        GP_LOG_PRINTF("PC:0x%04x",0, pageCounter);
                        copy2Valid = true;
                        if(IS_NVM_PAGE_COUNTER_LINK_ENABLED(temporalPageCounter))
                        {
                            copy2Valid = false;
                        }
                        // this must have been the following page

                        // prepare the expect memory range and page counter of the
                        // next page in the block, if available
        
                        pageCounter = temporalPageCounter ;
                        memoryRangeSpecifier = temporalMemoryRangeSpecifier ;
                    }
                }
            }
            
            if (++physicalPage >= NVM_NUMBER_PAGES)
            {
                // same reasoning here. see above comment.
                physicalPage = 0 ;
            }
        
            if (++maxSteps >= NVM_NUMBER_PAGES)
            {
                break ;
            }
        }

        // if there are no memory range reference duplications, nothing went wrong during the last power down
        // and we can start-up normally. if there is a duplication, a write operation did not complete entirely - 
        // either old data was not deleted or not all the data was written. we need to select the new data (if complete)
        // or the old data that was potentially not deleted

        UInt8 memoryRangeSpecifier;
        memoryRangeSpecifier = firstMemoryRangeSpecifierCopy1 - 1 ;
        maxSteps = 0 ;

        if (!copy2Valid)
        {
            GP_LOG_PRINTF("Using copy 1", 0) ;
            // use copy 1 and delete copy 2
            while (1)
            {
                // this is the memory range that has a double reference
                // this is the logical page where copy1 starts
                // erase the page
                GP_LOG_SYSTEM_PRINTF("copy2 %d vs %d",0, firstPhysicalPageCopy2, nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[memoryRangeSpecifier].physicalPageCopy2);
                Nvm_FlashErasePage(nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[memoryRangeSpecifier].physicalPageCopy2) ;
                
                if (++maxSteps >= duplicateLength)
                {
                    break ;
                }
                
                // move on next memory range
                if (++memoryRangeSpecifier >= NVM_NUMBER_PAGES)
                {
                    // logicalPage is used here as a memoryRangeSpecifier - cannot exceed NVM memory space
                    GP_ASSERT_SYSTEM(false) ;
                }
            }
        }
        else
        {
            GP_LOG_PRINTF("Using copy 2", 0) ;
            // use copy 2 and delete copy 1
            while (1)
            {
                // this is the memory range that has a double reference
                // there could be gaps in the memory range - prevent damage to non-related data
                // this is the logical page where copy1 starts
                // erase the page
                GP_LOG_SYSTEM_PRINTF("copy1 %d vs %d",0, firstPhysicalPageCopy1, nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[memoryRangeSpecifier].physicalPageCopy1);
                Nvm_FlashErasePage(nvmFlashGlobal.nvmFlashBootVariables.memoryRangeInfo[memoryRangeSpecifier].physicalPageCopy1);
                
                if (++maxSteps >= duplicateLength)
                {
                    break ;
                }
                
                // move on next memory range
                if (++memoryRangeSpecifier >= NVM_NUMBER_PAGES)
                {
                    // logicalPage is used here as a memoryRangeSpecifier - cannot exceed NVM memory space
                    GP_ASSERT_SYSTEM(false) ;
                }
            }
        }
    }
    else
    {
        GP_LOG_PRINTF("No memory range duplications found. Starting up normally.", 0) ;
    }

    // now that we got rid of duplicate pages (if any), we can do the following:
    // 1. the value of globalNvmPageCounter
    // 2. remap address ranges
    
    // erase old mapping, duplicate pages may have been dropped
    MEMSET(nvmRemapTable, 0, sizeof(nvmRemapTable)) ;
    MEMSET(isNvmRemapTableValid, 0, sizeof(isNvmRemapTableValid)) ;
    globalNvmPageCounter = 0 ;
    
    for (physicalPage = 0 ; physicalPage < NVM_NUMBER_PAGES ; physicalPage++)
    {
        Nvm_FlashReadPage(physicalPage, FLASH_PAGE_SIZE, (UInt8 *)pageBackup) ;
        
        if (MEMCMP( (UInt8 *)pageBackup, pageFreeLabel, sizeof(pageFreeLabel) ) )
        {
            // page header CRC
            MEMCPY( &crc, (UInt8 *)pageBackup + NVM_PAGE_HEADER_OFFSET_TO_PAGE_CRC, NVM_PAGE_HEADER_CRC_BYTES) ;

            // calculate page content CRC
            crc -= gpUtils_CalculateCrc( (UInt8 *)pageBackup + NVM_PAGE_HEADER_CRC_BYTES, FLASH_PAGE_SIZE - NVM_PAGE_HEADER_CRC_BYTES) ;
            
            if (crc == 0)
            {
                // remap page
                MEMCPY(&memoryRangeSpecifier, (UInt8 *)pageBackup + NVM_PAGE_HEADER_OFFSET_TO_PAGE_MEMORY_RANGE_SPECIFIER, NVM_PAGE_HEADER_MEMORY_RANGE_SPECIFIER_BYTES) ;
                nvmRemapTable[memoryRangeSpecifier - 1] = physicalPage;

                NVM_REMAP_TABLE_SET_VALID(memoryRangeSpecifier - 1);
                
                // global page counter
                MEMCPY(&pageCounter, (UInt8 *)pageBackup + NVM_PAGE_HEADER_OFFSET_TO_PAGE_COUNTER, NVM_PAGE_HEADER_PAGE_COUNTER_BYTES) ;
                
                // the global page counter should be bigger than the biggest page counter available
                if (NVM_PAGE_COUNTER_WITHOUT_LINK(globalNvmPageCounter) < NVM_PAGE_COUNTER_WITHOUT_LINK(pageCounter) )
                {
                    globalNvmPageCounter = NVM_PAGE_COUNTER_WITHOUT_LINK(pageCounter) ;
                }
                GP_LOG_PRINTF("nvmRemapTable[%d]=0x%x", 0, physicalPage, nvmRemapTable[memoryRangeSpecifier - 1]) ;
            }
            else
            {
                GP_LOG_SYSTEM_PRINTF("CRC fail on page %d", 0, physicalPage);
            }
        }
    }

    GP_LOG_PRINTF("determineNvmOperationalStatus<-", 0);
}
 
/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

extern UInt32 gpNvm_PagedStart;

// Allow for a changing NVM start address depending on the Flash size, as defined
// in the linker file
// (indirect storage of 32bit addr in a global variable)
__asm__ ("    .global gpNvm_PagedStart");
__asm__ ("    .rodata");
__asm__ ("gpNvm_PagedStart:");
__asm__ ("    .long  gpNvm_Start");

void Nvm_Init(void)
{
    GP_LOG_PRINTF("Nvm_Init->", 0);

    COMPILE_TIME_ASSERT(sizeof(pageFreeLabel) == NVM_PAGE_HEADER_SIZE) ;
    
    // make sure NVM size is a multiple of flash page size, otherwise error
    COMPILE_TIME_ASSERT(NVM_SIZE % FLASH_PAGE_SIZE == 0) ;
    
    nvmFlashBase = gpNvm_PagedStart ;
    
    GP_LOG_PRINTF("0x%lx 0x%lx", 0, nvmFlashBase, NVM_SIZE) ;
    
    MEMSET(nvmRemapTable, 0, sizeof(nvmRemapTable)) ;
    
    globalNvmPageCounter = 0 ;
    
    freePageSearchStart = 0 ;
    
    numberOfDeletedPages = 0 ;
    
    // configure the NVM flash region for first use if no magic words exist in Flash
    determineNvmOperationalStatus() ;

    GP_LOG_PRINTF("Nvm_Init<-", 0);
}

UInt16 Nvm_GetMaxSize(void)
{
    return (NVM_SIZE - NVM_NUMBER_PAGES * NVM_PAGE_HEADER_SIZE);
}

void Nvm_Flush(void)
{
}

void Nvm_WriteByte(UIntPtr address, UInt8 value, gpNvm_UpdateFrequency_t updateFrequency)
{
    GP_LOG_PRINTF("Nvm_WriteByte->", 0);
    (void)Nvm_WriteBlock(address, 1, &value, updateFrequency);
    GP_LOG_PRINTF("Nvm_WriteByte<-", 0);
}

UInt8 Nvm_ReadByte(UIntPtr address, gpNvm_UpdateFrequency_t updateFrequency)
{
    UInt8 value;
    
    GP_LOG_PRINTF("Nvm_ReadByte->", 0);
    Nvm_ReadBlock(address, 1, &value, updateFrequency);
    GP_LOG_PRINTF("Nvm_ReadByte<-[%d]=0x%lx", 0, address, value) ;
    
    return value ;
}

Bool Nvm_WriteBlock(UIntPtr address, UInt16 lengthBlock, UInt8* data, gpNvm_UpdateFrequency_t updateFrequency)
{
    UInt8 bytesWritten ;
    preparePage_t pageInfo ;

    gpHal_FlashError_t result = gpHal_FlashError_UnalignedAddress;
    
    Int16  length = (Int16)lengthBlock;
        
    GP_LOG_PRINTF("Nvm_WriteBlock->", 0);

    GP_ASSERT_DEV_INT( (address + lengthBlock) <= NVM_SIZE) ;
    
    if ( (address + lengthBlock) <= NVM_SIZE)
    {
        if (data && (length > 0) )
        {
            UInt8 numberOfPrecedingHeaders ;
            UInt8 virtualPage ;

            GP_LOG_PRINTF("Nvm_WriteBlock device offset %d, %d, 0x%lx", 0, address, length, data);
            
            numberOfPrecedingHeaders = NUMBER_OF_PRECEDING_HEADERS(address);

            address += NVM_PAGE_HEADER_SIZE * numberOfPrecedingHeaders;
            
            virtualPage = numberOfPrecedingHeaders - 1 ;
            
            while (length > 0)
            {
                gpHal_FlashError_t halResult;

                if (NVM_REMAP_TABLE_IS_VALID(virtualPage))
                {
                    // read page before we write it, because we need to transfer all the data into a new page in case of damage.
                    // we must separate reading and writing, so that we can deal with page failure!
                    halResult = Nvm_FlashReadPage(RESOLVE_TO_PHYSICAL_PAGE(virtualPage), FLASH_PAGE_SIZE, (UInt8 *)pageBackup);
                }
                else
                {
                    // if the addressed page is not mapped yet, we don't want to load data that might be referenced by another logical
                    // page! remember that data swirls around the NVM.
                    MEMSET( (UInt8 *)pageBackup, -1, sizeof(pageBackup) ) ;
                    halResult = gpHal_FlashError_Success ;
                }
                
                if (halResult == gpHal_FlashError_Success)
                {
                    // insert data into current page content and return the number of bytes until page boundary
                    pageInfo.address = address ;
                    pageInfo.data = data ;
                    pageInfo.length = length ;
                    pageInfo.bytesWritten = &bytesWritten ;
                    pageInfo.virtualPage = virtualPage ;

                    if (preparePage(&pageInfo) == gpHal_FlashError_Success)
                    {            
                        // Attempt to write the data to the chip as long as a failure in the write
                        // operation occurs, or we run out of free pages.
                        writePage(virtualPage) ;
                        
                        length -= *pageInfo.bytesWritten ;

                        address += *pageInfo.bytesWritten ;

                        // Page full ?
                        if ((address % FLASH_PAGE_SIZE) == 0)
                        {
                            address += NVM_PAGE_HEADER_SIZE;
                        }

                        data += *pageInfo.bytesWritten ;
                    }
                }
                else
                {
                    GP_LOG_SYSTEM_PRINTF("Nvm_WriteBlock: gpHal_FlashRead failure", 0);
                }

                ++virtualPage ;
            }

            result = gpHal_FlashError_Success;
        }
    }
    else
    {
        GP_LOG_SYSTEM_PRINTF("Nvm_WriteBlock: Invalid address", 0) ;        
    }
    
    deleteOldData() ;
    
    // now, it is safe to remove links embedded into page counters - write complete, no power failure occurred
    considerResettingPageCounters(pageInfo.virtualPageResetCounterTrigger) ;
    
    GP_LOG_PRINTF("Nvm_WriteBlock<-", 0);

    return (result == gpHal_FlashError_Success) ;
}

void Nvm_ReadBlock(UIntPtr address, UInt16 lengthBlock, UInt8* data, gpNvm_UpdateFrequency_t updateFrequency)
{
    UInt8 bytesRead = 0 ;
    UInt8 numberOfPrecedingHeaders ;
    UInt8 virtualPage ;
    
    Int16 length = (Int16)lengthBlock;

    GP_LOG_PRINTF("Nvm_ReadBlock->", 0);

    GP_LOG_PRINTF("Nvm_ReadBlock device offset %d %d %lx", 0, address, length, data);

    GP_ASSERT_DEV_INT( (address + lengthBlock) <= NVM_SIZE) ;
    
    if ( (address + lengthBlock) <= NVM_SIZE)
    {
        numberOfPrecedingHeaders = NUMBER_OF_PRECEDING_HEADERS(address) ;

        address += NVM_PAGE_HEADER_SIZE * numberOfPrecedingHeaders;

        virtualPage = numberOfPrecedingHeaders - 1 ;

        if (NVM_REMAP_TABLE_IS_VALID(virtualPage))
        {
            while (length > 0)
            {
                gpHal_FlashError_t halResult;
                
                halResult = Nvm_FlashReadPage(RESOLVE_TO_PHYSICAL_PAGE(virtualPage), FLASH_PAGE_SIZE, (UInt8 *)pageBackup);
                
                ++virtualPage ;

                if (halResult == gpHal_FlashError_Success)
                {
                    // if the number of bytes between the beginning of the data in the page (address) and end of the page
                    // is smaller than the number of bytes requested, we need to set it to the number of bytes read by
                    // less than a page, because there is more data to read (thus, set to the number of bytes until
                    // the end of the page).

                    if (FLASH_PAGE_SIZE - (address % FLASH_PAGE_SIZE) < (UInt32)length)
                    {
                        // we need to read more after this page is processed
                        bytesRead = FLASH_PAGE_SIZE - (address % FLASH_PAGE_SIZE);
                    }
                    else
                    {
                        // done reading after this iteration
                        bytesRead = (UInt8)length;
                    }

                    // move to user buffer
                    MEMCPY(data, (UInt8 *)pageBackup + (address % FLASH_PAGE_SIZE), bytesRead);

                    length -= bytesRead;

                    address += bytesRead;

                    // move on to next page
                    if ((address % FLASH_PAGE_SIZE) == 0)
                    {
                        address += NVM_PAGE_HEADER_SIZE;
                    }

                    data += bytesRead;

                    GP_LOG_PRINTF("Nvm_ReadBlock: read %d bytes, remaining %d bytes", 0, bytesRead, length);
                }
                else
                {
                    GP_LOG_SYSTEM_PRINTF("Nvm_ReadBlock: gpHal_FlashRead failed", 0);
                    goto Nvm_ReadBlockFail;
                    break;
                }
            }
        }
        else
        {
            // if the call has no side effects, we should clear the caller's 
            // buffer to reflect that the page is, in effect, empty
            goto Nvm_ReadBlockFail;
        }
    }
    else
    {
        GP_LOG_SYSTEM_PRINTF("Nvm_ReadBlock: Invalid address", 0) ;
        goto Nvm_ReadBlockFail;
    }
    
    GP_LOG_PRINTF("Nvm_ReadBlock<-", 0);
    return;

Nvm_ReadBlockFail:
     MEMSET(data, -1, lengthBlock) ;
     return;
}

Bool Nvm_CheckAccessible(void)
{
    return true;
}

void Nvm_Erase(void)
{
    UIntLoop i;
    
    GP_LOG_PRINTF("Nvm_Erase->", 0);

    for (i = 0; i < NVM_NUMBER_PAGES ; ++i)
    {
        Nvm_FlashErasePage(i) ;
    }

    GP_LOG_PRINTF("Nvm_Erase<-", 0);
}
