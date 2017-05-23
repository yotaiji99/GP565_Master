/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/halXap5/k7b/src/hal_ADC.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_HALXAP5

#include "hal.h"

//#define GP_LOCAL_LOG

#include "gpLog.h"
#include "gpHal.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define HAL_ADC_NBR_OF_REJECTED_CONVERSIONS 2 /*3rd sample is stable*/

#define HAL_ADC_CONVERT_TO_8BIT(voltage)    ((Int8)(((voltage)+0x80)>>8))
#define HAL_ADC_IS_ANIO_CHANNEL(channel)    ( (channel == GP_WB_ENUM_ADC_CHANNEL_ANIO0) || \
                                              (channel == GP_WB_ENUM_ADC_CHANNEL_ANIO1) || \
                                              (channel == GP_WB_ENUM_ADC_CHANNEL_ANIO2) || \
                                              (channel == GP_WB_ENUM_ADC_CHANNEL_ANIO3) )

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data
 *****************************************************************************/

#define HalADC_FreeBuffer 0xFF

UInt8 bufferA_channel;
UInt8 bufferB_channel;

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*
static void halADC_dumpConfig(void)
{
    UInt16 addr = 0x241;
    UInt8 config;
    UInt8 channel;
    UInt8 slotInUse = GP_WB_READ_ADCIF_NBR_OF_SLOTS_IN_CYCLE();

    GP_LOG_PRINTF("--- ADC CONFIG Slots %i ---",0,GP_WB_READ_ADCIF_NBR_OF_SLOTS_IN_CYCLE());

    switch(GP_WB_READ_ADCIF_TRIGGER_MODE())
    {
        case GP_WB_ENUM_ADC_TRIGGER_MODE_WHEN_RSSI_REQ:
        {
            GP_LOG_PRINTF("Rssi trigger",0);
            break;
        }
        case GP_WB_ENUM_ADC_TRIGGER_MODE_ALWAYS:
        {
            GP_LOG_PRINTF("Always trigger",0);
            break;
        }
        case GP_WB_ENUM_ADC_TRIGGER_MODE_WHEN_EXTERNAL_TRIGGER:
        {
            GP_LOG_PRINTF("External trigger",0);
            break;
        }
        case GP_WB_ENUM_ADC_TRIGGER_MODE_NEVER:
        {
            GP_LOG_PRINTF("Never trigger",0);
            break;
        }
        default:
        {
            GP_ASSERT_SYSTEM(false);
        }
    }
    gpLog_Flush();

    for(UIntLoop i = 0; i <= slotInUse; i++)
    {
        config = GP_WB_READ_U8(addr+i);
        channel = GP_WB_GET_ADCIF_SLOT_A_CHANNEL_FROM_SLOT_A_CONFIG(config);
        switch(channel)
        {
            case GP_WB_ENUM_ADC_CHANNEL_RSSI:
            {
                GP_LOG_PRINTF("Slot %i Rssi",0,i);
                break;
            }
            case GP_WB_ENUM_ADC_CHANNEL_VDDA:
            {
                GP_LOG_PRINTF("Slot %i Vdda",0,i);
                break;
            }
            case GP_WB_ENUM_ADC_CHANNEL_TEMP:
            {
                GP_LOG_PRINTF("Slot %i Temp",0,i);
                break;
            }
            case GP_WB_ENUM_ADC_CHANNEL_CHANNEL3:
            case GP_WB_ENUM_ADC_CHANNEL_CHANNEL4:
            case GP_WB_ENUM_ADC_CHANNEL_CHANNEL5:
            case GP_WB_ENUM_ADC_CHANNEL_CHANNEL6:
            case GP_WB_ENUM_ADC_CHANNEL_CHANNEL7:
            case GP_WB_ENUM_ADC_CHANNEL_CHANNEL14:
            case GP_WB_ENUM_ADC_CHANNEL_CHANNEL15:
            {
                GP_LOG_PRINTF("Slot %i Channel%i",0,i,channel);
                break;
            }
            case GP_WB_ENUM_ADC_CHANNEL_ANIO0:
            case GP_WB_ENUM_ADC_CHANNEL_ANIO1:
            case GP_WB_ENUM_ADC_CHANNEL_ANIO2:
            case GP_WB_ENUM_ADC_CHANNEL_ANIO3:

            {
                GP_LOG_PRINTF("Slot %i ANIO%i",0,i,channel-0x08);
                break;
            }
            case GP_WB_ENUM_ADC_CHANNEL_ADC_GPI0:
            case GP_WB_ENUM_ADC_CHANNEL_ADC_GPI1:
            {
                GP_LOG_PRINTF("Slot %i Rssi",0,i);
                break;
            }
            default:
            {
                GP_ASSERT_DEV_INT(false);
            }
        }
        gpLog_Flush();
        GP_LOG_PRINTF("Post To Rssi %i",0,GP_WB_GET_ADCIF_SLOT_A_POST_TO_RSSI_FROM_SLOT_A_CONFIG(config));
        GP_LOG_PRINTF("Post To Buf A %i",0,GP_WB_GET_ADCIF_SLOT_A_POST_TO_BUFFER_A_FROM_SLOT_A_CONFIG(config));
        gpLog_Flush();
        GP_LOG_PRINTF("Post To Buf B %i",0,GP_WB_GET_ADCIF_SLOT_A_POST_TO_BUFFER_B_FROM_SLOT_A_CONFIG(config));
        GP_LOG_PRINTF("Post To Fifo %i",0,GP_WB_GET_ADCIF_SLOT_A_POST_TO_FIFO_FROM_SLOT_A_CONFIG(config));
        GP_LOG_PRINTF("-----------------",0);
        gpLog_Flush();
    }
}
*/

