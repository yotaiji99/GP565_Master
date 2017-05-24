/*
 * Copyright (c) 2010-2015, GreenPeak Technologies
 *
 * This file gives a small IR database implementation
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
 *     0!                         $Header: //depot/main/Embedded/IrDbComps/vlatest/sw/comps/gpIRDatabase/src/gpIRDatabase.c#25 $
 *    M'   GreenPeak              $Change: 76337 $
 *   0'         Technologies      $DateTime: 2016/01/28 13:52:18 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

//#define GP_LOCAL_LOG

// General includes
#include "gpIRDatabase.h"
#include "gpLog.h"
#include "gpAssert.h"
#include "gpKeyboard.h"
#include "gpIRDatabase_defs.h"
#include "hal.h"
#include "gpLog.h"
#include "gpNvm.h"
#include "gpController_Main.h"


extern const gpIRDatabase_Reference_t ROM gpIRDatabase_Reference FLASH_PROGMEM;
#define IRDB_REFERENCE gpIRDatabase_Reference

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_IRDATABASE

// Macro's to access byte/nibble structures
// ----------------------------------------
#define LSB(c)                      ((UInt8)(c & 0xFF))
#define MSB(c)                      ((UInt8)((c>>8) & 0xFF))
#define PACK2LOWNIBBLE(b,n)         (b = (UInt8)((b & 0xF0) | (n & 0x0F)))
#define PACK2HIGHNIBBLE(b,n)        (b = (UInt8)((b & 0x0F) | ((n<<4) & 0xF0)))

// Macro's to extract record fields from the header
// ------------------------------------------------
#define FIRST_FRAME_SYMBOLS         (pHeader->firstFrameLength)
#define REPEAT_FRAME_SYMBOLS        (pHeader->repeatFrameLength)
#define RELEASE_FRAME_SYMBOLS       (pHeader->releaseFrameLength)
#define KEY_TOGGLES                 ((pHeader->control & 0x0008)==0 ? false : true)

// Macro's to define security
// ---------------------------------
#define AES_KEY_SIZE_BYTES          16
#define AES_KEY_SIZE_BITS           (AES_KEY_SIZE_BYTES*8)
#define AES_BLOCK_SIZE_BYTES        16
#if (AES_KEY_SIZE_BYTES != AES_BLOCK_SIZE_BYTES)
#error ERROR key/block size! Due to the nature of key generation the key size must match block size. For AES this means 16 both - always.
#endif

// Macro's to refer the keys in the compressed database
// ----------------------------------------------------
#define COMPRESSED_DB_POWER_TOGGLE_KEY_INDEX        0
#define COMPRESSED_DB_VOLUME_DOWN_KEY_INDEX         2
#define COMPRESSED_DB_VOLUME_UP_KEY_INDEX           3
#define COMPRESSED_DB_MUTE_KEY_INDEX                4
#define COMPRESSED_DB_INPUT_SELECT_KEY_INDEX        5

// Macro's to support entries compression
// --------------------------------------
#define MAX_USED_SYMBOLS_IN_ENTRY                   8

// Macro's to support NEC types
// ----------------------------
#define IR_DEFINITION_REVISION                      1
#define IR_DEFINITION_HEADER_SIZE                   8
#define IR_DEFINITION_REPEAT_INDEX                  1
#define IR_DEFINITION_EOP_CLEAR_INDEX               22

#define IR_DEFINITION_NEC_NBR_PATTERNS              2
#define IR_DEFINITION_NEC_NBR_TIMINGS               3

#define IR_DEFINITION_NEC_GET_PATTERNTYPE(nectype)  ((nectype) & 1)
#define IR_DEFINITION_NEC_GET_TIMINGTYPE(nectype)   (((necType) >> 1) - 1)

#define IR_DEFINITION_NEC_PATTERN0_SYMBOLS          6
#define IR_DEFINITION_NEC_PATTERN1_SYMBOLS          4

#define NEC_1_DITTO                                 0x45
#define NEC_EOP                                     0x03
#define ONE_ZERO_CLEAR_DIF                           281

// Macro's to access database in nonlinear memories
// ------------------------------------------------
#define IR_DATABASE_ACCESS_SYMBOLSET(index)         { gpIRDatabase_SymbolSet_t const ROM * pSymbolSets; MEMCPY_P(&pSymbolSets, &(IRDB_REFERENCE.pSymbolSets), sizeof(gpIRDatabase_SymbolSet_t const ROM *)); MEMCPY_P(&gpIRDatabase_SymbolSetBuffer, &(pSymbolSets[index]), sizeof(gpIRDatabase_SymbolSet_t)); }
#define IR_DATABASE_ACCESS_SYMBOL(index)            { gpIRDatabase_Symbol_t const ROM * pSymbols; MEMCPY_P(&pSymbols, &(IRDB_REFERENCE.pSymbols), sizeof(gpIRDatabase_Symbol_t const ROM *)); MEMCPY_P(&gpIRDatabase_SymbolBuffer, &(pSymbols[index]), sizeof(gpIRDatabase_Symbol_t)); }
#define IR_DATABASE_ACCESS_HEADER(index)            { gpIRDatabase_Header_t const ROM * pHeaders; MEMCPY_P(&pHeaders, &(IRDB_REFERENCE.pHeaders), sizeof(gpIRDatabase_Header_t const ROM *)); MEMCPY_P(&gpIRDatabase_HeaderBuffer, &(pHeaders[index]), sizeof(gpIRDatabase_Header_t)); }
#define IR_DATABASE_ACCESS_RECORD(index)            { gpIRDatabase_IRCodeRecord_t const ROM * pRecords; MEMCPY_P(&pRecords, &(IRDB_REFERENCE.pRecords), sizeof(gpIRDatabase_IRCodeRecord_t const ROM *)); MEMCPY_P(&gpIRDatabase_RecordBuffer, &(pRecords[index]), sizeof(gpIRDatabase_IRCodeRecord_t)); }
#define IR_DATABASE_ACCESS_SPECIALHEADER(index)     { gpIRDatabase_IRSpecialHeader_t const ROM * pSpecialHeaders; MEMCPY_P(&pSpecialHeaders, &(IRDB_REFERENCE.pSpecialHeaders), sizeof(gpIRDatabase_IRSpecialHeader_t const ROM *)); MEMCPY_P(&gpIRDatabase_SpecialHeaderBuffer, &(pSpecialHeaders[index]), sizeof(gpIRDatabase_IRSpecialHeader_t)); }
#define IR_DATABASE_ACCESS_SPECIALSEQUENCE(index)   { gpIRDatabase_IRSpecialSequence_t const ROM * pSpecialSequences; MEMCPY_P(&pSpecialSequences, &(IRDB_REFERENCE.pSpecialSequences), sizeof(gpIRDatabase_IRSpecialSequence_t const ROM *)); MEMCPY_P(&gpIRDatabase_SpecialSequenceBuffer, &(pSpecialSequences[index]), sizeof(gpIRDatabase_IRSpecialSequence_t)); }
#define IR_DATABASE_ACCESS_SEQUENCE(ptr)                    MEMCPY_P(&gpIRDatabase_SequenceDataBuffer, ptr, 1)
#define IR_DATABASE_ACCESS_NSYMBOLS(nSymbols)               MEMCPY_P(&nSymbols, &(IRDB_REFERENCE.nSymbols), sizeof(UInt16))
#define IR_DATABASE_ACCESS_NHEADERS(nHeaders)               MEMCPY_P(&nHeaders, &(IRDB_REFERENCE.nHeaders), sizeof(UInt8))
#define IR_DATABASE_ACCESS_NSPECIALHEADERS(nSpecialHeaders) MEMCPY_P(&nSpecialHeaders, &(IRDB_REFERENCE.nSpecialHeaders), sizeof(UInt8))
#define IR_DATABASE_ACCESS_NRECORDS(nRecords)               MEMCPY_P(&nRecords, &(IRDB_REFERENCE.nRecords), sizeof(UInt16))
#define IR_DATABASE_ACCESS_NKEYS(nKeys)                     MEMCPY_P(&nKeys, &(IRDB_REFERENCE.nKeys), sizeof(UInt8))
#define IR_DATABASE_ACCESS_NMAJORVERSION(nMajorVersion)     MEMCPY_P(&nMajorVersion, &(IRDB_REFERENCE.nMajorVersion), sizeof(UInt8))
#define IR_DATABASE_ACCESS_FLAGS(_flags)                    MEMCPY_P(&_flags, &(IRDB_REFERENCE.flags), sizeof(UInt8))


// Macro's to specify compatible databases
// ---------------------------------------
#define IR_DATABASE_COMPATIBILITY_MAJOR_MIN         1
#define IR_DATABASE_COMPATIBILITY_MAJOR_MAX         1

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

 /*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
static gpIRDatabase_SymbolSet_t         gpIRDatabase_SymbolSetBuffer;
static gpIRDatabase_Symbol_t            gpIRDatabase_SymbolBuffer;
static gpIRDatabase_Header_t            gpIRDatabase_HeaderBuffer;
static gpIRDatabase_IRCodeRecord_t      gpIRDatabase_RecordBuffer;
static gpIRDatabase_IRSpecialHeader_t   gpIRDatabase_SpecialHeaderBuffer;
static gpIRDatabase_IRSpecialSequence_t gpIRDatabase_SpecialSequenceBuffer;
static UInt8                            gpIRDatabase_SequenceDataBuffer;
static gpIRDatabase_IRTableId_t         gpIRDatabase_ActiveIRTableId;
static UInt16                           gpIRDatabase_ActiveIRTableIndex;
static Bool                             gpIRDatabase_MagicWordFailure;
static UInt8                            gpIRDatabase_ConfigFlags;

const UInt8 ROM IRDatabase_NecHeaders[IR_DEFINITION_NEC_NBR_PATTERNS][IR_DEFINITION_HEADER_SIZE] FLASH_PROGMEM =
    {
        {0x04, 0x00, IR_DEFINITION_NEC_PATTERN0_SYMBOLS, 0x22, 0x02, 0x00, 0xD2, 0x00},
        {0x04, 0x00, IR_DEFINITION_NEC_PATTERN1_SYMBOLS, 0x00, 0x22, 0x00, 0xD2, 0x00}
    };
const UInt16 ROM IRDatabase_NecSymbolSets[IR_DEFINITION_NEC_NBR_TIMINGS][IR_DEFINITION_NEC_PATTERN0_SYMBOLS*2] FLASH_PROGMEM =
    {
        {144,141,144,422,2250,1125,144,0,2250,562,144,24043},
        {139,142,139,423,2249,1125,139,0,2249,563,139,24048},
        {139,142,139,423,1124,1125,139,0,1124,563,139,24048}
    };
const UInt8 ROM IRDatabase_NecToSymbolsMap[4] FLASH_PROGMEM = {0x00, 0x10, 0x01, 0x11};

GP_NVM_CONST gpIRDatabase_IRTableId_t ROM gpIRDatabase_ActiveIRTableIdDefault FLASH_PROGMEM = 0xFFFF;

const gpNvm_Tag_t ROM gpIRDatabase_NvmSection[] FLASH_PROGMEM = {
    {(UInt8*)(&gpIRDatabase_ActiveIRTableId), sizeof(gpIRDatabase_IRTableId_t), gpNvm_UpdateFrequencyLow, (GP_NVM_CONST UInt8*)(&gpIRDatabase_ActiveIRTableIdDefault)}
};

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/
static UInt16 IRDatabase_DecompressSequence(const ROM UInt8 ** ppCompressedData, UInt8 * pBitIndex, UInt16 baseSymbolIndex, UInt8 nbSymbols,  UInt8 * pDecompressedKeyEntry, UInt8 * conversionTable);
static UInt16 IRDatabase_DecompressSequences(const gpIRDatabase_IRCodeRecord_t* pRecord, gpIRDatabase_Header_t* pHeader, UInt16 keyNbIndex, UInt8 * pDecompressedKeyEntry, UInt8 * conversionTable);
static UInt16 IRDatabase_TrimTimingData(UInt8 * pTimingData, UInt8 * conversionTable);
static void IRDatabase_MarkSymbolAsDetected(UInt8 symbol, UInt8 * conversionTable);
static Bool IRDatabase_SymbolAlreadyDetected(UInt8 symbol, UInt8 * conversionTable);
static void IRDatabase_ConvertSymbolsFromSequence(const ROM UInt8 ** ppCompressedData, UInt8 * pBitIndex, UInt16 baseSymbolIndex, UInt8 nbSymbols,  UInt8 * conversionTable);
static void IRDatabase_ConvertSymbolsFromSequences(const gpIRDatabase_IRCodeRecord_t* pRecord, UInt16 keyNbIndex, gpIRDatabase_Header_t* pHeader, UInt8 * conversionTable);
static UInt16 IRDatabase_CopyTimingData(const gpIRDatabase_IRCodeRecord_t* pRecord, UInt8 * pDecompressedKeyEntry);
static UInt8 IRDatabase_CopyHeader(const gpIRDatabase_IRCodeRecord_t* pRecord, gpIRDatabase_Header_t* pHeader, UInt8 * pDecompressedKeyEntry);
static gpIRDatabase_IRSpecialHeader_t* IRDatabase_GetSpecialHeader(UInt16 codeNb);
static gpIRDatabase_IRSpecialSequence_t* IRDatabase_GetSpecialSequence(UInt8 index);
static UInt16 IRDatabase_CodeToIndex(UInt16 codeNb, Bool baseIndex);
static UInt16 IRDatabase_NECToSequence(UInt8 necType, UInt8 D, UInt8 S, UInt8 F, UInt8 repeat, UInt8 * pDecompressedKeyEntry);
static UInt16 IRDatabase_DecompressKeyEntry(UInt16 codeNb, UInt16 keyNb, UInt8 * pDecompressedKeyEntry);
static UInt16 IRDatabase_ConvertKey(UInt8 rfKeyId);
static UInt8 IRDatabase_NecByteToSequence(UInt8 carryInSymbol, UInt8 necByte, UInt8 * sequence);
static UInt16 IRDatabase_NecEopClear(UInt8 necTimingType, UInt8 D, UInt8 S);
static UInt8 IRDatabase_FromConversionTable(UInt8 symbol,  UInt8 * conversionTable);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
//--------------------
//NEC helper functions
//--------------------

UInt8 IRDatabase_NecByteToSequence(UInt8 carryInSymbol, UInt8 necByte, UInt8 * sequence)
{
    UIntLoop i;
    sequence[0] = (carryInSymbol<<4) + (necByte&1);
    necByte>>=1;

    for(i=1; i < 4; i++)
    {
        MEMCPY_P(&(sequence[i]),&(IRDatabase_NecToSymbolsMap[necByte & 0x3]),sizeof(UInt8));
        necByte >>= 2;
    }

    return necByte;
}

static UInt16 IRDatabase_NecEopClear(UInt8 necTimingType, UInt8 D, UInt8 S)
{
    UInt16 eopClear;

    UInt16 DS   = (UInt16)D << 8 | S;
    Int8  ones = -8;

    while (DS != 0) {
        ones += DS & 1;
        DS >>= 1;
    }

    switch (necTimingType){
        case 0:
            eopClear = 9836 - (ones * ONE_ZERO_CLEAR_DIF) ;
            if (ones <= -4)
                eopClear++;
            if (ones <= -1)
                eopClear++;
            break;
        case 1:
            eopClear = 9990 - (ones * ONE_ZERO_CLEAR_DIF) ;
            break;
        case 2:
            eopClear = 11115 - (ones * ONE_ZERO_CLEAR_DIF) ;
            break;
        default:
            GP_ASSERT_DEV_INT(false);
            eopClear = 0;
            break;
    }
    return eopClear ;
}

//--------------------
//Huffman helper functions
//--------------------

 void IRDatabase_MarkSymbolAsDetected(UInt8 symbol,  UInt8 * conversionTable)
{
    UInt16 symbolCounter;
    for(symbolCounter = 0; symbolCounter < MAX_USED_SYMBOLS_IN_ENTRY; symbolCounter++)
    {
        if(conversionTable[symbolCounter] == 0xFF)
        {
            conversionTable[symbolCounter] = symbol;
            return;
        }
    }
    GP_ASSERT_DEV_EXT(false); //Should always find a free space /*symbolCounter < MAX_USED_SYMBOLS_IN_ENTRY*/
}

