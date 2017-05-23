/*
 * Copyright (c) 2011-2013, GreenPeak Technologies
 *
 *   This file contains the implementation of the gpRf4ceDispatcher.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpRf4ceKickOutLru/src/gpRf4ceKickOutLru.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_RF4CEKICKOUTLRU
//#define GP_LOCAL_LOG
#include "gpRf4ceDispatcher.h"
#include "gpRf4ceKickOutLru.h"
#include "gpAssert.h"
#include "gpSched.h"
#include "gpLog.h"
#include "gpNvm.h"
#include "gpPd.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#ifndef GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES
#define GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES             2 //stored in NVM
#endif //GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES
#define GP_RF4CE_KICKOUTLRU_USAGE_ENTRIES               (GP_RF4CE_NWKC_MAX_PAIRING_TABLE_ENTRIES-1) //stored in RAM
#define GP_RF4CE_KICKOUTLRU_REMOTE_USAGE_LAST           (GP_RF4CE_KICKOUTLRU_USAGE_ENTRIES-1)
#define GP_RF4CE_KICKOUTLRU_REMOTE_USAGE_KICKOUT_BASE   (GP_RF4CE_KICKOUTLRU_USAGE_ENTRIES-GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES)


#define GP_RF4CE_KICKOUTLRU_INVALID_PROFILE_ID                0xFF

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct {
    Bool updateNvm;
    gpRf4ceKickOutLru_Desc_t removeDesc;
} Rf4ceKickOutLru_UpdateReturnValue_t;

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

GP_RF4CE_DISPATCHER_DATA_DECLARATION(gpRf4ceKickOutLru);

//Data callbacks
static void gpRf4ceDispatcher_cbResetIndication(Bool setDefault);
static Bool gpRf4ceDispatcher_cbDEDataIndication(UInt8 pairingRef, gpRf4ce_ProfileId_t profileId, gpRf4ce_VendorId_t vendorId, UInt8 rxFlags, UInt8 nsduLength, gpPd_Offset_t nsduOffset, gpPd_Handle_t pdHandle);

GP_RF4CE_DISPATCHER_CONST gpRf4ceDispatcher_DataCallbacks_t ROM gpRf4ceKickOutLru_DataCallbacks FLASH_PROGMEM = {
        gpRf4ceDispatcher_cbResetIndication,
        gpRf4ceDispatcher_cbDEDataIndication,
        NULL,
        NULL,
        NULL};


STATIC UInt8 Rf4ceKickOutLru_UsageOrder[GP_RF4CE_KICKOUTLRU_USAGE_ENTRIES];
STATIC UInt8 Rf4ceKickOutLru_ProfileIdList[GP_RF4CE_NWKC_MAX_PAIRING_TABLE_ENTRIES];
#if GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES==1
STATIC GP_NVM_CONST UInt8 ROM Rf4ceKickOutLru_UsageOrderDefaultVal[GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES] FLASH_PROGMEM = {0xFF};
#elif GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES==2
STATIC GP_NVM_CONST UInt8 ROM Rf4ceKickOutLru_UsageOrderDefaultVal[GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES] FLASH_PROGMEM = {0xFF,0xFF};
#else
#error "Error: define a default value for non 1 or 2 value"
#endif

#if (GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES > GP_RF4CE_KICKOUTLRU_USAGE_ENTRIES)
#error "Error: Kickout entries in NVM should be minimum 1 smaller than pairing entries - <= usage entries in ram"
#endif

#define GP_RF4CE_KICKOUTLRU_NVM_USAGE_LIST_INDEX              0
#define GP_RF4CE_KICKOUTLRU_NVM_PROFILE_ID_LIST_INDEX         1

const gpNvm_Tag_t ROM gpRf4ceKickOutLru_NvmSection[] FLASH_PROGMEM = {
    /*Not storing all entries in NVM*/
    {(UInt8*)&(Rf4ceKickOutLru_UsageOrder[GP_RF4CE_KICKOUTLRU_USAGE_ENTRIES-GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES]), GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES, gpNvm_UpdateFrequencyLow, Rf4ceKickOutLru_UsageOrderDefaultVal},
    {(UInt8*)Rf4ceKickOutLru_ProfileIdList, GP_RF4CE_NWKC_MAX_PAIRING_TABLE_ENTRIES, gpNvm_UpdateFrequencyLow, NULL}};


/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static UInt8 Rf4ceKickOutLru_UsageGetPriority(UInt8 pairingRef);
static Rf4ceKickOutLru_UpdateReturnValue_t Rf4ceKickOutLru_UsageUpdate_NoNvmUpdate(UInt8 pairingRef);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

