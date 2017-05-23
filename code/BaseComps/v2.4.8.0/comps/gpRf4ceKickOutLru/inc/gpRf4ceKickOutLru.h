/*
 * Copyright (c) 2011-2012, GreenPeak Technologies
 *
 *   This file contains the definitions of the public functions and enumerations of the gpRf4ceDispatcher.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpRf4ceKickOutLru/inc/gpRf4ceKickOutLru.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */
 
#ifndef _GP_RF4CE_KICK_OUT_LRU_H_
#define _GP_RF4CE_KICK_OUT_LRU_H_

/**
 * @file gpRf4ceKickOutLru.h
 *
 * @defgroup KOL Kick-out LRU
 *
 * The Kick-out LRU (Least recently used) module is responsible for keeping track of the activity of binding references
 * in the pairing table.  It manages an ordered list of all pairing entries.  Each time a packet is received from a device,
 * this device is moved to the top of the ordered list.
 *
 * The ordered list will be used for selecting the device, that should be removed in case the pairing table is full and
 * a new device is to be added to the pairing table.  When adding this new device, the Kick-out LRU module will tell which 
 * device is to be removed from the pairing table (i.e. the one from which the last packet was received the longest time ago).
 *
 */
 
/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
 
#include <global.h>
#include "gpRf4ce.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
/** @typedef gpRf4ceKickOutLru_Desc_t
 *  This struct contains a pairing ref and a profile id.
*/
typedef struct {
    UInt8 pairingRef;
    gpRf4ce_ProfileId_t profileId;
} gpRf4ceKickOutLru_Desc_t;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup KOL
 * This primitive initializes the Kick-out LRU block.
 */
GP_API void gpRf4ceKickOutLru_Init(void);

/** @ingroup KOL
 * 
 *  This function should be called when a new device has bind.  If the pairing table isn't full, the new item
 *  can be added without any further action.  If the pairing table is full, the new item will be added, but the 
 *  return value will indicate which entry should be removed in order to make room for new devices.
 *
 *  @param    pairingRef    The pairing reference of the item to add.
 *  @param    profileId     The profileId of the item to add.
 *  @return   pairing reference and profile Id from the device that should be removed. Pairingref is 0xFF if no pairing entry needs to be removed.
 */
GP_API gpRf4ceKickOutLru_Desc_t gpRf4ceKickOutLru_AddDeviceToList( UInt8 pairingRef , gpRf4ce_ProfileId_t profileId );


/** @ingroup KOL
 *
 *  This function should be called when a device has unbound in order to update the ordered list.
 *
 *  @param  pairingRef    The pairing reference of the item to remove.
 */
GP_API void gpRf4ceKickOutLru_RemoveDeviceFromList( UInt8 pairingRef );

GP_API UInt8 gpRf4ceKickOutLru_GetList( UInt8* pList , UInt8 size);

#ifdef __cplusplus
}
#endif
#endif //_GP_RF4CE_KICK_OUT_LRU_H_

