/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
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
 *     0!                         $Header: //depot/main/Embedded/BaseComps/vlatest/sw/comps/gpAudio/src/gpAudio_Input.c#26 $
 *    M'   GreenPeak              $Change: 79997 $
 *   0'         Technologies      $DateTime: 2016/04/30 15:17:21 $
 *  F
 */

//#define GP_LOCAL_LOG

/*****************************************************************************
 *                    Include Definitions
 *****************************************************************************/

#define  GP_COMPONENT_ID       GP_COMPONENT_ID_AUDIO

#include "hal.h"
#include "gpAssert.h"
#include "gpAudio.h"
#include "gpCompression.h"
#include "gpLog.h"
#include "gpPd.h"
#include "gpSched.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
#define Audio_SessionIdle          0
#define Audio_SessionStarting      1
#define Audio_SessionStarted       2
typedef UInt8 Audio_SessionState_t;

#define Audio_ModePCM     0
#define Audio_ModeADPCM   1
typedef UInt8 Audio_Mode_t;

 /*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
static gpCompression_Compression_State_t Audio_CompressionState;
    
/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/
/** @brief Initialize Audio HW */
static void INLINE Audio_Init(void);
/** @brief Main loop to get ADPCM samples */
static void Audio_getADPCM(void);
/** @brief Starts Audio sampling HW */
static Bool INLINE Audio_StartProcessing(void);
/** @brief Stop Audio sampling HW */
static void INLINE Audio_StopProcessing(void);
/** @brief Function fills outputBuffer with incoming audio samples. Will compress them with ADPCM and
    @param pOutputBuffer    GP_AUDIO_PACKET_LENGTH bytes ADPCM compressed @ 16 kHz
    @param pState           ADPCM state to be kept during processing
*/
static void Audio_ProcessSamples(UInt8* pOutBuffer, gpCompression_Compression_State_t* pState);

/*****************************************************************************
 *                    Static Debug Function Prototypes
 *****************************************************************************/
#define Audio_DumpDebug()
#define Audio_AddDebug(pData, length)

/*****************************************************************************
 *                    External Function Prototypes
 *****************************************************************************/
#include "gpAudio_cx20823.c"

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
static UInt8 Audio_SessionState;

/*****************************************************************************
 *                    Debug Function Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Loop Function
 *****************************************************************************/
// main loop for getting ADPCM samples
void Audio_getADPCM(void)
{
    gpPd_Loh_t pdLoh;
    gpAudio_ADPCMData_t ADPCMData;
    UInt8 out_bytes[GP_AUDIO_PACKET_LENGTH];

    if(Audio_SessionState != Audio_SessionStarted)
    {
        return;
    }

    pdLoh.handle = gpPd_GetPd();
    if(gpPd_CheckPdValid(pdLoh.handle) != gpPd_ResultValidHandle)
    {
        /* reschedule asap to avoid missing too many samples */
        GP_LOG_SYSTEM_PRINTF("Could not claim pd",0);
        gpSched_ScheduleEvent(0, Audio_getADPCM);
        return;
    }

    // Fill in ADPCM info
    ADPCMData.stepSizeIndex      = Audio_CompressionState.stepIndex;
    ADPCMData.predictedSampleLsb = (Int8)Audio_CompressionState.startSample;
    ADPCMData.predictedSampleMsb = (Int8)(Audio_CompressionState.startSample>>8);

    // Compress audio data
    Audio_ProcessSamples(out_bytes, &Audio_CompressionState);


    // Write compressed stream to Pd
    gpPd_WriteByteStream(pdLoh.handle, 0, GP_AUDIO_PACKET_LENGTH, out_bytes);
    pdLoh.offset = 0;
    pdLoh.length = GP_AUDIO_PACKET_LENGTH;

    gpAudio_cbDataIndication( &ADPCMData, &pdLoh );

}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
#if 0
void gpAudio_TestStartRecording(void)
{
	Audio_StartProcessing();
}


void gpAudio_TestStopRecording(void)
{
	Audio_StopProcessing();
}
#endif
void gpAudio_Init(void)
{
    Audio_Init();
    Audio_SessionState = Audio_SessionIdle;
}

void gpAudio_Reset(void)
{
    gpAudio_StopRecording();
    gpAudio_Init();
}

Bool gpAudio_StartRecording( void )
{
    if(Audio_SessionState == Audio_SessionIdle)
    {
        Audio_CompressionState.stepIndex = 0;
        Audio_CompressionState.startSample = 0;
        Audio_SessionState = Audio_SessionStarting;

        if(Audio_StartProcessing())
        {
            GP_LOG_SYSTEM_PRINTF("Start Audio Session",0);
            Audio_SessionState = Audio_SessionStarted;
            return true;
        }
        else
        {
            GP_LOG_SYSTEM_PRINTF("Error starting audio session",0);
            Audio_SessionState = Audio_SessionIdle;
            return false;
        }
    }
    else
    {
        GP_LOG_SYSTEM_PRINTF("Audio state not idle",0);
        return false;
    }
}

void gpAudio_StopRecording( void )
{
    // polling based
    gpSched_UnscheduleEvent(Audio_getADPCM);
    if(Audio_SessionState != Audio_SessionIdle)
    {
        Audio_SessionState = Audio_SessionIdle;
        GP_LOG_SYSTEM_PRINTF("Stop audio session",0);
        Audio_StopProcessing();
        Audio_DumpDebug();
    }
    else
    {
        GP_LOG_SYSTEM_PRINTF("StopSession error: no active audio session",0);
    }
}
