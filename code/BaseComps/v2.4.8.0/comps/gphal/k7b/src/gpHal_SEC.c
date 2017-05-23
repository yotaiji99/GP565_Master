/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 * gpHal_SEC.c
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/src/gpHal_SEC.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpHal.h"
#include "gpHal_DEFS.h"
#include "gpHal_SEC.h"
#include "gpHal_MAC.h"


//GP hardware dependent register definitions
#include "gpHal_HW.h"
#include "gpHal_reg.h"

#include "gpAssert.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

#ifdef GP_DIVERSITY_GPHAL_EXTERN
/** @brief Security scratch: key, nonce */
#define GP_HAL_SEC_KEY_STORAGE          (GP_MM_RAM_LINEAR_START + (GP_MM_RAM_PBM_7_DATA_END - GP_MM_RAM_PBM_OPTS_START))         //Rf4ce + Mac2006 sec key length 16 Byte
#define GP_HAL_SEC_NONCE_STORAGE        (GP_HAL_SEC_KEY_STORAGE + 0x10)    //Nonce length fixed to 13Byte (Rf4ce + Mac2006)
#define GP_HAL_SEC_PACKET_BUFFER        (GP_HAL_SEC_NONCE_STORAGE + 13)
#endif

#define GP_HAL_SEC_USER_KEY_ID_TO_LOCATION(id)      (GP_MM_FLASH_USER_KEY_0_START + id*GP_MM_FLASH_USER_KEY_0_SIZE)
#define GP_HAL_SEC_PRODUCT_KEY_ID_TO_LOCATION(id)   (GP_MM_FLASH_MASTER_KEY_0_START + (id - gpEncryption_KeyIdProductKey0)*GP_MM_FLASH_MASTER_KEY_0_SIZE)

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

//Internal functions
static gpHal_Result_t gpHalSec_CCMbase (gpEncryption_CCMOptions_t* pCCMOptions,UInt8 mode);
/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Public functions
 *****************************************************************************/

//-------------------------------------------------------------------------------------------------------
//  SYNCHRONOUS SECURITY FUNCTIONS
//-------------------------------------------------------------------------------------------------------


gpHal_Result_t gpHal_AESEncrypt(UInt8* pInplaceBuffer, UInt8* pAesKey, UInt8 options)
{
    //24-bit address range
    UInt32 dataPtr;
    UInt32 keyPtr;

    UInt8* pPatchedAesKey;
    UInt8  keyBufferDummy[16] = {0x0};

    gpEncryption_KeyId_t keyId = GP_ENCRYPTION_OPTIONS_GET_KEYID(options);

#ifdef GP_DIVERSITY_GPHAL_EXTERN
    //Copy data to be encrypted into the security buffer
    dataPtr = GP_HAL_SEC_PACKET_BUFFER;
    GP_HAL_WRITE_BYTE_STREAM(dataPtr,(void *)pInplaceBuffer,16);
#else
    dataPtr = (UIntPtr)(pInplaceBuffer);
#endif

    //Set Data input and output Ptr
    GP_WB_WRITE_SSP_MSG_PTR(dataPtr);
    GP_WB_WRITE_SSP_MSG_OUT_PTR(dataPtr);

    //Select encryption key
    if(GP_ENCRYPTION_KEYID_IS_USER(keyId))
    {
        keyPtr = GP_HAL_SEC_USER_KEY_ID_TO_LOCATION(keyId);
    }
    else if(GP_ENCRYPTION_KEYID_IS_PRODUCT(keyId))
    {
        keyPtr = GP_HAL_SEC_PRODUCT_KEY_ID_TO_LOCATION(keyId);
    }
    else if(GP_ENCRYPTION_KEYID_IS_KEYPTR(keyId))
    {
        //resolve potential NULL key pointer
        if(NULL == pAesKey)
        {
            pPatchedAesKey = keyBufferDummy;
        }
        else
        {
            pPatchedAesKey = pAesKey;
        }
#ifdef GP_DIVERSITY_GPHAL_EXTERN
        //Copy encryption key into the key buffer
        keyPtr = GP_HAL_SEC_KEY_STORAGE;
        GP_HAL_WRITE_BYTE_STREAM(keyPtr,pPatchedAesKey,16);
#else
        keyPtr = (UIntPtr)pPatchedAesKey;
#endif
    }
    else
    {
        GP_ASSERT_DEV_INT(false);
        return gpHal_ResultInvalidParameter;
    }

    //Set encryption key
    GP_WB_WRITE_SSP_KEY_PTR(keyPtr);

    //Harden with secret key
    if(GP_ENCRYPTION_OPTIONS_IS_HARDENED(options))
    {
        GP_WB_WRITE_SSP_USE_SKEY(1);
    }
    else
    {
        GP_WB_WRITE_SSP_USE_SKEY(0);
    }

    //Set the lengths, till now keysize always 128bit and msg Length = 16byte
    GP_WB_WRITE_SSP_KEY_LEN(GP_WB_ENUM_SSP_KEY_LEN_KEY_128);
    GP_WB_WRITE_SSP_MSG_LEN(16);

    //Set the mode
    GP_WB_WRITE_SSP_MODE(GP_WB_ENUM_SSP_MODE_AES);

    //Start encryption and poll for finish
    GP_WB_SSP_START_ENCRYPT();
    GP_DO_WHILE_TIMEOUT_ASSERT(GP_WB_READ_SSP_BUSY(), GP_HAL_DEFAULT_TIMEOUT);

    //Assert when memory error occured
    GP_ASSERT_DEV_EXT(0 == GP_WB_READ_SSP_MEM_ERR());

#ifdef GP_DIVERSITY_GPHAL_EXTERN
    GP_HAL_READ_BYTE_STREAM(dataPtr,(void *)pInplaceBuffer,16);
#endif

    return gpHal_ResultSuccess;   //successfull request
}

