/*
 * Copyright (c) 2011,2013,2015, GreenPeak Technologies
 *
 *   This file contains the definitions of the public functions and enumerations of the gpIrTx_Generic_defs.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.6.1.0/sw/comps/gpIrTx/src/gpIrTx_Generic_defs.h#1 $
 *    M'   GreenPeak              $Change: 91078 $
 *   0'         Technologies      $DateTime: 2017/02/09 23:55:46 $
 *  F
 */

#ifndef _GP_IR_TX_GENERIC_DEFS_H_
#define _GP_IR_TX_GENERIC_DEFS_H_

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

extern gpIrTx_ConfigMode_t IrTx_ConfigMode;

/*  DIFFERENCES BETWEEN GP AND ZRC CONFIGMODE
 * - repeat mode
 *   GP:  controlFlag1[0] -> discontinuous - 0 continuous
 *   ZRC: controlFlag1[0] -> 0 discontinuous - 1 continuous
 * - mark space definition
 *   ignored in both config modes
 * - carrier type
 *   GP:  controlFlag1[2] -> 1 carrier - 0 no carrier
 *   ZRC: controlFlag1[1] -> 1 carrier - 0 no carrier
 * - toggle mode
 *   GP:  controlFlag1[4] -> 1 toggle - 0 no toggle
 *   ZRC: deduced from (s) field in definition
 *
 * size of the definition is not depending on config mode but on struct version controlFlag2[7:4]
 *   GP:  can support struct version 0 (bytes 6 (t) and 7 (s) will be ignored) and 1
 *   ZRC: only supports struct version 0 as (t) and (s) are used
 */

//Control flags offsets
#define IR_TX_OFFSET_CONTROL_FLAG_1             0
#define IR_TX_OFFSET_CONTROL_FLAG_2             1
#define IR_TX_OFFSET_NUM_ENTRIES                2
#define IR_TX_OFFSET_NUM_BITS_FIRST_FRAME       3
#define IR_TX_OFFSET_NUM_BITS_REPEAT_FRAME      4
#define IR_TX_OFFSET_NUM_BITS_RELEASE_FRAME     5
#define IR_TX_OFFSET_NUM_BITS_TOGGLE            6
#define IR_TX_OFFSET_NUM_TOGGLE_SEQUENCES       7

//Control flags
#define GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_ENTRIES(code)      (code[IR_TX_OFFSET_NUM_ENTRIES])
#define GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_FIRST(code)   (code[IR_TX_OFFSET_NUM_BITS_FIRST_FRAME])
#define GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_REPEAT(code)  (code[IR_TX_OFFSET_NUM_BITS_REPEAT_FRAME])
#define GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_RELEASE(code) (code[IR_TX_OFFSET_NUM_BITS_RELEASE_FRAME])
#define GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_TOGGLE(code)  (code[IR_TX_OFFSET_NUM_BITS_TOGGLE])

#define GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_OF_REPEATS(code)  (code[IR_TX_OFFSET_CONTROL_FLAG_2] & 0x0F)
#define GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_STRUCT_VERSION(code)  ((code[IR_TX_OFFSET_CONTROL_FLAG_2] & 0xF0) >> 4)
#define GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_REPEAT_MODE(code)     !!((code[IR_TX_OFFSET_CONTROL_FLAG_1] & 0x01) ^ (IrTx_ConfigMode == gpIrTx_ConfigModeZrc ? 0x01 : 0x00))
#define GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_CARRIER_TYPE(code)    !!(code[IR_TX_OFFSET_CONTROL_FLAG_1] & (IrTx_ConfigMode == gpIrTx_ConfigModeZrc ? 0x02 : 0x04))
#define GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_TOGGLE_MODE(code)     !!( IrTx_ConfigMode == gpIrTx_ConfigModeZrc ? code[IR_TX_OFFSET_NUM_TOGGLE_SEQUENCES] : code[IR_TX_OFFSET_CONTROL_FLAG_1] & 0x08)

