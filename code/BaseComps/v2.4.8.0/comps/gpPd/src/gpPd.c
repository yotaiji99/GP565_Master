/*
 * Copyright (c) 2012-2014, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpPd/src/gpPd.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/



// General includes
#include "gpLog.h"
#include "gpAssert.h"
#include "gpStat.h"

#include "gpPd.h"

 

#define GP_COMPONENT_ID GP_COMPONENT_ID_PD
#define GP_MODULE_ID GP_MODULE_ID_PD

#include "gpPd_pbm.c"
#include "gpPd_common.c"

//#define GP_LOCAL_LOG

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpPd_Init(void)
{
    Pd_Init();
}

gpPd_Handle_t gpPd_GetPd(void)
{
    GP_STAT_SAMPLE_TIME();
    return Pd_GetPd();
}

void gpPd_FreePd(gpPd_Handle_t pdHandle)
{
    GP_STAT_SAMPLE_TIME();
    Pd_FreePd( pdHandle);
}

gpPd_Result_t gpPd_CheckPdValid(gpPd_Handle_t pdHandle)
{
    return Pd_CheckPdValid( pdHandle);
}

UInt8 gpPd_ReadByte(gpPd_Handle_t pdHandle, gpPd_Offset_t offset)
{ 
    return Pd_ReadByte( pdHandle, offset);
}

void gpPd_WriteByte(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 byte)
{
    Pd_WriteByte( pdHandle, offset, byte);
}

void gpPd_ReadByteStream(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 length, UInt8* pData)
{
    Pd_ReadByteStream( pdHandle, offset, length, pData);
}

void gpPd_WriteByteStream(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 length, UInt8* pData)
{
    Pd_WriteByteStream( pdHandle, offset, length, pData);
}

//Convenience functions
void gpPd_AppendWithUpdate(gpPd_Loh_t *pPdLoh, UInt8 length, UInt8 const *pData)
{
    Pd_AppendWithUpdate(pPdLoh, length, pData);
}
void gpPd_PrependWithUpdate(gpPd_Loh_t *pPdLoh, UInt8 length, UInt8 const *pData)
{
    Pd_PrependWithUpdate(pPdLoh, length, pData);
}
void gpPd_ReadWithUpdate(gpPd_Loh_t *pPdLoh, UInt8 length, UInt8 *pData)
{
    Pd_ReadWithUpdate(pPdLoh, length, pData);
}

//Properties handling

gpPd_Rssi_t gpPd_GetRssi(gpPd_Handle_t pdHandle)
{
    return Pd_GetRssi(pdHandle);
}

void gpPd_SetRssi(gpPd_Handle_t pdHandle, gpPd_Rssi_t rssi)
{
    Pd_SetRssi(pdHandle, rssi);
}

gpPd_Lqi_t gpPd_GetLqi(gpPd_Handle_t pdHandle)
{
    return Pd_GetLqi(pdHandle);
}

void gpPd_SetLqi(gpPd_Handle_t pdHandle, gpPd_Lqi_t lqi)
{
    Pd_SetLqi(pdHandle, lqi);
}

gpPd_TimeStamp_t gpPd_GetRxTimestamp(gpPd_Handle_t pdHandle)
{
    return Pd_GetRxTimestamp( pdHandle);
}

gpPd_TimeStamp_t gpPd_GetTxTimestamp(gpPd_Handle_t pdHandle)
{
    return Pd_GetTxTimestamp( pdHandle);
}

void gpPd_SetRxTimestamp(gpPd_Handle_t pdHandle, gpPd_TimeStamp_t timestamp)
{
    Pd_SetRxTimestamp(pdHandle, timestamp);
}

gpPd_Handle_t gpPd_CopyPd(gpPd_Handle_t pdHandle)
{
    return Pd_CopyPd( pdHandle );
}



UInt8 gpPd_DataRequest(gpPd_Loh_t *p_PdLoh)
{
    return Pd_DataRequest(p_PdLoh);
}

void gpPd_cbDataConfirm(UInt8 pbmHandle, UInt8 pbmOffset, UInt8 pbmLength, gpPd_Loh_t *p_PdLoh)
{
    Pd_cbDataConfirm(pbmHandle, pbmOffset, pbmLength, p_PdLoh);
}

UInt8 gpPd_SecRequest(gpPd_Handle_t pdHandle, UInt8 dataOffset, UInt8 dataLength, UInt8 auxOffset, UInt8 auxLength )
{
    return Pd_SecRequest(pdHandle, dataOffset, dataLength, auxOffset,auxLength );
}

gpPd_Handle_t gpPd_cbSecConfirm(UInt8 pbmHandle, UInt8 dataOffset, UInt8 dataLength)
{
    return Pd_cbSecConfirm(pbmHandle, dataOffset, dataLength);
}

void gpPd_DataIndication(UInt8 pbmHandle, UInt8 pbmOffset, UInt8 pbmLength, gpPd_Loh_t *p_PdLoh)
{
    Pd_DataIndication(pbmHandle, pbmOffset, pbmLength, p_PdLoh);
}

UInt8 gpPd_PurgeRequest(gpPd_Handle_t pdHandle)
{
    return Pd_PurgeRequest(pdHandle);
}

void gpPd_cbPurgeConfirm(UInt8 pbmHandle)
{
    Pd_cbPurgeConfirm(pbmHandle);
}



