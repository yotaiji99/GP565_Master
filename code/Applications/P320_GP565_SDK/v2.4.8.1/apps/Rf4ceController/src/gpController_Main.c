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
#include "gpNvm.h"
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

#define NVM_TAG_CONTROLLER_MODE         0
#define NVM_TAG_SHORT_RF_RETRIES_PERIOD 1
#define NVM_TAG_SEND_VC_TO_DTA          2
#define NVM_TAG_VENDOR_ID               3
#define NVM_TAG_TX_COUNTER              4
#define SETUP_INVALID_INDEX             0xFFFF
#define SETUP_INVALID_TV_CODE           0xFFFF
#define APP_TV_IR_POWEROFF_INDEX            0
#define APP_TV_IR_VOL_CTRL_INDEX            2

#define gpStatus_UpdateTypeRF       0
#define gpStatus_UpdateTypeIR       1
UInt32 gpStatus_NumberOfSentIR = 0;
UInt32 gpStatus_NumberOfSentRF = 0;

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
Bool multipleKeysPressed = false;
Bool LedSequenceBlink = false;



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

Bool                            gpController_SetupBlocked;
Bool                            gpController_sendVolumeControlToDta=1;//where to send Volume Control (VC): DTA or TV
gpRf4ce_VendorId_t				gpController_VendorID;              //

gpController_cbKeyIndication_t  gpController_cbKeyIndication; //function pointer used to forward the key-information to the mode/setup specific handling
gpController_Mode_t             gpController_Mode=gpController_ModeIRNec;                  //current controller application mode

UInt8 ControllerOperationSpecial=0xff;
Bool gpSetup_SetupBusy = false;
searchTvIrCode_t     gpSetup_TvHunt;

//Static buffer needed for SetRIB request
UInt8 gpStatus_RIBData[11];
UInt32 gpController_ShortRfRetriesPeriod;       // RfRetriesPeriod
Bool                             gpController_KeyReleased;           //flag tracking of the pending key is already released or not
gpController_PendingKey_t        gpController_PendingKey;            //copy of key info of the key which is currently handled (is also use to block new keys if previous is not handled completly)


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

void setup_cbKeyIndicationSearchTvIrCode( gpKeyboard_pKeyInfo_t pKey );
static void LED_BatteryLow_Indicate(void);
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

