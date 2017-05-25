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
 *     0!                         $Header: //depot/main/Embedded/Applications/P857_RC_RemoteSolution_Rogers_Eclipse_GP565/vlatest/apps/Rf4ceController/src/gpController_BMA222E.c#1 $
 *    M'   GreenPeak              $Change: 79988 $
 *   0'         Technologies      $DateTime: 2016/04/30 14:07:02 $
 *  F
 */

//#define GP_LOCAL_LOG

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include "gpController.h"
#include "gpController_BMA222E.h"
#include "gpLog.h"
#include "gpSched.h"

/*******************************************************************************
 *                    Defines
 ******************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_APP

/*******************************************************************************
 *                    Global variables
 ******************************************************************************/
static UInt16 AccRegisterValues[] = { BMA222_INIT_REG_VALUES };

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
void enableLed0FadingDutycycling ( UInt8 threshold )
{
	// needs t o be bet ween 0 and 3 1 . 50% d u t y c y c l i n g = 15
	GP_WB_WRITE_LED_THRESHOLD_0(threshold) ;
	GP_WB_WRITE_LED_FADE_0(true) ; // Fadi ng !
	GP_WB_WRITE_LED_ENABLE_0(true) ;
}

void disableLed0FadingDutycycling ( UInt8 threshold )
{
	// needs t o be bet ween 0 and 3 1 . 50% d u t y c y c l i n g = 15
	GP_WB_WRITE_LED_THRESHOLD_0(threshold) ;
	GP_WB_WRITE_LED_FADE_0(true) ; // Fadi ng !
	GP_WB_WRITE_LED_ENABLE_0(false) ;
}
void initCarrierAndTimeUnit ( UInt8 prescale , UInt8 time_unit )
{
	// number bet ween 0 and 1 5 . Act u a l f r e q u e n c y = 4MHz/(2 ?? p r e s c a l e )
	GP_WB_WRITE_LED_PRESCALE_DIV(prescale) ;
	// do not use s h o r t t i me ba s e
	GP_WB_WRITE_LED_USE_SHORT_TIME_BASE( 0 ) ;
	// s l o p e = ( t i me_uni t +1) ? 1024 us
	GP_WB_WRITE_LED_SLOPE_TIME_UNIT( time_unit ) ;
}

static UInt8 BMA_Read_Byte(UInt8 addr)
{
    UInt8 readBuffer = 0xFF;
    UIntLoop i;

    for (i = 0; i < BMA222_MAX_TWI_RETRIES; i++)
    {
        if (hal_WriteReadTWI(BMA222_DEVICE_ADDRESS, 1, &addr, 1, &readBuffer))
        {
            break;
        }
    }
    return readBuffer;
}

static void BMA_Write_Byte(UInt8* TxBuffer)
{
    UIntLoop i;
    GP_ASSERT_SYSTEM(sizeof(TxBuffer) == 2*sizeof(UInt8));

    for (i = 0; i < BMA222_MAX_TWI_RETRIES; i++)
    {
        if (hal_WriteReadTWI(BMA222_DEVICE_ADDRESS, 2, TxBuffer, 0, NULL))
        {
            break;
        }
        HAL_WAIT_MS(1); /* see BMA222 datasheet 7.2.1 */
    }
}

/*****************************************************************************
 *                    Function Definitions
 *****************************************************************************/


void gpController_Acc_EnableInterrupt(void)
{
    UInt8 TxBuffer[2];
    GP_LOG_PRINTF("BMA222 En Int",0);
    /* Enable interrupts on INT1 */
    TxBuffer[0] = 0x19;
    TxBuffer[1] = 0x02;
    BMA_Write_Byte(TxBuffer);
    /* Open drain, active low */
    TxBuffer[0] = 0x20;
    TxBuffer[1] = 0x02;
    BMA_Write_Byte(TxBuffer);
}

void gpController_Acc_EnableSleep(void)
{
    UInt8 TxBuffer[2];
    GP_LOG_PRINTF("BMA222 Sleep En",0);
    TxBuffer[0] = 0x11;
    TxBuffer[1] = 0x80;
    BMA_Write_Byte(TxBuffer);
}

void gpController_Acc_DisableSleep(void)
{
    UInt8 TxBuffer[2];
    GP_LOG_PRINTF("BMA222 Sleep Dis",0);
    /* Low power mode */
    TxBuffer[0] = 0x11;
    TxBuffer[1] = 0x5C;
    BMA_Write_Byte(TxBuffer);
}

void gpController_Acc_CheckInterruptStatus(void)
{
    gpController_Acc_DisableSleep();
    // Read interrupt pending register of accelerometer
    UInt8 value = BMA_Read_Byte(BMA222_INT_REG);
    if(value & 0x02)
    {
        gpController_EnableBacklight();
        if(gpSched_ExistsEvent(gpController_DisableBacklight))
        {
            gpSched_UnscheduleEvent(gpController_DisableBacklight);
        }
    }
    else
    {
        if(!gpSched_ExistsEvent(gpController_DisableBacklight))
        {
            gpSched_ScheduleEvent(3000000, gpController_DisableBacklight);
        }

    }


}


void gpController_EnableBacklight(void)
{
	HAL_LED_INIT_LEDS();
	initCarrierAndTimeUnit(7,5);
	enableLed0FadingDutycycling(255);
//    HAL_LED_SET_BKL();
}

void gpController_DisableBacklight(void)
{
	initCarrierAndTimeUnit(7,5);
	disableLed0FadingDutycycling(255);
//    HAL_LED_CLR_BKL();
}


