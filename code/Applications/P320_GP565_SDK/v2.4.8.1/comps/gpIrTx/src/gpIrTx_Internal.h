/*
 * Copyright (c) 2012-2014, GreenPeak Technologies
 *
 *   This file contains all internal definitions for gpIrTx component.
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/comps/gpIrTx/src/gpIrTx_Internal.h#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */

#ifndef _GP_IR_TX_INTERNAL_H_
#define _GP_IR_TX_INTERNAL_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_IRTX

#include "gpIrTx.h"

#define IR_TX_FLAGS_KEYDOWN     0x0
#define IR_TX_FLAGS_KEYUP       0x1
#define IR_TX_FLAGS_REPEAT      0x8

#define IR_TX_CARRIER_FREQ_TO_PERIOD_125NS(freq)                 udiv_round(8000000UL, freq)
#define IR_TX_ROUND_TO_NEAREST_CARRIER_125NS(length, carrier)   (length)

#ifndef IRTX_MAX_SEQUENCE_SIZE
#define IRTX_MAX_SEQUENCE_SIZE 80
#endif //IRTX_MAX_SEQUENCE_SIZE

#define IRTX_BUFFER_SIZE_IN_WORDS (IRTX_MAX_SEQUENCE_SIZE * 2)

typedef struct IrTx_ProtocolInfo IrTx_ProtocolInfo_t;

struct IrTx_ProtocolInfo {
    gpIrTx_IrComponentType_t         type;
    UInt8                            repeat_ms;
    UInt16                           carrier_125ns;
    void                           (*transmitFun)(gpIrTx_Config_t* config, UInt8 flags);
};

void  IrTx_InitGeneric(void);
UInt8 IrTx_CalcNibbleChecksum(UInt32 packet);


#define IrTx_InitDrv()                  hal_InitIR()
#define IrTx_PrepareTransmit(period)    hal_SetIRCarrierPeriod(period)
#define IrTx_PrepareReTransmit(period)
void    IrTx_DisableIRCarrier(void);
void    IrTx_TriggerIR(void);
void    IrTx_SetClr(UInt32 time1, UInt32 time2);
void    IrTx_InitBuffers(void);
void    IrTx_cbIrStartOfPattern(void);

#define IrTx_MarkEndOfIRCode()          hal_MarkEndOfIRCode()

#define IrTx_SendSequence(pTimingData, numTimingData, numTimingBits, length, bitSequence) \
        IrTx_SendSequence_DrvFull(pTimingData, numTimingBits, length, bitSequence);
void    IrTx_SendSequence_DrvFull(
                const gpIrTx_TransmissionTimingData_t* pTimingData,
                      UInt8                            numTimingBits,
                      UInt8                            length,
                      UInt32                           bitSequence
            );

#endif  /* _GP_IR_TX_INTERNAL_H_ */
