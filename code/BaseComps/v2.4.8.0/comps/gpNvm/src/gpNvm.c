/*
 * Copyright (c) 2009-2014, GreenPeak Technologies
 *
 * This file gives an implementation of the Non Volatile Memory component
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpNvm/src/gpNvm.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */


/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
//#define GP_LOCAL_LOG

#include "global.h"
#include "hal.h"
#include "gpSched.h"
#include "gpLog.h"
#include "gpAssert.h"
#include "gpUtils.h" //CRC calculations

#include "gpNvm.h"
#include "gpNvm_defs.h"





/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_NVM

#ifndef GP_NVM_VERSION
#define GP_NVM_VERSION 1
#endif

#ifndef GP_NVM_NBR_OF_SECTIONS
#error error: GP_NVM_NBR_OF_SECTIONS should defined in the make environment
#endif


#define NVM_CRC_SIZE_IN_BYTES 0

#define NVM_SHIFT_SECTION_SIZE 0


#define NVM_MAX_WRITE_ATTEMPTS 3
#define NVM_MAX_READ_ATTEMPTS 3

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

gpNvm_Section_t gpNvm_Sections[GP_NVM_NBR_OF_SECTIONS];

Bool gpNvm_Locked;
gpNvm_VersionCrc_t gpNvm_VersionCrc;

#define NVM_TAG_NVMVERSION 0
const gpNvm_Tag_t ROM gpNvm_NvmSection[] FLASH_PROGMEM = {
    {(UInt8*)&(gpNvm_VersionCrc), sizeof(gpNvm_VersionCrc_t), gpNvm_UpdateFrequencyInitOnly, NULL},
};

#define gpNvm_RemoteNvmSize  0




/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static UIntPtr Nvm_GetTagBaseAddr(UInt8 sectionId, UInt8 tagId);
static Bool Nvm_WriteBlockWithRetries(UIntPtr nvmAddress, UInt16 length, UInt8* txBuffer, gpNvm_UpdateFrequency_t updateFrequency);



/*****************************************************************************
 *                    Internal Function Definitions
 *****************************************************************************/

UInt8 Nvm_GetSectionId(UInt8 componentId)
{
    UIntLoop   i;
    for(i=0; i < GP_NVM_NBR_OF_SECTIONS; i++)
    {
        if(gpNvm_Sections[i].componentId == componentId)
        {
            return i;
        }
    }
    GP_ASSERT_DEV_EXT(false);
    return 0xFF;
}

UInt16 Nvm_GetSectionSize(UInt8 sectionId)
{
    UIntLoop i;
    UInt16 size = 0;
    gpNvm_Tag_t pTag[1];
    for(i=0; i < gpNvm_Sections[sectionId].nbrOfTags; i++)
    {
        MEMCPY_P(pTag,&(gpNvm_Sections[sectionId].tags[i]),sizeof(gpNvm_Tag_t));
        size += (pTag->size + NVM_CRC_SIZE_IN_BYTES);
    }
    return size;
}

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

static UIntPtr Nvm_GetTagBaseAddr(UInt8 sectionId, UInt8 tagId)
{
    gpNvm_Tag_t pTag[1];
    UIntLoop   i;
    UIntPtr nvmAddr = (UIntPtr)gpNvm_Sections[sectionId].baseAddr;
    for(i=0; i < tagId; i++)
    {
        MEMCPY_P(pTag,&(gpNvm_Sections[sectionId].tags[i]),sizeof(gpNvm_Tag_t));
        nvmAddr += (pTag->size + NVM_CRC_SIZE_IN_BYTES);
    }
    return nvmAddr;
}

static Bool Nvm_WriteBlockWithRetries(UIntPtr nvmAddress, UInt16 length, UInt8* txBuffer, gpNvm_UpdateFrequency_t updateFrequency)
{
    UIntLoop i;
    for (i = 0; i < NVM_MAX_WRITE_ATTEMPTS; i++)
    {
        //GP_LOG_SYSTEM_PRINTF("WA %u addr:%x l:%i",0,(UInt16)i, (UInt16)nvmAddress, (UInt16)length);
        if (Nvm_WriteBlock(nvmAddress, length, txBuffer, updateFrequency))
        {
            return true;
        }
    }
    return false;
}







/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/


