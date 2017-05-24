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
#include "gpKeyboard.h"
#include "gpController_Main.h"
#include "gpIRDatabase.h"
#include "gpNvm.h"
#include "gpReset.h"
#include "gpController_Led.h"
#include "Hal.h"

/*******************************************************************************
 *                      Defines
 ******************************************************************************/
/** The component ID (logging) */
#define GP_COMPONENT_ID GP_COMPONENT_ID_APP
/** The time (in seconds) that the setup mode will remain enabled without user
    input */
#define SETUP_SPECIAL_MODE_TIMEOUT      10000000L

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

setup_Data_t         gpSetup_Data;


/*******************************************************************************
 *                      Static Function Defines
 ******************************************************************************/
/** @ingroup SETUP
 *
 *  This primitive triggers a timeout indication to the controller.
 */
void Setup_Timeout(void);

/** @ingroup SETUP
 *
 *  This primitive handles the selection of a command based on received keys.
 */
static void Setup_SelectCommand(gpController_Keys_t keys,UInt8 paramLength,UInt8 * params);

static void Setup_SendIndication(void* arg);
/*******************************************************************************
 *                      Public Functions
 ******************************************************************************/
gpController_cbKeyIndication_t gpController_GetKeyHandler( void )
{
    return gpController_cbKeyIndication;
}

void gpController_Setup_Init(void)
{
    GP_LOG_PRINTF("Setup Initialized", 0);
}

void gpController_Setup_Msg(gpController_Setup_MsgId_t msgId, UInt8 paramLength,
                            gpController_Setup_Msg_t *pMsg,UInt8 * params)
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
            Setup_SelectCommand(pMsg->keys,paramLength,params);
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
void Setup_Timeout( void )
{
    gpController_Setup_cbMsg(gpController_Setup_MsgId_cbSetupTimeoutIndication, NULL);
    gpController_Setup_cbMsg(gpController_Setup_MsgId_cbSetupLeftIndication, NULL);
	ControllerOperationMode = gpController_OperationModeNormal;
}