Bool IRDatabase_SymbolAlreadyDetected(UInt8 symbol,  UInt8 * conversionTable)
{
    UInt16 symbolCounter;
    for(symbolCounter = 0; symbolCounter < MAX_USED_SYMBOLS_IN_ENTRY; symbolCounter++)
    {
        if(conversionTable[symbolCounter] == symbol) return true;
    }

    return false;
}

void IRDatabase_ConvertSymbolsFromSequence(const ROM UInt8 ** ppCompressedData, UInt8 * pBitIndex, UInt16 baseSymbolIndex, UInt8 nbSymbols,  UInt8 * conversionTable)
{
    UInt16 symbolCounter;
    UInt16 symbolIndex = baseSymbolIndex;

    IR_DATABASE_ACCESS_SYMBOL(symbolIndex);
    symbolCounter = 0;

    IR_DATABASE_ACCESS_SEQUENCE(*ppCompressedData);

    while(symbolCounter < nbSymbols)
    {
        if(gpIRDatabase_SequenceDataBuffer & (1<<*pBitIndex))
        {
            // Go Right
            if(gpIRDatabase_SymbolBuffer.hufmanright & 0x80)
            {
                //next symbol found
                if(false == IRDatabase_SymbolAlreadyDetected((gpIRDatabase_SymbolBuffer.hufmanright & 0x0F), conversionTable))
                {
                    IRDatabase_MarkSymbolAsDetected((gpIRDatabase_SymbolBuffer.hufmanright & 0x0F), conversionTable);
                }
                // reset symbol pointer
                symbolIndex = baseSymbolIndex;
                IR_DATABASE_ACCESS_SYMBOL(symbolIndex);
                symbolCounter++;
            }
            else
            {
                symbolIndex = baseSymbolIndex + (gpIRDatabase_SymbolBuffer.hufmanright & 0x7F);
                IR_DATABASE_ACCESS_SYMBOL(symbolIndex);
            }
        }
        else
        {
            // Go Left
            if(gpIRDatabase_SymbolBuffer.hufmanleft & 0x80)
            {
                //next symbol found
                if(false == IRDatabase_SymbolAlreadyDetected((gpIRDatabase_SymbolBuffer.hufmanleft & 0x0F), conversionTable))
                {
                    IRDatabase_MarkSymbolAsDetected((gpIRDatabase_SymbolBuffer.hufmanleft & 0x0F), conversionTable);
                }
                // reset symbol pointer
                symbolIndex = baseSymbolIndex;
                IR_DATABASE_ACCESS_SYMBOL(symbolIndex);
                symbolCounter++;
            }
            else
            {
                symbolIndex = baseSymbolIndex + (gpIRDatabase_SymbolBuffer.hufmanleft & 0x7F);
                IR_DATABASE_ACCESS_SYMBOL(symbolIndex);
            }
        }

        if (*pBitIndex)
        {
            (*pBitIndex)--;
        }
        else
        {
            *pBitIndex=7;
            (*ppCompressedData)++;
            IR_DATABASE_ACCESS_SEQUENCE(*ppCompressedData);
        }
    }
}