#define GP_IR_TX_GENERIC_OVERRULE_FLAGS_OVERRULE(flags)          !!((flags) & 0x40)
#define GP_IR_TX_GENERIC_OVERRULE_FLAGS_FORCE_DISC(flags)        !((flags) & 0x10)
#define GP_IR_TX_GENERIC_OVERRULE_FLAGS_FORCE_REPCNT(flags)      !(((flags) & 0x0F) == 0x0F)
#define GP_IR_TX_GENERIC_OVERRULE_FLAGS_GET_N_OF_REPEATS(flags)  ((flags) & 0x0F)

//Offsets
#define GP_IR_TX_IRCODE_TXCARRIERTIME_OFFSET(code)  (GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_STRUCT_VERSION(code) > 0  ? 6 : 8)
#define GP_IR_TX_IRCODE_TXTIMINGDATA_OFFSET(code)   (GP_IR_TX_IRCODE_TXCARRIERTIME_OFFSET((code)) + 2 )
#define GP_IR_TX_IRCODE_BITSEQUENCE_OFFSET(code)    (GP_IR_TX_IRCODE_TXTIMINGDATA_OFFSET((code))  + (((code)[2])<<2) )

//CarrierTime
#define GP_IR_TX_IRCODE_TXCARRIERTIME(code)          ((code)[GP_IR_TX_IRCODE_TXCARRIERTIME_OFFSET((code))] + ((code)[GP_IR_TX_IRCODE_TXCARRIERTIME_OFFSET((code))+1] << 8))

//TimingData
#define GP_IR_TX_IRCODE_TXTIMINGDATA_SET_READ(code, index)  ((UInt16)((code)[GP_IR_TX_IRCODE_TXTIMINGDATA_OFFSET((code))+(index)*4]   + ((code)[GP_IR_TX_IRCODE_TXTIMINGDATA_OFFSET((code))+(index)*4+1] << 8)))
#define GP_IR_TX_IRCODE_TXTIMINGDATA_SET_WRITE(code, index, value) MEMCPY((UInt8*)&value, &(code)[GP_IR_TX_IRCODE_TXTIMINGDATA_OFFSET((code))+(index)*4], 2)
#define GP_IR_TX_IRCODE_TXTIMINGDATA_CLR_READ(code, index)  ((UInt16)((code)[GP_IR_TX_IRCODE_TXTIMINGDATA_OFFSET((code))+(index)*4+2] + ((code)[GP_IR_TX_IRCODE_TXTIMINGDATA_OFFSET((code))+(index)*4+3] << 8)))
#define GP_IR_TX_IRCODE_TXTIMINGDATA_CLR_WRITE(code, index, value) MEMCPY((UInt8*)&value, &(code)[GP_IR_TX_IRCODE_TXTIMINGDATA_OFFSET((code))+(index)*4+2], 2)

//BitSequence
#define GP_IR_TX_IRCODE_BITSEQUENCE(code)                                        ((code) + GP_IR_TX_IRCODE_BITSEQUENCE_OFFSET((code)) )

#define GP_IR_TX_TOGGLE_N_OF_SEQUENCES(code)                     (IrTx_ConfigMode == gpIrTx_ConfigModeZrc ? (code[IR_TX_OFFSET_NUM_TOGGLE_SEQUENCES] + 1) : IR_TX_GENERIC_MAX_TOGGLE_SEQUENCES)
#define GP_IR_TX_TOGGLE_N_OF_SYMBOLS(code,pTogInfo)              (IrTx_ConfigMode == gpIrTx_ConfigModeZrc ? code[IR_TX_OFFSET_NUM_BITS_TOGGLE] : (pTogInfo)[1])
#define GP_IR_TX_TOGGLE_BITSEQUENCES(pTogInfo)                   (IrTx_ConfigMode == gpIrTx_ConfigModeZrc ? (pTogInfo + 1) : (pTogInfo + 2) )

#endif // _GP_IR_TX_GENERIC_DEFS_H_
