/*
 * Copyright (c) 2011-2014, GreenPeak Technologies
 *
 *  The file irdb.h contains internal data structure used by the IR database API
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
 *     0!                         $Header: //depot/main/Embedded/IrDbComps/vlatest/sw/comps/gpIRDatabase/src/gpIRDatabase_defs.h#5 $
 *    M'   GreenPeak              $Change: 59296 $
 *   0'         Technologies      $DateTime: 2014/12/17 15:19:20 $
 *  F
 */

#ifndef _GP_IR_DATABASE_COMPRESSED_H_
#define _GP_IR_DATABASE_COMPRESSED_H_

/**
 * @file gpIRDatabase_Compressed.h
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

    #include "gpIRDatabase_Data_Full_Plain.h"

#ifndef GP_IRDB_COMPRESSION_REV
#define GP_IRDB_COMPRESSION_REV 0
#endif

#if GP_IRDB_COMPRESSION_REV > 7
#error only 3 bits allocated to encode compresison revision
#endif

#define GP_IR_DATABASE_FLAG_COMPRESSED          (1<<0)
#define GP_IR_DATABASE_FLAG_ENCRYPTED           (1<<1)
#define GP_IR_DATABASE_FLAG_COMPRESSION_REV     (GP_IRDB_COMPRESSION_REV<<5)

#define GP_IR_DATABASE_AES_BLOCK_SIZE_BYTES     16
#define GP_IR_DATABASE_VID_STRING_STORAGE_SIZE  (GP_IR_DATABASE_AES_BLOCK_SIZE_BYTES + 1)

typedef struct
{
  UInt8                                         nMajorVersion;      //FIXME: Make sure that this field is located at lowest address of the instantiated structure element
  UInt8 const ROM *                             pEncMagicArray;
  UInt8                                         flags;
  UInt16                                        nSymbols;
  UInt8                                         nSymbolSets;
  UInt8                                         nHeaders;
  UInt16                                        nSequences;
  UInt16                                        nRecords;
  UInt8                                         nSpecialHeaders;
  UInt8                                         nKeys;
  gpIRDatabase_Symbol_t const ROM *             pSymbols;
  gpIRDatabase_SymbolSet_t const ROM *          pSymbolSets;
  gpIRDatabase_Header_t const ROM *             pHeaders;
  UInt8 const ROM *                             pSequenceData;
  gpIRDatabase_IRCodeRecord_t const ROM *       pRecords;
  gpIRDatabase_IRSpecialHeader_t const ROM *    pSpecialHeaders;
  UInt8 const ROM *                             pVidString;
  gpIRDatabase_IRSpecialSequence_t const ROM *  pSpecialSequences;
} gpIRDatabase_Reference_t;

#endif // _GP_IR_DATABASE_COMPRESSED_H_

