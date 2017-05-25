/*
 * Copyright (c) 2014-2015, GreenPeak Technologies
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
 *     0!                         $Header: //depot/main/Embedded/BaseComps/vlatest/sw/comps/gpAudio/inc/gpAudio.h#11 $
 *    M'   GreenPeak              $Change: 78199 $
 *   0'         Technologies      $DateTime: 2016/03/09 17:42:36 $
 *  F
 */
 
#ifndef _GP_AUDIO_H_
#define _GP_AUDIO_H_

/*****************************************************************************
 *                    Include Definitions
 *****************************************************************************/

#include "gpPd.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_AUDIO_PACKET_LENGTH          90

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
typedef struct {
    UInt8 stepSizeIndex;
    UInt8 predictedSampleLsb;
    UInt8 predictedSampleMsb;
    UInt8 numSamples; //number of 4-bit samples
} gpAudio_ADPCMData_t;

#define gpAudio_SamplingRate16KHz   16
typedef UInt8 gpAudio_SamplingRate_t;

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
 
#ifdef __cplusplus
extern "C" {
#endif
 
void gpAudio_Init(void);
void gpAudio_Reset(void);

#ifdef GP_DIVERSITY_AUDIO_OUTPUT

void gpAudio_DataRequest(gpPd_Loh_t *pdLoh);
void gpAudio_cbDataConfirm(gpRf4ce_Result_t status, gpPd_Handle_t *pdLoh );
void gpAudio_PlayTone(void);

#endif // GP_DIVERSITY_AUDIO_OUTPUT

#ifdef GP_DIVERSITY_AUDIO_INPUT

Bool gpAudio_StartRecording( void );
void gpAudio_StopRecording( void );
void gpAudio_cbDataIndication(gpAudio_ADPCMData_t *pData, gpPd_Loh_t *pdLoh);

#if 0
//Just for testing only
void gpAudio_TestStartRecording(void);
void gpAudio_TestStopRecording(void);
#endif
#endif // GP_DIVERSITY_AUDIO_INPUT

#ifdef __cplusplus
}
#endif

#endif // _GP_AUDIO_H_
