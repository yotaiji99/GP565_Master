/*
 * Copyright (c) 2009-2011,2013-2014, GreenPeak Technologies
 *
 *   Hardware Abstraction Layer for the part of XAP5 HAL that needs WB.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/halXap5/inc/hal_WB.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _HAL_WB_H
#define _HAL_WB_H

#include "gpHal_reg.h"
#include "gpHal_HW.h"
#include "gpAssert.h"
#include <machine/xap.h>

#ifndef GP_COMPONENT_ID
#define GP_COMPONENT_ID GP_COMPONENT_ID_HALXAP5
#define GP_COMPONENT_ID_DEFINED_IN_HEADER
#endif //GP_COMPONENT_ID

/*****************************************************************************
 *              SLEEP
 *****************************************************************************/

static INLINE void hal_EnableGotoSleep(void)
{
    GP_WB_WRITE_XAP5_GP_SLEEP_ENABLE(1);
}
static INLINE void hal_DisableGotoSleep(void)
{
    GP_WB_WRITE_XAP5_GP_SLEEP_ENABLE(0);
}

/*****************************************************************************
 *                    TIMER
 *****************************************************************************/

static INLINE UInt32 hal_timer_get_current_time_us(void)
{
    UInt32 t;
    t = GP_WB_READ_ES_AUTO_SAMPLED_SYMBOL_COUNTER();
    return t*2;
}

static INLINE UInt32 hal_timer_get_current_time(void)
{
    return hal_timer_get_current_time_us()/32;
}

#define HAL_TIMER_GET_CURRENT_TIME(time)        \
    do {                                        \
        (time) = hal_timer_get_current_time();  \
    } while (false)

#define HAL_TIMER_START(x)
#define HAL_TIMER_STOP()
#define HAL_TIMER_RESTART()

#ifdef GP_COMPONENT_ID_DEFINED_IN_HEADER
#undef GP_COMPONENT_ID
#undef GP_COMPONENT_ID_DEFINED_IN_HEADER
#endif //GP_COMPONENT_ID_DEFINED_IN_HEADER


#endif