UInt8 IRDatabase_FromConversionTable(UInt8 symbol,  UInt8 * conversionTable)
{
    UInt16 symbolCounter;
    for(symbolCounter = 0; symbolCounter < MAX_USED_SYMBOLS_IN_ENTRY; symbolCounter++)
    {
        if(conversionTable[symbolCounter] == symbol) break;
    }
    GP_ASSERT_DEV_EXT(symbolCounter < MAX_USED_SYMBOLS_IN_ENTRY);
    return (UInt8)symbolCounter;
}

//--------------------
//Main Huffman function
//--------------------

UInt16 IRDatabase_DecompressSequence(const ROM UInt8 ** ppCompressedData, UInt8 * pBitIndex/*Gets adjusted*/, UInt16 baseSymbolIndex, UInt8 nbSymbols,  UInt8 * pDecompressedKeyEntry /*Return value*/, UInt8 * conversionTable)
{
    UInt16 byteCounter;
    UInt16 symbolCounter;
    Bool lowNibble;
    UInt16 symbolIndex = baseSymbolIndex;

    IR_DATABASE_ACCESS_SYMBOL(symbolIndex);
    byteCounter = 0;
    symbolCounter = 0;
    lowNibble = false;

    IR_DATABASE_ACCESS_SEQUENCE(*ppCompressedData);

    while(symbolCounter < nbSymbols)
    {
        if(gpIRDatabase_SequenceDataBuffer & (1<<*pBitIndex))
        {
            // Go Right
            if(gpIRDatabase_SymbolBuffer.hufmanright & 0x80)
            {
                //next symbol found
                lowNibble = !lowNibble;
                if(lowNibble)
                {
                    PACK2HIGHNIBBLE(pDecompressedKeyEntry[byteCounter], IRDatabase_FromConversionTable((gpIRDatabase_SymbolBuffer.hufmanright & 0x0F), conversionTable));
                }
                else
                {
                    PACK2LOWNIBBLE(pDecompressedKeyEntry[byteCounter], IRDatabase_FromConversionTable((gpIRDatabase_SymbolBuffer.hufmanright & 0x0F), conversionTable));
                    byteCounter++;
                }
                // reset symbol pointer
                symbolIndex = baseSymbolIndex;
                IR_DATABASE_ACCESS_SYMBOL(symbolIndex);
                symbolCounter++;
            }
            else
            {
                symbolIndex = baseSymbolIndex + (gpIRDatabase_SymbolBuffer.hufmanright & 0x7F);
                IR_DATABASE_ACCESS_SYMBOL(symbolIndex);
            }
        }
        else
        {
            // Go Left
            if(gpIRDatabase_SymbolBuffer.hufmanleft & 0x80)
            {
                //next symbol found
                lowNibble = !lowNibble;
                if(lowNibble)
                {
                    PACK2HIGHNIBBLE(pDecompressedKeyEntry[byteCounter], IRDatabase_FromConversionTable((gpIRDatabase_SymbolBuffer.hufmanleft & 0x0F), conversionTable));
                }
                else
                {
                    PACK2LOWNIBBLE(pDecompressedKeyEntry[byteCounter], IRDatabase_FromConversionTable((gpIRDatabase_SymbolBuffer.hufmanleft & 0x0F), conversionTable));
                    byteCounter++;
                }
                // reset symbol pointer
                symbolIndex = baseSymbolIndex;
                IR_DATABASE_ACCESS_SYMBOL(symbolIndex);
                symbolCounter++;
            }
            else
            {
                symbolIndex = baseSymbolIndex + (gpIRDatabase_SymbolBuffer.hufmanleft & 0x7F);
                IR_DATABASE_ACCESS_SYMBOL(symbolIndex);
            }
        }

        if (*pBitIndex)
        {
            (*pBitIndex)--;
        }
        else
        {
            *pBitIndex=7;
            (*ppCompressedData)++;
            IR_DATABASE_ACCESS_SEQUENCE(*ppCompressedData);
        }
    }

    if(lowNibble)
    {
        pDecompressedKeyEntry[byteCounter] = (UInt8)(pDecompressedKeyEntry[byteCounter] & 0xF0);
        byteCounter++;
    }

    return byteCounter;
}