static UInt16 halADC_ConvertToVoltage(UInt16 raw, UInt8 channel)
{
    UInt32 temp;
    UInt16 adc_reference_calibration;
    UInt8 adc_divider = 3;


    gpHal_FlashReadInf(GP_MM_FLASH_INF_PAGE_START + 0xF4,2,(UInt8*)&adc_reference_calibration);

    //For backwards compatibility
    if (adc_reference_calibration == 0 || adc_reference_calibration > 0x8000)
    {
       adc_reference_calibration = 0x4CE0; //Nominal value, for VDDREF = 1.2V
    }

    /* Multiply the raw measured value with the calibration value, and scale with gain factor 3, and divided
       by 2^10, according to formula above. Factor 3 is because only 1/3 of the voltage is measured.
       The formula below results in right aligned ADC value of the measured ADC channel. The two MSbits represent the
       voltage before the comma, the other 14 bits contain the decimal value.
       Jira ticket sw-3211 explains how the calibration-value is calculated.
    */

    if (!GP_WB_READ_ADCIF_ADC_VOLT_DIV_ENA() && HAL_ADC_IS_ANIO_CHANNEL(channel) )
    {
        //Divider only applicable for ANIO measurements.
        //There is always a divider for the VDDA measurement.
        adc_divider = 1;
    }
    temp = ((UInt32)raw * adc_reference_calibration * adc_divider) >> 10;

    //Clip to max (~4V).
    if(temp > 0xFFFF)
    {
        temp = 0xFFFF;
    }

    //Reduce to 16 bit number, with 2 MSB and 14 LSB (format that we might report in for next version - exploiting the 10-bit nature of the ADC)
    return (UInt16)temp;
}

