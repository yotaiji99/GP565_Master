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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpPd/inc/gpPd.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GPPD_H_
#define _GPPD_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <global.h>
// inclusion for number of pbms
#include "gpHal_Pbm.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#ifndef GP_PD_NR_OF_HANDLES
#define GP_PD_NR_OF_HANDLES         GPHAL_NUMBER_OF_PBMS_USED
#endif

#define GP_PD_INVALID_HANDLE           0xFF

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef UInt8 gpPd_Handle_t;
typedef UInt8 gpPd_Offset_t;
typedef UInt32 gpPd_TimeStamp_t;
typedef Int8 gpPd_Rssi_t;
typedef UInt8 gpPd_Lqi_t;


#define gpPd_ResultValidHandle        0x0
#define gpPd_ResultInvalidHandle      0x1
#define gpPd_ResultNotInUse           0x2
typedef UInt8 gpPd_Result_t;

typedef struct gpPd_Loh_s{
    UInt8           length;
    gpPd_Offset_t   offset;
    gpPd_Handle_t   handle;
}gpPd_Loh_t; // Length Offset Handle

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void    gpPd_Init(void);

//Descriptor handling
gpPd_Handle_t gpPd_GetPd(void);
void          gpPd_FreePd(gpPd_Handle_t pdHandle);
gpPd_Result_t gpPd_CheckPdValid(gpPd_Handle_t pdHandle);

//Data handling
UInt8         gpPd_ReadByte(gpPd_Handle_t pdHandle, gpPd_Offset_t offset);
void          gpPd_WriteByte(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 byte);
void          gpPd_ReadByteStream(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 length, UInt8* pData);
void          gpPd_WriteByteStream(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 length, UInt8* pData);

//Data handling with update of pdLoh
void          gpPd_AppendWithUpdate(gpPd_Loh_t *pPdLoh, UInt8 length, UInt8 const *pData);
void          gpPd_PrependWithUpdate(gpPd_Loh_t *pPdLoh, UInt8 length, UInt8 const *pData);
void          gpPd_ReadWithUpdate(gpPd_Loh_t *pPdLoh, UInt8 length, UInt8 *pData);

//Properties handling
//Rx
gpPd_Rssi_t         gpPd_GetRssi(gpPd_Handle_t pdHandle);
gpPd_Lqi_t          gpPd_GetLqi(gpPd_Handle_t pdHandle);
gpPd_TimeStamp_t    gpPd_GetRxTimestamp(gpPd_Handle_t pdHandle);

//TxConfirm
gpPd_TimeStamp_t    gpPd_GetTxTimestamp(gpPd_Handle_t pdHandle);

//Helper functions
void                gpPd_SetRssi(gpPd_Handle_t pdHandle, gpPd_Rssi_t rssi);
void                gpPd_SetLqi(gpPd_Handle_t pdHandle, gpPd_Lqi_t lqi);
void                gpPd_SetRxTimestamp(gpPd_Handle_t pdHandle, gpPd_TimeStamp_t timestamp);

gpPd_Handle_t       gpPd_CopyPd(gpPd_Handle_t pdHandle);

//Data
UInt8         gpPd_DataRequest(gpPd_Loh_t *p_PdLoh);
void          gpPd_cbDataConfirm(UInt8 pbmHandle, UInt8 pbmOffset, UInt8 pbmLength, gpPd_Loh_t *p_PdLoh);
void          gpPd_DataIndication(UInt8 pbmHandle, UInt8 pbmOffset, UInt8 pbmLength, gpPd_Loh_t *p_PdLoh);

//Security
UInt8         gpPd_SecRequest(gpPd_Handle_t pdHandle, UInt8 dataOffset, UInt8 dataLength, UInt8 auxOffset, UInt8 auxLength );
gpPd_Handle_t gpPd_cbSecConfirm(UInt8 pbmHandle, UInt8 dataOffset, UInt8 dataLength);

//Purge
UInt8         gpPd_PurgeRequest(gpPd_Handle_t pdHandle);
void          gpPd_cbPurgeConfirm(UInt8 pbmHandle);

#include "gpPd_pbm.h"


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // _GPPD_H_