void Setup_Timeout_without_setno( void )
{
    gpController_Setup_cbMsg(gpController_Setup_MsgId_cbSetupTimeoutIndication, NULL);
    gpController_Setup_cbMsg(gpController_Setup_MsgId_cbSetupLeftIndication, NULL);
	ControllerOperationMode = gpController_OperationModeNormal;
    // if IR hunt failed, restore programmable keys to previous values and restore IrTableId.
    gpController_SelectDeviceInDatabase( gpSetup_TvHunt.backupActiveIRTableId );
	//gpSetup_TvHunt.lastAttemptedTvCode=gpSetup_TvHunt.backupActiveIRTableId;
	//gpSetup_TvHunt.currentSearchIndex=gpSetup_TvHunt.backupActiveIRTableId;
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

static UInt16 setup_ParseNumber( UInt8 * params, UInt8 ndigits )
{
    UInt16 n=0;
    UInt8 i;
    for( i=0; i<ndigits; i++ )
    {
        UInt8 digit= params[i];
        n*=10;
        if( digit > gpKeyboard_LogicalId_Number9 )
            return 0xFFFF;
        if( digit < gpKeyboard_LogicalId_Number0 )
            return 0xFFFF;
        n += params[i] - gpKeyboard_LogicalId_Number0;        
    }
    return n;
}


gpSetup_Command_t setup_TranslateKeyCombination( UInt8 paramLength, UInt8 * params )
{
    switch( params[0] )
    {
        // <<SETUP>><MENU> - MSO binding
        case gpKeyboard_LogicalId_RootMenu:
        {
            return SETUP_CMD_MSO_BIND;
        }
        // <<SETUP>><1> - command code to enter the TV brand number
        case gpKeyboard_LogicalId_Number1:
        {
            return IR_TV_SETUP;
        }
        // <<SETUP>><A> - search TV IR code
        case gpKeyboard_LogicalId_F4Yellow:
        {
            return SEARCH_TV_IR_CODE;
        }
        // <<SETUP>><B> - blink battery status
        case gpKeyboard_LogicalId_F1Blue:
        {
            return BLINK_BATTERY_STATUS;
        }
        // <<SETUP>><Vol-> - send Volume Control to DTA
        case gpKeyboard_LogicalId_VolumeDown:
        {
            return SEND_VC_TO_DTA;
        }
        // <<SETUP>><Vol+> - send Volume Control to TV
        case gpKeyboard_LogicalId_VolumeUp:
        {
            return SEND_VC_TO_TV;
        }
        // <<SETUP>><9xx> - several commands
        case gpKeyboard_LogicalId_Number9:
        {
            switch( params[1] )
            {
                case gpKeyboard_LogicalId_Number8:
                {
                    switch( params[2] )
                    {
                        // <<SETUP>><9><8><1> factory reset
                        case gpKeyboard_LogicalId_Number1:
                            return SETUP_CMD_FACTORY_RESET;
                        // <<SETUP>><9><8><3> blink SW version
                        case gpKeyboard_LogicalId_Number3:
                            return BLINK_SW_VERSION;
                        // <<SETUP>><9><8><6> reset TV IR Code
                        case gpKeyboard_LogicalId_Number6:
                            return SETUP_CMD_UNPAIR_IR;
                        // <<SETUP>><9><8><7> 
                        case gpKeyboard_LogicalId_Number7:
                            return SETUP_CMD_UNBIND;
                    	}
                }
                case gpKeyboard_LogicalId_Number9:
                {
                    switch( params[2] )
                    {
                        // <<SETUP>><9><9><0>  blink IR selection
                        case gpKeyboard_LogicalId_Number0:
                            return BLINK_TV_IR_CODE;
                    }   // end switch( params[2] )
                }
               
            }
            return INVALID_SETUP_COMMAND;
        }
        default:
        {
            return INVALID_SETUP_COMMAND;
        }
    }

}



void LED_SetOk_Control_Factory(void)
{
    gpController_Led_SetLed(true,gpController_Led_ColorGreen);
    HAL_WAIT_MS(200);
    gpController_Led_SetLed(false,gpController_Led_ColorGreen);
    HAL_WAIT_MS(100);
    gpController_Led_SetLed(true,gpController_Led_ColorGreen);
    HAL_WAIT_MS(200);
    gpController_Led_SetLed(false,gpController_Led_ColorGreen);
}


static void Setup_SelectCommand(gpController_Keys_t keys, UInt8 paramLength,UInt8 * params )
{
    volatile UInt16 deviceID=0;
    volatile UInt16 activeDeviceId=0;

    /* setup commands only support 1 key */
    if (keys.count > 1)
    {
        return;
    }

    if (keys.count == 1)
    {
///        Setup_CommandId_t command = SETUP_CMD_INVALID;
///        Setup_SelectSetupCommand(keys.codes[0], &command);
        switch(setup_TranslateKeyCombination( paramLength, params ))
        {
#if defined(GP_DIVERSITY_APP_ZRC1_1) || defined(GP_DIVERSITY_APP_ZRC2_0)
            case SETUP_CMD_ZRC_BIND:
            {
                gpSched_UnscheduleEvent(Setup_Timeout);
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetupLeftIndication);
	            ControllerOperationMode = gpController_OperationModeNormal;
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
	            ControllerOperationMode = gpController_OperationModeNormal;
                SETUP_INDICATION(gpController_Setup_MsgId_cbMsoBindStartIndication);
                break;
            }
#endif /*GP_DIVERSITY_APP_MSO*/
            case SETUP_CMD_UNBIND:
            {

                gpSched_UnscheduleEvent(Setup_Timeout);
                /* Order is important here for main state machine. */
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetupLeftIndication);
	            //ControllerOperationMode = gpController_OperationModeNormal;
	            ControllerOperationMode = gpController_OperationSetsequence;
                SETUP_INDICATION(gpController_Setup_MsgId_cbUnbindStartIndication);
				gpSched_ScheduleEvent( 100000L, LED_SetOk_Control);
                break;
            }

            case SETUP_CMD_UNPAIR_IR:
            {

				gpController_Mode=gpController_ModeIRNec;
                gpSched_UnscheduleEvent(Setup_Timeout);
	            //ControllerOperationMode = gpController_OperationModeNormal;
	            ControllerOperationMode = gpController_OperationSetsequence;
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetupLeftIndication);
				gpIRDatabase_Unpair();
				gpSched_ScheduleEvent( 1000000L, LED_SetOk_Control);
                break;
            }

            case SETUP_CMD_FACTORY_RESET:
            {
				gpController_Mode=gpController_ModeIRNec;
                gpSched_UnscheduleEvent(Setup_Timeout);
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetupLeftIndication);
	            //ControllerOperationMode = gpController_OperationModeNormal;
	            ControllerOperationMode = gpController_OperationSetsequence;
                SETUP_INDICATION(gpController_Setup_MsgId_cbFactoryResetIndication);
				HAL_WDT_RESET();
				HAL_WAIT_MS(500);
				HAL_WDT_RESET();
				HAL_WAIT_MS(500);
	            gpNvm_ClearNvm();
				LED_SetOk_Control_Factory();
	            gpReset_ResetSystem( );
                break;
            }

            case SETUP_CMD_INVALID:
            {
                gpSched_UnscheduleEvent(Setup_Timeout);
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetupLeftIndication);
	            //ControllerOperationMode = gpController_OperationModeNormal;
	            ControllerOperationMode = gpController_OperationSetsequence;
                SETUP_INDICATION(gpController_Setup_MsgId_cbInvalidCommandIndication);
				gpSched_ScheduleEvent( 500000L, LED_SetError_Control );
                break;
            }

            case SETUP_CMD_SET_VENDOR_ID:
            {
                gpSched_UnscheduleEvent(Setup_Timeout);
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetupLeftIndication);
	            //ControllerOperationMode = gpController_OperationModeNormal;
	            ControllerOperationMode = gpController_OperationSetsequence;
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetVendorID);
                break;
            }

	        case IR_TV_SETUP:
	        {
                gpSched_UnscheduleEvent(Setup_Timeout);
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetupLeftIndication);
	            //ControllerOperationMode = gpController_OperationModeNormal;
	            ControllerOperationMode = gpController_OperationSetsequence;
	            if( paramLength != 5 )
	            {
	                goto fail;
	            }
	            deviceID = setup_ParseNumber( &params[1], 4 );
	            if( deviceID == 0xFFFF )
	            {
	                goto fail;
	            }
	            // device entries for this remote are 10000 based, so add again
	            //deviceID += 10000;

	            activeDeviceId = gpIRDatabase_GetIRTableId();
	            if( !gpController_SelectDeviceInDatabase( deviceID ) )
	            {
	                gpController_SelectDeviceInDatabase( activeDeviceId );
                    GP_LOG_SYSTEM_PRINTF("Setup fail!!!,",0);
					gpSched_ScheduleEvent( 1000000L, LED_SetError_Control );
	                goto fail;
	            }
	            // also make sure the controller 'knows' to send volume control to the TV
	            gpController_SetDtaMode( false );            
                GP_LOG_SYSTEM_PRINTF("Setup Complete!!!,",0);
				gpSched_ScheduleEvent( 1000000L, LED_SetOk_Control);
	            break;
	        }

	        case BLINK_TV_IR_CODE:	//setupEA special mode·I AøAO.
	        {
                gpSched_UnscheduleEvent(Setup_Timeout);
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetupLeftIndication);
				ControllerOperationMode = gpController_OperationModeSpecial;
	            gpSetup_Data.expectedButton = gpKeyboard_LogicalId_Number1;
				gpSched_ScheduleEvent( 0, LED_SetOk_Control);;
				ControllerOperationSpecial = SET_BLINK_TV_IR_CODE;
	            break;
	        }
	        case BLINK_SW_VERSION:	//setupEA special mode·I AøAO.
	        {
                gpSched_UnscheduleEvent(Setup_Timeout);
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetupLeftIndication);
				ControllerOperationMode = gpController_OperationModeSpecial;
	            gpSetup_Data.expectedButton = gpKeyboard_LogicalId_Number1;
				gpSched_ScheduleEvent( 100000L, LED_SetOk_Control);
				ControllerOperationSpecial = SET_BLINK_SW_VERSION;
	            break;
	        }
	        case SEARCH_TV_IR_CODE:	//setup후 special mode로 진입.
	        {
                gpSched_UnscheduleEvent(Setup_Timeout);
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetupLeftIndication);
				ControllerOperationMode = gpController_OperationModeSpecial;
				ControllerOperationSpecial = SET_SEARCH_TV_IR_CODE;
	            // I always want to start with the first entry in the database
	            gpSetup_TvHunt.lastAttemptedTvCode = 0xFFFF;
	            gpSetup_TvHunt.currentSearchIndex = 0;
	            // store previous IrTable ID
	            gpSetup_TvHunt.backupActiveIRTableId = gpIRDatabase_GetIRTableId();
				setup_SearchTvIrCodeNextAttempt();
				// ED wants the first code to be sent immediately, so schedule an intermediate event
	            // since I will send an IR message immediately, I don't want to generate a success blink here
	            break;
	        }
	        case SEND_VC_TO_DTA:
	        {
                gpSched_UnscheduleEvent(Setup_Timeout);
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetupLeftIndication);
	            //ControllerOperationMode = gpController_OperationModeNormal;
	            ControllerOperationMode = gpController_OperationSetsequence;
	            if( paramLength != 1 )
	            {
	                goto fail;
	            }
	            gpController_SetDtaMode( true );
				gpSched_ScheduleEvent( 500000L, LED_SetOk_Control);
				//LED_SetOk_Control_Factory();
				break;
	        }
	        case SEND_VC_TO_TV:
	        {
                gpSched_UnscheduleEvent(Setup_Timeout);
                SETUP_INDICATION(gpController_Setup_MsgId_cbSetupLeftIndication);
	            //ControllerOperationMode = gpController_OperationModeNormal;
	            ControllerOperationMode = gpController_OperationSetsequence;
	            if( paramLength != 1 )
	            {
	                goto fail;
	            }
	            // only when we have a valid IR selection should we enable VC to TV
	            if( 0xFFFF != gpIRDatabase_GetIRTableId() )
	            {
	                gpController_SetDtaMode( false );
	            }
	            //else
	            //{
	            //    goto fail;
	            //}
				gpSched_ScheduleEvent( 500000L, LED_SetOk_Control);
				//LED_SetOk_Control_Factory();
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
	fail:
    return;
		
}

static void Setup_SendIndication(void* arg)
{
    UInt8 id = *(UInt8*)arg;
    gpController_Setup_cbMsg(id, NULL);
}
