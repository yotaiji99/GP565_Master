/*
 * Copyright (c) 2015, GreenPeak Technologies
 *
 * gpController_Voice.c
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
 *     0!                         $Header: //depot/main/Embedded/Applications/P857_RC_RemoteSolution_Rogers_Eclipse_GP565/vlatest/apps/Rf4ceController/src/gpController_Voice.c#2 $
 *    M'   GreenPeak              $Change: 80001 $
 *   0'         Technologies      $DateTime: 2016/04/30 15:22:01 $
 *  F
 */


/*******************************************************************************
 *                      Include Files
 ******************************************************************************/
#include "hal.h"
#include "gpLog.h"
#include "gpSched.h"

#include "gpController_Voice.h"
#include "gpAudio.h"
#include "gpRf4ceVoice.h"

/*******************************************************************************
 *                      Defines
 ******************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_APP
#define GP_AUDIO_PACKET_LENGTH          91

/*******************************************************************************
 *                      Type definitions
 ******************************************************************************/
#define Voice_SessionNotBusy       0
#define Voice_SessionStarting      1
#define Voice_SessionStarted       2
static UInt8 gpVoiceSessionState;

/*******************************************************************************
 *                      Static Function Declarations
 ******************************************************************************/
static void gpController_Voice_SessionStart(void);
static void gpController_Voice_SessionStop(void);

/*******************************************************************************
 *                      Static data Declarations
 ******************************************************************************/
static UInt8 VoiceBindingId = 0xFF;
/*******************************************************************************
 *                      Public Functions
 ******************************************************************************/

void gpController_Voice_Init(void)
{
    gpVoiceSessionState = Voice_SessionNotBusy;
    GP_WB_WRITE_IOB_GPIO_16_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN);
}

void gpController_Voice_Msg(    gpController_Voice_MsgId_t msgId,
                                gpController_Voice_Msg_t *pMsg)
{
    switch(msgId)
    {
        case gpController_Voice_MsgId_Start:
        {
            VoiceBindingId = pMsg->bindingId;
            gpController_Voice_SessionStart();
//				gpAudio_TestStartRecording(); //for testing only
            break;
        }
        case gpController_Voice_MsgId_Stop:
        {
            gpController_Voice_SessionStop();
//			gpAudio_TestStopRecording(); //for testing only
            break;
        }
        default:
        {
            GP_ASSERT_DEV_EXT(false);
            break;
        }
    }
}

/*******************************************************************************
 *                      Static Functions
 ******************************************************************************/
static void gpController_Voice_SessionStart(void)
{
    if(gpVoiceSessionState == Voice_SessionNotBusy)
    {
        gpRf4ceVoice_SessionRequest(VoiceBindingId);
        gpVoiceSessionState = Voice_SessionStarting;
    }
}

static void gpController_Voice_DoSessionStop(void)
{
    gpRf4ceVoice_SessionStopRequest(VoiceBindingId);
}

static void gpController_Voice_SessionStop(void)
{
    if( gpVoiceSessionState == Voice_SessionStarted)
    {
        gpAudio_StopRecording();
    }

    /* stop the voice session */
    if(!gpSched_ExistsEvent(gpController_Voice_DoSessionStop))
    {
        gpSched_ScheduleEvent(100000, gpController_Voice_DoSessionStop);
    }
}

/*******************************************************************************
 *                      Gdp20Voice Callbacks
 ******************************************************************************/
void gpRf4ceVoice_cbSessionConfirm(gpRf4ceVoice_SessionStatus_t status,
                                        UInt8 pairingRef)
{
    GP_LOG_SYSTEM_PRINTF( "Started session", 0);

    /* fixme: check on status */
    if(status == gpRf4ceVoice_SessionStatusAvailable)
    {
        if(gpAudio_StartRecording())
        {
            gpVoiceSessionState = Voice_SessionStarted;
        }
        else
        {
            /* unable to lock DMA - stop voice session */
            GP_LOG_SYSTEM_PRINTF(   "DMA - problem : %x (ref: %x)",
                                    0,
                                    status,
                                    pairingRef);

            gpRf4ceVoice_SessionStopRequest(pairingRef);
        }
    }
    else
    {
        GP_LOG_SYSTEM_PRINTF("SessionConfirm failure 0x%x",1,status);
    }
}

void gpRf4ceVoice_cbSessionStopConfirm(gpRf4ceVoice_CommandStatus_t status,
                                            UInt8 pairingRef)
{
    gpVoiceSessionState = Voice_SessionNotBusy;
}

void gpRf4ceVoice_cbSessionTimeoutIndication(UInt8 pairingRef, gpRf4ceVoice_TimeoutReason_t reason)
{
    if( gpVoiceSessionState == Voice_SessionStarted)
    {
        gpAudio_StopRecording();
        gpVoiceSessionState = Voice_SessionNotBusy;
    }
}

void gpRf4ceVoice_cbADPCMDataConfirm(  gpRf4ce_Result_t status,
                                       gpPd_Handle_t pdHandle,
                                       UInt8 pairingRef)
{
    gpPd_FreePd(pdHandle);
}

/*******************************************************************************
 *                      Audio Callbacks
 ******************************************************************************/
void gpAudio_cbDataIndication(  gpAudio_ADPCMData_t *pADPCMData,
                                gpPd_Loh_t *pPdLoh )
{
    gpRf4ceVoice_ADPCMData_t zrcVoiceADPCMData;

    if( gpVoiceSessionState != Voice_SessionStarted)
    {
        gpPd_FreePd(pPdLoh->handle);
        return;
    }
    zrcVoiceADPCMData.stepSizeIndex      = pADPCMData->stepSizeIndex;
    zrcVoiceADPCMData.predictedSampleLsb = pADPCMData->predictedSampleLsb;
    zrcVoiceADPCMData.predictedSampleMsb = pADPCMData->predictedSampleMsb;
    zrcVoiceADPCMData.numSamples         = GP_AUDIO_PACKET_LENGTH; /* number of 4-bit samples */

    gpRf4ceVoice_ADPCMDataRequest( &zrcVoiceADPCMData, *pPdLoh, VoiceBindingId);
}