//--------------------
//Sequence getting
//--------------------

UInt16 IRDatabase_GetSequenceValue(const gpIRDatabase_IRCodeRecord_t* pRecord, UInt8 keyNbIndex)
{

    UInt16 sequenceValue = 0xFFFF;

    if(pRecord->symbolSetIndex == 0xFF) //NEC encodings
    {
        if (pRecord->encoding.raw.keyMask & (1<<keyNbIndex))
        {
            sequenceValue = pRecord->encoding.raw.values[keyNbIndex];
        }
    }
    else //non-NEC encodings
    {
        if(GP_IR_DATABASE_GET_FLAG_COMPRESSION_REV(gpIRDatabase_ConfigFlags) == 0)
        {
            UInt8 startSymbol = (pRecord->encoding.sequenceIds.base >> 13) & 0x7;

            if ( (keyNbIndex >= startSymbol) &&
                 (pRecord->encoding.sequenceIds.deltas[keyNbIndex-1-startSymbol] != -128))
            {
                UIntLoop i;

                sequenceValue = pRecord->encoding.sequenceIds.base & 0x1FFF;

                //Base pointing to nowhere
                GP_ASSERT_DEV_EXT(sequenceValue != 0x1FFF);

                for(i=0; i < (keyNbIndex-startSymbol); i++)
                {
                    if (pRecord->encoding.sequenceIds.deltas[i] != -128)
                    {
                        sequenceValue += pRecord->encoding.sequenceIds.deltas[i];
                    }
                }
            }
        }
        else
        {
            UIntLoop i;
            Bool validFirstSymbol = ((pRecord->encoding.sequenceIds.base) & 0x8000) != 0x8000;
            Bool specialSequence  = ((pRecord->encoding.sequenceIds.base) & 0x4000) == 0x4000;
            UInt16 base = pRecord->encoding.sequenceIds.base & 0x3FFF;

            if (specialSequence)
            {
                if(keyNbIndex == 0)
                {
                    return base;
                }
                gpIRDatabase_IRSpecialSequence_t* specSeq = IRDatabase_GetSpecialSequence(pRecord->encoding.sequenceIds.deltas[0]);
                return specSeq->sequence[keyNbIndex-1];
            }

            if ( !validFirstSymbol && (keyNbIndex == 0) )
            {
                return sequenceValue;
            }
            if (pRecord->encoding.sequenceIds.deltas[keyNbIndex-1] == -128)
            {
                return sequenceValue;
            }

            sequenceValue = base;

            for(i=0; i < keyNbIndex; i++)
            {
                if (pRecord->encoding.sequenceIds.deltas[i] != -128)
                {
                    sequenceValue += pRecord->encoding.sequenceIds.deltas[i];
                }
            }
        }
    }
    return sequenceValue;
}

