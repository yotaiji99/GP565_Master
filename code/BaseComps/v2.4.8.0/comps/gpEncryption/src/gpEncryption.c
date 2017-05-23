/*
 * Copyright (c) 2013-2014, GreenPeak Technologies
 *
 * gpEncryption.c
 *   
 * Contains encryption API
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpEncryption/src/gpEncryption.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpEncryption.h"
#include "gpHal.h"
#include "gpHal_SEC.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_ENCRYPTION

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public functions
 *****************************************************************************/

//-------------------------------------------------------------------------------------------------------
//  SYNCHRONOUS SECURITY FUNCTIONS
//-------------------------------------------------------------------------------------------------------
gpEncryption_Result_t gpEncryption_AESEncrypt(UInt8* pInplaceBuffer, UInt8* pAesKey, UInt8 options)
{
    return gpHal_AESEncrypt(pInplaceBuffer, pAesKey, options);
}

gpEncryption_Result_t gpEncryption_CCMEncrypt(gpEncryption_CCMOptions_t * pCCMOptions)
{
    return gpHal_CCMEncrypt(pCCMOptions);
}

gpEncryption_Result_t gpEncryption_CCMDecrypt(gpEncryption_CCMOptions_t* pCCMOptions)
{
    return gpHal_CCMDecrypt(pCCMOptions);
}

void gpEncryption_Init(void)
{
}