void gpNvm_Init(void)
{
    UIntLoop i;
    Nvm_Init();

    for(i=0; i < GP_NVM_NBR_OF_SECTIONS; i++)
    {
        gpNvm_Sections[i].tags = NULL;
    }

    gpNvm_Locked = true;

    gpNvm_VersionCrc = GP_NVM_VERSION;
    gpNvm_RegisterSection(GP_COMPONENT_ID, gpNvm_NvmSection, sizeof(gpNvm_NvmSection)/sizeof(gpNvm_Tag_t),NULL);




    gpSched_ScheduleEvent(0,gpNvm_CheckConsistency);
}

void gpNvm_DeInit(void)
{
}


Bool gpNvm_CheckAccessible(void)
{
    return Nvm_CheckAccessible();
}

void gpNvm_ClearNvm(void)
{

    if(!gpNvm_Locked)
    {
        //Erase the flash structure
        Nvm_Erase();
    }
    //clear all registerd sections
    gpNvm_Clear(gpNvm_AllComponents,gpNvm_AllTags);

    //update version field
    gpNvm_Backup(GP_COMPONENT_ID, NVM_TAG_NVMVERSION,NULL);

    gpNvm_Flush();
}

void gpNvm_Flush(void)
{
    Nvm_Flush();
}

void gpNvm_CheckConsistency(void)
{
    UIntLoop i;

    //check version-crc field
    gpNvm_VersionCrc_t versionCrc_stored = gpNvm_VersionCrc;
    gpNvm_Restore(GP_COMPONENT_ID, NVM_TAG_NVMVERSION, (UInt8*)&versionCrc_stored);

    if(MEMCMP(&versionCrc_stored,&gpNvm_VersionCrc,sizeof(gpNvm_VersionCrc_t)))
    {
        {
            goto gpNvm_CheckConsistency_fail;
        }
    }
    {
        UInt16 totalSize = 0;
        for(i=0; i < GP_NVM_NBR_OF_SECTIONS; i++)
        {
            totalSize += (Nvm_GetSectionSize(i) << NVM_SHIFT_SECTION_SIZE);
        }
        if(totalSize > (Nvm_GetMaxSize() - gpNvm_RemoteNvmSize))
        {
            GP_LOG_SYSTEM_PRINTF("ERROR: too many NVM required %i > %i",0,totalSize,(Nvm_GetMaxSize() - gpNvm_RemoteNvmSize));
            goto gpNvm_CheckConsistency_fail;
        }
        else
        {
            GP_LOG_PRINTF("In use:%i bytes",0,totalSize);
        }
    }

    //call registered consistency functions
    for(i=0; i < GP_NVM_NBR_OF_SECTIONS; i++)
    {
        if((gpNvm_Sections[i].tags !=0) && (gpNvm_Sections[i].cbCheckConsistency!=NULL))
        {
            if(!gpNvm_Sections[i].cbCheckConsistency())
            {
                goto gpNvm_CheckConsistency_fail;
            }
        }
    }

    return;

gpNvm_CheckConsistency_fail:
    //Put NVM to default state
    gpNvm_ClearNvm();
    gpNvm_cbFailedCheckConsistency();

}

void gpNvm_RegisterSection(UInt8 componentId, const ROM gpNvm_Tag_t *tags, UInt8 nbrOfTags, gpNvm_cbCheckConsistency_t cbCheckConsistency)
{
    UIntLoop i;

    GP_ASSERT_DEV_EXT(tags != NULL);

    for(i=0; i < GP_NVM_NBR_OF_SECTIONS; i++)
    {
        if(gpNvm_Sections[i].tags == tags)
        {
            // Allow re-registration
            return;
        }
        if(gpNvm_Sections[i].tags == NULL)
        {
            gpNvm_Sections[i].componentId        = componentId;
            gpNvm_Sections[i].tags               = tags;
            gpNvm_Sections[i].nbrOfTags          = nbrOfTags;
            gpNvm_Sections[i].cbCheckConsistency = cbCheckConsistency;

            if(i==0) //First section that's registered
            {
                gpNvm_Sections[i].baseAddr = (UInt8*)gpNvm_NvmBaseAddr;
            }
            else
            {
                gpNvm_Sections[i].baseAddr = gpNvm_Sections[i-1].baseAddr + (Nvm_GetSectionSize(i-1) << NVM_SHIFT_SECTION_SIZE);
            }
            GP_LOG_PRINTF("REG %i:%i -> %i @0x%x",8,(UInt16)componentId,(UInt16)i,(UInt16)nbrOfTags,(UInt16)gpNvm_Sections[i].baseAddr);


            //update version-crc field
            {
                UInt8 pVersionInfo[4];
                UInt16 sectionSize = (Nvm_GetSectionSize(i) << NVM_SHIFT_SECTION_SIZE);
                pVersionInfo[0] = componentId;
                pVersionInfo[1] = nbrOfTags;
                MEMCPY(&pVersionInfo[2], &sectionSize, 2);
                gpUtils_CalculatePartialCrc(&gpNvm_VersionCrc, pVersionInfo, 4);
            }
            return;
        }
    }
    //remark: as this method will be called from the init-calltree, there will no assert message be printed
    GP_ASSERT_DEV_EXT(false);
}

