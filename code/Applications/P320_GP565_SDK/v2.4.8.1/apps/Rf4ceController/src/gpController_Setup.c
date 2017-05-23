/*
 * Copyright (c) 2015, GreenPeak Technologies
 *
 * gpController_Setup.c
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/apps/Rf4ceController/src/gpController_Setup.c#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */


/*******************************************************************************
 *                      Include Files
 ******************************************************************************/
#include "gpController_Setup.h"

#include "gpSched.h"
#include "gpRf4ceActionControl_CommandCodes.h"
#include "gpLog.h"
#include "gpAssert.h"

/*******************************************************************************
 *                      Defines
 ******************************************************************************/
/** The component ID (logging) */
#define GP_COMPONENT_ID GP_COMPONENT_ID_APP
/** The time (in seconds) that the setup mode will remain enabled without user
    input */
#define SETUP_TIMEOUT 10

/* Schedule indication to relief call stack size */
#define SETUP_INDICATION(x) {\
    static UInt8 id = x; \
    gpSched_ScheduleEventArg(0, Setup_SendIndication, &id); \
    }
/*******************************************************************************
 *                      Static Defines
 ******************************************************************************/
/** A table of setup commands that need indication to the controller */

static const Setup_CommandTableEntry_t commandTable[] =
{
    /* Id,                   Index, */
#if defined(GP_DIVERSITY_APP_ZRC1_1) || defined(GP_DIVERSITY_APP_ZRC2_0)
    {  SETUP_CMD_ZRC_BIND,          gpRf4ceActionControl_CommandCodeSelect },
#endif
#ifdef GP_DIVERSITY_APP_MSO
    {  SETUP_CMD_MSO_BIND,          gpRf4ceActionControl_CommandCodeRootMenu },
#endif /*GP_DIVERSITY_APP_MSO*/

    {  SETUP_CMD_SET_VENDOR_ID,     gpRf4ceActionControl_CommandCodeElectronicProgramGuide },

    {  SETUP_CMD_UNBIND,            gpRf4ceActionControl_CommandCodeInputSelect },
    {  SETUP_CMD_FACTORY_RESET,     gpRf4ceActionControl_CommandCodePowerToggleFunction }
};


/*******************************************************************************
 *                      Static Function Defines
 ******************************************************************************/
/** @ingroup SETUP
 *
 *  This primitive triggers a timeout indication to the controller.
 */
static void Setup_Timeout(void);

/** @ingroup SETUP
 *
 *  This primitive handles the selection of a command based on received keys.
 */
static void Setup_SelectCommand(gpController_Keys_t keys);

static void Setup_SendIndication(void* arg);
/*******************************************************************************
 *                      Public Functions
 ******************************************************************************/
void gpController_Setup_Init(void)
{
    GP_LOG_PRINTF("Setup Initialized", 0);
}

void gpController_Setup_Msg(gpController_Setup_MsgId_t msgId,
                            gpController_Setup_Msg_t *pMsg)
{
    switch(msgId)
    {
        case gpController_Setup_MsgId_SetupEnterIndication:
        {
            /* Trigger a timeout indication in SETUP_TIMEOUT seconds. */
            gpSched_ScheduleEvent( (1000000UL * SETUP_TIMEOUT), Setup_Timeout );
            break;
        }
        case gpController_Setup_MsgId_KeyPressedIndication:
        {
            Setup_SelectCommand(pMsg->keys);
            break;
        }
        case gpController_Setup_MsgId_KeyReleasedIndication:
        {
            /* Not handled yet, just break */
            break;
        }
        default:
        {
            /* Unknown message ID */
            GP_ASSERT_DEV_EXT(false);
            break;
        }
    }
}

/*******************************************************************************
 *                      Static Functions
 ******************************************************************************/
static void Setup_Timeout( void )
{
    gpController_Setup_cbMsg(gpController_Setup_MsgId_cbSetupTimeoutIndication, NULL);
    gpController_Setup_cbMsg(gpController_Setup_MsgId_cbSetupLeftIndication, NULL);
}

static void Setup_SelectSetupCommand(UInt8 key, Setup_CommandId_t *cmd)
{
    UInt8 i = 0;

    *cmd = SETUP_CMD_INVALID;
    for(i=0; i<(sizeof(commandTable)/sizeof(Setup_CommandTableEntry_t)); i++)
    {
        if(key == commandTable[i].keyIndex)
        {
            *cmd = commandTable[i].commandId;
        }
    }
}

static void Setup_SelectCommand(gpController_Keys_t keys)
{

    /* setup commands only support 1 key */
    if (keys.count > 1)
    {
        return;
    }

    if (keys.count == 1)
    {
        Setup_CommandId_t command = SETUP_CMD_INVALID;
        Setup_SelectSetupCommand(keys.codes[0], &command);

        switch(command)
        {
#if defined(GP_DIVERSITY_APP_ZRC1_1) || defined(GP_DIVERSITY_APP_ZRC2_0)
            case SETUP_CMD_ZRC_BIND:
            {
                gpSched_UnscheduleEvent(Setup_Timeout);
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetupLeftIndication);
                SETUP_INDICATION(gpController_Setup_MsgId_cbZrc_BindStartIndication);
                break;
            }
#endif /*defined(GP_DIVERSITY_APP_ZRC1_1) || defined(GP_DIVERSITY_APP_ZRC2_0)*/
#ifdef GP_DIVERSITY_APP_MSO
            case SETUP_CMD_MSO_BIND:
            {
                gpSched_UnscheduleEvent(Setup_Timeout);
                /* Order is important here for main state machine. */
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetupLeftIndication);
                SETUP_INDICATION(gpController_Setup_MsgId_cbMsoBindStartIndication);
                break;
            }
#endif /*GP_DIVERSITY_APP_MSO*/
            case SETUP_CMD_UNBIND:
            {

                gpSched_UnscheduleEvent(Setup_Timeout);
                /* Order is important here for main state machine. */
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetupLeftIndication);
                SETUP_INDICATION(gpController_Setup_MsgId_cbUnbindStartIndication);
                break;
            }

            case SETUP_CMD_FACTORY_RESET:
            {
                gpSched_UnscheduleEvent(Setup_Timeout);
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetupLeftIndication);
                SETUP_INDICATION(gpController_Setup_MsgId_cbFactoryResetIndication);
                break;
            }

            case SETUP_CMD_INVALID:
            {
                gpSched_UnscheduleEvent(Setup_Timeout);
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetupLeftIndication);
                SETUP_INDICATION(gpController_Setup_MsgId_cbInvalidCommandIndication);
                break;
            }

            case SETUP_CMD_SET_VENDOR_ID:
            {
                gpSched_UnscheduleEvent(Setup_Timeout);
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetupLeftIndication);
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetVendorID);
                break;
            }

            default:
            {
                /* Unknown command ID. */
                GP_ASSERT_DEV_EXT(false);
                break;
            }
        }
    }
}

static void Setup_SendIndication(void* arg)
{
    UInt8 id = *(UInt8*)arg;
    gpController_Setup_cbMsg(id, NULL);
}
