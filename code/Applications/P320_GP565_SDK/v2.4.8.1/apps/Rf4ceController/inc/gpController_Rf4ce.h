/*
 * Copyright (c) 2015, GreenPeak Technologies
 *
 * gpController_Rf4ce.h
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/apps/Rf4ceController/inc/gpController_Rf4ce.h#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */


#ifndef _GPCONTROLLER_RF4CE_H_
#define _GPCONTROLLER_RF4CE_H_

///@file gpController_Rf4ce.h
///

/*******************************************************************************
 *                      Include Files
 ******************************************************************************/
#include "global.h"
#include "gpRf4ce.h"

 /*******************************************************************************
 *                    Type Definitions
 ******************************************************************************/
/** @name gpController_Rf4ce_MsgId_t */
//@{
/** @brief Request start of the RF4CE network layer. */
#define  gpController_Rf4ce_MsgId_StartRequest              0x01
/** @brief Confirm to the controller the RF4CE network layer has started. */
#define  gpController_Rf4ce_MsgId_cbStartConfirm            0x02
/** @brief Indicate to the RF4CE module a factory reset needs to be done. */
#define  gpController_Rf4ce_MsgId_FactoryResetIndication    0x03
/** @brief Indicate to the RF4CE module a NVM backup needs to be done. */
#define  gpController_Rf4ce_MsgId_BackupNvm                 0x04
/** @brief Indicate to the RF4CE module some vendor specific data needs sending. */
#define  gpController_Rf4ce_MsgId_SendVendorDataRequest     0x05
/** @brief Indicate to the MAIN module the vendor specific data was sent. */
#define  gpController_Rf4ce_MsgId_cbSendVendorDataConfirm   0x06
/** @brief Request to get the security status for the link specified by the bindingId parameter. True will be returned if the link supports security, false if not.*/
#define  gpController_Rf4ce_MsgId_IsLinkSecure              0x07


/** @brief Set/Get the binding ID attribute */
#define  gpController_Rf4ce_MsgId_AttrBindingId             0x83
/** @brief Set/Get the profile ID attribute */
#define  gpController_Rf4ce_MsgId_AttrProfileId             0x84
/** @brief Set/Get the binding node capabilities attribute */
#define  gpController_Rf4ce_MsgId_AttrNodeCapabilities      0x85
/** @brief Request to set/get the vendor ID (16-bit value) to be used by the RF4CE stack. */
#define  gpController_Rf4ce_MsgId_AttrVendorId              0x86
/** @brief Request to set/get the vendor string (7 bytes ASCII encoded) to be used by the RF4CE stack. */
#define  gpController_Rf4ce_MsgId_AttrVendorString          0x87
/** @brief Request to set/get the user string (8 bytes ASCII encoded for ZRC, 9 bytes ASCII encoded for MSO profile) to be used by the RF4CE stack. */
#define  gpController_Rf4ce_MsgId_AttrUserString            0x88


/** @typedef gpController_Rf4ce_MsgId_t
 *  @brief The gpController_Rf4ce_MsgId_t type defines the Rf4ce message IDs.
*/
typedef UInt8 gpController_Rf4ce_MsgId_t;
//@}


/** @typedef gpController_Rf4ce_SendVendorData_t
 *  The gpController_Rf4ce_SendVendorData_t type specifies vendor specific messages
 *  to send to a target.
*/
typedef struct gpController_Rf4ce_VendorData
{
    gpRf4ce_VendorId_t vendorId;
    UInt16 txOptions;
    UInt8 *data;
    UInt8 len;

}gpController_Rf4ce_SendVendorData_t;

/** @typedef gpController_Rf4ce_SendVendorDataConfirm_t
 *  The gpController_Rf4ce_SendVendorDataConfirm_t type defines parameters that need to be
 * passed to the controller when an send vendor specific data action is confirmed.
*/
typedef struct gpController_Rf4ce_SendVendorDataConfirm
{
    UInt8 bindingId;
    gpRf4ce_Result_t status;
}gpController_Rf4ce_SendVendorDataConfirm_t;


/** @typedef gpController_Rf4ce_Msg_t
 *  The gpController_Rf4ce_Msg_t type specifies RF4CE specific messages to
 *  exchange with the MAIN module.
*/
typedef union {
    /** The binding ID. */
    UInt8 bindingId;
    /** The profile ID. */
    UInt8 profileId;
    /* The node capabilities.*/
    UInt8 nodeCapabilities;
    gpRf4ce_VendorId_t vendorId;
    gpRf4ce_VendorString_t vendorString;
    gpRf4ce_UserString_t userString;
    /* Vendor data. */
    gpController_Rf4ce_SendVendorData_t vendorData;
    gpController_Rf4ce_SendVendorDataConfirm_t vendorDataConfirm;
    Bool isLinkSecure;
} gpController_Rf4ce_Msg_t;


/*******************************************************************************
 *                    Public Function Definitions
 ******************************************************************************/
 /** @ingroup RF4CE
 *
 *  This primitive can be used to initialize the Rf4ce module.
  */
GP_API void gpController_Rf4ce_Init(void);

/** @ingroup RF4CE
 *
 *  This primitive can be used to do a request to the Rf4ce module.
 *
 *  Completion of the request will be indicated by triggering a confirm using
 *  the gpController_Rf4ce_cbMsg callback.
 *
 *  @param msgId Identifier for the message.
 *  @param pMsg  Data passed in the message (NULL for indication without data).
 */
GP_API void gpController_Rf4ce_Msg(gpController_Rf4ce_MsgId_t msgId, gpController_Rf4ce_Msg_t *pMsg);

/** @ingroup RF4CE
 *
 *  This primitive indicates (to the next higher layer) that an event occured.
 *
 *  @param msgId Identifier for the message.
 *  @param pMsg  Data passed in the message (NULL for indication without data).
 */
GP_API void gpController_Rf4ce_cbMsg(gpController_Rf4ce_MsgId_t msgId, gpController_Rf4ce_Msg_t *pMsg);

/** @ingroup RF4CE
 *
 *  This primitive can be used to set an attribute for the RF4CE module.
 *
 *  @param msgId Identifier for the attribute.
 *  @param pMsg  Data passed to the attribute.
 */
GP_API void gpController_Rf4ce_AttrGet(gpController_Rf4ce_MsgId_t msgId, gpController_Rf4ce_Msg_t *pMsg);


/** @ingroup RF4CE
 *
 *  This primitive can be used to get an attribute from the RF4CE module.
 *
 *  @param msgId Identifier for the attribute.
 *  @param pMsg  Data passed back to the requesting module.
 */
GP_API void gpController_Rf4ce_AttrSet(gpController_Rf4ce_MsgId_t msgId, gpController_Rf4ce_Msg_t *pMsg);

#endif /* _GPCONTROLLER_RF4CE_H_ */
