/*
 * Copyright (c) 2010-2014, GreenPeak Technologies
 *
 *  The file gpIRDatabase.h contains the API to access the IR database
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
 *     0!                         $Header: //depot/main/Embedded/IrDbComps/vlatest/sw/comps/gpIRDatabase/inc/gpIRDatabase.h#6 $
 *    M'   GreenPeak              $Change: 59296 $
 *   0'         Technologies      $DateTime: 2014/12/17 15:19:20 $
 *  F
 */

#ifndef _GP_IR_DATABASE_H_
#define _GP_IR_DATABASE_H_

/**
 * @file gpIRDatabase.h
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

 #include <global.h>

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

#define gpIRDatabase_IRRawDataSize      80
#define gpIRDatabase_IRCodeSize         gpIRDatabase_IRRawDataSize + 1
/**
 * @typedef gpIRDatabase_BrandId_t
 * @brief   The type representing an id of a particular TV brand.
 */
typedef UInt16 gpIRDatabase_BrandId_t;

/**
 * @typedef gpIRDatabase_IRTableId_t
 * @brief   The type representing an id for the supported IR-code tables.
 */
typedef UInt16 gpIRDatabase_IRTableId_t;

/** @name gpIRDatabase_IRTableType_t */
//@{
/** @brief TV IR type */
#define gpIRDatabase_IRTableTypeTv      0x00
/** @brief AVR IR type */
#define gpIRDatabase_IRTableTypeAvr     0x01

/** @typedef gpIRDatabase_IRTableType_t
 *  @brief The gpIRDatabase_IRTableType_t type defines the different types of supported IR databases
*/
typedef UInt8 gpIRDatabase_IRTableType_t;
//@}

/**
 * @typedef gpIRDatabase_Brand_t
 * @brief   The type containing the additional information of a particular TV brand.
 */
typedef struct gpIRDatabase_Brands_s{
    const char*                      name;
    const UInt16                     NbrOfSubTypes;
    const gpIRDatabase_IRTableId_t*  pIRTableIds;
} gpIRDatabase_Brand_t;


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_IR_DATABASE_GET_FLAG_COMPRESSED(flags)          ((flags >> 0) & 0x01)
#define GP_IR_DATABASE_GET_FLAG_ENCRYPTED(flags)           ((flags >> 1) & 0x01)
#define GP_IR_DATABASE_GET_FLAG_COMPRESSION_REV(flags)     ((flags >> 5) & 0x07)


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/**
 * This function initializes the IR-DB. It should be called before calling any other request function.
 *
*/
GP_API void gpIRDatabase_Init( void );

/**
 *  Function called to reset the gpIRDatabase component.
 */
GP_API void gpIRDatabase_Reset(void);

/**
 *  Setter function to specify the active IR-code table in the gpIRDatabase component.
 *
 *  @param IRTableId    The identifier of the IR-code table to be selected. If an invalid IR-code identifier is used,
 *                      it will be converted to 0xFFFF and no IR-code table will be selected.
 */
GP_API void gpIRDatabase_SetIRTableId(gpIRDatabase_IRTableId_t IRTableId);

/**
 *  Getter function for the identifier of the active IR-code table in the gpIRDatabase component.
 *
 *  @return             The requested identifier.
 */
GP_API gpIRDatabase_IRTableId_t gpIRDatabase_GetIRTableId(void);

/**
 *  Getter function for the type of the active IR-code table in the gpIRDatabase component.
 *
 *  @return             The requested type.
 */
GP_API gpIRDatabase_IRTableType_t gpIRDatabase_GetIRTableType(void);

/**
 *  Setter function to specify the active IR-code table in the gpIRDatabase component based on the index.
 *
 *  @param index    The index of the IR-code table to be selected. If an invalid IR-code identifier is used,
 *                  it will be converted to 0xFFFF and no IR-code table will be selected.
 */
void gpIRDatabase_SetIRTableIndex(UInt16 index);

/**
 *  Getter function for the index of the active IR-code table in the gpIRDatabase component.
 *
 *  @return             The requested index.
 */
UInt16 gpIRDatabase_GetIRTableIndex(void);