void gpNvm_Backup(UInt8 componentId, UInt8 tagId, UInt8* pRamLocation)
{
    //GP_LOG_SYSTEM_PRINTF("Backup %u %u 0x%x", 6, (UInt16)componentId, (UInt16)tagId, (UInt16)pRamLocation);
    UInt8 i,j;
    Bool SectionFound = false;

    
    //don't allow access to NVM before the consistency check (exception
    //for gpReset, reset reason is cleared during stack initialisation)
    if(componentId != GP_COMPONENT_ID_RESET)
    {
        GP_ASSERT_DEV_EXT(!gpSched_ExistsEvent(gpNvm_CheckConsistency));
    }
    if (tagId == gpNvm_AllTags && componentId == gpNvm_AllComponents)
    {
        SectionFound = true;
    }
    if (tagId == gpNvm_AllTags || componentId == gpNvm_AllComponents)
    {
        //When gpNvm_AllTags used, default ram location will be used to backup data
        GP_ASSERT_DEV_EXT(pRamLocation == NULL);
        //When gpNvm_AllComponents is used, gpNvm_AllTags should be given !
        GP_ASSERT_DEV_EXT(tagId == gpNvm_AllTags);
    }
    for(i=0; i < GP_NVM_NBR_OF_SECTIONS; i++)
    {
        if ((gpNvm_Sections[i].componentId != componentId) && (componentId != gpNvm_AllComponents))
        {
            continue;
        }
        for(j=0; j < gpNvm_Sections[i].nbrOfTags; j++)
        {
            if ((j != tagId) && (tagId != gpNvm_AllTags))
            {
                continue;
            }
            SectionFound = true;
            {
                UIntPtr nvmAddr = Nvm_GetTagBaseAddr(i, j);
                gpNvm_Tag_t pTag[1];
                MEMCPY_P(pTag,&(gpNvm_Sections[i].tags[j]),sizeof(gpNvm_Tag_t));

                if (pTag->updateFrequency == gpNvm_UpdateFrequencyReadOnly && gpNvm_Locked)
                {
                    GP_ASSERT_DEV_EXT(tagId == gpNvm_AllTags); //Only pass here when going through all tags
                    continue;
                }

                //Force default ram location if AllTags is used !
                if (pRamLocation == NULL || tagId == gpNvm_AllTags)
                {
                    pRamLocation = pTag->pRamLocation;
                }
                GP_ASSERT_DEV_EXT(pRamLocation);
                Nvm_WriteBlockWithRetries(nvmAddr, pTag->size, pRamLocation, pTag->updateFrequency);

            }
    //gpNvm_Dump(componentId,tagId);
        }
    }
    GP_ASSERT_DEV_EXT(SectionFound);
}

