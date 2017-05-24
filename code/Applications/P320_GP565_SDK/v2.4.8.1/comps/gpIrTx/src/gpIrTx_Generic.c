/*
 * Copyright (c) 2012-2015, GreenPeak Technologies
 *
 *   This file contains the implementation for sending generic IR codes (IR database format)
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/comps/gpIrTx/src/gpIrTx_Generic.c#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_IRTX
#define GP_IR_TX_DIVERSITY_PREDEFINED

#include "hal.h"
#include "gpBsp.h"
#include "gpLog.h"
#include "gpAssert.h"
#include "gpSched.h"

#include "gpIrTx.h"
#include "gpIrTx_Internal.h"

#include "gpIrTx_Generic_defs.h"

#include "hal_WB.h"


#ifdef GP_DIVERSITY_GPHAL_REUSE_PBM_MEMORY
#include "gpHal_kx_public.h"
#endif //GP_DIVERSITY_GPHAL_REUSE_PBM_MEMORY

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

/*
 * Done on the fly to avoid UInt32 to be used in gpIRTransmissionTimingData.set and gpIRTransmissionTimingData.clr
 * Use when hal expects timing to be expressed in 1us units and be carried in UInt32,
 *
 * If higher accuracy is required, use alternative define below (carrierPeriod to be added to SendSequenceGeneric):
#define IR_TX_CORRECT_TIMING_GENERIC(x) IR_TX_ROUND_TO_NEAREST_CARRIER_125NS((UInt32)(x)<<2, carrierPeriod)
 */
# define IR_TX_CORRECT_TIMING_GENERIC(x) ((UInt32)(x)<<2)

#define NON_BLOCKING_SCHEDULE_INTERVAL(clr)     (clr > 500 ? (UInt32)(clr - 500) * 4 : 0)

#define IR_TX_GENERIC_SEND_IR_SET_CLR(pulseWidth, pauseWidth)      IrTx_SetClr(IR_TX_CORRECT_TIMING_GENERIC(pulseWidth), IR_TX_CORRECT_TIMING_GENERIC(pauseWidth))

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

// config for generic
typedef struct {
    gpIrTx_TvIrDesc_t*  pRequestData;
    UInt8               repeats;
    Bool                discontinuous;
#define IR_TX_GENERIC_MAX_TOGGLE_TIMING_ELEMENTS 8
#define IR_TX_GENERIC_MAX_TOGGLE_SEQUENCES       4
    UInt8               pOriginalToggleData[IR_TX_GENERIC_MAX_TOGGLE_TIMING_ELEMENTS>>1];
    UInt8               toggleId;
} IrTx_ConfigurationGeneric_t;

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static IrTx_ConfigurationGeneric_t currentConfig;
static volatile Bool gpIrTx_LastFrameGeneric = true;

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

static
UInt16 IrTx_SendSequenceGeneric(
                    UInt8* code,
                    UInt8  length,
                    UInt8* pBitSequence
                );
static void   IrTx_HandleRepeatGeneric(void);

static void   IrTx_CopyNibblesGeneric(
                    UInt8* pDstBuff,
                    UInt8  dstOffset,
                    UInt8* pSrcBuff,
                    UInt8  srcOffset,
                    UInt8  length
                );
static void IrTx_UpdateToggle(void);

/*****************************************************************************
 *                    Function Definitions
 *****************************************************************************/

//Translation of timing data to bitsequences
UInt16 IrTx_SendSequenceGeneric(
                UInt8* code,
                UInt8  length,
                UInt8* pBitSequence
            )
{
    UIntLoop  i;
    UInt8  index;
    UInt16 cumulatedSet = 0;
    UInt16 lastClr      = 0;

    for(i = 0; i < length ;i++)
    {
        if(i % 2 == 0)
        {
            //First execute sequence in top 4 bits
            index = pBitSequence[i>>1] >> 4;
        }
        else
        {
            //Then execute sequence in bottom nibble
            index = pBitSequence[i>>1] & 0x0F;
        }

        if (GP_IR_TX_IRCODE_TXTIMINGDATA_CLR_READ(code, index) == 0)
        {
            if (cumulatedSet + GP_IR_TX_IRCODE_TXTIMINGDATA_SET_READ(code, index) < cumulatedSet)   /* overflow */
            {
                IR_TX_GENERIC_SEND_IR_SET_CLR(cumulatedSet, 0);
                cumulatedSet = 0;
            }
            cumulatedSet += GP_IR_TX_IRCODE_TXTIMINGDATA_SET_READ(code, index);
        }
        else
        {
            //Select from signal timing 'database'
            {
                IR_TX_GENERIC_SEND_IR_SET_CLR(cumulatedSet+GP_IR_TX_IRCODE_TXTIMINGDATA_SET_READ(code, index), GP_IR_TX_IRCODE_TXTIMINGDATA_CLR_READ(code, index));
            }
            cumulatedSet = 0;
            lastClr      = GP_IR_TX_IRCODE_TXTIMINGDATA_CLR_READ(code, index);
        }
    }
    if (cumulatedSet > 0)
    {
        IR_TX_GENERIC_SEND_IR_SET_CLR(cumulatedSet, 0);
        lastClr = 0;
    }

    return lastClr;
}