UInt16 IRDatabase_DecompressSequences(const gpIRDatabase_IRCodeRecord_t* pRecord, gpIRDatabase_Header_t* pHeader, UInt16 keyNbIndex, UInt8 * pDecompressedKeyEntry, UInt8 * conversionTable)
{
    UInt16 byteCounter;
    const UInt8 ROM *pCompressedData;
    UInt8 TogglePosition = 0;
    UInt8 NbOfNibblesToToggle = 0;
    UInt8 bitIndex;
    UInt8 const ROM * pSequenceData;

    byteCounter = 0;

    MEMCPY_P(&pSequenceData, &(IRDB_REFERENCE.pSequenceData), sizeof(UInt8 const ROM *));
    pCompressedData = &(pSequenceData[IRDatabase_GetSequenceValue(pRecord,keyNbIndex)]);
    IR_DATABASE_ACCESS_SYMBOLSET(pRecord->symbolSetIndex);

    if(KEY_TOGGLES)
    {
        //Take into account toggle data position
        IR_DATABASE_ACCESS_SEQUENCE(pCompressedData);
        pCompressedData++;
        TogglePosition = gpIRDatabase_SequenceDataBuffer;
        IR_DATABASE_ACCESS_SEQUENCE(pCompressedData);
        pCompressedData++;
        NbOfNibblesToToggle = gpIRDatabase_SequenceDataBuffer;
    }

    //Fetch 0120310... etc. sequences
    bitIndex = 7; //bit index while traversing the list of sequences
    byteCounter += IRDatabase_DecompressSequence(&pCompressedData, &bitIndex, gpIRDatabase_SymbolSetBuffer.symbolIndex, FIRST_FRAME_SYMBOLS, &pDecompressedKeyEntry[byteCounter], conversionTable);
    byteCounter += IRDatabase_DecompressSequence(&pCompressedData, &bitIndex, gpIRDatabase_SymbolSetBuffer.symbolIndex, REPEAT_FRAME_SYMBOLS, &pDecompressedKeyEntry[byteCounter], conversionTable);
    byteCounter += IRDatabase_DecompressSequence(&pCompressedData, &bitIndex, gpIRDatabase_SymbolSetBuffer.symbolIndex, RELEASE_FRAME_SYMBOLS, &pDecompressedKeyEntry[byteCounter], conversionTable);

    if(KEY_TOGGLES)
    {
        //Go and fetch toggle data
        UIntLoop i;
        pDecompressedKeyEntry[byteCounter++] = TogglePosition;
        pDecompressedKeyEntry[byteCounter++] = NbOfNibblesToToggle;
        for(i=0; i<3; i++)
        {
            byteCounter += IRDatabase_DecompressSequence(&pCompressedData, &bitIndex, gpIRDatabase_SymbolSetBuffer.symbolIndex, NbOfNibblesToToggle, &pDecompressedKeyEntry[byteCounter], conversionTable);
        }
    }

    return byteCounter;
}
UInt16 IRDatabase_CopyTimingData(const gpIRDatabase_IRCodeRecord_t* pRecord, UInt8 * pDecompressedKeyEntry)
{
    UInt16 byteCounter;
    UInt16 symbolCounter;
    UInt16 symbolIndex;

    IR_DATABASE_ACCESS_SYMBOLSET(pRecord->symbolSetIndex);
    symbolIndex = gpIRDatabase_SymbolSetBuffer.symbolIndex;
    byteCounter = 0;
    for(symbolCounter = 0; symbolCounter < gpIRDatabase_SymbolSetBuffer.length; symbolCounter++)
    {
        IR_DATABASE_ACCESS_SYMBOL(symbolIndex);
        symbolIndex++;
        pDecompressedKeyEntry[byteCounter++] = LSB(gpIRDatabase_SymbolBuffer.mark);
        pDecompressedKeyEntry[byteCounter++] = MSB(gpIRDatabase_SymbolBuffer.mark);
        pDecompressedKeyEntry[byteCounter++] = LSB(gpIRDatabase_SymbolBuffer.space);
        pDecompressedKeyEntry[byteCounter++] = MSB(gpIRDatabase_SymbolBuffer.space);
	    //GP_LOG_SYSTEM_PRINTF("space [%d] = [%x]",0,byteCounter,pDecompressedKeyEntry[byteCounter]);
    }

    return (byteCounter);
}

UInt8 IRDatabase_CopyHeader(const gpIRDatabase_IRCodeRecord_t* pRecord, gpIRDatabase_Header_t* pHeader, UInt8 * pDecompressedKeyEntry)
{
    IR_DATABASE_ACCESS_SYMBOLSET(pRecord->symbolSetIndex);

    pDecompressedKeyEntry[0] = LSB(pHeader->control);
    pDecompressedKeyEntry[1] = MSB(pHeader->control);
    pDecompressedKeyEntry[1] |= ((IR_DEFINITION_REVISION & 0xF) << 4);
    pDecompressedKeyEntry[2] = gpIRDatabase_SymbolSetBuffer.length;
    pDecompressedKeyEntry[3] = FIRST_FRAME_SYMBOLS;
    pDecompressedKeyEntry[4] = REPEAT_FRAME_SYMBOLS;
    pDecompressedKeyEntry[5] = RELEASE_FRAME_SYMBOLS;
    pDecompressedKeyEntry[6] = LSB(pHeader->carrierPeriod);
    pDecompressedKeyEntry[7] = MSB(pHeader->carrierPeriod);

    return 8;
}

