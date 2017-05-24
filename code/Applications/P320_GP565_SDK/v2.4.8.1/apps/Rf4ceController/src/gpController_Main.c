/*
 * Copyright (c) 2015, GreenPeak Technologies
 *
 * gpController_Main.c
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/apps/Rf4ceController/src/gpController_Main.c#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */


/*******************************************************************************
 *                    Include Files
 ******************************************************************************/
#include "gpController_Main.h"
#include "gpController_Led.h"
#include "gpController_Rf4ce.h"
#include "gpController_Zrc.h"
#ifdef GP_DIVERSITY_APP_ZID
#include "gpController_Zid.h"
#endif
#include "gpController_KeyBoard.h"
#include "gpController_Setup.h"
#include "gpController_BatteryMonitor.h"
#ifdef GP_RF4CEVOICE_DIVERSITY_ORIGINATOR
#include "gpController_Voice.h"
#endif

#ifdef GP_DIVERSITY_APP_MSO
#include "gpController_Mso.h"
#endif

#include "gpLog.h"
#include "gpAssert.h"
#include "gpSched.h"
#include "gpPoolMem.h"
#include "gpRf4ceActionControl_CommandCodes.h"

#include "gpIrTx.h"
#include "gpIrDatabase.h"
#include "gpReset.h"
#include "Hal.h"
/*******************************************************************************
 *                    Defines
 ******************************************************************************/
/** The component ID (logging) */
#define GP_COMPONENT_ID GP_COMPONENT_ID_APP
#define GP_PROGRAMMABLE_KEY_NUMBER_OF_KEYS           6  /* The keyboard support 4 IR keys{Power toggle, Vol up, Vol down, Mute} */
#ifdef GP_DIVERSITY_XSIF_DEBUG_ENABLED
#define GP_DEFAULT_DEVICE_ID                        	1
#else
#define GP_DEFAULT_DEVICE_ID                        	136
#endif
#define GP_PROGKEY_IRCODE_TXDEFINITION(code)        (gpIrTx_TransmissionDefinition_t*)((code))
#define GP_PROGKEY_IRCODE_TXTIMINGDATA(code)        (gpIrTx_TransmissionTimingData_t*)((code)+8)
#define APP_TV_IR_DATA_AVAILABLE                        0x01

#define APP_KEYMAP_VOL_CTRL_INDEX           21

#define APP_KEYMAP_TV_POWER_INDEX           36
#define APP_KEYMAP_VOL_UP_INDEX           	21
#define APP_KEYMAP_VOL_DN_INDEX           	33
#define APP_KEYMAP_MUTE_INDEX  	          	07
#define APP_KEYMAP_TV_INPUT_INDEX           20

#define APP_NUMBER_OF_TV_IR_KEYS                        sizeof( controller_TvIrKey_LogicalKeyId )
#define APP_NUMBER_OF_VOLUME_KEYS                       3
#define APP_NUMBER_OF_TV_KEYS                           5
#define APP_REGULAR_KEY_NUMBER_OF_KEYS                  29+2
#define APP_TOTAL_NUMBER_OF_KEYS                        APP_REGULAR_KEY_NUMBER_OF_KEYS + APP_NUMBER_OF_VOLUME_KEYS + APP_NUMBER_OF_TV_KEYS
#define SETUP_INVALID_INDEX             0xFFFF
#define SETUP_INVALID_TV_CODE           0xFFFF
#define APP_TV_IR_POWEROFF_INDEX            0
#define APP_TV_IR_VOL_CTRL_INDEX            2
/*******************************************************************************
 *                    Static Data Definitions
 ******************************************************************************/
/* Controller operation mode */
UInt8 ControllerOperationMode = gpController_OperationModeNormal;
/* Default binding ID */
static UInt8 ControllerBindingId = 0xFF;
static UInt8 ControllerProfileId = 0xFF;
static UInt16 ControllerTxOptions = 0;
static UInt16 ControllerVendorId  = GP_RF4CE_NWKC_VENDOR_IDENTIFIER;
static Bool ControllerKeyPressConfirmPending = false;



const UInt8 controller_TvIrKey_LogicalKeyId[] = {
        gpKeyboard_LogicalId_PowerToggleFunction,
        gpKeyboard_LogicalId_PowerToggleFunction ,
        gpKeyboard_LogicalId_VolumeUp        ,
        gpKeyboard_LogicalId_VolumeDown      ,
        gpKeyboard_LogicalId_Mute            ,
        gpKeyboard_LogicalId_InputSelect	 ,
    };


#ifdef GP_RF4CEVOICE_DIVERSITY_ORIGINATOR
static Bool audioActive = false;
#endif

typedef struct gpProgrammableKey_TvIrDesc_s
{
    UInt8                           code[GP_PROGKEY_MAX_IRCODE_SIZE];
} gpProgrammableKey_TvIrDesc_t;

typedef struct 
	gpKeyboard_Command {
    gpKeyboard_CmdType_t cmd;
    UInt8 params[GP_KEYBOARD_MAX_CMD_PARAM_LENGTH];
} gpKeyboard_Command_t;

typedef struct ProgrammableKey_KeyDesc_s{
    UInt8                               keyConfig;
    gpProgrammableKey_TvIrDesc_t        tvIrDesc;
} ProgrammableKey_KeyDesc_t;

typedef struct gpKeyVendorIdCode {
    UInt8 gpKeyCodeMap;
    UInt8 gpKeyVendorIDCode;
} gpKeyVendorIdCode_t;


UInt8 PowerToggle = 0;
Bool gpController_PowerToggle;
ProgrammableKey_KeyDesc_t gpProgrammableKey_Db[GP_PROGRAMMABLE_KEY_NUMBER_OF_KEYS];
controller_KeyDesc_t controller_TvIrKey_Db[APP_NUMBER_OF_TV_IR_KEYS];
UInt8 keyTransmitAction[ APP_TOTAL_NUMBER_OF_KEYS ];
gpKeyboard_Command_t  gpKeyboard_Cmd = {gpKeyboard_CmdTypeInvalid, {0x0}};
UInt16 gpDeviceID = 0xFFFF;             // default device id
gpController_cbKeyIndication_t  gpController_cbKeyIndication; //function pointer used to forward the key-information to the mode/setup specific handling
gpController_Mode_t             gpController_Mode=gpController_ModeIRNec;                  //current controller application mode

//Static buffer needed for SetRIB request
UInt8 gpStatus_RIBData[11];


/* keyboard mapping needs to match gpController_KeyBoard module. */
static const UInt8 ROM gpKeyCodeMap[] FLASH_PROGMEM =
{
    /* r=row, c=column */
    /* r0, c0*/
    /* 01 */    gpRf4ceActionControl_CommandCodeF1Blue                      ,
    /* 02 */    gpRf4ceActionControl_CommandCodePageDown                    ,
    /* 03 */    gpRf4ceActionControl_CommandCodeNumber6                     ,
    /* 04 */    gpRf4ceActionControl_CommandCodeUp                          ,
    /* 05 */    gpRf4ceActionControl_CommandCodeRight                       ,
    /* 06 */    gpRf4ceActionControl_CommandCodeDown                        ,
    /* 07 */    gpRf4ceActionControl_CommandCodeMute                        ,
    /* 08 */    gpRf4ceActionControl_CommandCodeF3Green                     ,
    /* 09 */    gpRf4ceActionControl_CommandCodeNumber9                     ,
    /* 10 */    gpRf4ceActionControl_CommandCodeNumber8                     ,
    /* 11 */    gpRf4ceActionControl_CommandCodeSelect                      ,
    /* 12 */    gpRf4ceActionControl_CommandCodeDisplayInformation          ,
    /* 13 */    gpRf4ceActionControl_CommandCodeRootMenu                    ,
    /* 14 */    gpRf4ceActionControl_CommandCodeF2Red                       ,
    /* 15 */    gpRf4ceActionControl_CommandCodeNone                        ,
    /* 16 */    gpRf4ceActionControl_CommandCodeNumber5                     ,
    /* 17 */    gpRf4ceActionControl_CommandCodeNumber2                     ,
    /* 18 */    gpRf4ceActionControl_CommandCodeNumber0                     ,
    /* 19 */    gpRf4ceActionControl_CommandCodeNone                        ,
    /* 20 */    gpRf4ceActionControl_CommandCodeInputSelect		            ,
    /* 21 */    gpRf4ceActionControl_CommandCodeVolumeUp                    ,								
    /* 22 */    gpRf4ceActionControl_CommandCodeNumber1                     ,
    /* 23 */    gpRf4ceActionControl_CommandCodeNone                        ,
    /* 24 */    gpRf4ceActionControl_CommandCodeSetupMenu                   ,
    /* 25 */    gpRf4ceActionControl_CommandCodeExit                        ,
    /* 26 */    gpRf4ceActionControl_CommandCodeNumber4                     ,
    /* 27 */    gpRf4ceActionControl_CommandCodeChannelUp					,
    /* 28 */    gpRf4ceActionControl_CommandCodeLeft                        ,
    /* 29 */    gpRf4ceActionControl_CommandCodeNumber3                     ,
    /* 30 */    gpRf4ceActionControl_CommandCodePageUp			    	    ,
    /* 31 */    gpRf4ceActionControl_CommandCodeF4Yellow                    ,
    /* 32 */    gpRf4ceActionControl_CommandCodePreviousChannel             ,
    /* 33 */    gpRf4ceActionControl_CommandCodeVolumeDown                  ,
    /* 34 */    gpRf4ceActionControl_CommandCodeElectronicProgramGuide      ,
    /* 35 */    gpRf4ceActionControl_CommandCodeNumber7			    	    ,                     								
    /* 36 */    gpRf4ceActionControl_CommandCodeNone                        ,
    /* 37 */    gpRf4ceActionControl_CommandCodeChannelDown					,
    /* 38 */    gpRf4ceActionControl_CommandCodePageUp			    	    ,
    /* 39 */    gpRf4ceActionControl_CommandCodeNone                        ,
    /* 40 */    gpRf4ceActionControl_CommandCodePowerToggleFunction         ,
    /* 41 */    gpRf4ceActionControl_CommandCodeNumber3                     ,
};

