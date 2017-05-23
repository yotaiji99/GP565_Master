/*
 * Copyright (c) 2014, GreenPeak Technologies
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
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */
#ifndef _ENUM_MANUAL_H
#define _ENUM_MANUAL_H

/*
 *  this is a file not only for enums but for all kind of stuff 
 * that should be derived from regmap
 */
    
#define GPHAL_MM_CM_RESTORE_START   GP_MM_FLASH_GPDATA_CM_START

//wakeup pins
#define GPHAL_PSM_WAKEUP_PIN_0     0
#define GPHAL_PSM_WAKEUP_PIN_1     5

//Backwards compatibility
#define GPHAL_ENUM_EVENT_STATE_INVALID                              GP_WB_ENUM_EVENT_STATE_INVALID
#define GPHAL_ENUM_EVENT_STATE_SCHEDULED                            GP_WB_ENUM_EVENT_STATE_SCHEDULED
#define GPHAL_ENUM_EVENT_STATE_SCHEDULED_FOR_IMMEDIATE_EXECUTION    GP_WB_ENUM_EVENT_STATE_SCHEDULED_FOR_IMMEDIATE_EXECUTION
#define GPHAL_ENUM_EVENT_STATE_RESCHEDULED                          GP_WB_ENUM_EVENT_STATE_RESCHEDULED
#define GPHAL_ENUM_EVENT_STATE_DONE                                 GP_WB_ENUM_EVENT_STATE_DONE

#define GPHAL_ENUM_EVENT_RESULT_UNKNOWN                             GP_WB_ENUM_EVENT_RESULT_UNKNOWN
#define GPHAL_ENUM_EVENT_RESULT_EXECUTED_ON_TIME                    GP_WB_ENUM_EVENT_RESULT_EXECUTED_ON_TIME
#define GPHAL_ENUM_EVENT_RESULT_EXECUTED_TOO_LATE                   GP_WB_ENUM_EVENT_RESULT_EXECUTED_TOO_LATE
#define GPHAL_ENUM_EVENT_RESULT_MISSED_TOO_LATE                     GP_WB_ENUM_EVENT_RESULT_MISSED_TOO_LATE

//Cca mode fixed on 7.2
#define GPHAL_ENUM_PHY_CCA_MODE_ENERGY                              0
#define GPHAL_ENUM_PHY_CCA_MODE_CARRIER                             0
#define GPHAL_ENUM_PHY_CCA_MODE_ENERGY_AND_CARRIER                  0

//Frame type filter mask
#define GPHAL_ENUM_FRAME_TYPE_FILTER_BCN_MASK                       GP_WB_MACFILT_ACCEPT_FT_BCN_MASK
#define GPHAL_ENUM_FRAME_TYPE_FILTER_DATA_MASK                      GP_WB_MACFILT_ACCEPT_FT_DATA_MASK
#define GPHAL_ENUM_FRAME_TYPE_FILTER_ACK_MASK                       GP_WB_MACFILT_ACCEPT_FT_ACK_MASK
#define GPHAL_ENUM_FRAME_TYPE_FILTER_CMD_MASK                       GP_WB_MACFILT_ACCEPT_FT_CMD_MASK
#define GPHAL_ENUM_FRAME_TYPE_FILTER_RSV_MASK                      (GP_WB_MACFILT_ACCEPT_FT_RSV_4_MASK \
                                                                  | GP_WB_MACFILT_ACCEPT_FT_RSV_5_MASK \
                                                                  | GP_WB_MACFILT_ACCEPT_FT_RSV_6_MASK \
                                                                  | GP_WB_MACFILT_ACCEPT_FT_RSV_7_MASK)

/* Battery monitor definitions - TODO: keep or change API */
#define GPHAL_ENUM_ADCIF_CHANNEL_VBATT                              GP_WB_ENUM_ADC_CHANNEL_VDDA
#define GPHAL_ENUM_ADCIF_VOLTAGE_RANGE_0V_2V                        255 /* dummy value - not used */

#define GP_MM_FLASH_INF_PAGE_MAC_ADDR                              (GP_MM_FLASH_INF_PAGE_START + 0x110)

#endif //_ENUM_MANUAL_H