static Int16 halADC_ConvertToTemperature(UInt16 raw)
{
    Int32 temp;

    UInt16 adc_reference_value_calibration;
    UInt16 temperature_offset_calibration;
    UInt16 temperature_slope_calibration;

    gpHal_FlashReadInf(GP_MM_FLASH_INF_PAGE_START + 0xF4, 2, (UInt8*)&adc_reference_value_calibration);
    gpHal_FlashReadInf(GP_MM_FLASH_INF_PAGE_START + 0xF6, 2, (UInt8*)&temperature_offset_calibration);
    gpHal_FlashReadInf(GP_MM_FLASH_INF_PAGE_START + 0xF8, 2, (UInt8*)&temperature_slope_calibration);

    //For backwards compatibility
    if (adc_reference_value_calibration == 0 || adc_reference_value_calibration >= 0x8000)
    {
        adc_reference_value_calibration = 0x4CE0; //Nominal value, for VDDREF = 1.2V
    }

    if (temperature_offset_calibration == 0 || adc_reference_value_calibration >= 0x8000)
    {
        temperature_offset_calibration = 0x295E; //Nominal value, for Temperature_ADC_Result_25 = 551 and VDDREF = 1.2V
    }

    if (temperature_slope_calibration == 0 || adc_reference_value_calibration >= 0x8000)
    {
       temperature_slope_calibration = 0x412D; //Nominal value, for Temperature_Slope = 3.27 and VDDREF = 1.2V
    }

    temp = ((Int32)raw * adc_reference_value_calibration); //unsigned: 25 bits before comma + 0 bits behind the comma
    temp = temp >> 10;                                     //unsigned: 15 bits before comma + 0 bits behind the comma
    temp -= temperature_offset_calibration;                //signed: sign bit + 15 bits before + 0 bits behind the comma
    temp *= temperature_slope_calibration;                 //signed: sign bit + 30 bits before + 0 bits behind the comma =(implicitly /2^20)=> sign bit + 10 bits before + 20 bits behind the comma
    temp += ((UInt32)25 << 20);                            //signed: sign bit + 10 bits before + 20 bits behind the comma
    temp = temp >> 14;                                     //signed: sign bit + 10 bits before +  6 bits behind the comma

    //Avoid overflows in unsigned 16-bit (only needed for extreme calibration values, so could probably be left out).
    if(temp> (Int32)0x00007FFF)
    {
        temp = 0x00007FFF;
    }
    else if (temp < (Int32)0xFFFF8000)
    {
      temp = 0xFFFF8000;
    }

    return (Int16)temp; //with 6 bits behind the comma.
}

static UInt16 halADC_ConvertToFixedPointValue(UInt16 raw, UInt8 channel)
{
    if(channel == GP_WB_ENUM_ADC_CHANNEL_TEMP)
    {
        return halADC_ConvertToTemperature(raw);
    }
    return halADC_ConvertToVoltage(raw, channel);
}

static void halADC_WriteConfigToFreeChannel(UInt8 SlotConfig)
{
    GP_LOG_PRINTF("Add Slot %i",0,GP_WB_READ_ADCIF_NBR_OF_SLOTS_IN_CYCLE() + 1);

    switch(GP_WB_READ_ADCIF_NBR_OF_SLOTS_IN_CYCLE() + 1)
    {
        /* Slot 0 always Rssi */
        case 1:
        {
            GP_WB_WRITE_ADCIF_SLOT_B_CONFIG(SlotConfig);
            GP_WB_WRITE_ADCIF_NBR_OF_SLOTS_IN_CYCLE(1);
            break;
        }
        case 2:
        {
            GP_WB_WRITE_ADCIF_SLOT_C_CONFIG(SlotConfig);
            GP_WB_WRITE_ADCIF_NBR_OF_SLOTS_IN_CYCLE(2);
            break;
        }
        case 3:
        {
            GP_WB_WRITE_ADCIF_SLOT_D_CONFIG(SlotConfig);
            GP_WB_WRITE_ADCIF_NBR_OF_SLOTS_IN_CYCLE(3);
            break;
        }
        default:
        {
            GP_ASSERT_SYSTEM(false);
        }
    }
}

static void halADC_ReleaseChannel(UInt8 channel)
{
    UInt8 nrOfSlotsInUse = GP_WB_READ_ADCIF_NBR_OF_SLOTS_IN_CYCLE();
    UInt16 startAddr = 0x241; /* Slot A config addr */

    /* Skip slot A, always in use by Rssi */
    for(UIntLoop i = 1; i <= nrOfSlotsInUse; i++)
    {
        if (channel == GP_WB_GET_ADCIF_SLOT_A_CHANNEL_FROM_SLOT_A_CONFIG(GP_WB_READ_U8(startAddr + i)))
        {
            /* Clear Config */
            GP_WB_WRITE_U8(startAddr + i,0x00);

            /* If not the last one of the running slots,
               fill empty slot with last one from used slots */
            GP_LOG_PRINTF("Remove Slot %i",0,i);
            if (i != nrOfSlotsInUse)
            {
                UInt8 config;
                GP_LOG_PRINTF("Copy Config from %x to %x",0,(startAddr + nrOfSlotsInUse),(startAddr + i));
                config = GP_WB_READ_U8(startAddr + nrOfSlotsInUse);
                GP_WB_WRITE_U8(startAddr + i,config);
            }
            GP_WB_WRITE_ADCIF_NBR_OF_SLOTS_IN_CYCLE(nrOfSlotsInUse-1);
            return;
        }
    }
    /* Unknow slot given */
    GP_ASSERT_DEV_INT(false);
}