const UInt8 gpProgrammableKey_LogicalKeyId[GP_PROGRAMMABLE_KEY_NUMBER_OF_KEYS] = {
    gpController_KeyBoard_CommandCodePowerToggleFunction,
    gpController_KeyBoard_CommandCodePowerToggleFunction,
    gpController_KeyBoard_CommandCodeVolumeUp,
    gpController_KeyBoard_CommandCodeVolumeDown,
	gpController_KeyBoard_CommandCodeMute,
	gpController_KeyBoard_CommandCodeInputSelect    
};

///IR Default code
static const gpKeyVendorIdCode_t ROM gpKeyVendorIdMap[] FLASH_PROGMEM =
{
	/*KEY No.*/		/*KEY Index*/											/*Data*/
	
	/*01*/	{ gpRf4ceUserControl_CommandCodeF1Blue,                     0x44 },
	/*02*/	{ gpRf4ceUserControl_CommandCodePageDown,                   0x61 },
	/*03*/	{ gpRf4ceUserControl_CommandCodeNumber6,                    0x16 },
	/*04*/	{ gpRf4ceUserControl_CommandCodeUp,                         0x46 },
	/*05*/	{ gpRf4ceUserControl_CommandCodeRight,                      0x49 },
	/*06*/	{ gpRf4ceUserControl_CommandCodeDown,                       0x48 },
	/*07*/	{ gpRf4ceUserControl_CommandCodeMute,                       0x22 },
	/*08*/	{ gpRf4ceUserControl_CommandCodeF3Green,                    0x42 },
	/*09*/	{ gpRf4ceUserControl_CommandCodeNumber9,                    0x19 },
	/*10*/	{ gpRf4ceUserControl_CommandCodeNumber8,                    0x18 },
	/*11*/	{ gpRf4ceUserControl_CommandCodeSelect,                     0x47 },
	/*12*/	{ gpRf4ceUserControl_CommandCodeDisplayInformation,         0x36 },
	/*13*/	{ gpRf4ceUserControl_CommandCodeRootMenu,                   0x51 },
	/*14*/	{ gpRf4ceUserControl_CommandCodeF2Red,                      0x41 },
	/*15*/	{ gpRf4ceUserControl_CommandCodeNone,                       0xFE },
	/*16*/	{ gpRf4ceUserControl_CommandCodeNumber5,                    0x15 },
	/*17*/	{ gpRf4ceUserControl_CommandCodeNumber2,                    0x12 },
	/*18*/	{ gpRf4ceUserControl_CommandCodeNumber0,                    0x10 },
	/*19*/	{ gpRf4ceUserControl_CommandCodeNone,                       0xFE },
	/*20*/	{ gpRf4ceUserControl_CommandCodeInputSelect,                0x00 },
	/*21*/	{ gpRf4ceUserControl_CommandCodeVolumeUp,                   0x32 },
	/*22*/	{ gpRf4ceUserControl_CommandCodeNumber1,                    0x11 },
	/*23*/	{ gpRf4ceUserControl_CommandCodeNone,                       0xFE },
	/*24*/	{ gpRf4ceUserControl_CommandCodeSetupMenu,                  0x71 },
	/*25*/	{ gpRf4ceUserControl_CommandCodeExit,                       0x52 },
	/*26*/	{ gpRf4ceUserControl_CommandCodeNumber4,                    0x14 },
	/*27*/	{ gpRf4ceUserControl_CommandCodeChannelUp,                  0x34 },
//	/*27*/	{ gpRf4ceUserControl_CommandCodeChannelDown,                0x35 },
	/*28*/	{ gpRf4ceUserControl_CommandCodeLeft,                       0x45 },
	/*29*/	{ gpRf4ceUserControl_CommandCodeNumber3,                    0x13 },
	/*30*/	{ gpRf4ceUserControl_CommandCodePageUp,                     0x64 },
	/*31*/	{ gpRf4ceUserControl_CommandCodeF4Yellow,                   0x43 },
	/*32*/	{ gpRf4ceUserControl_CommandCodePreviousChannel,            0x28 },
	/*33*/	{ gpRf4ceUserControl_CommandCodeVolumeDown,                 0x33 },
	/*34*/	{ gpRf4ceUserControl_CommandCodeElectronicProgramGuide,     0x31 },
	/*35*/	{ gpRf4ceUserControl_CommandCodeNumber7,                    0x17 },
	/*36*/	{ gpRf4ceUserControl_CommandCodeNone,                       0xFE },
	/*37*/	{ gpRf4ceUserControl_CommandCodeChannelDown,                0x35 },
	/*38*/	{ gpRf4ceUserControl_CommandCodePageUp,                     0x64 },
	/*39*/	{ gpRf4ceUserControl_CommandCodeNone,                       0xFE },
	/*40*/	{ gpRf4ceUserControl_CommandCodePowerOnFunction,            0x00 },
	/*41*/	{ gpRf4ceUserControl_CommandCodeNumber3,                    0x13 },
};

static const gpController_Led_Sequence_t Controller_LedSequenceError =
    {gpController_Led_ColorRed, 50, 100, 0, 1 };

static const gpController_Led_Sequence_t Controller_LedSequenceBinding =
    {gpController_Led_ColorGreen, 50, 50, 50, 0xFF };

static const gpController_Led_Sequence_t Controller_LedSequenceSuccess =
    {gpController_Led_ColorGreen, 50, 10, 10, 2 };

static const gpController_Led_Sequence_t Controller_LedSequenceGrnBlink =
    {gpController_Led_ColorGreen, 50, 50, 300, 1 };

static const gpController_Led_Sequence_t Controller_LedSequenceRedBlink =
    {gpController_Led_ColorRed, 50, 50, 0, 1 };

static const gpController_Led_Sequence_t Controller_LedSequenceTest1 =
    {gpController_Led_ColorGreen, 50, 200, 100, 2 };

static const gpController_Led_Sequence_t Controller_LedSequenceTest2 =
    {gpController_Led_ColorGreen, 50, 200, 100, 1 };

static const gpController_Led_Sequence_t Controller_LedSequenceTest3 =
    {gpController_Led_ColorRed, 50, 200, 100, 1 };

static const gpController_Led_Sequence_t Controller_LedSequenceTest4 =
    {gpController_Led_ColorRed, 50, 200, 100, 2 };

static const gpController_Led_Sequence_t Controller_LedSequenceError_Green =
    {gpController_Led_ColorGreen, 50, 80, 0, 1 };

gpController_Led_Sequence_t Controller_LedSequenceBlinkIR;



/*******************************************************************************
 *                    Static Function Declarations
 ******************************************************************************/
static void Controller_KeyIndicesToKeyCodes(gpController_KeyBoard_Keys_t *keyboardKeys, gpController_Keys_t *rf4ceCodes);
static void Controller_LedIndication(const gpController_Led_Sequence_t *sequence);
static void Controller_LedToggle(gpController_Led_Color_t color);
static void Controller_LedEnable(Bool enable, gpController_Led_Color_t color);
static void Controller_SetVendorIdTimeOut(void);
static Bool Controller_HandleVendorID(gpController_Keys_t *keys, gpRf4ce_VendorId_t *NewVendorID);

static void gpController_BindSuccess(UInt8 bindingId, UInt8 profileId);
static void gpController_BindAbortFull(void);
static void gpController_UnbindConfirm(UInt8 bindingId);
static void gpController_SendBatteryStatusRequest(void);
static void gpController_cbSendBatteryStatusConfirm(UInt8 bindingId, gpRf4ce_Result_t status);

#ifdef GP_DIVERSITY_APP_MSO
static void gpController_SendSWVersionInfoRequest(void);
static void gpController_cbSendSWVersionInfoConfirm(UInt8 bindingId, gpRf4ce_Result_t status, UInt8 attributeIndex);
#endif

static void gpController_SetTXOptions(UInt8 bindingId, gpRf4ce_ProfileId_t profileId);
static void gpController_BindFailure(gpRf4ce_Result_t status);


#ifdef GP_DIVERSITY_APP_ZID
static void gpController_Zid_ReportConfirm(gpRf4ce_Result_t status);
#endif /* GP_DIVERSITY_APP_ZID */
static void App_DoReset(void);

/*******************************************************************************
 *                    Helper functions
 *******************************************************************************/
 UInt8 gpController_GetCurrentBindingId(void)
 {
    return ControllerBindingId;
 }

void LED_Sequence_Control(gpController_Led_Sequence_t sequence)
{
	UInt8 i;
	if(sequence.color == gpController_Led_ColorGreen)
	{
		for(i=0;i<sequence.numOfBlinks;i++)
		{
		    gpController_Led_SetLed(true,gpController_Led_ColorGreen);
		    HAL_WAIT_MS(sequence.onTime);
		    gpController_Led_SetLed(false,gpController_Led_ColorGreen);
		    HAL_WAIT_MS(sequence.offTime);
		}
	}
	else if(sequence.color == gpController_Led_ColorRed)
	{
		for(i=0;i<sequence.numOfBlinks;i++)
		{
		    gpController_Led_SetLed(true,gpController_Led_ColorRed);
		    HAL_WAIT_MS(sequence.onTime);
		    gpController_Led_SetLed(false,gpController_Led_ColorRed);
		    HAL_WAIT_MS(sequence.offTime);
		}

	}
	
}

void LED_SetOk_Control(void)
{
	#if 0
    gpController_Led_SetLed(true,gpController_Led_ColorGreen);
    HAL_WAIT_MS(200);
    gpController_Led_SetLed(false,gpController_Led_ColorGreen);
    HAL_WAIT_MS(100);
    gpController_Led_SetLed(true,gpController_Led_ColorGreen);
    HAL_WAIT_MS(200);
    gpController_Led_SetLed(false,gpController_Led_ColorGreen);
	#else
    gpLed_GenerateBlinkSequence(gpController_Led_ColorGreen, 10,10,10,2 );
	gpController_Led_Msg(gpController_Led_MsgId_SequenceIndication,&Controller_LedSequenceBlinkIR);
	#endif
}

