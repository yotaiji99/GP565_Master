/*
 * Copyright (c) 2008-2014, GreenPeak Technologies
 *
 * gpHal_SEC.h
 *
 * Contains all security functionality of the HAL
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/inc/gpHal_SEC.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _HAL_GP_SEC_H_
#define _HAL_GP_SEC_H_

/** @file gpHal_SEC.h
 *  This file contains all security functionality of the HAL.  Standalone AES encryption can be performed as well as CCM encryption and decryption.
 *
 *  @brief Contains all security functionality of the HAL
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gp_global.h"
#include "gpEncryption.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

// SEC public functions
/**
 * @brief Performs a synchronous AES Encryption.
 *
 * The function will encrypt 16 bytes with the AES algorithm and return the result in place.
 *
 * Possible results are:
 *          - gpHal_ResultSuccess
 *          - gpHal_ResultBusy
 *
 * @param pInplaceBuffer   Pointer to the buffer of the 16 to be encrypted bytes. Encrypted result will be returned in same buffer
 * @param pAesKey          Pointer to the 16 byte key, this key is only uses if specified by the options parameter. When used but specified as NULL, 0 will be used as key value.
 * @param options          This parameter is an 8bit bitmask specifying the options: bits[6:0] specify the keyid to be used (see gpEncryption_API_Manual); bit[7] indicates additional hardening
 */
GP_API gpHal_Result_t gpHal_AESEncrypt(UInt8* pInplaceBuffer, UInt8* pAesKey, UInt8 options);

/**
 * @brief Performs a synchronous CCM Encryption.
 *
 * The function will encrypt the bytes with the CCM algorithm according to the specified options
 * in the gpHal_CCMOptions structure.
 *
 * Possible results are:
 *          - gpHal_ResultSuccess
 *          - gpHal_ResultBusy
 *
 * @param pCCMOptions        Pointer to the gpHal_CCMOptions structure.
*/
GP_API gpHal_Result_t gpHal_CCMEncrypt(gpEncryption_CCMOptions_t * pCCMOptions);


/**
 * @brief Performs a synchronous CCM Decryption.
 *
 * The function will decrypt the bytes with the CCM algorithm according to the specified options
 * in the gpHal_CCMOptions structure.
 *
 * Possible results are:
 *          - gpHal_ResultSuccess
 *          - gpHal_ResultBusy
 *          - gpHal_ResultInvalidParameter
 *
 * @param pCCMOptions        Pointer to the gpHal_CCMOptions structure.
*/
GP_API gpHal_Result_t gpHal_CCMDecrypt(gpEncryption_CCMOptions_t * pCCMOptions);

#endif //_HAL_GP_SEC_H_
