/*
 * Copyright (c) 2013-2014, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/inc/gpHal_DP.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */



#ifndef _GPHAL_DATAPENDING_H_
#define _GPHAL_DATAPENDING_H_

/** @file gpHal_DataPending.h
 *  @brief This file contains all the functions needed for DataPending functionality.
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gp_global.h"

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef union {
/** The short address (2 bytes).*/
    UInt16  Short;
/** The extended address (MAC address), 8 bytes.*/
    MACAddress_t Extended;
} gpHal_RfAddress_t;

typedef struct {
    gpHal_RfAddress_t address;
    UInt16 panId;
    UInt8  addressMode;
} gpHal_AddressInfo_t;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

gpHal_Result_t gpHal_DpClearEntries(UInt8 id);

gpHal_Result_t gpHal_DpAddEntry(gpHal_AddressInfo_t* pAddressInfo, UInt8 id);
gpHal_Result_t gpHal_DpRemoveEntry(gpHal_AddressInfo_t* pAddressInfo, UInt8 id);

#endif  /* _GPHAL_DATAPENDING_H_ */

