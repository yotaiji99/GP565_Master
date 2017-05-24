/*
* Copyright (c) 2011-2013, GreenPeak Technologies
*
*
*  The file contains the definitions for transmitting generic IR codes
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
*     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/comps/gpIrTx/inc/gpIrTx_Generic.h#1 $
*    M'   GreenPeak              $Change: 77946 $
*   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
*  F
*/

#ifndef _GP_IR_TX_GENERIC_H_
#define _GP_IR_TX_GENERIC_H_

/*****************************************************************************
*                    Includes Definitions
*****************************************************************************/

#include <global.h>

/**
 * @file gpIrTx_Generic.h
 *
 * @brief This file contains definitions specific to generic part of IrTx
 *
 * This interface file shouldn't be directly included by the user application.
 * If needed, it will be included automaticaly by the main interface file.
 *
 */

/*****************************************************************************
*                    Macro Definitions
*****************************************************************************/

/** @brief  Maximal size of the generic IR signal timing descriptor */
#define GP_IR_TX_MAX_IRCODE_SIZE                          80

/*****************************************************************************
*                   Functional Macro Definitions
*****************************************************************************/

/*****************************************************************************
*                    Type Definitions
*****************************************************************************/

/** @struct gpIrTx_TransmissionDefinition
 *  @brief The gpIrTx_TransmissionDefinition structure is used to describe the transmission definition header */
/** @typedef gpIrTx_TransmissionDefinition_t
 *  @brief The gpIrTx_TransmissionDefinition_t type is used to describe the transmission definition header. 
 */

/**
 *  The detailed description of the fields in the gpIrTx_TransmissionDefinition structure is based on gpIrTx_ConfigMode setting gpIrTx_ConfigModeGp.
 *  More details can be found in GP_P215_AS_03407_Generic_IRDB_structures. The interpretation of the fields will be slightly
 *  different if gpIrTx_ConfigModeZrc mode is selected (see Annex B: Standard IR Database format of ZigBee RF4CE: ZRC Profile Specification Version 2.0)
 */

typedef struct gpIrTx_TransmissionDefinition {
/**
 * controlFlag1 first control byte. Bit functions:
 *          - bit0 Repeat Status: 0= Continuous Tx; 1= Discontinuous Tx
 *          - bit1 Timing Data Type: 0= Mark/Space period pair; 1=Single Mark and Single Space
 *          - bit2 Carrier Type: 0=No Carrier (Pulse mode; 1=use carrier as defined in Carrier Period field
 *          - bit3 Data Type: 0=Normal Data; 1=Toggle data
 *          - bit4 Clock Frequency: 0=8MHz; 1=16MHz  (Default 8MHz)
 *          - bit5...bit7 reserved
 */

    UInt8  controlFlag1;
/**
 * controlFlag2 second control byte. Bit functions:
 *          - bit0...bit3 Number of Repeats (4 bits value)
 *          - bit4...bit7 Structure Revision level
 */
    UInt8  controlFlag2;
/**
 * numOfEntries Number of Timing data entries (n)
 */
    UInt8  numOfEntries;
/**
 * numOfBitsFirstFrame Number of bits in 1st frame (x)
 */
    UInt8  numOfBitsFirstFrame;
/**
 * numOfBitsRepeatFrame Number of bits in repeat frame (y)
 */
    UInt8  numOfBitsRepeatFrame;
/**
 * numOfBitsReleaseFrame Number of bits in release frame (z)
 */
    UInt8  numOfBitsReleaseFrame;
/**
 * numOfBitsToggle Number of symbols in toggle sequences (t) - ignored in GP config mode
 */
    UInt8 numOfBitsToggle;
/**
 * numOfToggleSequences Number of toggle sequences in toggle definition (s) - ignored in GP config mode
 */
    UInt8 numOfToggleSequences;

} gpIrTx_TransmissionDefinition_t;

/** @typedef gpIrTx_TransmissionCarrierTime_t
 *  @brief The gpIrTx_TransmissionCarrierTime_t type is used to describe carrier period (in 0x.125 us)
 */
typedef UInt16 gpIrTx_TransmissionCarrierTime_t;  /* carrierTime (in 0.125us) Carrier Period */

/** @struct gpIrTx_TvIrDesc
 *  @brief The gpIrTx_TvIrDesc structure is used to describe the IR signal timing */
/** @typedef gpIrTx_TvIrDesc_t
 *  @brief The gpIrTx_TvIrDesc_t type is used to describe the IR signal timing
 *
 *  For more detail refer to [add link TBD]
 */
typedef struct gpIrTx_TvIrDesc{
/** code Array containing generic code describing IR signal timing */
    UInt8  code[GP_IR_TX_MAX_IRCODE_SIZE];
} gpIrTx_TvIrDesc_t;

#endif // _GP_IR_TX_GENERIC_H_
