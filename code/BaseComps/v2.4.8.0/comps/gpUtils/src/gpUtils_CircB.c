/*
 * Copyright (c) 2008-2012,2014, GreenPeak Technologies
 *
 * gpUtils_circB.c
 *
 * This file contains the circular buffer features of the Utils component.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpUtils/src/gpUtils_CircB.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_UTILS

#include "hal.h"
#include "gpUtils.h"
#include "gpLog.h"
#include "gpAssert.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    External Data Definitions
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

void
gpUtils_CircBInit (gpUtils_CircularBuffer_t* pCircularBuffer, void * pBuffer, UInt16 size) {
    pCircularBuffer->pBuffer = (UInt8*)pBuffer;
    pCircularBuffer->size = size;
    gpUtils_CircBClear(pCircularBuffer);
}

void
gpUtils_CircBClear (gpUtils_CircularBuffer_t* pCircularBuffer) {
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());

    pCircularBuffer->readIndex = 0;
    pCircularBuffer->writeIndex = 0;
    pCircularBuffer->full = false;
}

UInt16
gpUtils_CircBAvailableData (gpUtils_CircularBuffer_t* pCircularBuffer) {
    UInt32 availableData;
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());

    availableData = pCircularBuffer->size + pCircularBuffer->writeIndex - pCircularBuffer->readIndex;
    availableData %= pCircularBuffer->size;
    
    return (UInt16)(pCircularBuffer->full ? pCircularBuffer->size : availableData);
}

UInt16
gpUtils_CircBAvailableSpace (gpUtils_CircularBuffer_t* pCircularBuffer) {
    UInt32 availableSpace;
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());

    if(pCircularBuffer->readIndex == pCircularBuffer->writeIndex)
        return (pCircularBuffer->full ? 0 : pCircularBuffer->size);

    availableSpace = pCircularBuffer->size + pCircularBuffer->readIndex - pCircularBuffer->writeIndex;
    availableSpace %= pCircularBuffer->size;
    return (UInt16)availableSpace;
}

Bool
gpUtils_CircBWriteData (gpUtils_CircularBuffer_t* pCircularBuffer, UInt8* pData, UInt16 length) {
    // Check if AtomicOn
    UInt16 i;
    UInt16 availableSpace;
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());

    availableSpace = gpUtils_CircBAvailableSpace(pCircularBuffer);
    if(availableSpace < length)
        return false;

    for(i = 0; i < length; ++i){
        pCircularBuffer->pBuffer[pCircularBuffer->writeIndex] = pData[i];
        pCircularBuffer->writeIndex++;
        pCircularBuffer->writeIndex %= pCircularBuffer->size;
    }

    pCircularBuffer->full = (availableSpace == length);

    return true;
}

Bool
gpUtils_CircBWriteByte (gpUtils_CircularBuffer_t* pCircularBuffer , UInt8 Data) {
    return gpUtils_CircBWriteData(pCircularBuffer, &Data, 1);
}

Bool
gpUtils_CircBReadData (gpUtils_CircularBuffer_t* pCircularBuffer, UInt8* pData, UInt16 length) {
    UInt16 i;
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());

    if(gpUtils_CircBAvailableData(pCircularBuffer) < length)
        return false;

    for(i = 0; i < length; ++i){
        pData[i] = pCircularBuffer->pBuffer[pCircularBuffer->readIndex];
        pCircularBuffer->readIndex++;
        pCircularBuffer->readIndex %= pCircularBuffer->size;
    }

    pCircularBuffer->full = false;

    return true;
}

Bool
gpUtils_CircBReadByte (gpUtils_CircularBuffer_t* pCircularBuffer , UInt8* pData) {
    return gpUtils_CircBReadData(pCircularBuffer, pData, 1);
}

Bool
gpUtils_CircBGetData (gpUtils_CircularBuffer_t* pCircularBuffer , UInt16 index, UInt8* pData, UInt16 length) {
    UInt16 i;
    UInt16 readIndex;
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());

    if(gpUtils_CircBAvailableData(pCircularBuffer) < (length + index))
        return false;

    readIndex = pCircularBuffer->readIndex + index;
    readIndex %= pCircularBuffer->size;
        
    for(i = 0; i < length; ++i){
        pData[i] = pCircularBuffer->pBuffer[readIndex];
        readIndex++;
        readIndex %= pCircularBuffer->size;
    }

    return true;
}

Bool
gpUtils_CircBGetByte (gpUtils_CircularBuffer_t* pCircularBuffer , UInt16 index, UInt8* pData) {
    return gpUtils_CircBGetData(pCircularBuffer, index, pData, 1);
}


