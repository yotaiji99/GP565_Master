
/*
 * Copyright (c) 2012-2013, GreenPeak Technologies
 *
 * gpXcl.h
 * 
 * This file defines the remote autenticated code execution component api
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
 *     0!                         $Header: //depot/release/Embedded/TestCompsRace/v1.3.3.0/sw/comps/gpRace/inc/gpRace.h#1 $
 *    M'   GreenPeak              $Change: 71337 $
 *   0'         Technologies      $DateTime: 2015/10/05 16:20:27 $
 *  F
 */


#ifndef _GPRACE_H_
#define _GPRACE_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_RACE_INFINITE_DOWN_COUNT     (UInt16)(0xFFFF)
#define GP_RACE_STARTUP_DOWN_COUNT      (UInt16)(500) //100ms

#define GP_RACE_STOP_IDX                      (0) 
#define GP_RACE_INFINITE_IDX                  (1)
#define GP_RACE_TRANSMIT_PACKET_IDX           (2)
#define GP_RACE_RETURN_TO_CONTROL_CHANNEL_IDX (3)

#define GP_RACE_STOP_MASK                         ((UInt16)(1) << GP_RACE_STOP_IDX)
#define GP_RACE_INFINITE_MASK                     ((UInt16)(1) << GP_RACE_INFINITE_IDX)
#define GP_RACE_TRANSMIT_PACKET_MASK              ((UInt16)(1) << GP_RACE_TRANSMIT_PACKET_IDX)
#define GP_RACE_RETURN_TO_CONTROL_CHANNEL_MASK    ((UInt16)(1) << GP_RACE_RETURN_TO_CONTROL_CHANNEL_IDX)


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef void (*gpRace_ActionMapHandlerCallback_t)(UInt16);

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
                                                
/**@ingroup GEN_RACE
 * @brief Opens a time window in which the radio is enabled and the device is ready to receive RACE packets.
 * 
 * @param[in] UInt16 initialTimeOutInMs                            - the size of the initial time window. The time window can be extended automatically when the different RACE packets are received.
 * @param[in] gpRace_ActionMapHandlerCallback_t cbActionMapHandler - an optional handler for handling project specific action maps. 
 */
void         gpRace_Start(UInt16 initialTimeOutInMs, gpRace_ActionMapHandlerCallback_t cbActionMapHandler);

#endif // _GPRACE_H_