void IrTx_HandleEndGeneric(void)
{
    IrTx_UpdateToggle();

    //Wait untill all IR has ended
    IrTx_DisableIRCarrier();

#ifdef GP_DIVERSITY_GPHAL_REUSE_PBM_MEMORY
    gpHal_ReleasePbmMemory();
#endif //GP_DIVERSITY_GPHAL_REUSE_PBM_MEMORY
    gpIrTx_cbSendCommandConfirmGeneric(gpIrTx_ResultSuccess, currentConfig.pRequestData);
}

void IrTx_HandleReleaseFrameGeneric(void)
{
    UInt8* code         = currentConfig.pRequestData->code;
    UInt8* pBitSequence = GP_IR_TX_IRCODE_BITSEQUENCE(code);

    IrTx_SendSequenceGeneric(currentConfig.pRequestData->code,
                             GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_RELEASE(code),
                             pBitSequence
                           + udiv_ceil(GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_FIRST(code),2)
                           + udiv_ceil(GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_REPEAT(code),2));

    //Start IR transmission - function executed after repeat time of last repeat frame
    IrTx_TriggerIR();

    IrTx_HandleEndGeneric();
}

void IrTx_HandleRepeatGeneric(void)
{
#define lastFrame                    gpIrTx_LastFrameGeneric
    UInt8*                           code         = currentConfig.pRequestData->code;
    UInt8*                           pBitSequence = GP_IR_TX_IRCODE_BITSEQUENCE(code);
    UInt16                           clearPeriod  = 0;

    //Send repeat frames
    //------------------
    /*If there are repeats pending, we send them anyway, regardless of the state of the button (pressed or released)
      In case there are no repeats pending anymore, we check the state of the button. 
         In case the IR code is not discontinuous, repeat frame is sent only if the button is still pressed. Otherwise a release is sent.
         In case the IR code is discontinuous, a release frame is sent, regardless of the button state.*/
    
    if (currentConfig.repeats != 0 || (!currentConfig.discontinuous && gpIrTx_cbIsKeyPressed()))
    {
        UInt8  repeatBits;
        UInt8* pRepeatSequence;
        
        /*We will be sending a repeat. So need to avoid that a release frame is sent.*/
        lastFrame = false;

        //Determine transmission type : AAA ABB - set repeat information correct
        if(GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_REPEAT(code) == 0)
        {
            pRepeatSequence = pBitSequence; //Repeat sequence = first sequence
            repeatBits = GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_FIRST(code);
        }
        else
        {
            pRepeatSequence = pBitSequence + udiv_ceil(GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_FIRST(code),2);
            repeatBits = GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_REPEAT(code);
        }

        if (currentConfig.repeats != 0)
        {
            currentConfig.repeats--;
        }

        //Send bitsequence for repeat frame
        clearPeriod =
            IrTx_SendSequenceGeneric(code, repeatBits, pRepeatSequence);

        //Start IR transmission
        IrTx_TriggerIR();
    }
    else
    {
        lastFrame = true;
    }

    if (lastFrame)
    {
        //Load release frame
        //------------------
        if(GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_RELEASE(code))
        {
            IrTx_HandleReleaseFrameGeneric();
    		//GP_LOG_SYSTEM_PRINTF("IrTx_HandleReleaseFrameGeneric",0);
        }
        else
        {
            IrTx_HandleEndGeneric();
    		//GP_LOG_SYSTEM_PRINTF("IrTx_HandleEndGeneric",0);
        }
    }
    else
    {
        IrTx_MarkEndOfIRCode(); //enables long interrupts, just continue after the end of the current frame (guaranteeing a perfect repeat interval)
                                //only guaranteed to work correctly if clear period is large enough (>750us), otherwise start of next frame could get corrupted
    }
#undef lastFrame
}