UInt8 Rf4ceKickOutLru_UsageGetPriority(UInt8 pairingRef)
{
    UIntLoop i;
    for(i=0; i<GP_RF4CE_KICKOUTLRU_USAGE_ENTRIES; i++)
    {
        if(Rf4ceKickOutLru_UsageOrder[i] == pairingRef)
        {
            return i;
        }
    }
    return GP_RF4CE_KICKOUTLRU_USAGE_ENTRIES;
}

Rf4ceKickOutLru_UpdateReturnValue_t Rf4ceKickOutLru_UsageUpdate_NoNvmUpdate(UInt8 pairingRef)
{
    IntLoop i;
    UInt8 previousPriority;
    Rf4ceKickOutLru_UpdateReturnValue_t returnVal = {false,{0xFF,GP_RF4CE_KICKOUTLRU_INVALID_PROFILE_ID}};
    //no update required if pairingRef is already remote with highest priority or if default 'no-remote' is added
    if( (pairingRef == Rf4ceKickOutLru_UsageOrder[0] ) ||
        (pairingRef == 0xFF) )
    {
        return returnVal;
    }

    // check if pairingentry is alredy in the list
    previousPriority = Rf4ceKickOutLru_UsageGetPriority(pairingRef);
    //if device is in the list, update list, no device needs to be removed.
    if( previousPriority  == GP_RF4CE_KICKOUTLRU_USAGE_ENTRIES) //=new device
    {
#if (GP_RF4CE_KICKOUTLRU_REMOTE_USAGE_KICKOUT_BASE > 0) //Avoid warnings in corner cases
        if(Rf4ceKickOutLru_UsageOrder[GP_RF4CE_KICKOUTLRU_REMOTE_USAGE_KICKOUT_BASE-1] != 0xFF )
#endif
        {
            returnVal.updateNvm = true;
        }

        // kickoutlist full => remove least recently used
        if( Rf4ceKickOutLru_UsageOrder[GP_RF4CE_KICKOUTLRU_REMOTE_USAGE_LAST] != 0xFF)
        {
            returnVal.removeDesc.pairingRef = Rf4ceKickOutLru_UsageOrder[GP_RF4CE_KICKOUTLRU_REMOTE_USAGE_LAST];
            returnVal.removeDesc.profileId  = Rf4ceKickOutLru_ProfileIdList[ returnVal.removeDesc.pairingRef ];
            Rf4ceKickOutLru_ProfileIdList[ returnVal.removeDesc.pairingRef ] = GP_RF4CE_KICKOUTLRU_INVALID_PROFILE_ID;
        }
        for(i=GP_RF4CE_KICKOUTLRU_REMOTE_USAGE_LAST;i>0;i--)
        {
            Rf4ceKickOutLru_UsageOrder[i] = Rf4ceKickOutLru_UsageOrder[i-1];
        }
    }
    else
    {
        // update order
        for(i=previousPriority; i>0; i--)
        {
            Rf4ceKickOutLru_UsageOrder[i] = Rf4ceKickOutLru_UsageOrder[i-1];
        }

#if ( GP_RF4CE_KICKOUTLRU_USAGE_ENTRIES - GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES ) != 0 //Avoid warnings in corner cases
        if( previousPriority >= ( GP_RF4CE_KICKOUTLRU_USAGE_ENTRIES - GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES ))
#endif
        {
            returnVal.updateNvm = true;
        }
    }
    Rf4ceKickOutLru_UsageOrder[0]=pairingRef;

    return returnVal;
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

// ----------------------------- INIT ---------------------------

void gpRf4ceKickOutLru_Init(void)
{
    gpNvm_RegisterSection(GP_COMPONENT_ID, gpRf4ceKickOutLru_NvmSection, sizeof(gpRf4ceKickOutLru_NvmSection)/sizeof(gpNvm_Tag_t),NULL);
}

gpRf4ceKickOutLru_Desc_t gpRf4ceKickOutLru_AddDeviceToList( UInt8 pairingRef , gpRf4ce_ProfileId_t profileId )
{
    Rf4ceKickOutLru_UpdateReturnValue_t returnVal;

    GP_ASSERT_DEV_EXT(profileId != GP_RF4CE_KICKOUTLRU_INVALID_PROFILE_ID);
    //check if pairing entry is valid.
    if( !gpRf4ce_IsPairingTableEntryValid(pairingRef) )
    {
        returnVal.removeDesc.pairingRef = pairingRef;
        returnVal.removeDesc.profileId = profileId;
        return returnVal.removeDesc;
    }
    returnVal = Rf4ceKickOutLru_UsageUpdate_NoNvmUpdate(pairingRef);

    if( Rf4ceKickOutLru_ProfileIdList[ pairingRef ] != profileId )
    {
        Rf4ceKickOutLru_ProfileIdList[ pairingRef ] = profileId;
        gpNvm_Backup( GP_COMPONENT_ID , GP_RF4CE_KICKOUTLRU_NVM_PROFILE_ID_LIST_INDEX , NULL );
    }

    //update Nvm if needed
    if(returnVal.updateNvm)
    {
        gpNvm_Backup( GP_COMPONENT_ID , GP_RF4CE_KICKOUTLRU_NVM_USAGE_LIST_INDEX , NULL );
    }

    GP_LOG_PRINTF("A KOL %i %i %i %i %i",10,Rf4ceKickOutLru_UsageOrder[0],Rf4ceKickOutLru_UsageOrder[1],Rf4ceKickOutLru_UsageOrder[2],Rf4ceKickOutLru_UsageOrder[3],Rf4ceKickOutLru_UsageOrder[4]);
    //GP_LOG_PRINTF("dem KOL %i",2,returnVal.removePairingRef);

    return returnVal.removeDesc;
}

void gpRf4ceKickOutLru_RemoveDeviceFromList( UInt8 pairingRef )
{
    UInt8 previousPriority = Rf4ceKickOutLru_UsageGetPriority(pairingRef);

    //no need to remove an unexisting remote
    if(previousPriority  == GP_RF4CE_KICKOUTLRU_USAGE_ENTRIES)
    {
        return;
    }

    //rearrange remote usage list
#if GP_RF4CE_KICKOUTLRU_REMOTE_USAGE_LAST != 0 //Avoid warnings in corner cases
    {
    UIntLoop i;
    for(i=previousPriority; i<GP_RF4CE_KICKOUTLRU_REMOTE_USAGE_LAST; i++)
    {
        Rf4ceKickOutLru_UsageOrder[i] = Rf4ceKickOutLru_UsageOrder[i+1];
    }
    }
#endif
    //put last element of remote usage list to default value
    Rf4ceKickOutLru_UsageOrder[GP_RF4CE_KICKOUTLRU_REMOTE_USAGE_LAST] = 0xFF;

#if (GP_RF4CE_KICKOUTLRU_REMOTE_USAGE_KICKOUT_BASE > 0) //Avoid warnings in corner cases
    //if first element of kickout-list had a valid pairing entry, the NVM storage needs to be updated
    if(Rf4ceKickOutLru_UsageOrder[GP_RF4CE_KICKOUTLRU_REMOTE_USAGE_KICKOUT_BASE-1] != 0xFF)
#endif    
    {
        gpNvm_Backup( GP_COMPONENT_ID , GP_RF4CE_KICKOUTLRU_NVM_USAGE_LIST_INDEX , NULL );
    }
    //Update profile list
    Rf4ceKickOutLru_ProfileIdList[ pairingRef ] = GP_RF4CE_KICKOUTLRU_INVALID_PROFILE_ID;
    gpNvm_Backup( GP_COMPONENT_ID , GP_RF4CE_KICKOUTLRU_NVM_PROFILE_ID_LIST_INDEX , NULL );

    GP_LOG_PRINTF("Rem KOL %i %i %i %i %i",10,Rf4ceKickOutLru_UsageOrder[0],Rf4ceKickOutLru_UsageOrder[1],Rf4ceKickOutLru_UsageOrder[2],Rf4ceKickOutLru_UsageOrder[3],Rf4ceKickOutLru_UsageOrder[4]);
}

// ----------------------------- RESET ---------------------------

void gpRf4ceDispatcher_cbResetIndication(Bool setDefault)
{
    UIntLoop i;
    UInt8 kickoutList[GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES];
    //load kickout remotes from NVM
    gpNvm_Restore( GP_COMPONENT_ID , GP_RF4CE_KICKOUTLRU_NVM_USAGE_LIST_INDEX , NULL);
    //load kickout remotes from NVM
    gpNvm_Restore( GP_COMPONENT_ID , GP_RF4CE_KICKOUTLRU_NVM_PROFILE_ID_LIST_INDEX, NULL);

    // put restored values in the local buffer
    MEMCPY(kickoutList,&Rf4ceKickOutLru_UsageOrder[GP_RF4CE_KICKOUTLRU_USAGE_ENTRIES-GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES],GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES);

    //first remove valid RF4CE paring entries that have no valid profile id
    for(i=0; i<GP_RF4CE_NWKC_MAX_PAIRING_TABLE_ENTRIES; i++)
    {
        if(gpRf4ce_IsPairingTableEntryValid(i) )
        {
            if( Rf4ceKickOutLru_ProfileIdList[i] == GP_RF4CE_KICKOUTLRU_INVALID_PROFILE_ID )
            {
                gpRf4ce_SetPairingTableEntry( i , NULL );
            }
        }
        else
        {
            Rf4ceKickOutLru_ProfileIdList[i] = GP_RF4CE_KICKOUTLRU_INVALID_PROFILE_ID;
        }
    }

    //default values
    for(i=0; i<GP_RF4CE_KICKOUTLRU_USAGE_ENTRIES; i++)
    {
        Rf4ceKickOutLru_UsageOrder[i] = 0xFF;
    }

    //add kickout remotes
    for(i=0 ; i < GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES; i++)
    {
        //check if paired
        if(gpRf4ce_IsPairingTableEntryValid(kickoutList[(GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES-1)-i]))
        {
            Rf4ceKickOutLru_UsageUpdate_NoNvmUpdate(kickoutList[(GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES-1)-i]);
        }
    }

    //add other paired remotes
    for(i=0; i<GP_RF4CE_NWKC_MAX_PAIRING_TABLE_ENTRIES; i++)
    {
        //check if paired
        if(gpRf4ce_IsPairingTableEntryValid(i))
        {
            Bool skip = false;
            UIntLoop j;

            //check if not in kickout list
            for(j=0; j < GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES; j++)
            {
                if(i == kickoutList[(GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES-1)-j])
                {
                    skip = true;
                    break;
                }
            }
            if(!skip)
            {
                Rf4ceKickOutLru_UpdateReturnValue_t returnVal = Rf4ceKickOutLru_UsageUpdate_NoNvmUpdate(i);
                if( returnVal.removeDesc.pairingRef != 0xFF )
                {
                    gpRf4ce_SetPairingTableEntry( returnVal.removeDesc.pairingRef , NULL );
                }
            }
        }
    }

    //check if KickOutList needs an update.
    if( MEMCMP(kickoutList,&Rf4ceKickOutLru_UsageOrder[GP_RF4CE_KICKOUTLRU_USAGE_ENTRIES-GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES],GP_RF4CE_KICKOUTLRU_KICKOUT_ENTRIES)!= 0 )
    {
        gpNvm_Backup( GP_COMPONENT_ID , GP_RF4CE_KICKOUTLRU_NVM_USAGE_LIST_INDEX , NULL );
    }
    // update always, check uses too much code.
    gpNvm_Backup( GP_COMPONENT_ID , GP_RF4CE_KICKOUTLRU_NVM_PROFILE_ID_LIST_INDEX, NULL );
    GP_LOG_PRINTF("Res KOL %i %i %i %i %i",10,Rf4ceKickOutLru_UsageOrder[0],Rf4ceKickOutLru_UsageOrder[1],Rf4ceKickOutLru_UsageOrder[2],Rf4ceKickOutLru_UsageOrder[3],Rf4ceKickOutLru_UsageOrder[4]);
}
// ----------------------------- CHECK VALIDATION ---------------------------

Bool gpRf4ceDispatcher_cbDEDataIndication(UInt8 pairingRef, gpRf4ce_ProfileId_t profileId, gpRf4ce_VendorId_t vendorId, UInt8 rxFlags, UInt8 nsduLength, gpPd_Offset_t nsduOffset, gpPd_Handle_t pdHandle)
{
    Rf4ceKickOutLru_UpdateReturnValue_t returnVal;
    // update in list
    if( GP_RF4CE_KICKOUTLRU_USAGE_ENTRIES == Rf4ceKickOutLru_UsageGetPriority(pairingRef) )
    {
        //not in list, so ignore
        return false;
    }
    //if device is in the list, update list, no device needs to be removed.

    returnVal = Rf4ceKickOutLru_UsageUpdate_NoNvmUpdate(pairingRef);
    if( returnVal.updateNvm )
    {
        gpNvm_Backup( GP_COMPONENT_ID , GP_RF4CE_KICKOUTLRU_NVM_USAGE_LIST_INDEX , NULL );
    }
    // do not take other actions
    GP_LOG_PRINTF("I KOL %i %i %i %i %i",10,Rf4ceKickOutLru_UsageOrder[0],Rf4ceKickOutLru_UsageOrder[1],Rf4ceKickOutLru_UsageOrder[2],Rf4ceKickOutLru_UsageOrder[3],Rf4ceKickOutLru_UsageOrder[4]);
    return false;
}

// ----------------------------- (DBG) ACCESS FUNCTIONS ---------------------------

UInt8 gpRf4ceKickOutLru_GetList( UInt8* pList , UInt8 size)
{
    GP_ASSERT_DEV_EXT(size >= GP_RF4CE_KICKOUTLRU_USAGE_ENTRIES);
    MEMCPY(pList,Rf4ceKickOutLru_UsageOrder,GP_RF4CE_KICKOUTLRU_USAGE_ENTRIES);
    return GP_RF4CE_KICKOUTLRU_USAGE_ENTRIES;
}

