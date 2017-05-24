/*
 * Copyright (c) 2015, GreenPeak Technologies
 *
 * gpController_Setup.h
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/apps/Rf4ceController/inc/gpController_Setup.h#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */


#ifndef _GPCONTROLLER_SETUP_H_
#define _GPCONTROLLER_SETUP_H_

///@file gpController_Setup.h
///

/*******************************************************************************
 *                      Include Files
 ******************************************************************************/
#include "global.h"
#include "gpController.h"
#include "gpController_Led.h"

/*******************************************************************************
 *                    Type Definitions
 ******************************************************************************/

/** @name gpController_Setup_MsgId_t */
//@{
/** @brief Indicate to Setup that Setup Mode needs entering. */
#define gpController_Setup_MsgId_SetupEnterIndication               0x01
/** @brief Indicate to controlller Setup Mode was left. */
#define gpController_Setup_MsgId_cbSetupLeftIndication              0x02
/** @brief Indicate to Setup that a key(s) were pressed */
#define gpController_Setup_MsgId_KeyPressedIndication               0x03
/** @brief Indicate to Setup that a key(s) were released */
#define gpController_Setup_MsgId_KeyReleasedIndication              0x04
/** @brief Indicate to controller that a ZRC bind needs to be triggered. */
#define gpController_Setup_MsgId_cbZrc_BindStartIndication          0x05
#ifdef GP_DIVERSITY_APP_MSO
/** @brief Indicate to controller that a MSO bind needs to be triggered. */
#define gpController_Setup_MsgId_cbMsoBindStartIndication           0x07
#endif /*GP_DIVERSITY_APP_MSO*/
/** @brief Indicate to controller that a unbind needs to be triggered. */
#define gpController_Setup_MsgId_cbUnbindStartIndication            0x08
/** @brief Indicate to controller an invalid setup command was provided. */
#define gpController_Setup_MsgId_cbInvalidCommandIndication         0x09
/** @brief Indicate to controller that setup mode timed out. */
#define gpController_Setup_MsgId_cbSetupTimeoutIndication           0x0A
/** @brief Indicate to controller that a factory reset needs to be triggered. */
#define gpController_Setup_MsgId_cbFactoryResetIndication           0x0B
/** @brief Indicate to controller that a set vendor ID to be triggered. */
#define gpController_Setup_MsgId_cbSetVendorID                      0x0C

/** @typedef gpController_Setup_MsgId_t
 *  @brief The gpController_Setup_MsgId_t type defines the Setup message IDs.
*/
typedef UInt8 gpController_Setup_MsgId_t;
//@}
typedef UInt8 gpSetup_Command_t;
typedef union {
    gpController_Keys_t keys;
} gpController_Setup_Msg_t;
typedef union {
    gpController_Keys_t keys;
} gpController_IR_Msg_t;



#define BIND_MSO_COMBO          0x00
#define BIND_ZRC_COMBO          0x01
#define INIT_IR_TV_SETUP        0x02
#define ENTER_XIR_MODE          0x04
#define ENTER_IRMOTO_MODE       0x05
#define ENTER_IRCISCO_MODE      0x06
#define SETUP_OTHERS            0x07

#define FACTORY_RESET           0x08
#define BLINK_SW_VERSION        0x09
#define RESET_TV_IR_CODE        0x0a
#define UNPAIR_RF4CE            0x0b
#define BLINK_TV_IR_CODE        0x0c
#define BLINK_BATTERY_STATUS    0x0d
#define SEND_VC_TO_DTA          0x0e
#define SEND_VC_TO_TV           0x0f
#define SEARCH_TV_IR_CODE       0x10
#define BLINK_MODE              0x11
#define ENABLE_RF_STATISTICS    0x12
#define ENABLE_DYN_RF_STAT      0x20
#define ENTER_DYN_STAT_CNT      0x21
#define SETUP_CMD_UNPAIR_IR		0x22
#define SET_VENDOR_ID           0x14

#define INVALID_SETUP_COMMAND   0xff


/** @typedef gpController_Setup_Msg_t
 *  The gpController_Setup_Msg_t type specifies Setup specific messages to
 *  exchange with the MAIN module.
*/

#define SETUP_CMD_ZRC_BIND                  0x01
#ifdef GP_DIVERSITY_APP_MSO
#define SETUP_CMD_MSO_BIND                  0x03
#endif /*GP_DIVERSITY_APP_MSO*/
#define SETUP_CMD_UNBIND                    0x04
#define SETUP_CMD_FACTORY_RESET             0x05
#define SETUP_CMD_SET_VENDOR_ID             0x06
#define IR_TV_SETUP							0x07
#define SETUP_CMD_INVALID                   0xff

typedef UInt8 Setup_CommandId_t;

typedef struct
{
    Setup_CommandId_t commandId;
    UInt8             keyIndex;
} Setup_CommandTableEntry_t;

typedef union setup_Data
{
    UInt8   expectedButton;
    struct  {
        UInt8 keyCount;
        union
        {
            UInt8   initialKey;
            UInt32  efcCode;
        } data;
    } RemapKey;
} setup_Data_t;


/******************************************************************************
 *                    Public Function Definitions
 ******************************************************************************/
/** @ingroup SETUP
 *
 *  This primitive can be used to initialize the Setup module.
  */
GP_API void gpController_Setup_Init(void);

/** @ingroup SETUP
 *
 *  This primitive can be used to do a request to the Setup module.
 *
 *  Completion of the request will be indicated by triggering a confirm using
 *  the gpController_Setup_cbMsg callback.
 *
 *  @param msgId Identifier for the message.
 *  @param pMsg  Data passed in the message (NULL for indication without data).
 */
GP_API void gpController_Setup_Msg(     gpController_Setup_MsgId_t msgId,UInt8 paramLength,
                                        gpController_Setup_Msg_t *pMsg,UInt8 * params);

/** @ingroup SETUP
 *
 *  This primitive indicates (to the next higher layer) that an event occured.
 *
 *  @param msgId Identifier for the message.
 *  @param pMsg  Data passed in the message (NULL for indication without data).
 */
GP_API void gpController_Setup_cbMsg(    gpController_Setup_MsgId_t msgId,
                                         gpController_Setup_Msg_t *pMsg);
#endif /* _GPCONTROLLER_SETUP_H_ */

GP_API void Setup_Timeout(void);
GP_API void Setup_Timeout_without_setno( void );

GP_API setup_Data_t         gpSetup_Data;
