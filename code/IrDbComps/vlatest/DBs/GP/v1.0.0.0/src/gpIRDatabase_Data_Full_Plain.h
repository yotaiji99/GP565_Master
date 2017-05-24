/*
 * Copyright (c) 2008, GreenPeak Technologies
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
 *     0!                         
 *    M'   GreenPeak              $
 *   0'         Technologies      
 *  F
 */

#ifndef _GP_IR_DATABASE_GENERATED_H_
#define _GP_IR_DATABASE_GENERATED_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

 #include <global.h>

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


#define GP_IRDB_COMPRESSION_REV                      1
#define GP_IRDB_NUMBER_OF_SYMBOLS_SETS             141
#define GP_IRDB_NUMBER_OF_SYMBOLS                 1323
#define GP_IRDB_NUMBER_OF_HEADERS                  114
#define GP_IRDB_NUMBER_OF_SEQUENCES               1722
#define GP_IRDB_NUMBER_OF_RECORDS                  466
#define GP_IRDB_NUMBER_OF_SPECIAL_HEADERS           34

#define GP_IRDB_NUMBER_OF_KEYS                       6

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
 

typedef struct {
  UInt8  length;
  UInt16 symbolIndex;
} gpIRDatabase_SymbolSet_t;

typedef struct {
  UInt16 mark;
  UInt16 space;
  UInt8  hufmanleft;
  UInt8  hufmanright;
} gpIRDatabase_Symbol_t;


typedef struct {
  UInt16 control;
  UInt8 firstFrameLength;
  UInt8 repeatFrameLength;
  UInt8 releaseFrameLength;
  UInt16 carrierPeriod;
} gpIRDatabase_Header_t;

typedef struct {
  UInt8 position;
  UInt8 numberToToggle;
  UInt8 toggledata[6];
} gpIRDatabase_ToggleData_t;

typedef struct {
    UInt16 codeNb;
    union {
        struct{
          UInt16 base; //0xE000: startKey referende / 0x1FFF: base address in sequences table
          Int8 deltas[GP_IRDB_NUMBER_OF_KEYS-1];
        } sequenceIds;
        struct{
          UInt8 keyMask;
          UInt8 values[GP_IRDB_NUMBER_OF_KEYS];
        } raw;
    } encoding;
    UInt8 symbolSetIndex;
    UInt8 headerIndex;
} gpIRDatabase_IRCodeRecord_t;

typedef struct {
    UInt16 codeNb;
    UInt8 headerIndex[GP_IRDB_NUMBER_OF_KEYS];
} gpIRDatabase_IRSpecialHeader_t;

typedef struct {
    UInt16 sequence[GP_IRDB_NUMBER_OF_KEYS-1];
} gpIRDatabase_IRSpecialSequence_t;

/*****************************************************************************
 *                    Public Data Definitions
 *****************************************************************************/

extern const gpIRDatabase_SymbolSet_t ROM gpIRDatabase_SymbolSets[] FLASH_PROGMEM;
extern const gpIRDatabase_Symbol_t ROM gpIRDatabase_Symbols[] FLASH_PROGMEM;
extern const gpIRDatabase_Header_t ROM gpIRDatabase_Headers[] FLASH_PROGMEM;
extern const UInt8 ROM gpIRDatabase_SequenceData[] FLASH_PROGMEM;
extern const gpIRDatabase_IRCodeRecord_t ROM gpIRDatabase_Records[] FLASH_PROGMEM;
extern const gpIRDatabase_IRSpecialHeader_t ROM gpIRDatabase_SpecialHeaders[] FLASH_PROGMEM;
extern const gpIRDatabase_IRSpecialSequence_t ROM gpIRDatabase_SpecialSequences[] FLASH_PROGMEM;

#endif // _GP_IR_DATABASE_GENERATED_H_