void LED_SetError_Control(void)
{
	#if 0
    gpController_Led_SetLed(true,gpController_Led_ColorRed);
    HAL_WAIT_MS(1600);
    gpController_Led_SetLed(false,gpController_Led_ColorRed);
	#else
    gpLed_GenerateBlinkSequence(gpController_Led_ColorRed, 10, 200, 0, 1 );
	gpController_Led_Msg(gpController_Led_MsgId_SequenceIndication,&Controller_LedSequenceBlinkIR);
	#endif
}

/*******************************************************************************
 *                    ZRC 2.0 and 1.1 Callback Functions
 ******************************************************************************/
void gpController_Zrc_cbMsg(gpController_Zrc_MsgId_t msgId,
                            gpController_Zrc_Msg_t *pMsg)
{

    switch(msgId)
    {
        case gpController_Zrc_MsgId_cbResetConfirm:
        {
#ifdef GP_DIVERSITY_APP_ZRC2_0
            /* Set supported key codes based on key map */
            gpController_Zrc_Msg_t msg;
            gpController_Zrc_KeyMap_t *keyMap;

            keyMap =&msg.keyMap;

            keyMap->keys = gpKeyCodeMap;
            keyMap->count = sizeof(gpKeyCodeMap);

            gpController_Zrc_Msg(gpController_Zrc_MsgId_ActionCodesSupportedIndication, &msg);
#endif /*GP_DIVERSITY_APP_ZRC2_0*/
            /* Start RF4CE. */
            gpController_Rf4ce_Msg(gpController_Rf4ce_MsgId_StartRequest, NULL);
            GP_LOG_SYSTEM_PRINTF("Reset done", 0);
            break;
        }

        case gpController_Zrc_MsgId_cbBindConfirm:
        {
            Controller_LedEnable(false, gpController_Led_ColorGreen);
           
            if(pMsg->BindConfirmParams.status == gpRf4ce_ResultSuccess)
            {
                gpController_BindSuccess(pMsg->BindConfirmParams.bindingId, pMsg->BindConfirmParams.profileId);
                //Controller_LedIndication(&Controller_LedSequenceSuccess);
				gpSched_ScheduleEvent( 300000L, LED_SetOk_Control );
				gpController_Mode = gpController_ModeZrc;
            }
            else
            {
                Controller_LedEnable(false, gpController_Led_ColorGreen);
                gpController_BindFailure(pMsg->BindConfirmParams.status);
				gpSched_ScheduleEvent( 100000L, LED_SetError_Control );
                //Controller_LedIndication(&Controller_LedSequenceError);
				GP_LOG_SYSTEM_PRINTF("bind confirm fail!!!,",0);
            }
            /* stop binding blink */

            break;
        }

        case gpController_Zrc_MsgId_cbUnbindConfirm:
        {
            /*confirmation for previously issued unbind request*/
            gpController_UnbindConfirm(pMsg->UnbindConfirmParams.bindingId);
            if(pMsg->UnbindConfirmParams.status == gpRf4ce_ResultSuccess)
            {
                Controller_LedIndication(&Controller_LedSequenceSuccess);
				gpController_Mode=gpController_ModeIRNec;
            }
            else
            {
                //Controller_LedIndication(&Controller_LedSequenceError);
				gpSched_ScheduleEvent( 100000L, LED_SetError_Control );
				GP_LOG_SYSTEM_PRINTF("unbind confirm fail!!!",0);
            }
            break;
        }

        case gpController_Zrc_MsgId_cbUnbindIndication:
        {
            /*Indication received that a target wanted to unbind*/
            gpController_UnbindConfirm(pMsg->bindingId);
            break;
        }

        case gpController_Zrc_MsgId_cbKeyConfirmedIndication:
        {
            if (ControllerOperationMode != gpController_OperationModeBinding)
            {
                if(pMsg->ActionControlConfirmParams.status != gpRf4ce_ResultSuccess)
                {
                    Controller_LedEnable(false,gpController_Led_ColorGreen);
                    Controller_LedIndication(&Controller_LedSequenceError);
                }

                if(ControllerKeyPressConfirmPending)
                {
                    ControllerKeyPressConfirmPending = false;
                }
                else
                {
					if(ControllerOperationMode != gpController_OperationModeSetup)
                    	Controller_LedEnable(false,gpController_Led_ColorGreen);


                }
            }
            break;
        }

        case gpController_Zrc_MsgId_cbKeyFailedIndication:
        {
            if (ControllerOperationMode != gpController_OperationModeBinding)
            {
                Controller_LedEnable(false, gpController_Led_ColorGreen);
                Controller_LedIndication(&Controller_LedSequenceError);
				GP_LOG_SYSTEM_PRINTF("KeyFailedIndication fail!!!",0);
            }
            break;
        }
#ifdef GP_DIVERSITY_APP_ZRC2_0
        case gpController_Zrc_MsgId_cbStartValidationIndication:
        {
            /* Set up temporarely binding */
            ControllerOperationMode = gpController_OperationModeBinding;

            ControllerBindingId = pMsg->ValidationParams.bindingId;
            ControllerProfileId = pMsg->ValidationParams.profileId;
            gpController_SetTXOptions(ControllerBindingId, ControllerProfileId);

            break;
        }
        case gpController_Zrc_MsgId_cbFailValidationIndication:
        {
            /*Validation phase failed*/
            break;
        }

        case gpController_Zrc_MsgId_cbSendPowerStatusConfirm:
        {
            gpController_cbSendBatteryStatusConfirm(pMsg->PowerStatusUpdateConfirm.bindingId,pMsg->PowerStatusUpdateConfirm.status);
            break;
        }

#endif /*GP_DIVERSITY_APP_ZRC2_0*/
        case gpController_Zrc_MsgId_cbEarlyRepeatIndication:
        {
            ///Controller_LedToggle(gpController_Led_ColorGreen);
            break;
        }

        default:
        {
            /* Unknown message ID */
///            GP_LOG_SYSTEM_PRINTF("Unknown message ID 0x%x", 0, msgId);
            GP_ASSERT_DEV_EXT(false);
            break;
        }
    }
}

#ifdef GP_DIVERSITY_APP_MSO
/*******************************************************************************
 *                    MSO Callback Functions
 ******************************************************************************/
void gpController_Mso_cbMsg(gpController_Mso_MsgId_t msgId,
                            gpController_Mso_Msg_t *pMsg)
{
    switch(msgId)
    {
        case gpController_Mso_MsgId_cbBindConfirm:
        {
            /* stop binding blink */
            Controller_LedEnable(false, gpController_Led_ColorGreen);
            if(pMsg->BindConfirmParams.status == gpRf4ce_ResultSuccess)
            {
                Controller_LedIndication(&Controller_LedSequenceSuccess);
                gpController_BindSuccess(pMsg->BindConfirmParams.bindingId,pMsg->BindConfirmParams.profileId);
            }
            else
            {
                //Controller_LedIndication(&Controller_LedSequenceError);
                //Controller_LedIndication(&Controller_LedSequenceError_Green);
				gpSched_ScheduleEvent( 100000L, LED_SetError_Control );
                gpController_BindFailure(pMsg->BindConfirmParams.status);
				GP_LOG_SYSTEM_PRINTF("BindConfirm fail!!!",0);
            }


            break;
        }
        case gpController_Mso_MsgId_cbStartValidationIndication:
        {
            /*Initial validation phase started*/

#if !defined(GP_DIVERSITY_APP_ZRC2_0)
            /* Work-around bug ZRC1.1_MSO pair/abort
               It is possible that there are still key's pressed. e.g. abort key.
               When a key-release comes after the binding ID is changed gpRf4ce_UserControl get
               blocked because the received key-release does not match the binding Id of the earlier
               send key press.
               To avoid we generate here a key-release event, this code should by replaced be a
               structural solution */
               
            gpController_KeyBoard_Msg_t keys;

            keys.keys.count = 0;
            keys.keys.indices[0] = 0;
        
            gpController_KeyBoard_cbMsg(gpController_KeyBoard_MsgId_KeysPressedIndication, &keys);

            /* End work-around bug ZRC1.1_MSO pair/abort */
#endif // !defined(GP_DIVERSITY_APP_ZRC2_0)
            
            ControllerOperationMode = gpController_OperationModeBinding;
            Controller_LedIndication(&Controller_LedSequenceBinding);
            ControllerBindingId = pMsg->ValidationParams.bindingId;
            ControllerProfileId = pMsg->ValidationParams.profileId;
            gpController_SetTXOptions(ControllerBindingId, ControllerProfileId);

            break;
        }
        case gpController_Mso_MsgId_cbFailValidationIndication:
        {
            /*Validation phase failed*/
            break;
        }
        case gpController_Mso_MsgId_cbUnbindConfirm:
        {
            /*confirmation for previously issued unbind request*/
            gpController_UnbindConfirm(pMsg->UnbindConfirmParams.bindingId);
///            GP_LOG_SYSTEM_PRINTF("MsgId_cbUnbindConfirm. BindId: 0x%x, ProfileId: 0x%x",0,ControllerBindingId,ControllerProfileId);

            break;
        }
        case gpController_Mso_MsgId_cbUnbindIndication:
        {
            /*Indication received that a target wanted to unbind*/
            gpController_UnbindConfirm(pMsg->bindingId);
            break;
        }
        case gpController_Mso_MsgId_cbSendBatteryStatusConfirm:
        {
            gpController_cbSendBatteryStatusConfirm(pMsg->RIBSetConfirmParams.bindingId,pMsg->RIBSetConfirmParams.status);
            gpSched_ScheduleEvent(0,gpController_SendSWVersionInfoRequest);
            break;
        }
        case gpController_Mso_MsgId_cbSendVersionInfoConfirm:
        {
            gpController_cbSendSWVersionInfoConfirm(pMsg->RIBSetConfirmParams.bindingId,pMsg->RIBSetConfirmParams.status,pMsg->RIBSetConfirmParams.attributeIndex);
            break;
        }

        default:
        {
            break;
        }
    }
}
#endif

