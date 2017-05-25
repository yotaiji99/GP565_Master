/*
 * Copyright (c) 2012-2016, GreenPeak Technologies
 *
 * Driver for Bosch BMA222
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
 *     0!                         $Header: //depot/main/Embedded/Applications/P857_RC_RemoteSolution_Rogers_Eclipse_GP565/vlatest/apps/Rf4ceController/inc/gpController_BMA222E.h#2 $
 *    M'   GreenPeak              $Change: 80006 $
 *   0'         Technologies      $DateTime: 2016/04/30 16:52:21 $
 *  F
 */

#ifndef _GPCONTROLLER_BMA222E_H_
#define _GPCONTROLLER_BMA222E_H_

/*******************************************************************************
 *                      Include Files
 ******************************************************************************/
#include "global.h"
#include "hal.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define BMA222_MAX_TWI_RETRIES 3
#define BMA222_DEVICE_ADDRESS	0x30 /* 0x18 << 1 */
#define BMA222_INIT_REG_VALUES                                              \
                        0x10, 0x0E, /* 500Hz filter bandwidth */            \
                        0x11, 0x5C, /* 500ms Low power mode */              \
                        0x13, 0x40, /* Shadowing disabled */                \
                        0x17, 0x07, /* Enable XYZ axis high-g interrupt */  \
                        0x19, 0x02, /* Map high-g to pin 1 */               \
                        0x20, 0x02, /* open drain, active LOW */            \
                        0x24, 0x01,                                         \
                        0x25, 0x04,                                         \
                        0x26, 0x02,                                         \
                        0x36, 0x07 /* Enable slow offset compensation */
#define BMA222_INT_REG         0x09
#define BMA222_POLLING_TIMEOUT 5000000

/*******************************************************************************
 *                      Function declarations
 ******************************************************************************/
void gpController_Acc_EnableInterrupt(void);
void gpController_Acc_ClearInterrupt(void);
void gpController_Acc_EnableSleep(void);
void gpController_Acc_DisableSleep(void);
void gpController_Acc_CheckInterruptStatus(void);
void gpController_Acc_cbISR(void);

void gpController_EnableBacklight(void);
void gpController_DisableBacklight(void);

#endif /* _GPCONTROLLER_BMA222E_H_ */
