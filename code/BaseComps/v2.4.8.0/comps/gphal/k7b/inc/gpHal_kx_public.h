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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/inc/gpHal_kx_public.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GPHAL_KX_PUBLIC_H_
#define _GPHAL_KX_PUBLIC_H_

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*Backwards compatibility - same implementation as GP_HAL_PBM_ENTRY2ADDR_DATA_BASE(entry)*/
#define GP_HAL_PBM_ENTRY2ADDR(entry)        (GP_MM_RAM_PBM_0_DATA_START + (entry)*GP_MM_RAM_PBM_0_DATA_SIZE)
#define GP_HAL_CHECK_PBM_VALID(entry)       ((entry) < GPHAL_NUMBER_OF_PBMS_USED)
#define GP_HAL_IS_PBM_ALLOCATED(entry)      (GP_WB_READ_PBM_ADM_PBM_ENTRY_CLAIMED() & BM(entry))

#ifdef GP_DIVERSITY_GPHAL_EXTERN
typedef void (*gpHal_IRCallback_t)(void);
void gpHal_RegisterIRCallback(gpHal_IRCallback_t cb);
#endif //GP_DIVERSITY_GPHAL_EXTERN

/*
 * FIXME: handle STBC interrupt - left masked below.
 * FIXME: no IPC interrupts in below 16-bit write.
 Also need: 
GP_WB_INT_CTRL_MASK_INT_WATCHDOG_INTERRUPT_MASK  ??
GP_WB_INT_CTRL_MASK_INT_TRC_INTERRUPT_MASK ??
GP_WB_INT_CTRL_MASK_INT_QTA_INTERRUPT_MASK ??
 */
#if defined(GP_DIVERSITY_GPHAL_EXTERN) 
#define GP_HAL_ENABLE_INTERRUPTS(enable)                                \
    do {                                                                \
        GP_WB_WRITE_INT_CTRL_MASK_EXT_ES_INTERRUPT(enable);             \
        GP_WB_WRITE_INT_CTRL_MASK_EXT_RCI_INTERRUPT(enable);            \
        GP_WB_WRITE_INT_CTRL_MASK_EXT_PHY_INTERRUPT(enable);            \
        GP_WB_WRITE_INT_CTRL_GLOBAL_INTERRUPTS_ENABLE(enable);          \
    } while (false)
#else
#define GP_HAL_ENABLE_INTERRUPTS(enable)                                \
    do {                                                                \
        GP_WB_WRITE_INT_CTRL_MASK_INT_ES_INTERRUPT(enable);             \
        GP_WB_WRITE_INT_CTRL_MASK_INT_RCI_INTERRUPT(enable);            \
        GP_WB_WRITE_INT_CTRL_MASK_INT_PHY_INTERRUPT(enable);            \
        GP_WB_WRITE_INT_CTRL_MASK_INT_STBC_INTERRUPT(enable);           \
    } while (false)
#endif

#define GP_HAL_ENABLE_PIO_INT(enable) /*PIO = RCI */     \
    do {                                                       \
        GP_HAL_WRITE_REG(GP_WB_INT_CTRL_MASK_RCI_DATA_IND_INTERRUPT_ADDRESS, \
                                (enable) ? GP_WB_INT_CTRL_MASK_RCI_DATA_IND_INTERRUPT_MASK   |  \
                                           GP_WB_INT_CTRL_MASK_RCI_DATA_CNF_0_INTERRUPT_MASK /*Data Cfm*/ | \
                                           GP_WB_INT_CTRL_MASK_RCI_DATA_CNF_1_INTERRUPT_MASK /*ED scan*/ : 0x0);  \
    } while (false)

#define GP_HAL_ENABLE_EMPTY_QUEUE_CALLBACK_INTERRUPT(enable) GP_WB_WRITE_INT_CTRL_MASK_QTA_SCH0_EMPTY_INTERRUPT(enable)
#define GP_HAL_ENABLE_BUSY_TX_CALLBACK_INTERRUPT(enable)     /*No longer available ?*/

#define GP_HAL_ENABLE_EXTERNAL_EVENT_INTERRUPT(enable)       GP_WB_WRITE_INT_CTRL_MASK_ES_EXTERNAL_EVENT_INTERRUPT(enable)

//gpHal_MAC.h specific macro's

#define GP_HAL_GET_ADDRESS_RECOGNITION()          (GP_WB_READ_MACFILT_DST_PAN_ID_CHECK_ON() || GP_WB_READ_MACFILT_DST_ADDR_CHECK_ON())
#define GP_HAL_SET_FRAME_TYPE_FILTER_MASK(bitmap) GP_HAL_WRITE_REG(GP_WB_MACFILT_ACCEPT_FT_BCN_ADDRESS, (UInt8)(~(bitmap)))
#define GP_HAL_GET_FRAME_TYPE_FILTER_MASK()       ((UInt8)(~(GP_HAL_READ_REG(GP_WB_MACFILT_ACCEPT_FT_BCN_ADDRESS))))
#define GP_HAL_GET_RX_ON_WHEN_IDLE()              !!(GP_HAL_READ_REG(GP_WB_RIB_RX_ON_WHEN_IDLE_CH0_ADDRESS)) /*5 bits*/

//Mac state containing timed, calibration and receiver status
#define GP_HAL_MAC_STATE_INIT()            { gpHal_MacState = 0; }
#define GP_HAL_MAC_STATE_SET_TIMED()       BIT_SET(gpHal_MacState, 0)
#define GP_HAL_MAC_STATE_CLR_TIMED()       BIT_CLR(gpHal_MacState, 0)
#define GP_HAL_MAC_STATE_GET_TIMED()       BIT_TST(gpHal_MacState, 0)

#define GP_HAL_MAC_STATE_SET_CALIBRATED()  BIT_SET(gpHal_MacState, 1)
#define GP_HAL_MAC_STATE_GET_CALIBRATED()  BIT_TST(gpHal_MacState, 1)

#define GP_HAL_MAC_STATE_SET_RXON()        BIT_SET(gpHal_MacState, 2)
#define GP_HAL_MAC_STATE_CLR_RXON()        BIT_CLR(gpHal_MacState, 2)
#define GP_HAL_MAC_STATE_GET_RXON()        BIT_TST(gpHal_MacState, 2)

#define GP_HAL_MAC_STATE_SET_PROMISCUOUS_MODE()        BIT_SET(gpHal_MacState, 3)
#define GP_HAL_MAC_STATE_CLR_PROMISCUOUS_MODE()        BIT_CLR(gpHal_MacState, 3)
#define GP_HAL_MAC_STATE_GET_PROMISCUOUS_MODE()        BIT_TST(gpHal_MacState, 3)

// Number of SRCIDS supported by this hal
#define GP_HAL_MAC_NUMBER_OF_RX_SRCIDS       1

/** @brief Disables the HW RX MAC.
 *
 *  This functions enables the re-use of PBMs for other purposes.
 *  The use case for this is IR TX.
*/
GP_API Bool gpHal_ClaimPbmMemory(void);
/** @brief Enables the HW RX MAC.
*/
GP_API Bool gpHal_ReleasePbmMemory(void);

#endif //_GPHAL_KX_PUBLIC_H_