gpHal_Result_t gpHal_CCMEncrypt(gpEncryption_CCMOptions_t * pCCMOptions)
{
    //Call CCM base function
    return gpHalSec_CCMbase(pCCMOptions,GP_WB_ENUM_SSP_MODE_ENCRYPT);
}

gpHal_Result_t gpHal_CCMDecrypt(gpEncryption_CCMOptions_t* pCCMOptions)
{
    //Call CCM base function
    return gpHalSec_CCMbase(pCCMOptions,GP_WB_ENUM_SSP_MODE_DECRYPT);
}

//-------------------------------------------------------------------------------------------------------
//  INTERNAL HELPER FUNCTIONS
//-------------------------------------------------------------------------------------------------------

gpHal_Result_t gpHalSec_CCMbase (gpEncryption_CCMOptions_t* pCCMOptions,UInt8 mode)
{
    UInt32 GP_HAL_SEC_KEY_STORAGE    = ((gpHal_Address_t)pCCMOptions->pKey);
    UInt32 GP_HAL_SEC_NONCE_STORAGE  = ((gpHal_Address_t)pCCMOptions->pNonce);
    UInt8 PBMhandle;
    UInt8 dataWithMicLength;
    UInt32 tempPtrBuf;
    UInt8 checkHandle;

    GP_ASSERT_DEV_EXT(NULL != pCCMOptions);
    GP_ASSERT_DEV_EXT(NULL != pCCMOptions->pKey);
    GP_ASSERT_DEV_EXT(NULL != pCCMOptions->pNonce);
    GP_ASSERT_DEV_EXT((pCCMOptions->dataLength + pCCMOptions->auxLength + pCCMOptions->micLength) <= 0x80);

    pCCMOptions->auxOffset &= 0x7f;
    pCCMOptions->dataOffset &= 0x7f;
    dataWithMicLength = pCCMOptions->dataLength + pCCMOptions->micLength;

    //Copy data and aux from Pd to Pbm in case of pdram
    PBMhandle = gpPd_SecRequest(pCCMOptions->pdHandle,
                                pCCMOptions->dataOffset,
                                //For decryption we also need to copy mic from pd to pbm !
                                (mode == GP_WB_ENUM_SSP_MODE_ENCRYPT) ? pCCMOptions->dataLength : dataWithMicLength,
                                pCCMOptions->auxOffset,
                                pCCMOptions->auxLength);
    if(!GP_HAL_CHECK_PBM_VALID(PBMhandle))
    {
        return gpHal_ResultBusy;
    }

    //Point circular writing window to packet buffer location
    {
    UInt16 pbmStartPtr = 0x0;
    GP_HAL_READ_REGS16(GP_WB_MM_PBM_0_DATA_BASE_ADDRESS_ADDRESS + PBMhandle*2, &pbmStartPtr); //Relative to RAM linear start
    GP_WB_WRITE_MM_CIRC_WINDOW_0_BASE_ADDRESS(pbmStartPtr); //address Relative to RAM linear start - already OK, we read it out the PBM window address register
    }
    //Set data pointer and length, linearize security data + reserved space for MIC if needed
    tempPtrBuf = GP_MM_RAM_CIRC_WINDOW_LEFT_0_START + pCCMOptions->dataOffset;
    GP_WB_WRITE_SSP_MSG_PTR(tempPtrBuf);
    GP_WB_WRITE_SSP_MSG_OUT_PTR(tempPtrBuf);
    GP_WB_WRITE_SSP_MSG_LEN(pCCMOptions->dataLength);

    //Write micPtr and length, mic will be placed after data
    tempPtrBuf += pCCMOptions->dataLength;
    GP_WB_WRITE_SSP_MIC_PTR(tempPtrBuf);
    GP_WB_WRITE_SSP_MIC_LEN((pCCMOptions->micLength) >> 1); //MIC length divide by 2!

    //Set the auxPtr and length, if needed linearize aux data
    //aux is only input for security engine so no need to delinearize and restore data later on
    tempPtrBuf = GP_MM_RAM_CIRC_WINDOW_LEFT_0_START + pCCMOptions->auxOffset;
    GP_WB_WRITE_SSP_A_PTR(tempPtrBuf);
    GP_WB_WRITE_SSP_A_LEN(pCCMOptions->auxLength);

    //Write key + keyPtr
    GP_WB_WRITE_SSP_KEY_PTR(GP_HAL_SEC_KEY_STORAGE);
    GP_WB_WRITE_SSP_KEY_LEN(GP_WB_ENUM_SSP_KEY_LEN_KEY_128);

    //Write Nonce + noncePtr
    GP_WB_WRITE_SSP_NONCE_PTR(GP_HAL_SEC_NONCE_STORAGE);

    //Set the mode & start encryption/decryption
    GP_WB_WRITE_SSP_USE_SKEY(0);
    GP_WB_WRITE_SSP_MODE(mode);
    GP_WB_SSP_START_ENCRYPT();

    //Polled wait for encryption to finish & assert when memory error occured
    GP_DO_WHILE_TIMEOUT_ASSERT(GP_WB_READ_SSP_BUSY(), GP_HAL_DEFAULT_TIMEOUT);
    GP_ASSERT_DEV_EXT(0 == GP_WB_READ_SSP_MEM_ERR());

    //returned pdHandle should be te same, copy mic aswell for decryption !
    checkHandle = gpPd_cbSecConfirm(PBMhandle, pCCMOptions->dataOffset, dataWithMicLength);
    GP_ASSERT_DEV_EXT(checkHandle == pCCMOptions->pdHandle);

    //If decryption was succesfull, check MIC (should be all zero)
    if (GP_WB_ENUM_SSP_MODE_DECRYPT == mode)
    {
        UInt8 micData[16]={0}; //Maximum MIC length
        UIntLoop i;
        //Mic will be after data, check if all 0 after decryption
        gpPd_ReadByteStream(pCCMOptions->pdHandle,pCCMOptions->dataOffset + pCCMOptions->dataLength, pCCMOptions->micLength, micData);

        for (i = 0; i < pCCMOptions->micLength; ++i)
        {
            if (0 != micData[i])
            {
                //Decryption failed
                return gpHal_ResultInvalidParameter;
            }
        }
    }

    return gpHal_ResultSuccess;
}

