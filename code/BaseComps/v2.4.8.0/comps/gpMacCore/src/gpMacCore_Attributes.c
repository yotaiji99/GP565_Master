/*
 * Copyright (c) 2012-2014, GreenPeak Technologies
 *
 * gpMacCore_Attributes.c
 *   This file contains the handling of all MAC attributes (for different stacks).
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpMacCore/src/gpMacCore_Attributes.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

// General includes
#include "gpPd.h"
#include "gpPad.h"
#include "gpHal.h"
#include "gpMacCore.h"
#include "gpMacCore_defs.h"
#include "gpLog.h"

#if GP_PAD_NR_OF_HANDLES < GP_DIVERSITY_NR_OF_STACKS
#error Every stack needs a Pad - increase GP_PAD_NR_OF_HANDLES
#endif

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_MACCORE

#define GP_MACCORE_DEFAULT_MAX_CSMA_BACKOFFS    4
#define GP_MACCORE_DEFAULT_MIN_BE               3
#define GP_MACCORE_DEFAULT_MAX_BE               5
#define GP_MACCORE_DEFAULT_NONBEACONED_PAN      0x0F


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

// typedefs for different stack specific gpHal functions
typedef void (*MacCore_HalSetPanIdFunction_t)(UInt16);
typedef void (*MacCore_HalSetShortAddressFunction_t)(UInt16);

typedef struct gpMacCore_PIB {
    gpPad_Handle_t padHandle;
    MACAddress_t coordExtendedAddress;
    UInt16 coordShortAddress;
    Bool panCoordinator;
    UInt8 dsn;
    gpMacCore_PanId_t panId;
    UInt16 shortAddress;
    Bool securityEnabled;
    gpMacCore_MacVersion_t macVersion;
    Bool associationPermit;
    UInt8 beaconPayload[GP_MACCORE_MAX_SUPPORTED_BEACON_PAYLOAD_LENGTH];
    UInt8 beaconPayloadLength;
    Bool BeaconStarted;
    Bool promiscuousMode;
    UInt16 TransactionPersistenceTime;
    Bool indicateBeaconNotifications;
    Bool forwardPollIndications;
} gpMacCore_PIB_t;


/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

gpMacCore_PIB_t gpMacCore_PIB[GP_DIVERSITY_NR_OF_STACKS];


MACAddress_t gpMacCore_extendedAddress;

static const gpPad_Attributes_t ROM MacCore_DefaultPadAttributes FLASH_PROGMEM = {
    {0xFF, 0xFF, 0xFF}, //UInt8 channels[GP_PAD_MAX_CHANNELS];
    0, //UInt8 antenna;
    gpHal_DefaultTransmitPower, //Int8  txPower;
    GP_MACCORE_DEFAULT_MIN_BE, //UInt8 minBE;
    GP_MACCORE_DEFAULT_MAX_BE, //UInt8 maxBE;
    GP_MACCORE_DEFAULT_MAX_CSMA_BACKOFFS, //UInt8 maxCsmaBackoffs;
    GP_MACCORE_NUMBER_OF_RETRIES,         //UInt8 maxFrameRetries;
    gpHal_CollisionAvoidanceModeCSMA,     //UInt8 csma;
    gpHal_CCAModeEnergyAndModulated       //UInt8 cca;
};

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

void MacCore_InitPad (gpMacCore_StackId_t stackId)
{
    gpMacCore_PIB[stackId].padHandle = GP_PAD_INVALID_HANDLE;
}


void MacCore_SetDefaultStackValues(Bool resetPib, gpMacCore_StackId_t stackId)
{

    gpHal_GetExtendedAddress(&gpMacCore_extendedAddress);
    // initialize this stacks to it's default values (do not care at this moment whether they are registered or not).
     // reset PIB
    if(resetPib)
    {
        gpPad_Attributes_t padAttributes;
        gpPad_Handle_t padHandleBup;
        MEMCPY_P(&padAttributes, &MacCore_DefaultPadAttributes, sizeof(gpPad_Attributes_t));

        padHandleBup = gpMacCore_PIB[stackId].padHandle;

        // initialize complete PIB to 0
        MEMSET(&(gpMacCore_PIB[stackId]), 0, sizeof(gpMacCore_PIB_t));

        // restore bup pad handle
        gpMacCore_PIB[stackId].padHandle = padHandleBup;
        if(gpPad_CheckPadValid(gpMacCore_PIB[stackId].padHandle) == gpPad_ResultValidHandle)
        {
            gpPad_SetAttributes(gpMacCore_PIB[stackId].padHandle, &padAttributes);
        }
        else
        {
            gpMacCore_PIB[stackId].padHandle = GP_PAD_INVALID_HANDLE;
        }

        {
            MACAddress_t invalidMacAddr = { 0xffffffff, 0xffffffff };
            gpMacCore_SetCoordShortAddress(GP_MACCORE_SHORT_ADDR_BROADCAST, stackId);
            gpMacCore_SetCoordExtendedAddress(&invalidMacAddr, stackId);
        }

        // initialize attributes with nonzero default value
        gpMacCore_PIB[stackId].panId = GP_MACCORE_PANID_BROADCAST;
        gpHal_SetPanId(gpMacCore_PIB[stackId].panId, stackId);
        gpMacCore_PIB[stackId].shortAddress = GP_MACCORE_SHORT_ADDR_UNALLOCATED;
        gpHal_SetShortAddress(gpMacCore_PIB[stackId].shortAddress, stackId);
        gpMacCore_PIB[stackId].TransactionPersistenceTime = GP_MACCORE_DEFAULT_TRANSACTION_PERSISTENCE_TIME;
        gpMacCore_PIB[stackId].forwardPollIndications = false;
    }

    gpHal_SetPromiscuousMode(false);

    // allow data and command packets but no beacons, beacons are only received when in active scan mode
#ifdef GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR
    if(!gpMacCore_pScanResult) //Only set when no scans are active - otherwise interferes with other stacks running request
#endif //GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR
    {
        gpHal_SetFrameTypeFilterMask(GP_MACCORE_FRAME_TYPE_FILTER_MASK_DEFAULT);
        gpHal_SetBeaconSrcPanChecking(true);
    }

}

gpPad_Handle_t MacCore_GetPad(UInt8 stackId)
{
    return gpMacCore_PIB[stackId].padHandle;
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpMacCore_SetCurrentChannel_STACKID (UInt8 channel MACCORE_STACKID_ARG_2)
{
    gpRxArbiter_SetStackChannel(channel,MACCORE_STACKID_REF);
}

UInt8 gpMacCore_GetCurrentChannel_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpRxArbiter_GetStackChannel(MACCORE_STACKID_REF);
}

void gpMacCore_SetDefaultTransmitPowers(Int8* pDefaultTransmitPowerTable)
{
    gpHal_SetDefaultTransmitPowers(pDefaultTransmitPowerTable);
}

void gpMacCore_SetTransmitPower_STACKID(Int8 transmitPower MACCORE_STACKID_ARG_2)
{
    gpPad_SetTxPower(gpMacCore_PIB[MACCORE_STACKID_REF].padHandle, transmitPower);
}

Int8 gpMacCore_GetTransmitPower_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpPad_GetTxPower(gpMacCore_PIB[MACCORE_STACKID_REF].padHandle);
}

void gpMacCore_SetCCAMode_STACKID(UInt8 cCAMode MACCORE_STACKID_ARG_2)
{
    gpPad_SetCcaMode(gpMacCore_PIB[MACCORE_STACKID_REF].padHandle, cCAMode);
}

UInt8 gpMacCore_GetCCAMode_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpPad_GetCcaMode(gpMacCore_PIB[MACCORE_STACKID_REF].padHandle);
}

void gpMacCore_SetCoordExtendedAddress_STACKID(MACAddress_t* pCoordExtendedAddress MACCORE_STACKID_ARG_2)
{
    if(pCoordExtendedAddress)
    {
        MEMCPY(&gpMacCore_PIB[MACCORE_STACKID_REF].coordExtendedAddress, pCoordExtendedAddress, sizeof(MACAddress_t));
    }
}

void gpMacCore_GetCoordExtendedAddress_STACKID(MACAddress_t* pCoordExtendedAddress MACCORE_STACKID_ARG_2)
{
    if(pCoordExtendedAddress)
    {
        MEMCPY(pCoordExtendedAddress, &gpMacCore_PIB[MACCORE_STACKID_REF].coordExtendedAddress, sizeof(MACAddress_t));
    }
}

void gpMacCore_SetCoordShortAddress_STACKID(UInt16 coordShortAddress MACCORE_STACKID_ARG_2)
{
    gpMacCore_PIB[MACCORE_STACKID_REF].coordShortAddress = coordShortAddress;
}

UInt16 gpMacCore_GetCoordShortAddress_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpMacCore_PIB[MACCORE_STACKID_REF].coordShortAddress;
}

void gpMacCore_SetPanCoordinator_STACKID(Bool panCoordinator MACCORE_STACKID_ARG_2)
{
    gpMacCore_PIB[MACCORE_STACKID_REF].panCoordinator = panCoordinator;

#if GP_DIVERSITY_NR_OF_STACKS > 1
    {
        UIntLoop i;
        //Check if other stacks behave as panCoordinator
        for(i = 0; i < GP_DIVERSITY_NR_OF_STACKS; i++)
        {
            if(gpMacCore_cbValidStack(i))
            {
                panCoordinator |= gpMacCore_PIB[i].panCoordinator;
            }
        }
    }
#endif

    gpHal_SetPanCoordinator(panCoordinator);
}

Bool gpMacCore_GetPanCoordinator_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpMacCore_PIB[MACCORE_STACKID_REF].panCoordinator;
}

void gpMacCore_SetDsn_STACKID(UInt8 dsn MACCORE_STACKID_ARG_2)
{
    gpMacCore_PIB[MACCORE_STACKID_REF].dsn = dsn;
}

UInt8 gpMacCore_GetDsn_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpMacCore_PIB[MACCORE_STACKID_REF].dsn;
}

void gpMacCore_SetMaxCsmaBackoffs_STACKID(UInt8 maxCsmaBackoffs MACCORE_STACKID_ARG_2)
{
    gpPad_SetTxMaxCsmaBackoffs(gpMacCore_PIB[MACCORE_STACKID_REF].padHandle, maxCsmaBackoffs);
}

UInt8 gpMacCore_GetMaxCsmaBackoffs_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpPad_GetTxMaxCsmaBackoffs(gpMacCore_PIB[MACCORE_STACKID_REF].padHandle);
}

void gpMacCore_SetMinBE_STACKID(UInt8 minBE MACCORE_STACKID_ARG_2)
{
    gpPad_SetTxMinBE(gpMacCore_PIB[MACCORE_STACKID_REF].padHandle, minBE);
}

UInt8 gpMacCore_GetMinBE_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpPad_GetTxMinBE(gpMacCore_PIB[MACCORE_STACKID_REF].padHandle);
}

void gpMacCore_SetMaxBE_STACKID(UInt8 maxBE MACCORE_STACKID_ARG_2)
{
    gpPad_SetTxMaxBE(gpMacCore_PIB[MACCORE_STACKID_REF].padHandle, maxBE);
}

UInt8 gpMacCore_GetMaxBE_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpPad_GetTxMaxBE(gpMacCore_PIB[MACCORE_STACKID_REF].padHandle);
}

void gpMacCore_SetPanId_STACKID(UInt16 panId MACCORE_STACKID_ARG_2)
{
    gpMacCore_PIB[MACCORE_STACKID_REF].panId = panId;
    gpHal_SetPanId(panId, MACCORE_STACKID_REF);
}

UInt16 gpMacCore_GetPanId_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpMacCore_PIB[MACCORE_STACKID_REF].panId;
}

void gpMacCore_SetRxOnWhenIdle_STACKID(Bool rxOnWhenIdle MACCORE_STACKID_ARG_2)
{
    gpRxArbiter_SetStackRxOn( rxOnWhenIdle , MACCORE_STACKID_REF);
}

Bool gpMacCore_GetRxOnWhenIdle_STACKID(MACCORE_STACKID_ARG_1)
{
    //FIXME do correct
    return gpRxArbiter_GetCurrentRxOnState();
}

void gpMacCore_SetSecurityEnabled_STACKID(Bool securityEnabled MACCORE_STACKID_ARG_2)
{
    UInt8 stackIndex;
    UInt8 stacksWithSecurity = 0;
    GP_ASSERT_DEV_INT(!securityEnabled); //Not allowed to enable security if not available in code

    gpMacCore_PIB[MACCORE_STACKID_REF].securityEnabled = securityEnabled;
    for(stackIndex = 0; stackIndex < GP_DIVERSITY_NR_OF_STACKS; stackIndex++)
    {
        if(gpMacCore_PIB[stackIndex].securityEnabled)
        {
            stacksWithSecurity++;
        }
    }
    GP_ASSERT_DEV_INT(stacksWithSecurity <= 1); //Not allowed to enable security on more then 1 stack
}

Bool gpMacCore_GetSecurityEnabled_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpMacCore_PIB[MACCORE_STACKID_REF].securityEnabled;
}

void gpMacCore_SetShortAddress_STACKID(UInt16 shortAddress MACCORE_STACKID_ARG_2)
{
    gpMacCore_PIB[MACCORE_STACKID_REF].shortAddress = shortAddress;
    gpHal_SetShortAddress(shortAddress, MACCORE_STACKID_REF);
}

UInt16 gpMacCore_GetShortAddress_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpMacCore_PIB[MACCORE_STACKID_REF].shortAddress;
}

void gpMacCore_SetAssociationPermit_STACKID(Bool associationPermit MACCORE_STACKID_ARG_2)
{
    gpMacCore_PIB[MACCORE_STACKID_REF].associationPermit = associationPermit;
}

Bool gpMacCore_GetAssociationPermit_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpMacCore_PIB[MACCORE_STACKID_REF].associationPermit;
}

void gpMacCore_SetBeaconPayload_STACKID(UInt8* pBeaconPayload MACCORE_STACKID_ARG_2)
{
    UInt8 payloadLength = gpMacCore_GetBeaconPayloadLength(MACCORE_STACKID_REF);
    //GP_LOG_SYSTEM_PRINTF("Driver: We are setting the beaconPayload!!!",0);
    //gpLog_PrintBuffer(15, pBeaconPayload);
    MEMCPY(gpMacCore_PIB[MACCORE_STACKID_REF].beaconPayload, pBeaconPayload, payloadLength);
}

void gpMacCore_GetBeaconPayload_STACKID(UInt8* pBeaconPayload MACCORE_STACKID_ARG_2)
{
    UInt8 payloadLength = gpMacCore_GetBeaconPayloadLength(MACCORE_STACKID_REF);
    //GP_LOG_SYSTEM_PRINTF("Driver: We are getting the beaconPayload!!!",0);
    //gpLog_PrintBuffer(15, gpMacCore_PIB[MACCORE_STACKID_REF].beaconPayload);
    MEMCPY(pBeaconPayload, gpMacCore_PIB[MACCORE_STACKID_REF].beaconPayload, payloadLength);
}

void gpMacCore_SetBeaconPayloadLength_STACKID(UInt8 beaconPayloadLength MACCORE_STACKID_ARG_2)
{
    GP_ASSERT_DEV_EXT(beaconPayloadLength <= GP_MACCORE_MAX_SUPPORTED_BEACON_PAYLOAD_LENGTH);

    gpMacCore_PIB[MACCORE_STACKID_REF].beaconPayloadLength = beaconPayloadLength;
}

UInt8 gpMacCore_GetBeaconPayloadLength_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpMacCore_PIB[MACCORE_STACKID_REF].beaconPayloadLength;
}

void gpMacCore_SetBeaconStarted_STACKID(Bool BeaconStarted MACCORE_STACKID_ARG_2)
{
    gpMacCore_PIB[MACCORE_STACKID_REF].BeaconStarted = BeaconStarted;
}

Bool gpMacCore_GetBeaconStarted_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpMacCore_PIB[MACCORE_STACKID_REF].BeaconStarted;
}

void gpMacCore_SetPromiscuousMode_STACKID(UInt8 promiscuousMode MACCORE_STACKID_ARG_2)
{
    gpMacCore_PIB[MACCORE_STACKID_REF].promiscuousMode = promiscuousMode;

#if GP_DIVERSITY_NR_OF_STACKS > 1
    {
        UIntLoop i;
        //Check if other stacks already want promiscuousmode
        for(i = 0; i < GP_DIVERSITY_NR_OF_STACKS; i++)
        {
            if(gpMacCore_cbValidStack(i) && gpMacCore_PIB[i].promiscuousMode)
            {
                promiscuousMode = true;
                break;
            }
        }
    }
#endif

    gpHal_SetPromiscuousMode(promiscuousMode);
}

UInt8 gpMacCore_GetPromiscuousMode_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpMacCore_PIB[MACCORE_STACKID_REF].promiscuousMode;
}

void gpMacCore_SetTransactionPersistenceTime_STACKID(UInt16 transactionPersistentTime MACCORE_STACKID_ARG_2)
{
    gpMacCore_PIB[MACCORE_STACKID_REF].TransactionPersistenceTime = transactionPersistentTime;
}

UInt16 gpMacCore_GetTransactionPersistenceTime_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpMacCore_PIB[MACCORE_STACKID_REF].TransactionPersistenceTime;
}

void gpMacCore_SetIndicateBeaconNotifications_STACKID(Bool enable MACCORE_STACKID_ARG_2)
{
    UIntLoop i;
    UInt8 frameType = gpHal_GetFrameTypeFilterMask();
    gpMacCore_PIB[MACCORE_STACKID_REF].indicateBeaconNotifications = enable;

    for( i=0;i<GP_DIVERSITY_NR_OF_STACKS;i++)
    {
        if (gpMacCore_cbValidStack(i) && gpMacCore_PIB[i].indicateBeaconNotifications)
        {
            enable = true;
            break;
        }
    }
    if(enable)
    {
        frameType |= GPHAL_ENUM_FRAME_TYPE_FILTER_BCN_MASK;
    }
    else
    {
        frameType &= (~GPHAL_ENUM_FRAME_TYPE_FILTER_BCN_MASK);
    }
    gpHal_SetFrameTypeFilterMask(frameType);
}

Bool gpMacCore_GetIndicateBeaconNotifications_STACKID( MACCORE_STACKID_ARG_1)
{
    return gpMacCore_PIB[MACCORE_STACKID_REF].indicateBeaconNotifications;
}

void gpMacCore_SetForwardPollIndications_STACKID(Bool enable MACCORE_STACKID_ARG_2)
{
    gpMacCore_PIB[MACCORE_STACKID_REF].forwardPollIndications = enable;
}

Bool gpMacCore_GetForwardPollIndications_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpMacCore_PIB[MACCORE_STACKID_REF].forwardPollIndications;
}

void gpMacCore_SetExtendedAddress(MACAddress_t * extendedMacAddress)
{
    MEMCPY(&gpMacCore_extendedAddress,extendedMacAddress,sizeof(MACAddress_t));
    gpHal_SetExtendedAddress(extendedMacAddress);
}

void gpMacCore_GetExtendedAddress( MACAddress_t * pExtendedAddress )
{
     MEMCPY(pExtendedAddress,&gpMacCore_extendedAddress,sizeof(MACAddress_t));
}

void gpMacCore_SetNumberOfRetries_STACKID(UInt8 numberOfRetries MACCORE_STACKID_ARG_2)
{
    gpPad_SetTxMaxFrameRetries(gpMacCore_PIB[MACCORE_STACKID_REF].padHandle, numberOfRetries);
}

UInt8 gpMacCore_GetNumberOfRetries_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpPad_GetTxMaxFrameRetries(gpMacCore_PIB[MACCORE_STACKID_REF].padHandle);
}

void gpMacCore_SetTxAntenna_STACKID(UInt8 txAntenna MACCORE_STACKID_ARG_2)
{
    gpPad_SetTxAntenna(gpMacCore_PIB[MACCORE_STACKID_REF].padHandle, txAntenna);
}

UInt8 gpMacCore_GetTxAntenna_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpPad_GetTxAntenna(gpMacCore_PIB[MACCORE_STACKID_REF].padHandle);
}

void gpMacCore_SetMacVersion_STACKID(gpMacCore_MacVersion_t macVersion MACCORE_STACKID_ARG_2)
{
    GP_ASSERT_DEV_EXT(macVersion <= gpMacCore_MacVersion2006);

    gpMacCore_PIB[MACCORE_STACKID_REF].macVersion = macVersion;
}

gpMacCore_MacVersion_t gpMacCore_GetMacVersion_STACKID(MACCORE_STACKID_ARG_1)
{
    return gpMacCore_PIB[MACCORE_STACKID_REF].macVersion;
}


void gpMacCore_StackAdded(gpMacCore_StackId_t stackId)
{
    //Allocate pad handle
    gpPad_Attributes_t padAttributes;
    MEMCPY_P(&padAttributes, &MacCore_DefaultPadAttributes, sizeof(gpPad_Attributes_t));

    if(gpPad_CheckPadValid(gpMacCore_PIB[stackId].padHandle) == gpPad_ResultValidHandle)
    {
        gpPad_SetAttributes(gpMacCore_PIB[stackId].padHandle, &padAttributes);
        GP_LOG_PRINTF("Stack %i re-added",0, stackId);
    }
    else
    {
        gpMacCore_PIB[stackId].padHandle = gpPad_GetPad(&padAttributes);
        GP_LOG_PRINTF("Stack %i added",0, stackId);
    }
}

void gpMacCore_StackRemoved(gpMacCore_StackId_t stackId)
{

    //Stop all actions of this stack
    MacCore_StopRunningRequests(stackId);

    //Free Pad for this stack
    if(gpPad_ResultValidHandle == gpPad_CheckPadValid(gpMacCore_PIB[stackId].padHandle))
    {
        gpPad_FreePad(gpMacCore_PIB[stackId].padHandle);
        gpMacCore_PIB[stackId].padHandle = GP_PAD_INVALID_HANDLE;
    }
    MacCore_SetDefaultStackValues(true, stackId);

    gpRxArbiter_ResetStack(stackId);

    GP_LOG_PRINTF("Stack %i removed",0, stackId);
}

gpMacCore_Result_t gpMacCore_Start(gpMacCore_PanId_t panId, UInt8 logicalChannel, Bool panCoordinator, UInt8 stackId)
{
    if(GP_MACCORE_IS_SHORT_ADDR_BROADCAST(gpMacCore_GetShortAddress(stackId)))
    {
        return gpMacCore_ResultNoShortAddress;
    }

    gpMacCore_SetPanCoordinator(panCoordinator, stackId);
    gpMacCore_SetPanId(panId, stackId);
    gpRxArbiter_SetStackChannel(logicalChannel, stackId);
#ifdef GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT
    gpMacCore_SetBeaconStarted(true, stackId);
#endif //GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT
    return gpMacCore_ResultSuccess;
}

