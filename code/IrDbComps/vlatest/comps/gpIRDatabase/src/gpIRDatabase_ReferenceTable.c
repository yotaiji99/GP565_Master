#include "gpIRDatabase_defs.h"

const UInt8 ROM gpIRDatabase_vidString[] FLASH_PROGMEM;
const UInt8 ROM gpIRDatabase_encryptedMagicString[] FLASH_PROGMEM;

const gpIRDatabase_Reference_t ROM gpIRDatabase_Reference FLASH_PROGMEM =
{
    1,
    gpIRDatabase_encryptedMagicString,
    GP_IR_DATABASE_FLAG_COMPRESSED + GP_IR_DATABASE_FLAG_ENCRYPTED + GP_IR_DATABASE_FLAG_COMPRESSION_REV,
    GP_IRDB_NUMBER_OF_SYMBOLS,
    GP_IRDB_NUMBER_OF_SYMBOLS_SETS,
    GP_IRDB_NUMBER_OF_HEADERS,
    GP_IRDB_NUMBER_OF_SEQUENCES,
    GP_IRDB_NUMBER_OF_RECORDS,
    GP_IRDB_NUMBER_OF_SPECIAL_HEADERS,
    GP_IRDB_NUMBER_OF_KEYS,
    gpIRDatabase_Symbols,
    gpIRDatabase_SymbolSets,
    gpIRDatabase_Headers,
    gpIRDatabase_SequenceData,
    gpIRDatabase_Records,
    gpIRDatabase_SpecialHeaders,
    gpIRDatabase_vidString,
    gpIRDatabase_SpecialSequences
};

const UInt8 ROM gpIRDatabase_vidString[]            FLASH_PROGMEM = XSTRINGIFY(GP_IR_DATABASE_VENDOR_ID_STRING);
const UInt8 ROM gpIRDatabase_encryptedMagicString[] FLASH_PROGMEM = GP_IR_DATABASE_ENCRYPTED_MAGIC_STRING;