void LED_BatteryLow_Indicate(void)
{
    if(gpController_BatterMonitor_BatteryLevelLoaded <= BATTERY_LOW_LEVEL)
    {
//		if (ControllerOperationMode != gpController_OperationSetsequence) 
//		if(	!LedSequenceBlink)
		{
            //Critical level reached - everything is a battery low blink
			LedSequenceBlink=true;
            gpLed_GenerateBlinkSequence( gpController_Led_ColorRed, 50,40,20,5 );
			gpController_Led_Msg(gpController_Led_MsgId_SequenceIndication,&Controller_LedSequenceBlinkIR);
		}

    }
	
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

UInt8 Keyboard_GetCommandLength( gpKeyboard_LogicalId_t logicalId )
{
    switch( logicalId )
    {
        case gpKeyboard_LogicalId_VolumeDown:
        case gpKeyboard_LogicalId_VolumeUp:
        //case gpKeyboard_LogicalId_SetupMenu:
        {
            return 1;
        } 
        // gpKeyboard_LogicalId_Number2 is used as command code to configure the remote
        case gpKeyboard_LogicalId_Number9:
        {
            return 3;
        }
        // gpKeyboard_LogicalId_Number1 is used as command code to enter the TV brand number
        case gpKeyboard_LogicalId_Number1:
        {
            return 5;
        }
    }
    // if no length is avalable, set length 1
    return gpKeyboard_CmdTypeInvalid;
}

static void Keyboard_SetupTimeout( void )
{
    // Cleanup command
    gpKeyboard_Cmd.cmd = gpKeyboard_CmdTypeInvalid;
    MEMSET( gpKeyboard_Cmd.params, gpKeyboard_CmdTypeInvalid, GP_KEYBOARD_MAX_CMD_PARAM_LENGTH );

    gpController_Setup_Msg(gpController_Setup_MsgId_SetupEnterIndication, NULL, NULL,NULL);
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
	    gpController_PendingKey.pendingKeyInfo.keyInfo = 0xff;
    	//설정모드일경우,lvd표시하지않음.
    	if(ControllerOperationMode != gpController_OperationSetsequence)
			gpSched_ScheduleEvent(100000L,LED_BatteryLow_Indicate);
		else
			ControllerOperationMode = gpController_OperationModeNormal;

        if(ControllerOperationMode == gpController_OperationModeIR)
        {
	        gpStatus_UpdateBatteryStatus(gpStatus_UpdateTypeIR);
		    if (gpController_BatterMonitor_BatteryLevelLoadedUpdated)
		    {
				gpSched_ScheduleEvent(0, gpController_SendBatteryStatusRequest);
		    }
            ControllerOperationMode = gpController_OperationModeNormal;
            return true;
        }
    	multipleKeysPressed = false;
		return false;
    }

	if(multipleKeysPressed)
	{
        ControllerOperationMode = gpController_OperationModeNormal;
        return true;
	}

    if(pZrc->KeyPressedIndication.keys.count > 1)
    {
        multipleKeysPressed = true;
		gpSched_UnscheduleEvent(Keyboard_SetupKeyPressed);//double key입력시 setup되지않게 처리.				        
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


//Setup+990 Blink IR
void setup_cbKeyIndicationTVCode( UInt8 pKey )
{
    UInt16 IRTableId;
	
    if( pKey == NULL )
    {
        return;
    }
    IRTableId = gpIRDatabase_GetIRTableId();
	IRTableId+=10000;
    if( (pKey == gpSetup_Data.expectedButton) && ( IRTableId!= 0xFFFF) )
    {
        UInt8 i;
        // add the 1 in the beginning:
        for( i=0; i<(5 - (pKey - gpKeyboard_LogicalId_Number0)); i++ )
        {
            IRTableId = IRTableId/10;
        }

        if( gpSetup_Data.expectedButton != gpKeyboard_LogicalId_Number5 )
        {
            gpSetup_Data.expectedButton++;
            gpLed_GenerateBlinkSequence( gpController_Led_ColorRed, GP_LED_BLINK_NUMBER(IRTableId % 10) );
			LED_Sequence_Control(Controller_LedSequenceBlinkIR);
		    gpSched_ScheduleEvent( 10000000L, Setup_Timeout );

        }
        else
        {
            gpLed_GenerateBlinkSequence( gpController_Led_ColorRed, GP_LED_BLINK_NUMBER(IRTableId % 10) );
			LED_Sequence_Control(Controller_LedSequenceBlinkIR);
			//ControllerOperationMode = gpController_OperationModeNormal;
            ControllerOperationMode = gpController_OperationSetsequence;
			gpSched_UnscheduleEvent(Setup_Timeout);
			gpSched_UnscheduleEvent(Setup_Timeout);
			gpSched_UnscheduleEvent(Setup_Timeout);
			gpSched_UnscheduleEvent(Setup_Timeout);
			gpSched_ScheduleEvent( 1000000L, LED_SetOk_Control );
//			LED_SetOk_Control();
		}
    }
	else
	{
		gpSched_UnscheduleEvent(Setup_Timeout);
		gpSched_UnscheduleEvent(Setup_Timeout);
		gpSched_UnscheduleEvent(Setup_Timeout);
		gpSched_UnscheduleEvent(Setup_Timeout);
		Setup_Timeout();
		gpSched_ScheduleEvent( 1000000L, LED_SetError_Control );
	}
}

//Setup+983 SW VERSION
void setup_cbKeyIndicationVersion( UInt8 pKey )
{
    if( pKey == NULL )
    {
        return;
    }
    if( pKey == gpSetup_Data.expectedButton )
    {
        UInt8 versionData[] = { 2, 0, 0, 0 };

        ///gpSetup_GetVersionInfo( versionData );
        if( gpSetup_Data.expectedButton != gpKeyboard_LogicalId_Number4 )
        {
            gpSetup_Data.expectedButton++;
            gpLed_GenerateBlinkSequence( gpController_Led_ColorRed, GP_LED_BLINK_NUMBER(versionData[pKey - gpKeyboard_LogicalId_Number1]) );
			LED_Sequence_Control(Controller_LedSequenceBlinkIR);
		    gpSched_ScheduleEvent( 10000000L, Setup_Timeout );
        }
        else
        {
            // also send out sw version via RIB
            gpLed_GenerateBlinkSequence( gpController_Led_ColorRed, GP_LED_BLINK_NUMBER(versionData[pKey - gpKeyboard_LogicalId_Number1]) );
			LED_Sequence_Control(Controller_LedSequenceBlinkIR);
			//ControllerOperationMode = gpController_OperationModeNormal;
            ControllerOperationMode = gpController_OperationSetsequence;
			gpSched_UnscheduleEvent(Setup_Timeout);
			gpSched_UnscheduleEvent(Setup_Timeout);
			gpSched_UnscheduleEvent(Setup_Timeout);
			gpSched_ScheduleEvent( 1000000L, LED_SetOk_Control );
//			LED_SetOk_Control();
		}
        return;
    }
	else
	{
		gpSched_UnscheduleEvent(Setup_Timeout);
		gpSched_UnscheduleEvent(Setup_Timeout);
		gpSched_UnscheduleEvent(Setup_Timeout);
		Setup_Timeout();
		gpSched_ScheduleEvent( 1000000L, LED_SetError_Control );
	}
}

static controller_TvIrDesc_t* controller_GetTvIrDesc( UInt8 key )
{
    if( key >= APP_NUMBER_OF_TV_IR_KEYS )
    {
        return NULL;
    }
    return &(gpProgrammableKey_Db[key].tvIrDesc);
}

void gpController_ActionIdTxTvIRPowerOff_func(void)
{
    controller_TvIrDesc_t* pDesc;
    pDesc = controller_GetTvIrDesc( APP_TV_IR_POWEROFF_INDEX );
    if( NULL != pDesc )
    {
		Controller_LedEnable(true,gpController_Led_ColorRed);
        gpIrTx_SendCommandRequestGeneric( (gpIrTx_TvIrDesc_t*)pDesc->code, false, 0 );
    }

}

static UInt16 setup_searchIndexToTvCode( UInt16 index )
{
    gpIRDatabase_SetIRTableIndex( index );
    return gpIRDatabase_GetIRTableId();
}

static UInt16 setup_incrementSearchIndex( UInt16 index )
{
    UInt16 devCode; 
    for(;;)
    {
        index++;
        gpIRDatabase_SetIRTableIndex( index );
        if(index != gpIRDatabase_GetIRTableIndex())
        {
            index = SETUP_INVALID_INDEX;
            break;
        }
        devCode = setup_searchIndexToTvCode( index );
        if( (devCode >= 0) && (devCode <= 9999) )
        {
            break;
        }
    }

    return index;
}

void setup_SearchTvIrCodeNextAttempt( void )
{
    if( SETUP_INVALID_INDEX != gpSetup_TvHunt.currentSearchIndex )
    {
        gpSetup_TvHunt.lastAttemptedTvCode = setup_searchIndexToTvCode( gpSetup_TvHunt.currentSearchIndex );
        gpController_SelectDeviceInDatabase( gpSetup_TvHunt.lastAttemptedTvCode );

///        gpController_ActionQueuePushBack( gpController_ActionIdTxTvIRPowerOff, 0, 0 );
		gpController_ActionIdTxTvIRPowerOff_func();
        gpSetup_TvHunt.currentSearchIndex = setup_incrementSearchIndex( gpSetup_TvHunt.currentSearchIndex );

        gpSched_ScheduleEvent( 10000000L, Setup_Timeout_without_setno );
    }
    else
    {
		gpSched_UnscheduleEvent(Setup_Timeout_without_setno);
		Setup_Timeout_without_setno();
		LED_SetError_Control();
    }
}

void setup_cbKeyIndicationSearchTvIrCode( gpKeyboard_pKeyInfo_t pKey )
{
    if( pKey == NULL )
    {
        return;
    }
    if( gpKeyboard_LogicalId_ChannelUp == pKey )
    {
        // Only start with next TV ir if previous is ready.
        if( !GP_WB_READ_IR_BUSY() )
        {
			gpSched_UnscheduleEvent(Setup_Timeout_without_setno);
            gpSched_ScheduleEvent( 0, setup_SearchTvIrCodeNextAttempt );
        }
    }
    else if( gpKeyboard_LogicalId_SetupMenu == pKey )
    {
        if( SETUP_INVALID_TV_CODE != gpSetup_TvHunt.lastAttemptedTvCode )
        {
            // setting correct IRTableId is done in gpProgrammableKey_SelectDeviceInDatabase.
            gpController_SelectDeviceInDatabase( gpSetup_TvHunt.lastAttemptedTvCode );
            // also make sure the controller 'knows' to send to the TV
            gpController_SetDtaMode( false );            
//            gpSetup_BlinkFail = false;
        }
		gpSched_UnscheduleEvent(Setup_Timeout_without_setno);
		//Setup_Timeout_without_setno();
		gpSched_ScheduleEvent( 1000000L, LED_SetOk_Control );
        ControllerOperationMode = gpController_OperationModeNormal;
    }
    else
    {
		gpSched_UnscheduleEvent(Setup_Timeout_without_setno);
		Setup_Timeout_without_setno();
		gpSched_ScheduleEvent( 1000000L, LED_SetError_Control);
        ControllerOperationMode = gpController_OperationModeNormal;
    }
}

void gpStatus_UpdateBatteryStatus(UInt8 type)
{
    gpController_BatteryMonitor_Msg_t BatteryMonitorMsg;
    Bool forceLoadedBatteryMeasurement = false;

    GP_LOG_PRINTF("us %x",2,type);

    //RF press
    if(type == gpStatus_UpdateTypeRF)
    {
        gpStatus_NumberOfSentRF++;
        if((gpStatus_NumberOfSentRF & 0xF) == 0x0) //Every 16 presses
        {
            forceLoadedBatteryMeasurement = true;
        }

    }
    //IR press
    else if(type == gpStatus_UpdateTypeIR)
    {
        gpStatus_NumberOfSentIR++;
        if((gpStatus_NumberOfSentIR & 0xF) == 0x0) //Every 16 presses
        {
            forceLoadedBatteryMeasurement = true;
        }
    }
	gpController_BatteryMonitor_Measure(forceLoadedBatteryMeasurement);

}

void gpController_LockKeyHandling(gpKeyboard_pKeyInfo_t pKey)
{
    GP_LOG_PRINTF("LOCK KEY",0);
    gpController_KeyReleased = false;
    gpController_PendingKey.pendingKeyInfo = *pKey;
}

void gpController_ReleaseKeyHandling(void)
{
    GP_LOG_PRINTF("RELEASE KEY",0);
    gpController_PendingKey.pendingKeyInfo.keyInfo = 0xff;
}

Bool gpController_HasLockedKeyHandling(void)
{
    return (gpController_PendingKey.pendingKeyInfo.keyInfo != 0xFF);
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
				gpController_KeyBoard_Msg_t tmpKeycnt=*pMsg;
				gpController_Setup_Msg_t msg;
				UInt8 cmdLength;
            	Bool  done = false;
	            UInt8 logicalId;
            	UInt8 i;
				
                Controller_KeyIndicesToKeyCodes(&(pMsg->keys), &(msg.keys)) ;
				logicalId = msg.keys.codes[0];
                /* Forward the keys message to the Setup module. */


	            //Fill in command + parameters
	            if( (gpKeyboard_Cmd.cmd == gpKeyboard_CmdTypeInvalid) && (tmpKeycnt.keys.count==1) ) 
	            {
	                //No command yet
	                cmdLength = Keyboard_GetCommandLength( logicalId );
	                gpKeyboard_Cmd.cmd = gpKeyboard_CmdTypeExternParsed;
	                gpKeyboard_Cmd.params[0] = logicalId;
	                if( cmdLength == 1 )
	                {
	           			LED_Sequence_Control(Controller_LedSequenceGrnBlink);
	                    Controller_LedEnable(true, gpController_Led_ColorGreen);
	                    done = true;
	                }
					else if( cmdLength == gpKeyboard_CmdTypeInvalid)
	                    done = true;
					else
	                {
	           			LED_Sequence_Control(Controller_LedSequenceGrnBlink);
	                    Controller_LedEnable(true, gpController_Led_ColorGreen);
	                }
						

	            }
	            else if(tmpKeycnt.keys.count==1)
	            {
	                cmdLength = Keyboard_GetCommandLength( gpKeyboard_Cmd.params[0] );
	                // Fill in code parameters
	                for( i = 1; i < cmdLength; i++ )
	                {
	                    if( gpKeyboard_Cmd.params[i] == gpKeyboard_CmdTypeInvalid )
	                    {
	                    	if(i<4)
                    		{
			           			LED_Sequence_Control(Controller_LedSequenceGrnBlink);
			                    Controller_LedEnable(true, gpController_Led_ColorGreen);
	                    	}
	                        gpKeyboard_Cmd.params[i] = logicalId;
	                        break;
	                    }
	                }

	                if( i >= (cmdLength-1) )
	                {
	                    done = true;
	                }
	            }
	            
	            // Parameters filled in completely - no need to wait any further
	            if( done ) 
	            {
                    Controller_LedEnable(false, gpController_Led_ColorGreen);
	                gpSched_UnscheduleEvent( Keyboard_SetupTimeout );
	                //gpKeyboard_cbCommandIndication( gpKeyboard_Cmd.cmd, cmdLength, gpKeyboard_Cmd.params );
		            // Indicate full command
                	gpController_Setup_Msg(gpController_Setup_MsgId_KeyPressedIndication,cmdLength, &msg,gpKeyboard_Cmd.params);
	            }
				return;
				
            }

			//Enter Special Mode(Blink IR, SW version, Auto search.....etc)
            else if(ControllerOperationMode == gpController_OperationModeSpecial)
            {
				gpController_Setup_Msg_t msg;
			    UInt16 IRTableId;
				UInt8 keydata;
			    gpController_Zrc_Msg_t msgZrc;
                Controller_KeyIndicesToKeyCodes(&(pMsg->keys), &(msgZrc.KeyPressedIndication.keys));
                if((msgZrc.KeyPressedIndication.keys.count > 1) || (msgZrc.KeyPressedIndication.keys.count == 0))
                {
                    /* Generate release */
                    msgZrc.KeyPressedIndication.keys.count = 0;
                }
				else
				{
					Controller_KeyIndicesToKeyCodes(&(pMsg->keys), &(msg.keys)) ;
					switch(ControllerOperationSpecial)
					{
						case SET_BLINK_TV_IR_CODE:
						{
							//Setup+990 Blink IR
							setup_cbKeyIndicationTVCode(msg.keys.codes[0]);
							break;
						}
						case SET_BLINK_SW_VERSION:
						{
							//Setup+983 SW VERSION
							setup_cbKeyIndicationVersion(msg.keys.codes[0]);
							break;
						}

						case SET_SEARCH_TV_IR_CODE:
						{
							//Setup+A AutoSearch
							setup_cbKeyIndicationSearchTvIrCode(msg.keys.codes[0]);
							break;
						}
					}

					return;
				}
            }
            else if ((ControllerOperationMode == gpController_OperationModeNormal) || 
					  (ControllerOperationMode == gpController_OperationModeIR)		||
					  (ControllerOperationMode == gpController_OperationSetsequence))
            {
                gpController_Zrc_Msg_t msgZrc;
				gpController_KeyBoard_Msg_t tmpKeycnt=*pMsg;
				UInt8 	i;
				UInt8 	TxBuffer[1];
			    UInt8 	batLevel;
                Controller_KeyIndicesToKeyCodes(&(pMsg->keys), &(msgZrc.KeyPressedIndication.keys));

				if((gpController_PendingKey.pendingKeyInfo.keyInfo != 0xff) 
					|| (msgZrc.KeyPressedIndication.keys.count == 0) && (ControllerBindingId == 0xff))
				{
					if((gpController_PendingKey.pendingKeyInfo.keyInfo != msgZrc.KeyPressedIndication.keys.codes[0])
						||(msgZrc.KeyPressedIndication.keys.count != 1))
					{
						if(msgZrc.KeyPressedIndication.keys.codes[0]!=0x0A)
						{
							gpSched_UnscheduleEvent(Keyboard_SetupKeyPressed);//double key입력시 setup되지않게 처리.				        
							if(!GP_WB_READ_IR_BUSY())
							{
								Controller_LedEnable(false, gpController_Led_ColorRed);
								Controller_LedEnable(false, gpController_Led_ColorGreen);
							}
					    	if(ControllerOperationMode != gpController_OperationSetsequence)
								gpSched_ScheduleEvent(100000L,LED_BatteryLow_Indicate);
					        gpStatus_UpdateBatteryStatus(gpStatus_UpdateTypeIR);
						    if (gpController_BatterMonitor_BatteryLevelLoadedUpdated)
						    {
								gpSched_ScheduleEvent(0, gpController_SendBatteryStatusRequest);
						    }
				            ControllerOperationMode = gpController_OperationModeNormal;
							if(msgZrc.KeyPressedIndication.keys.count > 1)
						    	multipleKeysPressed = true;
							else if(msgZrc.KeyPressedIndication.keys.count == 0)
						    	multipleKeysPressed = false;
							return;
						}
					}
				}
				//현재key를 backup.
			    gpController_PendingKey.pendingKeyInfo.keyInfo = msgZrc.KeyPressedIndication.keys.codes[0];
//				gpSched_UnscheduleEvent(LED_BatteryLow_Indicate);
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
                    if(msgZrc.KeyPressedIndication.keys.count > 1) 
                    {
                        /* Generate release */
                        msgZrc.KeyPressedIndication.keys.count = 0;
			            //gpController_KeyBoard_cbMsg(gpController_KeyBoard_MsgId_KeysReleasedIndication, NULL);
						//gpSched_UnscheduleEvent(Keyboard_SetupKeyPressed);				        
						gpStatus_UpdateBatteryStatus(gpStatus_UpdateTypeRF);
					    if (gpController_BatterMonitor_BatteryLevelLoadedUpdated)
					    {
							gpSched_ScheduleEvent(0, gpController_SendBatteryStatusRequest);
					    }
                    }
					else if(msgZrc.KeyPressedIndication.keys.count == 0)
					{
                        /* Generate release */
                        msgZrc.KeyPressedIndication.keys.count = 0;
			            //gpController_KeyBoard_cbMsg(gpController_KeyBoard_MsgId_KeysReleasedIndication, NULL);
				        gpStatus_UpdateBatteryStatus(gpStatus_UpdateTypeRF);
					    if (gpController_BatterMonitor_BatteryLevelLoadedUpdated)
					    {
							gpSched_ScheduleEvent(0, gpController_SendBatteryStatusRequest);
					    }
		        		multipleKeysPressed = false;
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
///                    GP_LOG_SYSTEM_PRINTF("Valid vendor ID entered: 0x%x",0,(UInt16)msgRF4CE.vendorId);
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

			if(ControllerOperationMode != gpController_OperationModeSetup)
            	Controller_LedEnable(false,gpController_Led_ColorGreen);
			if(ControllerBindingId!=0xFF)
			{
				msgZrc.KeyPressedIndication.keys.count = 0;
	            msgZrc.KeyPressedIndication.bindingId = ControllerBindingId;
	            msgZrc.KeyPressedIndication.profileId = ControllerProfileId;
	            msgZrc.KeyPressedIndication.vendorId  = ControllerVendorId;
	            msgZrc.KeyPressedIndication.txOptions = ControllerTxOptions;
	            gpController_Zrc_Msg(gpController_Zrc_MsgId_KeyPressedIndication, &msgZrc);
			}
	        multipleKeysPressed = false;
			break;
        }

        case gpController_KeyBoard_MsgId_SetupEnteredIndication:	//setup key 3초후.
        {
            Controller_LedEnable(true, gpController_Led_ColorGreen);
            Controller_LedEnable(false, gpController_Led_ColorRed);
            /* only go to setup mode when in normal mode */
            if(ControllerOperationMode == gpController_OperationModeNormal ||
				ControllerOperationMode == gpController_OperationModeIR)
            {

                ControllerOperationMode = gpController_OperationModeSetup;
	            gpKeyboard_Cmd.cmd = gpKeyboard_CmdTypeInvalid;
	            MEMSET( gpKeyboard_Cmd.params, 0xFE, GP_KEYBOARD_MAX_CMD_PARAM_LENGTH );
                gpController_Setup_Msg(gpController_Setup_MsgId_SetupEnterIndication , NULL, NULL,NULL);
            }
            break;
        }

        case gpController_KeyBoard_MsgId_SetupKeysPressedIndication:	//setup key를 누르고잇는동안.
        {
			if(ControllerOperationMode == gpController_OperationModeSetup)
	            gpController_KeyBoard_cbMsg(gpController_KeyBoard_MsgId_KeysReleasedIndication, NULL);
			if(gpController_Mode == gpController_ModeIRNec)
			{
				/* only show setup key when in normal mode */
	            if(ControllerOperationMode == gpController_OperationModeNormal)
	            {
	                Controller_LedEnable(true, gpController_Led_ColorRed);
	            }
			}
			else if(gpController_Mode == gpController_ModeMSO)
			{
	            if(ControllerOperationMode == gpController_OperationModeNormal)
	            {
	                Controller_LedEnable(true, gpController_Led_ColorGreen);
	            }
			}
            break;
        }

        case gpController_KeyBoard_MsgId_SetupKeysReleasedIndication:
        {
            /* only handle setup key in normal mode */
			//setupkey를 2초이하 press after release.
            if(ControllerOperationMode == gpController_OperationModeNormal)
            {
				Controller_LedEnable(false, gpController_Led_ColorRed);
                Controller_LedEnable(false, gpController_Led_ColorGreen);
				gpSched_ScheduleEvent(300000L,LED_BatteryLow_Indicate);
            }
			else if(ControllerOperationMode == gpController_OperationModeSetup)
            {
               Controller_LedEnable(false, gpController_Led_ColorRed);
               HAL_WAIT_MS(5);
               Controller_LedEnable(true, gpController_Led_ColorGreen);
            }
            else
            {
				ControllerOperationMode = gpController_OperationModeNormal;
				gpSched_ScheduleEvent(300000L,LED_BatteryLow_Indicate);
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
//            ControllerOperationMode = gpController_OperationModeNormal;
            break;
        }

        case gpController_Setup_MsgId_cbSetupTimeoutIndication:
        case gpController_Setup_MsgId_cbInvalidCommandIndication: /* fall through */
        {
            gpController_Led_Msg_t msg;
            gpController_Led_Sequence_t *sequence = &msg.sequence;
            *sequence = Controller_LedSequenceError;
            gpController_Led_Msg( gpController_Led_MsgId_SequenceIndication, &msg);
			GP_LOG_SYSTEM_PRINTF("SetupTimeout!!!",0);
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
    gpNvm_Restore(GP_COMPONENT_ID , NVM_TAG_SEND_VC_TO_DTA , (UInt8*)&gpController_sendVolumeControlToDta);	//
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

void gpController_CheckBatteryLevel(void)
{
    gpController_BatteryMonitor_Msg_t BatteryMonitorMsg;
	volatile UInt8 value;
	volatile UInt8 value2;
	
    BatteryMonitorMsg.measurementType = gpController_BatteryMonitor_LoadedAndUnloaded;
    /*Trigger a battery voltage measurement. Results will be reported through BattyrMonitorMsg parameter*/
    gpController_BatteryMonitor_Msg(gpController_BatteryMonitor_MsgId_GetBatteryLevel, &BatteryMonitorMsg);
	value = BatteryMonitorMsg.measurementResult.BatteryLevelLoaded;

	BATTERY_TRANSLATE_LEVEL_TO_ZRC20(value, value2);

	switch(value2)
	{
		case 0x0f:
		{
			//High level(3.0v)
            LED_Sequence_Control(Controller_LedSequenceTest1);

			break;
		}
		case 0x0c:
		{
			//Medium level(2.75v)
            LED_Sequence_Control(Controller_LedSequenceTest2);
            LED_Sequence_Control(Controller_LedSequenceTest3);

			break;
		}
		case 0x08:
		{
			//Low level(2.5v)
            LED_Sequence_Control(Controller_LedSequenceTest3);
            LED_Sequence_Control(Controller_LedSequenceTest2);

			break;
		}
		case 0x04:
		{
			//Critical level(2.25v)
            LED_Sequence_Control(Controller_LedSequenceTest4);

			break;
		}
		default:
		{
			break;
		}
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


// this function will populate the keyTransmitAction[] entries for the volume control keys
// to indicate that vc should be sent to the TV;
// this function will only be called when we have TV IR info
void gpController_SetDtaMode( Bool mode )
{
    gpController_sendVolumeControlToDta = mode;
    gpNvm_Backup(GP_COMPONENT_ID , NVM_TAG_SEND_VC_TO_DTA , (UInt8*)&gpController_sendVolumeControlToDta);	//
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