/**
 *  Getter function for the index of the first IR-code in the record table which has a bigger IR table id than specified.
 *
 *  @param  tableId     The (virtual) tableId for which the base index is requested.
 *  @return             The requested index.
 */
UInt16 gpIRDatabase_GetIRTableBaseIndex(gpIRDatabase_IRTableId_t tableId);

/**
 *  Getter function for the number of supported TV brands in the gpIRDatabase component.
 *
 *  @return             The number of supported TV brands.
 */
GP_API UInt16 gpIRDatabase_GetNumberOfTVBrands(void);

/**
 *  Getter function for the name of a particular TV brand.
 *
 *  @param brandId      The identifier of the requested TV brand.
 *  @return             A pointer to the name string of the requested TV brand. If an invalid brandId was specified,
 *                      NULL will be returned.
 */
GP_API const char* gpIRDatabase_GetTVBrandName(gpIRDatabase_BrandId_t brandId);

/**
 *  Getter function for the names of a all supported TV brands.
 *
 *  @param pBrands      A pointer used to return the pointer to the list of TV brands.
 *  @return             The number of the supported TV brands.
 */
GP_API UInt16 gpIRDatabase_GetTVBrands(const gpIRDatabase_Brand_t** pBrands);

/**
 *  Getter function for the number of supported subtypes of a particular TV brand in the gpIRDatabase component.
 *
 *  @param brandId      The identifier of the requested TV brand.
 *  @return             The number of supported subtypes.If an invalid brandId was specified, 0 will be returned.
 */
GP_API UInt16 gpIRDatabase_GetNumberOfTVSubtypes(gpIRDatabase_BrandId_t brandId);

/**
 *  Getter function for the IR-code identifier of a particular subtype.
 *
 *  @param brandId      The identifier of the requested TV brand.
 *  @param index        The index in of the subtype in the subtype array.
 *  @return             The IR-code identifier of the requested subtype. If the combination of input parameters is
 *                      invalid, 0xFFFF will be returned.
 */
GP_API gpIRDatabase_IRTableId_t gpIRDatabase_GetTVBrandIRTableId(gpIRDatabase_BrandId_t brandId, UInt16 index);

/**
 *  Getter function for the IR-code identifier of a particular subtype.
 *
 *  @param brandId      The identifier of the selected TV brand.
 *  @param index        The index in of the selected subtype in the subtype array.
 *  @return             The value of the selected IR-code.
 */
GP_API gpIRDatabase_IRTableId_t gpIRDatabase_SelectTVBrandIRTableId(gpIRDatabase_BrandId_t brandId, UInt16 index);

/**
 *  Getter function for the IR-code identifiers of a particular TV brand.
 *
 *  @param brandId      The identifier of the requested TV brand.
 *  @param pSubtypeIds  A pointer used to return the pointer to the list of IR-code identifiers. If an invalid brandId is
 *                      specified, the pointer will get value NULL.
 *  @return             The number of returned subtypes. If an invalid brandId is specified, 0 will be returned.
 */
GP_API UInt16 gpIRDatabase_GetTVBrandIRTableIds(gpIRDatabase_BrandId_t brandId, const gpIRDatabase_IRTableId_t** pSubtypeIds);

/**
 *  Getter function to collect the IR code information for a particular key. The IR rawdata will be selected from the active subtype
 *  specified with the gpIRDatabase_SetIRTableId() function.
 *
 *  @param rfKeyId      An identifier key. The value is specified as RF value.
 *  @param pRaw         A pointer to an array in which the function will copy the IR flags and the IR rawdata.
 *  @return             The size of the IR rawdata. If no valid suptype is selected or if an invalid rfKeyId is specified,
 *                      0 will be returned.
 *  @note               The caller of the function is owner of the used buffer to pass the data. The buffer should be at least gpIRDatabase_IRCodeSize bytes.
 */
GP_API UInt8 gpIRDatabase_GetIRCode(UInt8 rfKeyId, UInt8* pRaw);

GP_API void gpIRDatabase_Unpair(void);

#endif //_IR_IR_DATABASE_H_

