/*
 * Copyright (c) 2011-2014, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpPd/src/gpPd_pbm.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

//#define GP_LOCAL_LOG

// General includes
#include "gpHal.h"
#include "gpSched.h"
#include "hal_WB.h" //For HAL_TIMER_GET_CURRENT_TIME - see SW-2293

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

#define PD_CHECK_HANDLE_VALID(pdHandle)         GP_HAL_CHECK_PBM_VALID(pdHandle)
#define PD_CHECK_HANDLE_IN_USE(pdHandle)        GP_HAL_IS_PBM_ALLOCATED(pdHandle)
#define PD_CHECK_HANDLE_ACCESSIBLE(pdHandle)    (PD_CHECK_HANDLE_VALID(pdHandle) && PD_CHECK_HANDLE_IN_USE(pdHandle))

#define PD_MAKE_OFFSET_VALID(offset)            (offset &= 0x7f)
#define PD_HANDLE_TO_ADDRESS(pdHandle)          GP_HAL_PBM_ENTRY2ADDR(pdHandle)

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/



/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

//Packet buffers
#ifdef USE_PRAGMA
#pragma USER_DATA_MAPPING GEN_EXTRAM_SECTION
#endif // USE_PRAGMA
gpPd_Descriptor_t gpPd_Descriptors[GP_PD_NR_OF_HANDLES] GP_EXTRAM_SECTION_ATTR;
#ifdef USE_PRAGMA
#pragma USER_DATA_MAPPING
#endif // USE_PRAGMA

#if GPHAL_NUMBER_OF_PBMS_USED <= 8
UInt8 Pd_BitMaskOfPbmsClaimedBySw = 0;
#else
UInt32 Pd_BitMaskOfPbmsClaimedBySw = 0;
#endif

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

static void    Pd_Init(void);

//Descriptor handling
static gpPd_Handle_t Pd_GetPd(void);
static void          Pd_FreePd(gpPd_Handle_t pdHandle);
static gpPd_Result_t Pd_CheckPdValid(gpPd_Handle_t pdHandle);

//Data handling
static UInt8         Pd_ReadByte(gpPd_Handle_t pdHandle, gpPd_Offset_t offset);
static void          Pd_WriteByte(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 byte);
static void          Pd_ReadByteStream(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 length, UInt8* pData);
static void          Pd_WriteByteStream(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 length, UInt8* pData);

//Properties handling
static gpPd_Rssi_t  Pd_GetRssi(gpPd_Handle_t pdHandle);
static void         Pd_SetRssi(gpPd_Handle_t pdHandle, gpPd_Rssi_t rssi);
static gpPd_Lqi_t   Pd_GetLqi(gpPd_Handle_t pdHandle);
static void         Pd_SetLqi(gpPd_Handle_t pdHandle, gpPd_Lqi_t lqi);
static UInt32       Pd_GetRxTimestamp(gpPd_Handle_t pdHandle);
static void         Pd_SetRxTimestamp(gpPd_Handle_t pdHandle, gpPd_TimeStamp_t timestamp);
static UInt32       Pd_GetTxTimestamp(gpPd_Handle_t pdHandle);

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

static void Pd_Init(void)
{
    UIntLoop i;

    COMPILE_TIME_ASSERT((sizeof(Pd_BitMaskOfPbmsClaimedBySw)*8) >= GP_PD_NR_OF_HANDLES);

    for(i = 0; i<GP_PD_NR_OF_HANDLES; i++)
    {
        MEMSET(&gpPd_Descriptors[i], 0, sizeof(gpPd_Descriptor_t));
    }
    Pd_BitMaskOfPbmsClaimedBySw = 0;
}

// for waking up the radio we only take PBM's into account claimed by the software
// So pbm's claimed for outgoing packets. Packets for incomming packets are related to interrupts.
static gpPd_Handle_t Pd_GetPd(void)
{
    UInt8 pdHandle;
    if( Pd_BitMaskOfPbmsClaimedBySw == 0 )
    {
        gpHal_GoToSleepWhenIdle(false);
    }
    pdHandle = gpHal_GetHandle();
    if(!GP_HAL_CHECK_PBM_VALID(pdHandle))
    {
        if( Pd_BitMaskOfPbmsClaimedBySw == 0 )
        {
            gpHal_GoToSleepWhenIdle(true);
        }
        return GP_PD_INVALID_HANDLE;
    }

    BIT_SET(Pd_BitMaskOfPbmsClaimedBySw,pdHandle);
    GP_LOG_PRINTF("Get %i",0, (UInt16)pdHandle);

    return pdHandle;
}

static void Pd_FreePd(gpPd_Handle_t pdHandle)
{
    GP_LOG_PRINTF("Free %i",0, (UInt16)pdHandle);
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));
    gpHal_FreeHandle(pdHandle);
    if( Pd_BitMaskOfPbmsClaimedBySw)
    {
        BIT_CLR(Pd_BitMaskOfPbmsClaimedBySw,pdHandle);
        if( Pd_BitMaskOfPbmsClaimedBySw == 0 )
        {
            gpHal_GoToSleepWhenIdle(true);
        }
    }
}

static gpPd_Result_t Pd_CheckPdValid(gpPd_Handle_t pdHandle)
{
    gpPd_Result_t result = gpPd_ResultValidHandle;

    if(!PD_CHECK_HANDLE_VALID(pdHandle))
    {
        result = gpPd_ResultInvalidHandle;
    }
    else if(!PD_CHECK_HANDLE_IN_USE(pdHandle))
    {
        result = gpPd_ResultNotInUse;
    }
    GP_LOG_PRINTF("Check %i %i",0, (UInt16)pdHandle, (UInt16)result);

    return result;
}

//------------------------------
// Data
//------------------------------

static UInt8 Pd_ReadByte(gpPd_Handle_t pdHandle, gpPd_Offset_t offset)
{
    gpHal_Address_t address;

    GP_LOG_PRINTF("R %i %i",0, (UInt16)pdHandle, (UInt16)offset);
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));

    PD_MAKE_OFFSET_VALID(offset);
    address = PD_HANDLE_TO_ADDRESS(pdHandle);

    return gpHal_ReadByteInPBMCyclic(address,offset);
}

static void Pd_WriteByte(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 byte)
{
    gpHal_Address_t address;

    GP_LOG_PRINTF("W %i %i %x",0, (UInt16)pdHandle, (UInt16)offset, (UInt16)byte);
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));

    PD_MAKE_OFFSET_VALID(offset);
    address = PD_HANDLE_TO_ADDRESS(pdHandle);

    gpHal_WriteByteInPBMCyclic(address,offset, byte);
}

static void Pd_ReadByteStream(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 length, UInt8* pData)
{
    gpHal_Address_t address;

    GP_LOG_PRINTF("RS %i %i %i",0, (UInt16)pdHandle, (UInt16)offset, (UInt16)length);
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));

    PD_MAKE_OFFSET_VALID(offset);
    address = PD_HANDLE_TO_ADDRESS(pdHandle);

    gpHal_ReadDataInPBMCyclic(address,offset,pData,length);
}

static void Pd_WriteByteStream(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 length, UInt8* pData)
{
    gpHal_Address_t address;

    GP_LOG_PRINTF("WS %i %i %i",0, (UInt16)pdHandle, (UInt16)offset, (UInt16)length);
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));

    PD_MAKE_OFFSET_VALID(offset);
    address = PD_HANDLE_TO_ADDRESS(pdHandle);

    gpHal_WriteDataInPBMCyclic(address,offset,pData,length);
}

//------------------------------
// Attributes
//------------------------------

static gpPd_Rssi_t Pd_GetRssi(gpPd_Handle_t pdHandle)
{
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));
    return gpPd_Descriptors[pdHandle].attr.rx.rssi;
}

static void Pd_SetRssi(gpPd_Handle_t pdHandle, gpPd_Rssi_t rssi)
{
    //Not to be overwritten
    GP_ASSERT_DEV_EXT(false);
}

static gpPd_Lqi_t Pd_GetLqi(gpPd_Handle_t pdHandle)
{
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));
    return gpPd_Descriptors[pdHandle].attr.rx.lqi;
}

static void Pd_SetLqi(gpPd_Handle_t pdHandle, gpPd_Lqi_t lqi)
{
    //Not to be overwritten
    GP_ASSERT_DEV_EXT(false);
}

static UInt32 Pd_GetRxTimestamp(gpPd_Handle_t pdHandle)
{
    UInt32 timeStamp;
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));
    gpHal_GetRxTimestamp(pdHandle, &timeStamp);


    return timeStamp;
}

static void Pd_SetRxTimestamp(gpPd_Handle_t pdHandle, gpPd_TimeStamp_t timestamp)
{
    //Not to be overwritten in PBM
    GP_ASSERT_DEV_EXT(false);
}

static UInt32 Pd_GetTxTimestamp(gpPd_Handle_t pdHandle)
{
    UInt32 timeStamp;
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));
    gpHal_GetTxTimestamp(pdHandle, &timeStamp);

    
    return timeStamp;
}


//------------------------------
// Helper functions
//------------------------------

static gpPd_Handle_t Pd_CopyPd( gpPd_Handle_t pdHandle)
{
    gpPd_Handle_t pdHandleCopy = gpPd_GetPd();
    if(pdHandleCopy != GP_PD_INVALID_HANDLE )
    {
        MEMCPY(&gpPd_Descriptors[pdHandleCopy].attr,  &gpPd_Descriptors[pdHandle].attr, sizeof(gpPd_Descriptors[pdHandle].attr));
        gpHal_MakeBareCopyPBM(pdHandle,pdHandleCopy);
    }
    return pdHandleCopy;
}

//--------------------
// Request/indications
//--------------------

static UInt8 Pd_DataRequest(gpPd_Loh_t *p_PdLoh)
{
    GP_ASSERT_DEV_EXT(p_PdLoh);
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(p_PdLoh->handle));
    return p_PdLoh->handle;
}

static void Pd_cbDataConfirm(UInt8 pbmHandle, gpPd_Offset_t pbmOffset, UInt8 pbmLength, gpPd_Loh_t *p_PdLoh)
{
    GP_ASSERT_DEV_EXT(p_PdLoh);
    p_PdLoh->length = pbmLength;
    p_PdLoh->offset = pbmOffset;
    p_PdLoh->handle = pbmHandle;
}

static void Pd_DataIndication(UInt8 pbmHandle, gpPd_Offset_t pbmOffset, UInt8 pbmLength, gpPd_Loh_t *p_PdLoh)
{
    GP_ASSERT_DEV_EXT(p_PdLoh);
    p_PdLoh->length = pbmLength;
    p_PdLoh->offset = pbmOffset;
    p_PdLoh->handle = pbmHandle;
    gpPd_Descriptors[p_PdLoh->handle].attr.rx.lqi = gpHal_GetLQI(pbmHandle);
    gpPd_Descriptors[p_PdLoh->handle].attr.rx.rssi = gpHal_GetRSSI(pbmHandle);
}

static UInt8 Pd_SecRequest(gpPd_Handle_t pdHandle, UInt8 dataOffset, UInt8 dataLength, UInt8 auxOffset, UInt8 auxLength )
{
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));
    return pdHandle;
}

static gpPd_Handle_t Pd_cbSecConfirm(UInt8 pbmHandle, UInt8 dataOffset, UInt8 dataLength)
{

    return pbmHandle;
}

static UInt8 Pd_PurgeRequest(gpPd_Handle_t pdHandle)
{
    //Pdhandle = PBMentry in this case
    //Check if Pd valid and Pd in use !
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));
    return pdHandle;
}

static void Pd_cbPurgeConfirm(UInt8 pbmHandle)
{
    //Do nothing
}