gpIRDatabase_IRSpecialHeader_t* IRDatabase_GetSpecialHeader(UInt16 codeNb)
{
    UInt16 index;
    UInt8 nSpecialHeaders;

    IR_DATABASE_ACCESS_NSPECIALHEADERS(nSpecialHeaders);
    for ( index = 0; index < nSpecialHeaders; index++)
    {
        IR_DATABASE_ACCESS_SPECIALHEADER(index);
        if (gpIRDatabase_SpecialHeaderBuffer.codeNb == codeNb)
        {
            return &gpIRDatabase_SpecialHeaderBuffer;
        }
    }

    return NULL;
}

gpIRDatabase_IRSpecialSequence_t* IRDatabase_GetSpecialSequence(UInt8 index)
{
    IR_DATABASE_ACCESS_SPECIALSEQUENCE(index);
    return &gpIRDatabase_SpecialSequenceBuffer;
}

UInt16 IRDatabase_CodeToIndex(UInt16 codeNb, Bool baseIndex)
{
    UInt16 codeNbIndex;
    UInt16 nRecords;

    IR_DATABASE_ACCESS_NRECORDS(nRecords);

    for(codeNbIndex =0; codeNbIndex < nRecords; codeNbIndex++)
    {
        IR_DATABASE_ACCESS_RECORD(codeNbIndex);
        if (gpIRDatabase_RecordBuffer.codeNb == codeNb)		//code ID (설정번호)search!!
        {
            break;
        }
        else if ((baseIndex) && (gpIRDatabase_RecordBuffer.codeNb > codeNb))
        {
            break;
        }
    }

    return codeNbIndex; //Max Index == Not found
}

void IRDatabase_ConvertSymbolsFromSequences(const gpIRDatabase_IRCodeRecord_t* pRecord, UInt16 keyNbIndex, gpIRDatabase_Header_t* pHeader, UInt8 * conversionTable)
{
    const UInt8 ROM *pCompressedData;
    UInt8 NbOfNibblesToToggle = 0;
    UInt8 bitIndex;
    UInt8 const ROM * pSequenceData;

    MEMCPY_P(&pSequenceData, &(IRDB_REFERENCE.pSequenceData), sizeof(UInt8 const ROM *));
    pCompressedData = &(pSequenceData[IRDatabase_GetSequenceValue(pRecord,keyNbIndex)]);

    IR_DATABASE_ACCESS_SYMBOLSET(pRecord->symbolSetIndex);

    if(KEY_TOGGLES)
    {
        pCompressedData++;
        IR_DATABASE_ACCESS_SEQUENCE(pCompressedData);
        pCompressedData++;
        NbOfNibblesToToggle = gpIRDatabase_SequenceDataBuffer;
    }

    bitIndex = 7;
    IRDatabase_ConvertSymbolsFromSequence(&pCompressedData, &bitIndex, gpIRDatabase_SymbolSetBuffer.symbolIndex, FIRST_FRAME_SYMBOLS, conversionTable);
    IRDatabase_ConvertSymbolsFromSequence(&pCompressedData, &bitIndex, gpIRDatabase_SymbolSetBuffer.symbolIndex, REPEAT_FRAME_SYMBOLS, conversionTable);
    IRDatabase_ConvertSymbolsFromSequence(&pCompressedData, &bitIndex, gpIRDatabase_SymbolSetBuffer.symbolIndex, RELEASE_FRAME_SYMBOLS, conversionTable);

    if(KEY_TOGGLES)
    {
        IRDatabase_ConvertSymbolsFromSequence(&pCompressedData, &bitIndex, gpIRDatabase_SymbolSetBuffer.symbolIndex, NbOfNibblesToToggle, conversionTable);
        IRDatabase_ConvertSymbolsFromSequence(&pCompressedData, &bitIndex, gpIRDatabase_SymbolSetBuffer.symbolIndex, NbOfNibblesToToggle, conversionTable);
        IRDatabase_ConvertSymbolsFromSequence(&pCompressedData, &bitIndex, gpIRDatabase_SymbolSetBuffer.symbolIndex, NbOfNibblesToToggle, conversionTable);
    }
}

UInt16 IRDatabase_TrimTimingData(UInt8 * pTimingData, UInt8 * conversionTable)
{
    UInt8 temporarySymbolSet[MAX_USED_SYMBOLS_IN_ENTRY * 4]; // 4 = 2x16bit timing data (set/clr)
    UInt16 byteCount = 0;
    UInt16 symbolCounter = 0;
    UInt16 symbolsFound = 0;

    while((symbolsFound < MAX_USED_SYMBOLS_IN_ENTRY) && (conversionTable[symbolsFound] != 0xFF))
    {
        //Clone all valid TimingData entries
        MEMCPY(&temporarySymbolSet[symbolsFound*4], &pTimingData[conversionTable[symbolsFound]*4], 4);
        symbolsFound++;
    }

    for(symbolCounter = 0; symbolCounter < symbolsFound; symbolCounter++)
    {
        //Fill up timing data with found entries
        MEMCPY(&pTimingData[symbolCounter*4], &temporarySymbolSet[symbolCounter*4], 4);
        byteCount += 4;
    }

    return byteCount;
}