void gpNvm_Restore(UInt8 componentId, UInt8 tagId, UInt8* pRamLocation)
{
    UInt8 i,j;
    Bool SectionFound = false;
    //GP_LOG_SYSTEM_PRINTF("Restore %u %u 0x%x", 6, (UInt16)componentId, (UInt16)tagId, (UInt16)pRamLocation);


    //don't allow access to NVM before the consistency check (exception
    //for gpReset, reset reason is read during stack initialisation)
    if(componentId != GP_COMPONENT_ID_RESET)
    {
        GP_ASSERT_DEV_EXT(!gpSched_ExistsEvent(gpNvm_CheckConsistency));
    }
    if (tagId == gpNvm_AllTags && componentId == gpNvm_AllComponents)
    {
        SectionFound = true;
    }
    if (tagId == gpNvm_AllTags || componentId == gpNvm_AllComponents)
    {
        //When gpNvm_AllTags used, default ram location will be used to restore data
        GP_ASSERT_DEV_EXT(pRamLocation == NULL);
        //When gpNvm_AllComponents is used, gpNvm_AllTags should be given !
        GP_ASSERT_DEV_EXT(tagId == gpNvm_AllTags);
    }
    for(i=0; i < GP_NVM_NBR_OF_SECTIONS; i++)
    {
        if ((gpNvm_Sections[i].componentId != componentId) && (componentId != gpNvm_AllComponents))
        {
            continue;
        }
        for(j=0; j < gpNvm_Sections[i].nbrOfTags; j++)
        {
            if ((j != tagId) && (tagId != gpNvm_AllTags))
            {
                continue;
            }
            SectionFound = true;
            {
                UIntPtr nvmAddr = Nvm_GetTagBaseAddr(i, j);

                gpNvm_Tag_t pTag[1];
                MEMCPY_P(pTag,&(gpNvm_Sections[i].tags[j]),sizeof(gpNvm_Tag_t));
                //Force default ram location if AllTags is used !
                if (pRamLocation == NULL || tagId == gpNvm_AllTags)
                {
                    pRamLocation = pTag->pRamLocation;
                }
                GP_ASSERT_DEV_EXT(pRamLocation);

                Nvm_ReadBlock(nvmAddr, pTag->size, pRamLocation, pTag->updateFrequency);
            }
        }
    }
    GP_ASSERT_DEV_EXT(SectionFound);
}