static void IrTx_CopyNibblesGeneric(
                UInt8* pDstBuff,
                UInt8  dstOffset,
                UInt8* pSrcBuff,
                UInt8  srcOffset,
                UInt8  length
            )
{
    UIntLoop i;

    for(i = 0; i < length ; i++)
    {
        UInt8 nibble;

        if(((srcOffset+i)%2) == 0)
        {
            nibble = pSrcBuff[(srcOffset+i)>>1] >> 4;
        }
        else
        {
            nibble = pSrcBuff[(srcOffset+i)>>1] & 0xF;
        }
        if(((dstOffset+i)%2) == 0)
        {
            pDstBuff[(dstOffset+i)>>1] &= 0x0F;
            pDstBuff[(dstOffset+i)>>1] |= nibble << 4;
        }
        else
        {
            pDstBuff[(dstOffset+i)>>1] &= 0xF0;
            pDstBuff[(dstOffset+i)>>1] |= nibble;
        }
    }
}

void IrTx_UpdateToggle(void)
{
    UInt8*  pBitSequence = GP_IR_TX_IRCODE_BITSEQUENCE( currentConfig.pRequestData->code);
    UInt8*  code = currentConfig.pRequestData->code;

    if(GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_TOGGLE_MODE(code))
    {
        UInt8 nOfToggleSymbols;
        UInt8* pToggleInfo = NULL; // pToggleInfo[0]: points to first timing nibble to be toggled
                                   // pToggleInfo[1]: number of timing nibbles to be toggled
                                   // ...          : 3*pToggleInfo[1] sets of toggle variants

        pToggleInfo = pBitSequence
                         + udiv_ceil(GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_FIRST(code),2)
                         + udiv_ceil(GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_REPEAT(code),2)
                         + udiv_ceil(GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_RELEASE(code),2);

        nOfToggleSymbols = GP_IR_TX_TOGGLE_N_OF_SYMBOLS(code,pToggleInfo);

        if(currentConfig.toggleId)
        {
            IrTx_CopyNibblesGeneric(pBitSequence, pToggleInfo[0],
                                    currentConfig.pOriginalToggleData, 0,
                                    nOfToggleSymbols);
        }
        currentConfig.toggleId = (currentConfig.toggleId + 1) % GP_IR_TX_TOGGLE_N_OF_SEQUENCES(code);
    }
}

static void IrTx_CorrectSymbolTimingGeneric(UInt8* code, gpIrTx_TransmissionCarrierTime_t carrierTime)
{
    UIntLoop  i;
    UInt16 correction = ((2*(carrierTime)/3) >> 5); // 0.125us -> 4us == /32

    for(i = 0; i < GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_ENTRIES(code); i++)
    {
        UInt16 set = GP_IR_TX_IRCODE_TXTIMINGDATA_SET_READ(code, i);
        UInt16 clr = GP_IR_TX_IRCODE_TXTIMINGDATA_CLR_READ(code, i);

        // GP_LOG_SYSTEM_PRINTF("%i: s %u c %u 2/3 %u",6,(UInt16)i, set<<2, clr<<2,(2*carrierTime/3)>>5);
        if (set)
        {
            set += correction;
            GP_IR_TX_IRCODE_TXTIMINGDATA_SET_WRITE(code, i, set);

            if (clr > correction)
            {
                clr -= correction;
                GP_IR_TX_IRCODE_TXTIMINGDATA_CLR_WRITE(code, i, clr);
            }
        }
    }
}

void IrTx_InitGeneric(void)
{
    currentConfig.toggleId = 0;
}