static void halADC_ConfigChannel(Bool anioRange3V6)
{
    if ( HAL_ADC_IS_ANIO_CHANNEL(bufferA_channel) ||
         HAL_ADC_IS_ANIO_CHANNEL(bufferB_channel) )
    {
        GP_WB_WRITE_ADCIF_ADC_INPUT_BUF_PUP(1);
        GP_WB_WRITE_ADCIF_ADC_VOLT_DIV_ENA(anioRange3V6);
    }
    else
    {
        GP_WB_WRITE_ADCIF_ADC_INPUT_BUF_PUP(0);
        GP_WB_WRITE_ADCIF_ADC_VOLT_DIV_ENA(0);
    }

    if ( (bufferA_channel == GP_WB_ENUM_ADC_CHANNEL_TEMP) ||
         (bufferB_channel == GP_WB_ENUM_ADC_CHANNEL_TEMP) )
    {
        GP_WB_WRITE_ADCIF_ADC_TEMP_SENSOR_PUP(1);
    }
    else
    {
        GP_WB_WRITE_ADCIF_ADC_TEMP_SENSOR_PUP(0);
    }
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void hal_InitADC(void)
{
    //Skip samples for stable conversion
    GP_LOG_PRINTF("Init ADC",0);

    bufferA_channel = HalADC_FreeBuffer;
    bufferB_channel = HalADC_FreeBuffer;

    GP_WB_WRITE_ADCIF_NBR_OF_REJECTED_CONVERSIONS(HAL_ADC_NBR_OF_REJECTED_CONVERSIONS);
}

UInt8 hal_GetContinuousADCMeasurement_8(UInt8 channel)
{
    UInt16 voltage = hal_GetContinuousADCMeasurement_16(channel);
    return HAL_ADC_CONVERT_TO_8BIT(voltage);
}


UInt16 hal_GetContinuousADCMeasurement_16(UInt8 channel)
{
    Bool timedOut = true;
    UInt16 raw;

    if (channel == bufferA_channel)
    {    
        GP_DO_WHILE_TIMEOUT(!GP_WB_READ_ADCIF_UNMASKED_BUFFER_A_UPDATED_INTERRUPT(),100,&timedOut);
        if(!timedOut)
        {
            GP_WB_ADCIF_CLR_BUFFER_A_UPDATED_INTERRUPT();
            raw = GP_WB_READ_ADCIF_BUFFER_A_RESULT();            
        }
    }
    else if (channel == bufferB_channel)
    {
        GP_DO_WHILE_TIMEOUT(!GP_WB_READ_ADCIF_UNMASKED_BUFFER_B_UPDATED_INTERRUPT(),100,&timedOut);
        if(!timedOut)
        {
            GP_WB_ADCIF_CLR_BUFFER_B_UPDATED_INTERRUPT();
            raw = GP_WB_READ_ADCIF_BUFFER_B_RESULT();
        }
    }

    if (!timedOut)
    {
        return halADC_ConvertToFixedPointValue(raw, channel);
    }
    else
    {
        /* Timeout or invalid slot */
        GP_ASSERT_DEV_INT(false);
        return 0;
    }
}

void hal_ClearContinuousADCMeasurement(UInt8 channel)
{
    if(channel == bufferA_channel)
    {
        GP_WB_ADCIF_CLR_BUFFER_A_UPDATED_INTERRUPT();
        if (GP_WB_READ_ADCIF_BUFFER_A_MAX_HOLD())
        {
            GP_WB_WRITE_ADCIF_BUFFER_PRESET_VALUE(0);
        }
        else
        {
            GP_WB_WRITE_ADCIF_BUFFER_PRESET_VALUE(0x3FF);
        }
        GP_WB_ADCIF_BUFFER_A_PRESET();
    }
    else if (channel == bufferB_channel)
    {
        GP_WB_ADCIF_CLR_BUFFER_B_UPDATED_INTERRUPT();
        if (GP_WB_READ_ADCIF_BUFFER_B_MAX_HOLD())
        {
            GP_WB_WRITE_ADCIF_BUFFER_PRESET_VALUE(0);
        }
        else
        {
            GP_WB_WRITE_ADCIF_BUFFER_PRESET_VALUE(0x3FF);
        }
        GP_WB_ADCIF_BUFFER_B_PRESET();
    }
}

void hal_StartContinuousADCMeasurement(UInt8 channel, Bool maxHold, Bool minHold, Bool anioRange3V6)
{
    UInt8 adcConfig = 0;

    GP_ASSERT_DEV_INT(!(maxHold && minHold));

    GP_WB_WRITE_ADCIF_TRIGGER_MODE(GP_WB_ENUM_ADC_TRIGGER_MODE_NEVER);

    GP_WB_SET_ADCIF_SLOT_A_CHANNEL_TO_SLOT_A_CONFIG(adcConfig,channel);
    if(minHold)
    {
        GP_WB_WRITE_ADCIF_BUFFER_PRESET_VALUE(0x3FF);
    }
    else //minHold
    {
        GP_WB_WRITE_ADCIF_BUFFER_PRESET_VALUE(0);
    }

    if(HalADC_FreeBuffer == bufferA_channel)
    {
        bufferA_channel = channel;
        GP_WB_SET_ADCIF_SLOT_A_POST_TO_BUFFER_A_TO_SLOT_A_CONFIG(adcConfig,true);
        GP_WB_WRITE_ADCIF_BUFFER_A_MAX_HOLD(maxHold);
        GP_WB_WRITE_ADCIF_BUFFER_A_MIN_HOLD(minHold);
        GP_WB_ADCIF_BUFFER_A_PRESET();
        GP_WB_ADCIF_CLR_BUFFER_A_UPDATED_INTERRUPT(); /* Clear Interrupt */
    }
    else if (HalADC_FreeBuffer == bufferB_channel)
    {
        bufferB_channel = channel;
        GP_WB_SET_ADCIF_SLOT_A_POST_TO_BUFFER_B_TO_SLOT_A_CONFIG(adcConfig,true);
        GP_WB_WRITE_ADCIF_BUFFER_B_MAX_HOLD(maxHold);
        GP_WB_WRITE_ADCIF_BUFFER_B_MIN_HOLD(minHold);
        GP_WB_ADCIF_BUFFER_B_PRESET();
        GP_WB_ADCIF_CLR_BUFFER_B_UPDATED_INTERRUPT(); /* Clear Interrupt */
    }
    else
    {
        GP_ASSERT_SYSTEM(false);
        return;
    }


    halADC_WriteConfigToFreeChannel(adcConfig);
    GP_WB_WRITE_ADCIF_TRIGGER_MODE(GP_WB_ENUM_ADC_TRIGGER_MODE_ALWAYS);
    //halADC_dumpConfig();

    halADC_ConfigChannel(anioRange3V6);
}

void hal_StopContinuousADCMeasurement(UInt8 channel)
{
    GP_WB_WRITE_ADCIF_TRIGGER_MODE(GP_WB_ENUM_ADC_TRIGGER_MODE_NEVER);
    if(channel == bufferA_channel)
    {
        bufferA_channel = HalADC_FreeBuffer;
    }
    else if (channel == bufferB_channel)
    {
        bufferB_channel = HalADC_FreeBuffer;
    }
    halADC_ReleaseChannel(channel);
    if (0 == GP_WB_READ_ADCIF_NBR_OF_SLOTS_IN_CYCLE())
    {
        GP_WB_WRITE_ADCIF_TRIGGER_MODE(GP_WB_ENUM_ADC_TRIGGER_MODE_WHEN_RSSI_REQ);
    }
    else
    {
        GP_WB_WRITE_ADCIF_TRIGGER_MODE(GP_WB_ENUM_ADC_TRIGGER_MODE_ALWAYS);
    }
}

UInt8 hal_MeasureADC_8(UInt8 channel, UInt8 measurementConfig)
{
    UInt16 voltage = hal_MeasureADC_16(channel, measurementConfig);
    return HAL_ADC_CONVERT_TO_8BIT(voltage);
}


UInt16 hal_MeasureADC_16(UInt8 channel, UInt8 measurementConfig)
{
    UInt16 voltage = 0;
    NOT_USED(measurementConfig);
    hal_StartContinuousADCMeasurement(channel, HAL_DISABLE_HOLD_MAX, HAL_DISABLE_HOLD_MIN, HAL_ENABLE_3V6);
    voltage = hal_GetContinuousADCMeasurement_16(channel);
    hal_StopContinuousADCMeasurement(channel);
    return voltage;
}