void gpNvm_Clear(UInt8 componentId, UInt8 tagId)
{
    UInt8 i,j;
    Bool SectionFound = false;
    gpNvm_Tag_t pTag[1];

    GP_ASSERT_DEV_EXT(!gpSched_ExistsEvent(gpNvm_CheckConsistency));

    //set default values if required
    for(i=0; i < GP_NVM_NBR_OF_SECTIONS; i++)
    {
        if ((gpNvm_Sections[i].componentId != componentId) && (componentId != gpNvm_AllComponents))
        {
            continue;
        }
        for(j=0; j < gpNvm_Sections[i].nbrOfTags; j++)
        {
            if ((j != tagId) && (tagId != gpNvm_AllTags))
            {
                continue;
            }
            SectionFound = true;

            MEMCPY_P(pTag,&(gpNvm_Sections[i].tags[j]),sizeof(gpNvm_Tag_t));

            if(pTag->updateFrequency == gpNvm_UpdateFrequencyReadOnly && gpNvm_Locked)
            {
                //Skip clearing of read-only tags
                GP_ASSERT_DEV_EXT(tagId == gpNvm_AllTags); //Only pass here when going through all tags
                continue;
            }
            {
            UInt8 pData[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
            UIntPtr nvmAddr = Nvm_GetTagBaseAddr(i, j);
            UInt16 pendingBytes;

            pendingBytes = pTag->size;

            while(pendingBytes)
            {
                UInt16  blockSize = (pendingBytes < 8) ? pendingBytes : 8;

                if(pTag->pDefaultValues != NULL)
                {
                    MEMCPY_P(pData,pTag->pDefaultValues+(pTag->size)-pendingBytes,blockSize);
                }
                Nvm_WriteBlockWithRetries(nvmAddr+(pTag->size)-pendingBytes,blockSize,pData, pTag->updateFrequency);
                pendingBytes -= blockSize;
            }
            }
        }
    }
    GP_ASSERT_DEV_EXT(SectionFound);
}

void gpNvm_SetLock(Bool lock)
{
    gpNvm_Locked = lock;
}

#ifdef GP_NVM_DIVERSITY_MAX_DUMP_SIZE
void gpNvm_Dump(UInt8 componentId, UInt8 tagId)
{
    UInt8 i;
    UInt8 sectionId;

    if(componentId == gpNvm_AllComponents)
    {
        for(i=0; i < GP_NVM_NBR_OF_SECTIONS; i++)
        {
            //FIXME - JAVE - not optimal, will need to lookup sectionId again
            gpNvm_Dump(gpNvm_Sections[i].componentId,tagId);
        }
        return;
    }

    sectionId = Nvm_GetSectionId(componentId);
    GP_ASSERT_DEV_EXT(sectionId < GP_NVM_NBR_OF_SECTIONS);

    if(tagId == gpNvm_AllTags)
    {
        for(i=0; i < gpNvm_Sections[sectionId].nbrOfTags; i++)
        {
            gpNvm_Dump(componentId,i);
        }
        return;
    }

    GP_ASSERT_DEV_EXT(tagId < gpNvm_Sections[sectionId].nbrOfTags);

    {
        UInt8 pData[GP_NVM_DIVERSITY_MAX_DUMP_SIZE];
        UInt8 printed = 0;

        gpNvm_Tag_t pTag[1];
        MEMCPY_P(pTag,&(gpNvm_Sections[sectionId].tags[tagId]),sizeof(gpNvm_Tag_t));

        GP_ASSERT_DEV_EXT(pTag->size <= GP_NVM_DIVERSITY_MAX_DUMP_SIZE);

        Nvm_ReadBlock(Nvm_GetTagBaseAddr(sectionId, tagId), pTag->size, pData, pTag->updateFrequency);

        if(pTag->pRamLocation && MEMCMP(pData,pTag->pRamLocation,pTag->size))
        {
            GP_LOG_SYSTEM_PRINTF("TAG[0x%x:%i] - (S%i) - *",6,(UInt16)componentId,(UInt16)tagId,(UInt16)pTag->size);

        }
        else
        {
            GP_LOG_SYSTEM_PRINTF("TAG[0x%x:%i] - (S%i)",6,(UInt16)componentId,(UInt16)tagId,(UInt16)pTag->size);
        }
        gpLog_Flush();
        while(printed < pTag->size)
        {
            switch(pTag->size-printed)
            {
                case 1:
                    GP_LOG_SYSTEM_PRINTF("    %x", 2,
                                         (UInt16)pData[printed+0]);
                    break;
                case 2:
                    GP_LOG_SYSTEM_PRINTF("    %x %x", 4,
                                         (UInt16)pData[printed+0], (UInt16)pData[printed+1]);
                    break;
                case 3:
                    GP_LOG_SYSTEM_PRINTF("    %x %x %x", 6,
                                         (UInt16)pData[printed+0], (UInt16)pData[printed+1], (UInt16)pData[printed+2]);
                    break;
                case 4:
                    GP_LOG_SYSTEM_PRINTF("    %x %x %x %x", 8,
                                         (UInt16)pData[printed+0], (UInt16)pData[printed+1], (UInt16)pData[printed+2], (UInt16)pData[printed+3]);
                    break;
                case 5:
                    GP_LOG_SYSTEM_PRINTF("    %x %x %x %x %x", 10,
                                         (UInt16)pData[printed+0], (UInt16)pData[printed+1], (UInt16)pData[printed+2], (UInt16)pData[printed+3],
                                         (UInt16)pData[printed+4]);
                    break;
                case 6:
                    GP_LOG_SYSTEM_PRINTF("    %x %x %x %x %x %x", 12,
                                         (UInt16)pData[printed+0], (UInt16)pData[printed+1], (UInt16)pData[printed+2], (UInt16)pData[printed+3],
                                         (UInt16)pData[printed+4], (UInt16)pData[printed+5]);
                    break;
                case 7:
                    GP_LOG_SYSTEM_PRINTF("    %x %x %x %x %x %x %x", 14,
                                         (UInt16)pData[printed+0], (UInt16)pData[printed+1], (UInt16)pData[printed+2], (UInt16)pData[printed+3],
                                         (UInt16)pData[printed+4], (UInt16)pData[printed+5], (UInt16)pData[printed+6]);
                    break;
                default:
                    GP_LOG_SYSTEM_PRINTF("    %x %x %x %x %x %x %x %x", 16,
                                         (UInt16)pData[printed+0], (UInt16)pData[printed+1], (UInt16)pData[printed+2], (UInt16)pData[printed+3],
                                         (UInt16)pData[printed+4], (UInt16)pData[printed+5], (UInt16)pData[printed+6], (UInt16)pData[printed+7]);
                    break;
            }
            printed += 8;
            gpLog_Flush();
        }

        gpLog_Flush();
    }
}
#endif //GP_NVM_DIVERSITY_MAX_DUMP_SIZE