//kh.kim : IRDB Buffer(80byte)에 raw data를 넣음.
UInt16 IRDatabase_NECToSequence(UInt8 necType, UInt8 D, UInt8 S, UInt8 F, UInt8 repeat, UInt8 * pDecompressedKeyEntry)
{
    UInt8 bytesToProcess;
    UInt16 byteCount = 0;
    UInt8 carrySymbol;
    UInt16 eopClear;
    UInt8 timingType  = IR_DEFINITION_NEC_GET_TIMINGTYPE(necType);
    UInt8 patternType = IR_DEFINITION_NEC_GET_PATTERNTYPE(necType);

    GP_ASSERT_DEV_EXT(repeat < 16);
    GP_ASSERT_DEV_EXT(timingType <= IR_DEFINITION_NEC_NBR_TIMINGS);
    GP_ASSERT_DEV_EXT(patternType <= IR_DEFINITION_NEC_NBR_PATTERNS);

    MEMCPY_P(&pDecompressedKeyEntry[byteCount], IRDatabase_NecHeaders[patternType], IR_DEFINITION_HEADER_SIZE);
    byteCount += IR_DEFINITION_HEADER_SIZE;

    pDecompressedKeyEntry[IR_DEFINITION_REPEAT_INDEX] = repeat;
    pDecompressedKeyEntry[IR_DEFINITION_REPEAT_INDEX] |= ((IR_DEFINITION_REVISION & 0xF) << 4);

    bytesToProcess = (patternType==0) ? IR_DEFINITION_NEC_PATTERN0_SYMBOLS*4 : IR_DEFINITION_NEC_PATTERN1_SYMBOLS*4;
    MEMCPY_P(&pDecompressedKeyEntry[byteCount], IRDatabase_NecSymbolSets[timingType], bytesToProcess);
    byteCount += bytesToProcess;

    eopClear = IRDatabase_NecEopClear(timingType, D, S);

    pDecompressedKeyEntry[IR_DEFINITION_EOP_CLEAR_INDEX]   = eopClear & 0xFF;
    pDecompressedKeyEntry[IR_DEFINITION_EOP_CLEAR_INDEX+1] = eopClear >> 8;

    carrySymbol = 2;
    carrySymbol = IRDatabase_NecByteToSequence(carrySymbol, D,  &pDecompressedKeyEntry[byteCount]);
    byteCount += 4;
    carrySymbol = IRDatabase_NecByteToSequence(carrySymbol, S,  &pDecompressedKeyEntry[byteCount]);
    byteCount += 4;
    carrySymbol = IRDatabase_NecByteToSequence(carrySymbol, F,  &pDecompressedKeyEntry[byteCount]);
    byteCount += 4;
    carrySymbol = IRDatabase_NecByteToSequence(carrySymbol, ~F, &pDecompressedKeyEntry[byteCount]);
    byteCount += 4;

    pDecompressedKeyEntry[byteCount] = (carrySymbol<<4) + NEC_EOP;
    byteCount++;

    if(patternType==0)
    {
        pDecompressedKeyEntry[byteCount] = NEC_1_DITTO;
        byteCount++;
    }

    return byteCount;
}

UInt16 IRDatabase_DecompressKeyEntry(UInt16 codeNb, UInt16 keyNb, UInt8 * pDecompressedKeyEntry /*Return value*/)
{
    UInt16 entryIndex = 0;
    gpIRDatabase_IRCodeRecord_t* pRecord;
    gpIRDatabase_Header_t* pHeader;
    gpIRDatabase_IRSpecialHeader_t* pSpecialHeader;
    UInt8 conversionTable[MAX_USED_SYMBOLS_IN_ENTRY];
    UInt8 newSymNo;
    UInt16 nRecords;
    UInt8 nKeys;

    //Initialize symbol conversion table to all undefined values
    MEMSET(conversionTable, -1, MAX_USED_SYMBOLS_IN_ENTRY);

    IR_DATABASE_ACCESS_NRECORDS(nRecords);
//	GP_LOG_SYSTEM_PRINTF("IR_DATABASE_ACCESS_NRECORDS = %d",0,nRecords);
    IR_DATABASE_ACCESS_NKEYS(nKeys);
//	GP_LOG_SYSTEM_PRINTF("IR_DATABASE_ACCESS_NKEYS = %d",0,nKeys);

    //Fetching a header
    if((nRecords > gpIRDatabase_ActiveIRTableIndex) && (nKeys > keyNb))
    {
        IR_DATABASE_ACCESS_RECORD(gpIRDatabase_ActiveIRTableIndex);
        pRecord = &gpIRDatabase_RecordBuffer;
        pSpecialHeader = IRDatabase_GetSpecialHeader(pRecord->codeNb); //Exception cases bits - also used for NEC seq

        if(NULL != pSpecialHeader)
        {
            //Special header fetching
            if(0xFF != pSpecialHeader->headerIndex[keyNb])
            {
                IR_DATABASE_ACCESS_HEADER(pSpecialHeader->headerIndex[keyNb]);
                pHeader = &gpIRDatabase_HeaderBuffer;
            }
            else
            {
                pHeader = NULL;
            }
        }
        else
        {
            //Normal header fetching
            IR_DATABASE_ACCESS_HEADER(pRecord->headerIndex);
            pHeader = &gpIRDatabase_HeaderBuffer;
        }

        //Header found - continue interpretation
        if(NULL != pHeader)
        {
            UInt16 sequenceValue;
            sequenceValue = IRDatabase_GetSequenceValue(pRecord,keyNb);
            //Fetch a sequence
            if(0xFFFF != sequenceValue)
            {
                if(0 != ((pHeader->control)>>12)) //Check special NEC control bit
                {
                    entryIndex = IRDatabase_NECToSequence(((pHeader->control)>>12), (UInt8)(pHeader->firstFrameLength),(UInt8)(pHeader->repeatFrameLength), (UInt8)sequenceValue,(UInt8)(((pHeader->control)>>8)&0xF), &pDecompressedKeyEntry[entryIndex]);
                }
                else
                {
                    entryIndex = IRDatabase_CopyHeader(pRecord, pHeader, &pDecompressedKeyEntry[entryIndex]);
                    IRDatabase_ConvertSymbolsFromSequences(pRecord, keyNb, pHeader, conversionTable);
                    //conversion table now has a list of symbols from the symbol et that are actually used for this device
                    IRDatabase_CopyTimingData(pRecord, &pDecompressedKeyEntry[entryIndex]);
                    newSymNo = IRDatabase_TrimTimingData(&pDecompressedKeyEntry[entryIndex], conversionTable); //Remove unused symbols
                    GP_ASSERT_DEV_EXT(0 != newSymNo);
                    newSymNo >>= 2; //convert number of timing data bytes into number of symbols (4bytes per symbol).
                    pDecompressedKeyEntry[2] = newSymNo;
                    entryIndex += newSymNo<<2;
                    entryIndex += IRDatabase_DecompressSequences(pRecord, pHeader, keyNb, &pDecompressedKeyEntry[entryIndex], conversionTable);
                }
            }
        }
    }

    return entryIndex;
}