void IrTx_cbIrStartOfPattern(void)
{
    if(!gpIrTx_LastFrameGeneric)
    {
        gpSched_ScheduleEvent(0,IrTx_HandleRepeatGeneric);
    }
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

Bool gpIrTx_GenericCommandBusy(void)
{
    Bool busy = false;

    busy = busy || (!gpIrTx_LastFrameGeneric);
    busy = busy || gpSched_ExistsEvent(IrTx_HandleRepeatGeneric);

    return busy;
}

void gpIrTx_CorrectTimingGeneric(UInt8* code)
{
    if(GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_CARRIER_TYPE(code))
    {
        IrTx_CorrectSymbolTimingGeneric(code, GP_IR_TX_IRCODE_TXCARRIERTIME(code));   /* 2/3 Carrier Frequency */
    }
}

void gpIrTx_SendCommandRequestGeneric(
                gpIrTx_TvIrDesc_t* pDesc,
                Bool               firstRequest,
                UInt8              commandFlagModifier
            )
{
    gpIrTx_TransmissionCarrierTime_t  carrierTime = GP_IR_TX_IRCODE_TXCARRIERTIME(pDesc->code);
    UInt8*                            pBitSequence = GP_IR_TX_IRCODE_BITSEQUENCE( pDesc->code);

    if (gpIrTx_GenericCommandBusy()
#ifdef GP_IR_TX_DIVERSITY_PREDEFINED
     || gpIrTx_PredefinedCommandBusy()
#endif //GP_IR_TX_DIVERSITY_GENERIC
        )
    {
        // IR generation running
        gpIrTx_cbSendCommandConfirmGeneric(gpIrTx_ResultBusy,pDesc);
        return;
    }
    if( (GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_FIRST(pDesc->code)   > IRTX_MAX_SEQUENCE_SIZE )||
        (GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_REPEAT(pDesc->code)  > IRTX_MAX_SEQUENCE_SIZE )||
        (GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_RELEASE(pDesc->code) > IRTX_MAX_SEQUENCE_SIZE ) )
    {
        gpIrTx_cbSendCommandConfirmGeneric(gpIrTx_ResultInvalidParameter,pDesc);
        return;
    }

    currentConfig.pRequestData  = pDesc;
    // Discontinuous mode
    currentConfig.discontinuous = GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_REPEAT_MODE(pDesc->code);
    // Minimum number of repeats (exact in case of discontinuous)
    currentConfig.repeats       = GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_OF_REPEATS(pDesc->code);
    // Handle command modifier
    if(!currentConfig.discontinuous && GP_IR_TX_GENERIC_OVERRULE_FLAGS_OVERRULE(commandFlagModifier))
    {
        if(GP_IR_TX_GENERIC_OVERRULE_FLAGS_FORCE_DISC(commandFlagModifier))
        {
            currentConfig.discontinuous = true;
        }
        if(GP_IR_TX_GENERIC_OVERRULE_FLAGS_FORCE_REPCNT(commandFlagModifier))
        {
            currentConfig.repeats = GP_IR_TX_GENERIC_OVERRULE_FLAGS_GET_N_OF_REPEATS(commandFlagModifier);
        }
    }

    //Set carrier frequency
    if(GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_CARRIER_TYPE(pDesc->code))
    {
        IrTx_PrepareTransmit(carrierTime); //in 0.125 us

        if(firstRequest)
        {
            IrTx_CorrectSymbolTimingGeneric(pDesc->code, carrierTime);   /* 2/3 Carrier Frequency */
        }
    }
    else
    {
        IrTx_PrepareTransmit(0);
    }


    if(GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_TOGGLE_MODE(pDesc->code))
    {
        UInt8* pToggleInfo = NULL; // pToggleInfo[0]: points to first timing nibble to be toggled
                                  // pToggleInfo[1]: number of timing nibbles to be toggled
                                  // ...          : 3*pToggleInfo[1] sets of toggle variants
        pToggleInfo = pBitSequence
                         + udiv_ceil(GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_FIRST(pDesc->code),2)
                         + udiv_ceil(GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_REPEAT(pDesc->code),2)
                         + udiv_ceil(GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_RELEASE(pDesc->code),2);

        //update bitsequence with new toggle data
        if(currentConfig.toggleId)
        {
            UInt8 nOfToggleSymbols = GP_IR_TX_TOGGLE_N_OF_SYMBOLS(pDesc->code, pToggleInfo);

            if( nOfToggleSymbols > IR_TX_GENERIC_MAX_TOGGLE_TIMING_ELEMENTS )
            {
                gpIrTx_cbSendCommandConfirmGeneric(gpIrTx_ResultInvalidParameter,pDesc);
                return;
            }
            IrTx_CopyNibblesGeneric(currentConfig.pOriginalToggleData,0,
                                    pBitSequence,pToggleInfo[0],
                                    nOfToggleSymbols);
            IrTx_CopyNibblesGeneric(pBitSequence,pToggleInfo[0],
                                    GP_IR_TX_TOGGLE_BITSEQUENCES(pToggleInfo),(currentConfig.toggleId-1)*udiv_ceil(nOfToggleSymbols,2)*2/*unit=nibbles*/,
                                    nOfToggleSymbols);
        }
    }

#ifdef GP_DIVERSITY_GPHAL_REUSE_PBM_MEMORY
    if(!gpHal_ClaimPbmMemory())
    {
        gpIrTx_cbSendCommandConfirmGeneric(gpIrTx_ResultBusy, pDesc);
    }
#endif //GP_DIVERSITY_GPHAL_REUSE_PBM_MEMORY

    //Send start frame
    //----------------
    if(GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_FIRST(pDesc->code))
    {
        UInt16 clearPeriod;

        clearPeriod =
            IrTx_SendSequenceGeneric(pDesc->code, GP_IR_TX_GENERIC_CONTROL_FLAGS_GET_NUM_BITS_FIRST(pDesc->code), pBitSequence);

        gpIrTx_LastFrameGeneric = false;  //IrTx_HandleRepeatGeneric will be called from ISR

        //Start IR transmission
        IrTx_TriggerIR();

    }
    else
    {
        IrTx_HandleRepeatGeneric();
    }
}
