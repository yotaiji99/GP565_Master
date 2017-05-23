/*
 * Copyright (c) 2012-2014, GreenPeak Technologies
 *
 * gpHal_MAC_Ext.h
 *   This file contains all the functions needed for Extra MAC functionality.
 *
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/inc/gpHal_MAC_Ext.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */



#ifndef _GPHAL_MAC_EXT_H_
#define _GPHAL_MAC_EXT_H_

/** @file gpHal_MAC_Ext.h
 *  @brief This file contains all the extra functionality for MAC.
*/


/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gp_global.h"
#include "gpHal_MAC.h"
#include "gpPd.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
//------------------------------------------------------------------------------
// ENUMERATIONS
//------------------------------------------------------------------------------

/** @name gpHal_ContinuousWaveMode_t */
//@{
/** @brief Unmodulated Continuous Wave Mode */
#define gpHal_CW_Unmodulated              'U' /*0x55*/
/** @brief Modulated Continuous Wave Mode */
#define gpHal_CW_Modulated                'M' /*0x4D*/
/** @brief Continuous Wave Mode is off */
#define gpHal_CW_Off                      'O' /*0x4F*/
/** @typedef gpHal_ContinuousWaveMode_t
 *  @brief The gpHal_ContinuousWaveMode_t type defines the different continuous wave modes.
*/                 
typedef UInt8 gpHal_ContinuousWaveMode_t;
//@}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/


/** @brief Set continuous wave mode on/off.
 *
 *  This function sets the continuous wave mode
 *
 *  @param mode  Continuous wave mode can be Unmodulated, Modulated or Off.
 *  @param channel channel on which CW will be generated
 *  @param txpower power at which the CW will be generated (in dB)
 *  @param antenna antenna on which CW will be generated. Auto will default to Ant0 or the antenna selected with GP_HAL_DIVERSITY_SINGLE_ANTENNA diversity
*/
void gpHal_SetContinuousWaveMode( gpHal_ContinuousWaveMode_t mode, UInt8 channel, Int8 txpower, gpHal_AntennaSelection_t antenna);

/** @brief Purges packet from TX queue.
 *
 *  This function purges a packet currently in the TX queue using the pd handle of the packet.
 *  When the handle is non-existent or the packet is already transmitted an error is returned.
 *
 *  Possible results are:
 *          - gpHal_ResultSuccess
 *          - gpHal_ResultInvalidRequest (Invalid pd handle given)
 *          - gpHal_ResultBusy           (Packet already sent)
 *
 *  @param pdHandle  Unique pd handle given to packet at request time.
*/
gpHal_Result_t gpHal_PurgeRequest(gpPd_Handle_t pdHandle);

#endif  /* _GPHAL_MAC_EXT_H_ */
