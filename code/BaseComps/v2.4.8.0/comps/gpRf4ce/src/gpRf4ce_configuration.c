/*
 * Copyright (c) 2010-2013, GreenPeak Technologies
 *
 * gpRf4ce_configuration.c
 *   This file contains the data structures for storing pairing entries used by the gpRf4ce component
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpRf4ce/src/gpRf4ce_configuration.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpRf4ce.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_RF4CE

/** The manufacturer specific vendor identifier for this node. */
#ifndef GP_RF4CE_NWKC_VENDOR_IDENTIFIER
#define GP_RF4CE_NWKC_VENDOR_IDENTIFIER gpRf4ce_VendorIdTestVendor1	//gpRf4ce_VendorIdGreenpeak
#endif

/** The manufacturer specific identification string for this node. */
#ifndef GP_RF4CE_NWKC_VENDOR_STRING
#define GP_RF4CE_NWKC_VENDOR_STRING
#endif

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
//Pairing Table
const UInt8 ROM gpRf4ce_nwkCMaxPairingTableEntries FLASH_PROGMEM = GP_RF4CE_NWKC_MAX_PAIRING_TABLE_ENTRIES;

#ifdef USE_PRAGMA
#pragma USER_DATA_MAPPING GEN_EXTRAM_SECTION
#endif //USE PRAGMA

gpRf4ce_PairingTableEntryTotal_t    gpRf4ce_PairingTable[GP_RF4CE_NWKC_MAX_PAIRING_TABLE_ENTRIES] GP_EXTRAM_SECTION_ATTR;

#ifdef USE_PRAGMA
#pragma USER_DATA_MAPPING
#endif //USE PRAGMA


const gpRf4ce_VendorInfo_t ROM gpRf4ce_VendorInfoDefault FLASH_PROGMEM = {
  GP_RF4CE_NWKC_VENDOR_IDENTIFIER ,
  { XSTRINGIFY(GP_RF4CE_NWKC_VENDOR_STRING) }
};

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/


/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