UInt16 IRDatabase_ConvertKey(UInt8 rfKeyId)
{
    UInt16 retVal;

    // PowerOn, PowerOff and PowerToggle are translated into PowerToggle
    if(gpKeyboard_LogicalId_PowerToggleFunction == rfKeyId)
    {
		if(PowerToggle&0x01)
		{
			retVal = COMPRESSED_DB_POWER_TOGGLE_KEY_INDEX;
			PowerToggle++;
		}
		else
		{
			retVal = COMPRESSED_DB_POWER_TOGGLE_KEY_INDEX+1;
			PowerToggle++;
		}
    }
    else if (gpKeyboard_LogicalId_VolumeUp == rfKeyId)
    {
        retVal = COMPRESSED_DB_VOLUME_UP_KEY_INDEX;
    }
    else if (gpKeyboard_LogicalId_VolumeDown == rfKeyId)
    {
        retVal = COMPRESSED_DB_VOLUME_DOWN_KEY_INDEX;
    }
    else if (gpKeyboard_LogicalId_Mute == rfKeyId)
    {
        retVal = COMPRESSED_DB_MUTE_KEY_INDEX;
    }
    else if (gpKeyboard_LogicalId_InputSelect == rfKeyId)
    {
        retVal = COMPRESSED_DB_INPUT_SELECT_KEY_INDEX;
    }
    else
    {
        retVal = 0xFFFF;
    }

    //GP_LOG_SYSTEM_PRINTF("Conv %i %i",0,(UInt16)rfKeyId,(UInt16)retVal);
    //gpLog_Flush();

    return retVal;
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpIRDatabase_Init( void )
{
    UInt8 nMajorVersion;

    gpNvm_RegisterSection(GP_COMPONENT_ID, gpIRDatabase_NvmSection, sizeof(gpIRDatabase_NvmSection)/sizeof(gpNvm_Tag_t), NULL );
    IR_DATABASE_ACCESS_NMAJORVERSION(nMajorVersion);
    GP_ASSERT_SYSTEM(IR_DATABASE_COMPATIBILITY_MAJOR_MIN <= nMajorVersion);
    GP_ASSERT_SYSTEM(IR_DATABASE_COMPATIBILITY_MAJOR_MAX >= nMajorVersion);

    IR_DATABASE_ACCESS_FLAGS(gpIRDatabase_ConfigFlags);


}

void gpIRDatabase_Reset(void)
{
    gpNvm_Restore(GP_COMPONENT_ID , 0 , (UInt8*)&gpIRDatabase_ActiveIRTableId);
    gpIRDatabase_ActiveIRTableIndex = 0xFFFF;
}

void gpIRDatabase_SetIRTableId(gpIRDatabase_IRTableId_t IRTableId)
{
    UInt16 nRecords;

    gpIRDatabase_ActiveIRTableIndex = IRDatabase_CodeToIndex(IRTableId, false);

    IR_DATABASE_ACCESS_NRECORDS(nRecords);
    //GP_LOG_SYSTEM_PRINTF("MagicWord : %d, Records : %d , Index : %d",2,gpIRDatabase_MagicWordFailure, nRecords,gpIRDatabase_ActiveIRTableIndex);
    if(gpIRDatabase_MagicWordFailure || (nRecords <= gpIRDatabase_ActiveIRTableIndex))
    {
        IRTableId = 0xFFFF;
    }
    //GP_LOG_PRINTF("SIRID %x->%x",2,gpIRDatabase_ActiveIRTableId, IRTableId);
    //GP_LOG_SYSTEM_PRINTF("SIRID %d->%d",2,gpIRDatabase_ActiveIRTableId, IRTableId);
    gpIRDatabase_ActiveIRTableId = IRTableId;
    gpNvm_Backup(GP_COMPONENT_ID , 0 , (UInt8*)&gpIRDatabase_ActiveIRTableId);
}

void gpIRDatabase_SetIRTableIndex(UInt16 index)
{
    UInt16 nRecords;

    IR_DATABASE_ACCESS_NRECORDS(nRecords);
    if(!(gpIRDatabase_MagicWordFailure) && (nRecords > index))
    {
        gpIRDatabase_ActiveIRTableIndex = index;
        IR_DATABASE_ACCESS_RECORD(gpIRDatabase_ActiveIRTableIndex);
        gpIRDatabase_ActiveIRTableId = gpIRDatabase_RecordBuffer.codeNb;
    }
    else
    {
        gpIRDatabase_ActiveIRTableId = 0xFFFF;
        gpIRDatabase_ActiveIRTableIndex = 0xFFFF;
    }
    //GP_LOG_PRINTF("SIRIN %x %x",4,gpIRDatabase_ActiveIRTableId,gpIRDatabase_ActiveIRTableIndex);
}

gpIRDatabase_IRTableId_t gpIRDatabase_GetIRTableId(void)
{
    return gpIRDatabase_ActiveIRTableId;
}

gpIRDatabase_IRTableType_t gpIRDatabase_GetIRTableType(void)
{
    if (gpIRDatabase_ActiveIRTableId >= 30000)
    {
        return gpIRDatabase_IRTableTypeAvr;
    }
    return gpIRDatabase_IRTableTypeTv;
}

UInt16 gpIRDatabase_GetIRTableIndex(void)
{
    return gpIRDatabase_ActiveIRTableIndex;
}

UInt16 gpIRDatabase_GetIRTableBaseIndex(gpIRDatabase_IRTableId_t tableId)
{
    return IRDatabase_CodeToIndex(tableId, true);
}

UInt8 gpIRDatabase_GetIRCode(UInt8 rfKeyId, UInt8* pRaw)
{
    UInt16 keyNb;
    UInt8 retVal;

    GP_LOG_PRINTF("Sel %i",2,gpIRDatabase_ActiveIRTableId);

    if(gpIRDatabase_ActiveIRTableId == 0xFFFF)
    {
        return 0;
    }

    keyNb = IRDatabase_ConvertKey(rfKeyId);

    if(keyNb == 0xFFFF)
    {
        return 0;
    }

    retVal = (UInt8)IRDatabase_DecompressKeyEntry(gpIRDatabase_ActiveIRTableId, keyNb, pRaw);

    return retVal;
}
void gpIRDatabase_Unpair(void)
{
    gpIRDatabase_ActiveIRTableId = 0xFFFF;
    gpIRDatabase_ActiveIRTableIndex = 0xFFFF;
	gpController_Mode=gpController_ModeIRNec;
    gpController_SetDtaMode( true);            
    gpNvm_Backup(GP_COMPONENT_ID , 0 , (UInt8*)&gpIRDatabase_ActiveIRTableId);
}