#ifdef GP_DIVERSITY_APP_ZID
void gpController_Zid_cbMsg(gpController_Zid_MsgId_t msgId, gpController_Zid_Msg_t *pMsg)
{
    switch(msgId)
    {
        case gpController_Zid_MsgId_BindConfirm:
        {
            Controller_LedEnable(false, gpController_Led_ColorGreen);
            gpController_BindSuccess(pMsg->bindingId, pMsg->profileId);
            Controller_LedIndication(&Controller_LedSequenceSuccess);
            /* stop binding blink */
            break;
        }
        case gpController_Zid_MsgId_ReportConfirm:
        {
            gpController_Zid_ReportConfirm(pMsg->status);
            break;
        }
        default:
            break;
    }       
}
#endif /* GP_DIVERSITY_APP_ZID */

/*******************************************************************************
 *                    RF4CE Callback Functions
 ******************************************************************************/
void gpController_Rf4ce_cbMsg(  gpController_Rf4ce_MsgId_t msgId,
                                gpController_Rf4ce_Msg_t *pMsg)
{
    switch(msgId)
    {
        case gpController_Rf4ce_MsgId_cbStartConfirm:
        {
            gpRf4ce_VendorString_t Application_VendorString = {XSTRINGIFY(GP_RF4CE_NWKC_VENDOR_STRING)};
            gpController_Rf4ce_Msg_t msgRf4ce;
			msgRf4ce.userString.str[0]=NULL;
            /* Get Binding Id from NVM. */
            gpController_Rf4ce_AttrGet(gpController_Rf4ce_MsgId_AttrBindingId, &msgRf4ce);
            /* Update local binding Id */
            ControllerBindingId = msgRf4ce.bindingId;

            /* Get Profile Id from NVM. */
            gpController_Rf4ce_AttrGet(gpController_Rf4ce_MsgId_AttrProfileId, &msgRf4ce);
            /* Update local binding Id */
            ControllerProfileId = msgRf4ce.profileId;

            gpController_SetTXOptions(ControllerBindingId, ControllerProfileId);

            /*Get vendor ID from NVM. */
            gpController_Rf4ce_AttrGet(gpController_Rf4ce_MsgId_AttrVendorId,&msgRf4ce);
            ControllerVendorId = msgRf4ce.vendorId;

            /*Set vendor string. Maximum length for vendorstring is 7 bytes. GP_RF4CE_NWKC_VENDOR_STRING comes from the preprocessor defines.*/
            msgRf4ce.vendorString = Application_VendorString;
            gpController_Rf4ce_AttrSet(gpController_Rf4ce_MsgId_AttrVendorString,&msgRf4ce);

            /*Set user string. Note that user string is limited in length as it is being used to share parameters during pairing.
              The maximum length for the application is 8 bytes, although the length of the userstring at RF4CE level is 15 bytes (but the most significant bytes will get overwritten during the pairing.*/
            MEMCPY_P(msgRf4ce.userString.str,"CRB36",5);
            gpController_Rf4ce_AttrSet(gpController_Rf4ce_MsgId_AttrUserString,&msgRf4ce);

#ifdef GP_DIVERSITY_APP_ZID
            /* Set the ZID profile and HID attributes */
            gpController_Zid_SetProfileAttr();
#endif /* GP_DIVERSITY_APP_ZID*/

            /* RF4CE started, enable key scanner now. */
            gpController_KeyBoard_Msg(gpController_KeyBoard_MsgId_KeyScanEnable, NULL);

///            GP_LOG_SYSTEM_PRINTF("RF4CE booted. Current BindId: 0x%x, ProfileId: 0x%x",0,ControllerBindingId,ControllerProfileId);
///            GP_LOG_SYSTEM_PRINTF("VendorId: 0x%x",0,(UInt16)ControllerVendorId);
#ifdef GP_DIVERSITY_LOG
            gpLog_Flush();
#endif
            break;
        }

        case gpController_Rf4ce_MsgId_cbSendVendorDataConfirm:
        {
            gpController_cbSendBatteryStatusConfirm(pMsg->vendorDataConfirm.bindingId,pMsg->vendorDataConfirm.status);
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

static void gpController_BindAbortFull(void)
{
#ifdef GP_DIVERSITY_APP_MSO
    if(ControllerProfileId == gpRf4ce_ProfileIdMso)
    {
        gpController_Mso_Msg(gpController_Mso_MsgId_BindAbortFull, NULL);
    }
    else /* ( ControllerProfileId == gpRf4ce_ProfileIdZrc || ControllerProfileId == gpRf4ce_ProfileIdZrc2_0) */
#endif /*GP_DIVERSITY_APP_MSO*/
    {
        gpController_Zrc_Msg(gpController_Zrc_MsgId_BindAbortFull, NULL);
    }

    /* blink error to indicate cancel */
//    Controller_LedIndication(&Controller_LedSequenceError);
	gpSched_ScheduleEvent( 100000L, LED_SetError_Control );
	GP_LOG_SYSTEM_PRINTF("BindAbortFull!!!",0);
}

/*******************************************************************************
 *                    Keyboard Callback Functions
 ******************************************************************************/
#ifdef GP_DIVERSITY_APP_ZID
Bool Controller_HandleZIDKeys(gpController_Zrc_Msg_t *pZrc)
{
    /* Emulated keyboard and mouse reporting using designated keys (Numeric = keyboard, Cursor = Mouse) */
    static Bool sendMouseRep = false, sendKeyboardRep = false;
    Int16 dx = 0, dy = 0;
    UInt8 i;
    UInt8 ZidKeyboardReport[8];
    
    if(pZrc->KeyPressedIndication.keys.count == 0)
    {
        if(sendKeyboardRep)    
        {
            sendKeyboardRep = false;
            /* Send NULL report */
            gpController_Zid_PrepKeyboardReport(true, 0, ZidKeyboardReport);
            gpController_Zid_Msg(gpController_Zid_MsgId_ReportRequest, NULL);
            return true;
        }
        if(sendMouseRep)
        {
            sendMouseRep = false;
            /* Send NULL report */
            gpController_Zid_PrepMouseReport(true, (Int8)dx, (Int8)dy);
            gpController_Zid_Msg(gpController_Zid_MsgId_ReportRequest, NULL);
            return true;
        }    
    }
    
    /* Unsolicited keyboard report */
    for(i=0; i<pZrc->KeyPressedIndication.keys.count; i++)
    {
        if (pZrc->KeyPressedIndication.keys.codes[i] <= 0x29 && pZrc->KeyPressedIndication.keys.codes[i] >= 0x20)
        {
            sendKeyboardRep = true;
            /* Convert CERC numeric code to HID keyboard numeric code */
            ZidKeyboardReport[i] = 0x1E + (pZrc->KeyPressedIndication.keys.codes[i] - 0x20);
        }
    }
    if(sendKeyboardRep)
    {
        gpController_Zid_PrepKeyboardReport(false, pZrc->KeyPressedIndication.keys.count, ZidKeyboardReport);
        gpController_Zid_Msg(gpController_Zid_MsgId_ReportRequest, NULL);
        return true;
    }   
    
    /* Unsolicited mouse report */
    if(pZrc->KeyPressedIndication.keys.count == 1)
    {
        if(pZrc->KeyPressedIndication.keys.codes[0] == gpRf4ceActionControl_CommandCodeUp)
        {
            dy = -5; dx = 0;
            sendMouseRep = true;
        }
        if(pZrc->KeyPressedIndication.keys.codes[0] == gpRf4ceActionControl_CommandCodeDown)
        {
            dy = 5; dx = 0;
            sendMouseRep = true;
        }        
        if(pZrc->KeyPressedIndication.keys.codes[0] == gpRf4ceActionControl_CommandCodeLeft)
        {
            dx = -5; dy = 0;
            sendMouseRep = true;
        }        
        if(pZrc->KeyPressedIndication.keys.codes[0] == gpRf4ceActionControl_CommandCodeRight)
        {
            dx = 5; dy = 0;
            sendMouseRep = true;
        }
    }
    if(sendMouseRep)
    {    
        /* Trim to boundaries */
        if( dx < -127 ) dx = -127;
        if( dy < -127 ) dy = -127;
        if( dx > 127 )  dx = 127;
        if( dy > 127 )  dy = 127;

        gpController_Zid_PrepMouseReport(false, (Int8)dx, (Int8)dy);
        gpController_Zid_Msg(gpController_Zid_MsgId_ReportRequest, NULL);
        return true;
    }
    
    return false;
    
}
#endif /* GP_DIVERSITY_APP_ZID */

#if defined (GP_RF4CEVOICE_DIVERSITY_ORIGINATOR)
Bool Controller_HandleAudioKeys(gpController_Zrc_Msg_t *pZrc)
{
    if((pZrc->KeyPressedIndication.keys.count == 1) && (pZrc->KeyPressedIndication.keys.codes[0] == gpRf4ceActionControl_CommandCodeElectronicProgramGuide))
    {
        if ((!audioActive) && (ControllerBindingId != 0xff) /* and paired */)
        {
            gpController_Voice_Msg_t msgVoice;
            msgVoice.bindingId = ControllerBindingId;
            gpController_Voice_Msg(gpController_Voice_MsgId_Start, &msgVoice);
            audioActive = true;
            Controller_LedEnable(true, gpController_Led_ColorRed);
        }
        return true;
    }
    if(pZrc->KeyPressedIndication.keys.count == 0)
    {
        if(audioActive)
        {
///            GP_LOG_SYSTEM_PRINTF("App: stopped voice transmission", 0);
            gpController_Voice_Msg(gpController_Voice_MsgId_Stop, NULL);
            audioActive = false;
            Controller_LedEnable(false, gpController_Led_ColorRed);
            return true;
        }
    }
    return false;
}
#endif /*GP_RF4CEVOICE_DIVERSITY_ORIGINATOR*/

	static UInt16 controller_GetRegularKeyCode( UInt8 modeIndex, UInt8 *keyIndex )
{
    UIntLoop i=0;
	UInt16 tmp=0;
	

	
    for(i=0;i<(sizeof(gpKeyVendorIdMap)/sizeof(gpKeyVendorIdCode_t));i++)
    {
        if(i == *keyIndex)
        {
            tmp = (UInt16)gpKeyVendorIdMap[i].gpKeyVendorIDCode;
//			tmp<<=8;
//            tmp |= (UInt16)gpKeyVendorIdMap[i].gpKeyVendorIDCode2;
           break;
        }
    }

    return tmp;
}

Bool Controller_HandleIRKeys(gpController_Zrc_Msg_t *pZrc,gpController_KeyBoard_Msg_t *pMsg)
{

    gpController_Zrc_Msg_t msgZrc;
	UInt8 Tmp_keycode;
    UInt8* IRCode = NULL;
    gpController_IR_Msg_t msg;
    gpProgrammableKey_TvIrDesc_t* pDesc;
    if((pZrc->KeyPressedIndication.keys.count == 0))
    {
			ControllerOperationMode = gpController_OperationModeNormal;
        if(ControllerOperationMode == gpController_OperationModeIR)
        {
            ControllerOperationMode = gpController_OperationModeNormal;
            return true;
        }
		return false;
    }
    if(pZrc->KeyPressedIndication.keys.count > 1)
    {
    	//pZrc->KeyPressedIndication.keys.count = 0;
        if(ControllerOperationMode == gpController_OperationModeIR)
        {
            ControllerOperationMode = gpController_OperationModeNormal;
            return true;
        }
        else
        {
            ControllerOperationMode = gpController_OperationModeNormal;
            return false;
        }
    }

//power,input은 모두 TV로 출력.
	if((pZrc->KeyPressedIndication.keys.codes[0]==gpRf4ceActionControl_CommandCodePowerToggleFunction)
	 ||(pZrc->KeyPressedIndication.keys.codes[0]==gpRf4ceActionControl_CommandCodeInputSelect))
	{
        Controller_KeyIndicesToKeyCodes(&(pMsg->keys), &(msg.keys));
        switch(pZrc->KeyPressedIndication.keys.codes[0])
        {
            case gpRf4ceActionControl_CommandCodePowerToggleFunction:
            {
				if(PowerToggle&0x01)
				{
                    pDesc = &(gpProgrammableKey_Db[0].tvIrDesc);
					PowerToggle++;
				}
				else
				{
                    pDesc = &(gpProgrammableKey_Db[1].tvIrDesc);
					PowerToggle++;
				}
                break;
            }
			
            case gpRf4ceActionControl_CommandCodeInputSelect:
            {
                pDesc = &(gpProgrammableKey_Db[5].tvIrDesc);
                break;
            }
            default:
            {
                return;
                break;
            }
        } // end switch
			///pDesc = &(gpProgrammableKey_Db[pMsg->keys.indices[0]].tvIrDesc.code);
	    ControllerOperationMode = gpController_OperationModeIR;
		if((pDesc->code[0]!=0xFF) && (pDesc->code[0]!=0x00) )
		{
			Controller_LedEnable(true,gpController_Led_ColorRed);
    		gpIrTx_SendCommandRequestGeneric((gpIrTx_TvIrDesc_t*)pDesc->code, false, 0/*irTxConfig*/);
		}
       return true;
	}
	
//vol+/-, mute는 tv설정이 없으면 STB,설정이되면 TV
	else if((pZrc->KeyPressedIndication.keys.codes[0]==gpRf4ceActionControl_CommandCodeVolumeUp)
	 	   ||(pZrc->KeyPressedIndication.keys.codes[0]==gpRf4ceActionControl_CommandCodeVolumeDown)
	 	   ||(pZrc->KeyPressedIndication.keys.codes[0]==gpRf4ceActionControl_CommandCodeMute))
	{
        Controller_KeyIndicesToKeyCodes(&(pMsg->keys), &(msg.keys));
        switch(pZrc->KeyPressedIndication.keys.codes[0])
        {
            case gpRf4ceActionControl_CommandCodeVolumeUp:
            {
                pDesc = &(gpProgrammableKey_Db[2].tvIrDesc);
                break;
            }
            case gpRf4ceActionControl_CommandCodeVolumeDown:
            {
                pDesc = &(gpProgrammableKey_Db[3].tvIrDesc);
                break;
            }
            case gpRf4ceActionControl_CommandCodeMute:
            {
                pDesc = &(gpProgrammableKey_Db[4].tvIrDesc);
                break;
            }
            default:
            {
                return;
                break;
            }
		}
		if(ControllerBindingId == 0xff)
		{

			if(0xFFFF!= gpIRDatabase_GetIRTableId())//default상태에서는 STB로 출력.
			{
				Controller_LedEnable(true,gpController_Led_ColorRed);
				if(!gpController_sendVolumeControlToDta)	//TV Vol
				{
				    ControllerOperationMode = gpController_OperationModeIR;
		        	gpIrTx_SendCommandRequestGeneric((gpIrTx_TvIrDesc_t*)pDesc->code, false, 0/*irTxConfig*/);
	           		return true;
				}
				else	//DTA
				{
			        gpIrTx_Config_t config;
			        gpIrTx_Config_Nec_t necConfig = { 0xAF,0, 0x60};

					ControllerOperationMode = gpController_OperationModeIR;
			        config.spec.nec = necConfig;
			        config.function         = (gpIrTx_CommandCode_t)controller_GetRegularKeyCode( 1, &(pMsg->keys.indices[0]));
			        config.repeated         = true;
					config.numberMinRepeats = 0;
			        gpIrTx_SendCommandRequestPredefined( gpIrTx_IrComponent_Nec, config );
			        return true;
				}
			}
		}
		else if(!gpController_sendVolumeControlToDta)//paired
		{
		    ControllerOperationMode = gpController_OperationModeIR;
			Controller_LedEnable(true,gpController_Led_ColorRed);
	       	gpIrTx_SendCommandRequestGeneric((gpIrTx_TvIrDesc_t*)pDesc->code, false, 0/*irTxConfig*/);
       		return true;
		}
		else
        	return false;
	}

///other keys out	
	if((ControllerBindingId == 0xff) && (multipleKeysPressed == 0))
	{
        gpIrTx_Config_t config;
        gpIrTx_Config_Nec_t necConfig = { 0xAF,0, 0x60};

		Controller_LedEnable(true,gpController_Led_ColorRed);
		ControllerOperationMode = gpController_OperationModeIR;
        config.spec.nec = necConfig;
        config.function         = (gpIrTx_CommandCode_t)controller_GetRegularKeyCode( 1, &(pMsg->keys.indices[0]));
        config.repeated         = true;
		config.numberMinRepeats = 0;
        gpIrTx_SendCommandRequestPredefined( gpIrTx_IrComponent_Nec, config );
        return true;
	}

	else
        return false;
}



void gpController_KeyBoard_cbMsg(   gpController_KeyBoard_MsgId_t msgId,
                                    gpController_KeyBoard_Msg_t *pMsg)
{
    switch(msgId)
    {

        case gpController_KeyBoard_MsgId_KeysPressedIndication:
        {
            if(ControllerOperationMode == gpController_OperationModeSetup )
            {
                gpController_Setup_Msg_t msg;
                Controller_KeyIndicesToKeyCodes(&(pMsg->keys), &(msg.keys)) ;

                /* Forward the keys message to the Setup module. */
                gpController_Setup_Msg(gpController_Setup_MsgId_KeyPressedIndication, &msg);
            }
            else if ((ControllerOperationMode == gpController_OperationModeNormal) || (ControllerOperationMode == gpController_OperationModeIR))
					  (ControllerOperationMode == gpController_OperationModeIR)		||
					  (ControllerOperationMode == gpController_OperationSetsequence))
            {
                gpController_Zrc_Msg_t msgZrc;
				gpController_KeyBoard_Msg_t tmpKeycnt=*pMsg;
				UInt8 	i;
				UInt8 	TxBuffer[1];
                Controller_KeyIndicesToKeyCodes(&(pMsg->keys), &(msgZrc.KeyPressedIndication.keys));

#ifdef GP_DIVERSITY_APP_ZID
                if(Controller_HandleZIDKeys(&msgZrc))
                {
                    return;
                }
#endif /*GP_DIVERSITY_APP_ZID*/
      
#ifdef GP_RF4CEVOICE_DIVERSITY_ORIGINATOR
                if(Controller_HandleAudioKeys(&msgZrc))
                {
                    return;
                }
#endif /*GP_RF4CEVOICE_DIVERSITY_ORIGINATOR*/
                if(Controller_HandleIRKeys(&msgZrc,&tmpKeycnt))
                {
                    return;
                }

                if( (ControllerProfileId == gpRf4ce_ProfileIdZrc) ||
                    (ControllerProfileId == gpRf4ce_ProfileIdMso))
                {
                    if((msgZrc.KeyPressedIndication.keys.count > 1) || (msgZrc.KeyPressedIndication.keys.count == 0))
                    {
                        /* Generate release */
                        msgZrc.KeyPressedIndication.keys.count = 0;
                    }
                }
                msgZrc.KeyPressedIndication.bindingId = ControllerBindingId;
                msgZrc.KeyPressedIndication.profileId = ControllerProfileId;
                msgZrc.KeyPressedIndication.vendorId  = ControllerVendorId;
                msgZrc.KeyPressedIndication.txOptions = ControllerTxOptions;

                if(msgZrc.KeyPressedIndication.keys.count == 1)
                {
                    ///GP_LOG_SYSTEM_PRINTF("Key pressed",0); /* grn LED on */
                    Controller_LedEnable(true, gpController_Led_ColorGreen);
                    ControllerKeyPressConfirmPending = true;
                }
				#if 1
                else
                {
                    ///GP_LOG_SYSTEM_PRINTF("Key released,",0); /*  grn LED off */
                    Controller_LedEnable(false, gpController_Led_ColorGreen);
                    Controller_LedEnable(false, gpController_Led_ColorRed);
					ControllerOperationMode = gpController_OperationModeNormal;
				}
				#endif
               	gpController_Zrc_Msg(gpController_Zrc_MsgId_KeyPressedIndication, &msgZrc);
            }
            else if (ControllerOperationMode == gpController_OperationModeSetVendorID)
            {
                Bool validVendorID = false;
                gpController_Keys_t keys;
                gpController_Rf4ce_Msg_t msgRF4CE;

                Controller_KeyIndicesToKeyCodes(&(pMsg->keys), &keys);
                validVendorID = Controller_HandleVendorID(&keys, &msgRF4CE.vendorId);
                if(validVendorID)
                {
                    GP_LOG_SYSTEM_PRINTF("Valid vendor ID entered: 0x%x",0,(UInt16)msgRF4CE.vendorId);
                    ControllerVendorId = msgRF4CE.vendorId;
                    gpController_Rf4ce_AttrSet(gpController_Rf4ce_MsgId_AttrVendorId,&msgRF4CE);
                    Controller_LedEnable(false,gpController_Led_ColorGreen);
                    gpSched_UnscheduleEvent(Controller_SetVendorIdTimeOut);
                    ControllerOperationMode = gpController_OperationModeNormal;
                }
            }            
            else /* (ControllerOperationMode == gpController_OperationModeBinding;) */
            {
                gpController_Zrc_Msg_t msgZrc;
                Controller_KeyIndicesToKeyCodes(&(pMsg->keys), &(msgZrc.KeyPressedIndication.keys));

                /* only handle single keys */
                if((msgZrc.KeyPressedIndication.keys.count == 1))
                {
                    if(msgZrc.KeyPressedIndication.keys.codes[0] == gpRf4ceActionControl_CommandCodePowerToggleFunction)
                    {
                        gpSched_ScheduleEvent(0, gpController_BindAbortFull);
                    }
                    msgZrc.KeyPressedIndication.bindingId = ControllerBindingId;
                    msgZrc.KeyPressedIndication.profileId = ControllerProfileId;
                    msgZrc.KeyPressedIndication.vendorId  = ControllerVendorId;
                    msgZrc.KeyPressedIndication.txOptions = ControllerTxOptions;

                    gpController_Zrc_Msg( gpController_Zrc_MsgId_KeyPressedIndication, &msgZrc);
                }
                else
                {
                    /* Number of keys pressed is 0 or > 1. In either case, generate release. */
                    msgZrc.KeyPressedIndication.keys.count = 0;
                    msgZrc.KeyPressedIndication.bindingId = ControllerBindingId;
                    msgZrc.KeyPressedIndication.profileId = ControllerProfileId;
                    msgZrc.KeyPressedIndication.vendorId  = ControllerVendorId;
                    msgZrc.KeyPressedIndication.txOptions = ControllerTxOptions;

                    gpController_Zrc_Msg(gpController_Zrc_MsgId_KeyPressedIndication, &msgZrc);
                }
            }
            break;
        }

        case gpController_KeyBoard_MsgId_KeysReleasedIndication:
        {
            gpController_Zrc_Msg_t msgZrc;

            Controller_LedEnable(false,gpController_Led_ColorGreen);
            msgZrc.KeyPressedIndication.keys.count = 0;
            msgZrc.KeyPressedIndication.bindingId = ControllerBindingId;
            msgZrc.KeyPressedIndication.profileId = ControllerProfileId;
            msgZrc.KeyPressedIndication.vendorId  = ControllerVendorId;
            msgZrc.KeyPressedIndication.txOptions = ControllerTxOptions;
            gpController_Zrc_Msg(gpController_Zrc_MsgId_KeyPressedIndication, &msgZrc);
            break;
        }

        case gpController_KeyBoard_MsgId_SetupEnteredIndication:
        {
            /* only go to setup mode when in normal mode */
            if(ControllerOperationMode == gpController_OperationModeNormal)
            {
                Controller_LedEnable(true, gpController_Led_ColorGreen);
                Controller_LedEnable(false, gpController_Led_ColorRed);
            if(ControllerOperationMode == gpController_OperationModeNormal ||
				ControllerOperationMode == gpController_OperationModeIR)
            {

                ControllerOperationMode = gpController_OperationModeSetup;
                gpController_Setup_Msg(gpController_Setup_MsgId_SetupEnterIndication , NULL);
            }
            break;
        }

        case gpController_KeyBoard_MsgId_SetupKeysPressedIndication:
        {
            /* only show setup key when in normal mode */
            if(ControllerOperationMode == gpController_OperationModeNormal)
            {
                Controller_LedEnable(true, gpController_Led_ColorRed);
            }
            break;
        }

        case gpController_KeyBoard_MsgId_SetupKeysReleasedIndication:
        {
            /* only handle setup key in normal mode */
            if(ControllerOperationMode == gpController_OperationModeNormal)
            {
              Controller_LedEnable(false, gpController_Led_ColorRed);
            }
            break;
        }

        default: /* Invalid message ID. */
        {
            /* Unknown message ID */
            GP_ASSERT_DEV_EXT(false);
            break;
        }
    }
}

/*******************************************************************************
 *                    SETUP Callback Functions
 ******************************************************************************/
void gpController_Setup_cbMsg(  gpController_Setup_MsgId_t msgId,
                                gpController_Setup_Msg_t *pMsg)
{
    switch(msgId)
    {

        case gpController_Setup_MsgId_cbSetupLeftIndication:
        {
            Controller_LedEnable(false, gpController_Led_ColorGreen);
            ControllerOperationMode = gpController_OperationModeNormal;
            break;
        }

        case gpController_Setup_MsgId_cbSetupTimeoutIndication:
        case gpController_Setup_MsgId_cbInvalidCommandIndication: /* fall through */
        {
            gpController_Led_Msg_t msg;
            gpController_Led_Sequence_t *sequence = &msg.sequence;
            *sequence = Controller_LedSequenceError;
            gpController_Led_Msg( gpController_Led_MsgId_SequenceIndication, &msg);
            break;
        }
#if defined(GP_DIVERSITY_APP_ZRC1_1) || defined(GP_DIVERSITY_APP_ZRC2_0)
        case gpController_Setup_MsgId_cbZrc_BindStartIndication:
        {
            ControllerOperationMode = gpController_OperationModeBinding;
            GP_LOG_SYSTEM_PRINTF("bindStart %x",0,msgId);
			
			Controller_LedEnable(false,gpController_Led_ColorRed);
			Controller_LedEnable(false,gpController_Led_ColorGreen);
            Controller_LedIndication(&Controller_LedSequenceBinding);
#ifdef GP_DIVERSITY_APP_ZID     
            gpController_Zid_Msg(gpController_Zid_MsgId_BindRequest, NULL);
#else  
            gpController_Zrc_Msg(gpController_Zrc_MsgId_BindRequest, NULL);
#endif        
            break;
        }
#endif /*defined(GP_DIVERSITY_APP_ZRC1_1) || defined(GP_DIVERSITY_APP_ZRC2_0)*/
#ifdef GP_DIVERSITY_APP_MSO
        case gpController_Setup_MsgId_cbMsoBindStartIndication:
        {
            ControllerOperationMode = gpController_OperationModeBinding;
            gpController_Mso_Msg(gpController_Mso_MsgId_BindRequest, NULL);
            break;
        }
#endif /*GP_DIVERSITY_APP_MSO*/
        case gpController_Setup_MsgId_cbUnbindStartIndication:
        {

#ifdef GP_DIVERSITY_APP_MSO
            if(ControllerProfileId == gpRf4ce_ProfileIdMso)
            {
                gpController_Mso_Msg_t msg;
                msg.bindingId = ControllerBindingId;
            	Controller_LedIndication(&Controller_LedSequenceSuccess);
                gpController_Mso_Msg(gpController_Mso_MsgId_UnbindRequest, &msg);
            }
            else
#endif /*GP_DIVERSITY_APP_MSO*/
            {
#if defined(GP_DIVERSITY_APP_ZRC1_1) || defined(GP_DIVERSITY_APP_ZRC2_0)
                gpController_Zrc_Msg_t msg;
                msg.bindingId = ControllerBindingId;
                gpController_Zrc_Msg(gpController_Zrc_MsgId_UnbindRequest, &msg);
#endif /*defined(GP_DIVERSITY_APP_ZRC1_1) || defined(GP_DIVERSITY_APP_ZRC2_0)*/
            }
            break;
        }

        case gpController_Setup_MsgId_cbSetVendorID:
        {
            GP_LOG_SYSTEM_PRINTF("Set Vendor ID mode Entered",0);
            Controller_LedEnable(true, gpController_Led_ColorGreen);
            ControllerOperationMode = gpController_OperationModeSetVendorID;
            /* Calling Controller_HandleVendorID with NULL, NULL result in init of handle vendor sequence */
            Controller_HandleVendorID(NULL, NULL);
            gpSched_ScheduleEvent(10000000UL, Controller_SetVendorIdTimeOut);
            break;
        }

        case gpController_Setup_MsgId_cbFactoryResetIndication:
        {
            gpController_Rf4ce_Msg(gpController_Rf4ce_MsgId_FactoryResetIndication, NULL);
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

#ifdef GP_RF4CEVOICE_DIVERSITY_ORIGINATOR
void gpController_Voice_cbMsg(  gpController_Voice_MsgId_t msgId,
                                gpController_Voice_Msg_t *pMsg)
{
    switch(msgId)
    {
        case gpController_Voice_MsgId_cbOutOfResources:
        {
            /* allow new key press */
            GP_LOG_SYSTEM_PRINTF("Out of resources for audio", 0);
            audioActive = false;
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
#endif
static Bool Controller_SelectDeviceInEmbeddedDatabase( UInt16 deviceId )
{
    Bool retVal = false;
    
    gpIRDatabase_SetIRTableId( deviceId );

    if( deviceId == gpIRDatabase_GetIRTableId() )
    {
        // make sure we keep in synch
        gpDeviceID = deviceId;
        retVal = true;
    }
    return retVal;
}

static Bool Controller_SetDeviceId(UInt16 deviceID)
{
    //UInt8 nProgrammableKeys;
///    GP_LOG_PRINTF( "=> set device %i", 0, deviceID );
    if( Controller_SelectDeviceInEmbeddedDatabase(deviceID) )
    {
         UInt8 i, keyId;
         if( 0xFFFF != gpIRDatabase_GetIRTableId() )
         {
            /* Device is set in the database */
            for( i=0; i<GP_PROGRAMMABLE_KEY_NUMBER_OF_KEYS; i++ )
            {
                keyId = gpProgrammableKey_LogicalKeyId[i];
                // Get the IR code of the key from the IR database. For a valid key the IR code length is greater than zero.
                if( 0 < gpIRDatabase_GetIRCode(keyId, gpProgrammableKey_Db[i].tvIrDesc.code) )
                {
                    gpProgrammableKey_TvIrDesc_t* pDesc;
                    pDesc = &gpProgrammableKey_Db[i].tvIrDesc;
                    gpIrTx_CorrectTimingGeneric(GP_PROGKEY_IRCODE_TXDEFINITION( pDesc->code )  );
                }
            }
        }
        return true;
    }
    else
    {
        return false;
    }

}

static void App_DoReset(void)
{
    UInt16 restoredDeviceId;
    gpIRDatabase_Reset();
    restoredDeviceId = gpIRDatabase_GetIRTableId();
   
    if(restoredDeviceId == 0xffff)
    {
        // set default id
        Controller_SetDeviceId(GP_DEFAULT_DEVICE_ID);
    }
    else
    {
		Controller_SetDeviceId(restoredDeviceId);
    }
    gpNvm_Restore(GP_COMPONENT_ID , NVM_TAG_SEND_VC_TO_DTA , (UInt8*)&gpController_sendVolumeControlToDta);
    gpController_PendingKey.pendingKeyInfo.keyInfo = 0xff;
    gpSched_ScheduleEvent( 0, gpController_CheckBatteryLevel);
}

/*******************************************************************************
 *                    Application Main
 ******************************************************************************/
void Application_Init( void )
{
    /* Initialize all modules. */
    gpBaseComps_StackInit();
    gpController_Rf4ce_Init();
    gpController_Zrc_Init();

#ifdef GP_DIVERSITY_APP_ZID
    gpController_Zid_Init();
#endif
    gpIRDatabase_Init();

    gpController_KeyBoard_Init();
	gpSched_ScheduleEvent(0, App_DoReset);
    gpController_Led_Init();
    gpController_Setup_Init();
#ifdef GP_RF4CEVOICE_DIVERSITY_ORIGINATOR
    gpController_Voice_Init();
#endif
    /* Trigger Reset of ZRC. */
    gpController_Zrc_Msg(gpController_Zrc_MsgId_ResetRequest, NULL);

    ControllerKeyPressConfirmPending = false;
    ControllerOperationMode = gpController_OperationModeNormal;
}

/*******************************************************************************
 *                    Static Functions
 ******************************************************************************/
static void Controller_KeyIndicesToKeyCodes(gpController_KeyBoard_Keys_t *keyboardKeys,
                                            gpController_Keys_t *rf4ceCodes)
{
    UInt8 i = 0;

    for (i=0; i<keyboardKeys->count; i++)
    {
        rf4ceCodes->codes[i] = gpKeyCodeMap[keyboardKeys->indices[i]];
    }
    rf4ceCodes->count = keyboardKeys->count;
}

static void Controller_LedIndication(const gpController_Led_Sequence_t *sequence)
{
    gpController_Led_Msg_t msg;
    msg.sequence = *sequence;
    gpController_Led_Msg( gpController_Led_MsgId_SequenceIndication, &msg);
}

static void Controller_LedToggle(gpController_Led_Color_t color)
{
    gpController_Led_Msg_t msg;
    msg.sequence.color = color;
    msg.sequence.startTime = 0; /* direct switch */

    gpController_Led_Msg( gpController_Led_MsgId_ToggleIndication, &msg);
}

static void Controller_LedEnable(Bool enable, gpController_Led_Color_t color)
{
    gpController_Led_Msg_t msg;
    msg.sequence.color = color;
    msg.sequence.startTime = 0; /* direct switch */

    if (enable)
    {
        gpController_Led_Msg( gpController_Led_MsgId_OnIndication, &msg);
    }
    else
    {
        gpController_Led_Msg( gpController_Led_MsgId_OffIndication, &msg);
    }
}

/*Function to be called after a successful ZRC or MSO bind*/
static void gpController_BindSuccess(UInt8 bindingId, UInt8 profileId)
{
    gpController_Rf4ce_Msg_t msg;
    /* Set the application binding ID. */
    ControllerBindingId = bindingId;
    ControllerProfileId = profileId;

    /* Set the RF4CE binding ID. */
    msg.bindingId = ControllerBindingId;
    gpController_Rf4ce_AttrSet(gpController_Rf4ce_MsgId_AttrBindingId, &msg);

    /* Set the RF4CE profile ID. */
    msg.profileId = ControllerProfileId;
    gpController_Rf4ce_AttrSet(gpController_Rf4ce_MsgId_AttrProfileId, &msg);

    /*Store in NVM*/
    gpController_Rf4ce_Msg(gpController_Rf4ce_MsgId_BackupNvm, NULL);

    gpController_SetTXOptions(ControllerBindingId, ControllerProfileId);

    ControllerOperationMode = gpController_OperationModeNormal;

    gpSched_ScheduleEvent(250000, gpController_SendBatteryStatusRequest);
    GP_LOG_SYSTEM_PRINTF("Bind success. BindId: 0x%x, ProfileId: 0x%x",0,bindingId,profileId);
}

#ifdef GP_DIVERSITY_APP_ZID
static void gpController_Zid_ReportConfirm(gpRf4ce_Result_t status)
{
    /* Report confirm handle here*/
}
#endif /* GP_DIVERSITY_APP_ZID */

static void gpController_BindFailure(gpRf4ce_Result_t status)
{
    gpController_Rf4ce_Msg_t msgRf4ce;

    /* Get previously set bindingId. */
    gpController_Rf4ce_AttrGet(gpController_Rf4ce_MsgId_AttrBindingId, &msgRf4ce);
    /* Update local binding Id */
    ControllerBindingId = msgRf4ce.bindingId;

    /* Get previously set bindingId. */
    gpController_Rf4ce_AttrGet(gpController_Rf4ce_MsgId_AttrProfileId, &msgRf4ce);
    /* Update local profile Id*/
    ControllerProfileId = msgRf4ce.profileId;

    gpController_SetTXOptions(ControllerBindingId, ControllerProfileId);

    ControllerOperationMode = gpController_OperationModeNormal;

    GP_LOG_SYSTEM_PRINTF("Bind failure. Status 0x%x",0,status);
}


static void gpController_UnbindConfirm(UInt8 bindingId)
{
    if(bindingId == ControllerBindingId)
    {
       gpController_Rf4ce_Msg_t msg;
       ControllerBindingId = 0xff;
       ControllerProfileId = 0xff;

       /* Push info to other parts of the application */
       msg.bindingId = ControllerBindingId;
       gpController_Rf4ce_AttrSet(gpController_Rf4ce_MsgId_AttrBindingId, &msg);
       msg.profileId = ControllerProfileId;
       gpController_Rf4ce_AttrSet(gpController_Rf4ce_MsgId_AttrProfileId, &msg);
       gpController_Rf4ce_Msg(gpController_Rf4ce_MsgId_BackupNvm, NULL);
    }
}

static void gpController_SendBatteryStatusRequest(void)
{
    gpController_BatteryMonitor_Msg_t BatteryMonitorMsg;

    BatteryMonitorMsg.measurementType = gpController_BatteryMonitor_LoadedAndUnloaded;
    /*Trigger a battery voltage measurement. Results will be reported through BattyrMonitorMsg parameter*/
    gpController_BatteryMonitor_Msg(gpController_BatteryMonitor_MsgId_GetBatteryLevel, &BatteryMonitorMsg);

    GP_LOG_SYSTEM_PRINTF("SendBatteryStatusRequest. loaded/unloaded: 0x%x/0x%x",0,BatteryMonitorMsg.measurementResult.BatteryLevelLoaded,BatteryMonitorMsg.measurementResult.BatteryLevelUnloaded);
    switch(ControllerProfileId)
    {
#ifdef GP_DIVERSITY_APP_MSO
        case gpRf4ce_ProfileIdMso:
        {
            /*MSO: battery info is pushed via a RIB attribute set request*/
            gpController_Mso_Msg_t msoMsg;
            msoMsg.BatterStatusUpdate.bindingId = ControllerBindingId;
            msoMsg.BatterStatusUpdate.vendorId = ControllerVendorId;
            msoMsg.BatterStatusUpdate.flags = BatteryMonitorMsg.measurementResult.Flags;
            msoMsg.BatterStatusUpdate.levelLoaded = BatteryMonitorMsg.measurementResult.BatteryLevelLoaded;
            msoMsg.BatterStatusUpdate.levelUnloaded = BatteryMonitorMsg.measurementResult.BatteryLevelUnloaded;
            msoMsg.BatterStatusUpdate.NumberOfRFSent = 0;
            msoMsg.BatterStatusUpdate.NumberOfIRSent = 0;
            gpController_Mso_Msg(gpController_Mso_MsgId_SendBatteryStatusRequest, &msoMsg);
            break;
        }
#endif
#if defined(GP_DIVERSITY_APP_ZRC1_1) || defined(GP_DIVERSITY_APP_ZRC2_0)
        case gpRf4ce_ProfileIdZrc:
        {
            /*ZRC 1.1 - send the battery info as a vendor specific packet*/
            gpController_Rf4ce_Msg_t rf4ceMsg;
            UInt8 VendorSpecificData[4];
            VendorSpecificData[0] = 0x03; /*Arbitrary chosen battery status attributeID*/
            VendorSpecificData[1] = BatteryMonitorMsg.measurementResult.Flags;
            VendorSpecificData[2] = BatteryMonitorMsg.measurementResult.BatteryLevelLoaded;
            VendorSpecificData[3] = BatteryMonitorMsg.measurementResult.BatteryLevelUnloaded;
            rf4ceMsg.vendorData.vendorId = ControllerVendorId;
            rf4ceMsg.vendorData.txOptions = ControllerTxOptions;
            GP_RF4CE_TXOPTION_ENABLE_VENDOR_SPECIFIC(rf4ceMsg.vendorData.txOptions);
            rf4ceMsg.vendorData.data = VendorSpecificData;
            rf4ceMsg.vendorData.len = 4;
            gpController_Rf4ce_Msg(gpController_Rf4ce_MsgId_SendVendorDataRequest, &rf4ceMsg);
            break;
        }
#endif /*defined(GP_DIVERSITY_APP_ZRC1_1) || defined(GP_DIVERSITY_APP_ZRC2_0)*/
        default:
        {
#ifdef GP_DIVERSITY_APP_ZRC2_0
            /*ZRC 2.0 --> send attributes via GDP profile*/
            gpController_Zrc_Msg_t zrcMsg;
            zrcMsg.PowerStatusUpdate.bindingId = ControllerBindingId;
            /*ZRC 2.0 needs power status as a nibble. It specifies the status of the power source in the node,
              measured under worst case load conditions, in 16 values ranging from lowest power (0x0) to highest power (0xf).
              So to get this, we take the output from batteryMonitor (which has voltage format: 2 MSbits in V, 6 bits in 1/64 volt) and map that on 4 hard coded voltage values.
              This is not entirely correct, but suits the purpose of the example application.*/
            zrcMsg.PowerStatusUpdate.powerStatus=0;
            BATTERY_TRANSLATE_LEVEL_TO_ZRC20(BatteryMonitorMsg.measurementResult.BatteryLevelLoaded, zrcMsg.PowerStatusUpdate.powerStatus);
            zrcMsg.PowerStatusUpdate.powerStatus |= ((BatteryMonitorMsg.measurementResult.Flags << 5)&0xe0);
            gpController_Zrc_Msg(gpController_Zrc_MsgId_SendPowerStatusRequest, &zrcMsg);
#endif /*GP_DIVERSITY_APP_ZRC2_0*/
            break;
        }
    }

}

static void gpController_cbSendBatteryStatusConfirm(UInt8 bindingId, gpRf4ce_Result_t status)
{
    GP_LOG_SYSTEM_PRINTF("cbSendBatteryStatusConfirm Id: 0x%x, S: 0x%x",0,bindingId, status);
}

#ifdef GP_DIVERSITY_APP_MSO
/*SW versioning only supported in MSO*/
static void gpController_SendSWVersionInfoRequest(void)
{
    gpVersion_ReleaseInfo_t swVersion;
    gpController_Mso_Msg_t msoMsg;

    gpVersion_GetSoftwareVersion(&swVersion);
    GP_LOG_SYSTEM_PRINTF("SendSWVersionInfo: v%i.%i.%i.%i",0,swVersion.major, swVersion.minor, swVersion.revision, swVersion.patch);

    msoMsg.VersionInfo.bindingId = ControllerBindingId;
    msoMsg.VersionInfo.vendorId = ControllerVendorId;
    msoMsg.VersionInfo.index = 0; /*SW version - HW version is 0x01, IRDB version is 0x02*/
    MEMCPY(&msoMsg.VersionInfo.version, &swVersion, sizeof(gpVersion_ReleaseInfo_t)/sizeof(UInt8));
    gpController_Mso_Msg(gpController_Mso_MsgId_SendVersionInfoRequest, &msoMsg);
}

static void gpController_cbSendSWVersionInfoConfirm(UInt8 bindingId, gpRf4ce_Result_t status, UInt8 attributeIndex)
{
    GP_LOG_SYSTEM_PRINTF("cbSendSWVersionInfoConfirm Id: 0x%x, S: 0x%x, index: 0x%x",0,bindingId, status, attributeIndex);
}
#endif

static void gpController_SetTXOptions(UInt8 bindingId, gpRf4ce_ProfileId_t profileId)
{
    gpController_Rf4ce_Msg_t msgRf4ce;

#ifdef GP_DIVERSITY_APP_MSO
    ControllerTxOptions = (profileId == gpRf4ce_ProfileIdMso)?TX_OPTIONS_MSO_BASE:TX_OPTIONS_ZRC_BASE;
#else
    ControllerTxOptions = TX_OPTIONS_ZRC_BASE;
#endif

    msgRf4ce.bindingId = bindingId;
    gpController_Rf4ce_Msg(gpController_Rf4ce_MsgId_IsLinkSecure, &msgRf4ce);
    if(msgRf4ce.isLinkSecure)
    {
        GP_RF4CE_TXOPTION_ENABLE_SECURITY(ControllerTxOptions);
    }

    GP_LOG_SYSTEM_PRINTF("TXOptions: 0x%x",0,ControllerTxOptions);
}

//IR종료 callback.
void gpIrTx_cbSendCommandConfirmGeneric(gpIrTx_Result_t status,gpIrTx_TvIrDesc_t* pDesc)
{
//	HAL_WDT_RESET();
//    gpPoolMem_Free(pDesc);
    
    Controller_LedEnable(false,gpController_Led_ColorRed);
    if(status != gpIrTx_ResultSuccess)
    {
        //Controller_LedIndication(&Controller_LedSequenceError);
		GP_LOG_SYSTEM_PRINTF("IR_end callback!!!",0);
    }
    
}
Bool gpIrTx_cbIsKeyPressed(void)
{
#if 0
    GP_LOG_PRINTF("gpIrTx_cbIsKeyPressed 0x%x",0,ControllerOperationMode);
    if(!gpController_KeyReleased)
    {
        //GP_LOG_SYSTEM_PRINTF("CLIP",0);
    }
    return !gpController_KeyReleased;
#else	
    if(ControllerOperationMode == gpController_OperationModeIR)
    {
        //Controller_LedToggle(gpController_Led_ColorRed);
		Controller_LedEnable(true,gpController_Led_ColorRed);
        return true;
    }
//	if(ControllerOperationMode == gpController_OperationModeSetup)
//    	return false;
    return false;
#endif
}


void gpIrTx_cbSendCommandConfirmPredefined( gpIrTx_Result_t status, Bool repeated, gpIrTx_CommandCode_t function )
{
		Controller_LedEnable(false,gpController_Led_ColorRed);
//		Controller_LedEnable(false,gpController_Led_ColorGreen);
}


void Controller_SetVendorIdTimeOut(void)
{
    /* This function Resets vendor ID and switch back to normal mode */
    /* Incompete vendorID will be dicarded */

    ControllerOperationMode = gpController_OperationModeNormal;
    GP_LOG_SYSTEM_PRINTF("Set vendorID Failed",0);
    Controller_LedEnable(false,gpController_Led_ColorGreen);
    Controller_LedIndication(&Controller_LedSequenceError);
}

Bool Controller_HandleVendorID(gpController_Keys_t *keys, gpRf4ce_VendorId_t *NewVendorID)
{
    /* This function will map the recieved keys to values for a new Vendor ID
       When a valid Vendor ID can be composed form the recieved keys, the function
       will return true, when no key is recieved, the key is not part of "gpKeyVendorIdMap"
       or the new vendorId is not complete the function will return false.
       When the function returns true, the new vendorID is written in *NewVendorID
    */
    
    static UInt8 vendorCount = 0;
    static UInt8 vendorCodes[4];
    UIntLoop i;
    UInt16 vendorId;
    
    /* If one parameter NULL, reset handler */
    if(keys == NULL || NewVendorID == NULL)
    {
        vendorCount = 0;
        return false;    
    }
    
    /* check key press or key release, handle only one key at a time */
    if((keys->count) != 1)
    {
        return false;
    }

    /* find binary code of key code in gpKeyVendorIdMap */
    for(i=0;i<(sizeof(gpKeyVendorIdMap)/sizeof(gpKeyVendorIdCode_t));i++)
    {
        if(keys->codes[0] == gpKeyVendorIdMap[i].gpKeyCodeMap)
        {
            vendorCodes[vendorCount] = gpKeyVendorIdMap[i].gpKeyVendorIDCode;
            break;
        }
    }
    
    /* key code not in gpKeyVendorIdMap, return false */
    if(i==(sizeof(gpKeyVendorIdMap)/sizeof(gpKeyVendorIdCode_t)))
    {
        return false;
    }
        
    vendorCount++;
    
    /* for codes recieved, compose new vendor ID */
    if(vendorCount >= 4)
    {
        vendorId = 0x0000;
        vendorId |= (((UInt16)vendorCodes[0] << 12) & 0xF000);
        vendorId |= (((UInt16)vendorCodes[1] <<  8) & 0x0F00);
        vendorId |= (((UInt16)vendorCodes[2] <<  4) & 0x00F0);
        vendorId |= (((UInt16)vendorCodes[3])       & 0x000F);
        *NewVendorID = vendorId;
        return true;
    }
    
    /* Vendor ID not complete */
    return false;   
}

static void controller_GetIRKeysFromDatabase( void )
{
    volatile UInt8 i, keyId;

    if( 0xFFFF != gpIRDatabase_GetIRTableId() )
	{

        for( i=0; i<APP_NUMBER_OF_TV_IR_KEYS; i++ )
        {
	        keyId = controller_TvIrKey_LogicalKeyId[i];
	        if( 0 < gpIRDatabase_GetIRCode( keyId, gpProgrammableKey_Db[i].tvIrDesc.code ) )
	        {
	            gpProgrammableKey_Db[i].keyConfig = APP_TV_IR_DATA_AVAILABLE;

	            gpIrTx_CorrectTimingGeneric(GP_PROGKEY_IRCODE_TXDEFINITION( gpProgrammableKey_Db[i].tvIrDesc.code ));

	        }
			else
			{
    			MEMSET(&gpProgrammableKey_Db[i], -1, sizeof(gpProgrammableKey_Db[i]));
			}

        }

	}

}


Bool gpController_SelectDeviceInDatabase( UInt16 deviceId )
{
    Bool retVal = false;

    gpIRDatabase_SetIRTableId( deviceId );

    if( deviceId == gpIRDatabase_GetIRTableId() )
    {
        controller_GetIRKeysFromDatabase();
        retVal = true;
    }
    return retVal;
}

