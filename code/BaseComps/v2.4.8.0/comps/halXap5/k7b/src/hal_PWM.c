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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.3.1.2/comps/halXap5/k7b/src/hal_PWM.c#1 $
 *    M'   GreenPeak              $Change: 67061 $
 *   0'         Technologies      $DateTime: 2015/07/02 08:59:25 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_HALXAP5

#include "hal.h"
#include "gpBsp.h"
#include "gpLog.h"
#include "gpHal.h"
#include <stdlib.h> //abs


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#ifndef HAL_PWM0_MAPPING
#define HAL_PWM0_MAPPING GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED
#endif //HAL_PWM0_MAPPING

#ifndef HAL_PWM1_MAPPING
#define HAL_PWM1_MAPPING GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED
#endif //HAL_PWM1_MAPPING

#ifndef HAL_PWM2_MAPPING
#define HAL_PWM2_MAPPING GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_D
#endif //HAL_PWM2_MAPPING

#ifndef HAL_PWM3_MAPPING
#define HAL_PWM3_MAPPING GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED
#endif //HAL_PWM3_MAPPING

#ifndef HAL_PWM0_DRIVE
#define HAL_PWM0_DRIVE GP_WB_ENUM_GENERIC_IO_DRIVE_PUSH_PULL
#endif //HAL_PWM0_DRIVE

#ifndef HAL_PWM1_DRIVE
#define HAL_PWM1_DRIVE GP_WB_ENUM_GENERIC_IO_DRIVE_PUSH_PULL
#endif //HAL_PWM1_DRIVE

#ifndef HAL_PWM2_DRIVE
#define HAL_PWM2_DRIVE GP_WB_ENUM_GENERIC_IO_DRIVE_PUSH_PULL
#endif //HAL_PWM2_DRIVE

#ifndef HAL_PWM3_DRIVE
#define HAL_PWM3_DRIVE GP_WB_ENUM_GENERIC_IO_DRIVE_PUSH_PULL
#endif //HAL_PWM3_DRIVE

#define HAL_PWM_NR_OF_PWM_CHANNELS  4
#define HAL_PWM_MAX_DUTY_CYCLE      100

#define HAL_PWM_PRESCALER_FREQUENCY             32000000UL
#define HAL_PWM_FREQUENCY_250_HZ    500

#define HAL_PWM_FREQUENCY_TO_TICKS(frequency)                           ((HAL_PWM_PRESCALER_FREQUENCY) / (frequency))
#define HAL_PWM_DUTY_CYCLE_TO_THRESHOLD(duty_cycle, frequency)  ((duty_cycle)*HAL_PWM_FREQUENCY_TO_TICKS(frequency)/HAL_PWM_MAX_DUTY_CYCLE)
#define HAL_PWM_THRESHOLD_TO_UNBALANCED(threshold,mode)                 (threshold << mode)
#define HAL_PWM_THRESHOLD_TO_BALANCED(threshold,mode)                   (threshold >> mode)

// in case of gradual transition, this is the increment we are using for each update
#define HAL_PWM_DUTY_CYCLE_GRADUAL_STEP_INTERVAL    100

#define HAL_PWM_DEFAULT_COUNTER_TICKS(frequency)         HAL_PWM_FREQUENCY_TO_TICKS(frequency)

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data
 *****************************************************************************/
static UInt8 defaultChannelMappings[HAL_PWM_NR_OF_PWM_CHANNELS];
/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static UInt16 hal_getThreshold(UInt8 channel, Bool upDown);
static void hal_updateThreshold(UInt8 channel, UInt16 threshold);
static UInt8 hal_getOutputPinmap(UInt8 channel);
static void hal_updateOutputPinmap(UInt8 channel, UInt8 pinmapValue);
static UInt8 hal_getOutputInverted(UInt8 channel);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

UInt16 hal_getThreshold(UInt8 channel, Bool currentMode)
{
    UInt8 channelOffset;
    UInt16 threshold;

    GP_ASSERT_DEV_INT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);

    channelOffset = channel * GP_WB_PWM_PWM0_THRESHOLD_LEN;
    threshold = GP_WB_READ_U16(GP_WB_PWM_PWM0_THRESHOLD_ADDRESS + channelOffset);

    return HAL_PWM_THRESHOLD_TO_UNBALANCED(threshold, currentMode);
}

void hal_updateThreshold(UInt8 channel, UInt16 threshold)
{
    UInt16 converted_threshold;
    UInt8 channelOffset;

    GP_ASSERT_DEV_INT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);
    GP_ASSERT_DEV_INT(threshold <= GP_WB_READ_PWM_MAIN_COUNTER_WRAP_VALUE());

    // in case of balanced mode, apply correction factor to threshold
    converted_threshold = HAL_PWM_THRESHOLD_TO_BALANCED(threshold, GP_WB_READ_PWM_UP_DOWN_ENABLE());
    channelOffset = GP_WB_PWM_PWM0_THRESHOLD_UPDATE_LSB + channel;

    // write last piece (when pwm is enabled) or write final threshold once (pwm disabled)
    GP_WB_WRITE_PWM_NEXT_THRESHOLD(converted_threshold);
    GP_WB_WRITE_U8(GP_WB_PWM_PWM0_THRESHOLD_UPDATE_ADDRESS, 1 << channelOffset);
}

UInt8 hal_getOutputPinmap(UInt8 channel)
{
    UInt8 channelOffset;
    UInt8 registerOffset;

    channelOffset = channel/2;
    registerOffset = (channel & 0x01)*4;

    return (GP_WB_READ_U8(GP_WB_PWM_PWM0_OUTPUT_PINMAP_ADDRESS + GP_WB_PWM_PWM0_OUTPUT_PINMAP_LSB/8 + channelOffset) >> registerOffset) & 0x07;
}

// fixme: this should be simplified when appropriate macros for writing properties are available
void hal_updateOutputPinmap(UInt8 channel, UInt8 pinmapValue)
{
    UInt8 channelOffset;
    UInt8 registerOffset;
    UInt8 channelMapping;

    channelOffset = channel/2;
    registerOffset = (channel & 0x01)*4;
    channelMapping = pinmapValue << registerOffset;

    GP_WB_MWRITE_U8(GP_WB_PWM_PWM0_OUTPUT_PINMAP_ADDRESS + GP_WB_PWM_PWM0_OUTPUT_PINMAP_LSB/8 + channelOffset, 0x07 << registerOffset , channelMapping);
}

UInt8 hal_getOutputInverted(UInt8 channel)
{
    return GP_WB_READ_U1(GP_WB_PWM_PWM0_OUTPUT_INVERT_ADDRESS, (GP_WB_PWM_PWM0_OUTPUT_INVERT_LSB + channel));
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void hal_InitPWM(void)
{
    UInt8 i;

    // init default channel mapping structure
//    defaultChannelMappings[0] = HAL_PWM0_MAPPING;
//    defaultChannelMappings[1] = HAL_PWM1_MAPPING;
    defaultChannelMappings[2] = HAL_PWM2_MAPPING;
//    defaultChannelMappings[3] = HAL_PWM3_MAPPING;

    for(i = 0; i < HAL_PWM_NR_OF_PWM_CHANNELS; i++)
    {
        hal_updateOutputPinmap(i, defaultChannelMappings[i]);
        hal_updateThreshold(i,0);
    }

    // set drive
///    GP_WB_WRITE_PWM_PWM0_OUTPUT_DRIVE(HAL_PWM0_DRIVE);
///    GP_WB_WRITE_PWM_PWM1_OUTPUT_DRIVE(HAL_PWM1_DRIVE);
    GP_WB_WRITE_PWM_PWM2_OUTPUT_DRIVE(HAL_PWM2_DRIVE);
///    GP_WB_WRITE_PWM_PWM3_OUTPUT_DRIVE(HAL_PWM3_DRIVE);

    // set frequency
    GP_WB_WRITE_PWM_MAIN_COUNTER_WRAP_VALUE(HAL_PWM_DEFAULT_COUNTER_TICKS(250));
}

void hal_EnablePwm(Bool enable)
{
    GP_WB_WRITE_PWM_PRESCALER_ENABLE(enable);
    if(!enable)
    {
        // reset the counter (start from clean state when block is re-enabled)
        GP_WB_WRITE_PWM_NEXT_MAIN_COUNTER(0);
        GP_WB_PWM_MAIN_COUNTER_UPDATE();
    }
}

void hal_SetChannelEnabled(UInt8 channel, Bool enabled)
{
    GP_ASSERT_DEV_EXT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);
    hal_updateOutputPinmap(channel, enabled ? defaultChannelMappings[channel]: GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED);
}

void hal_InvertOutput(UInt8 channel, Bool invert)
{
    GP_ASSERT_DEV_EXT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);

    GP_WB_MWRITE_U1(GP_WB_PWM_PWM0_OUTPUT_INVERT_ADDRESS, GP_WB_PWM_PWM0_OUTPUT_INVERT_LSB + channel, invert);
}

void hal_SetDutyCycle(UInt8 channel, UInt16 dutyCycle)
{
    GP_ASSERT_DEV_EXT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);
    GP_ASSERT_DEV_EXT(dutyCycle <= GP_WB_READ_PWM_MAIN_COUNTER_WRAP_VALUE());

    hal_updateThreshold(channel, dutyCycle);
}

void hal_SetBalancedMode(Bool enable)
{
    UInt8 i;
    UInt8 nrOfChannelsEnabled = 0;
    UInt8 channelsEnabled[2];
    Bool currentMode = GP_WB_READ_PWM_UP_DOWN_ENABLE();

    for(i = 0; i < HAL_PWM_NR_OF_PWM_CHANNELS; i++)
    {
        if(hal_getOutputPinmap(i) != GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED)
        {
            channelsEnabled[nrOfChannelsEnabled] = i;
            nrOfChannelsEnabled++;
        }
    }
    if(nrOfChannelsEnabled != 2)
    {
        GP_LOG_SYSTEM_PRINTF("Balanced mode only possible with 2 PWMs active",0);
    }
    else
    {
        if(hal_getOutputInverted(channelsEnabled[0]) != enable)
        {
            GP_WB_WRITE_PWM_UP_DOWN_ENABLE(enable);
            // switching mode means updating thresholds
            hal_updateThreshold(channelsEnabled[0], GP_WB_READ_PWM_MAIN_COUNTER_WRAP_VALUE() - hal_getThreshold(channelsEnabled[0], currentMode));
            hal_updateThreshold(channelsEnabled[1], hal_getThreshold(channelsEnabled[1], currentMode));
            hal_InvertOutput(channelsEnabled[0],enable);
        }
    }
}

void hal_SetFrequencyCorrectionFactor(UInt8 factor, UInt16 frequency)
{
    UInt16 currentMainCounter = GP_WB_READ_PWM_MAIN_COUNTER_WRAP_VALUE()/frequency;
    UInt16 newMainCounter = (HAL_PWM_DEFAULT_COUNTER_TICKS(frequency) >> factor)/frequency;
    UInt8 correctionFactorNew=0;
    UInt8 correctionFactorCurrent=0;
    UInt8 i;

    Bool currentMode = GP_WB_READ_PWM_UP_DOWN_ENABLE();

    GP_WB_WRITE_PWM_MAIN_COUNTER_WRAP_VALUE(HAL_PWM_DEFAULT_COUNTER_TICKS(frequency) >> factor);

    while(currentMainCounter > 0)
    {
        // higher frequency ==> lower threshold
        correctionFactorCurrent++;
        currentMainCounter >>= 1;
    }

    while(newMainCounter > 0)
    {
        // lower frequency ==> higher threshold
        correctionFactorNew++;
        newMainCounter >>= 1;
    }

    // first, convert threshold to base frequency (250 Hz), next convert them to new frequency
    for(i = 0; i < HAL_PWM_NR_OF_PWM_CHANNELS; i++)
    {
        // convert threshold to base units
        hal_updateThreshold(i, hal_getThreshold(i, currentMode) >> correctionFactorCurrent);
        // convert threshold to new unit
        hal_updateThreshold(i, hal_getThreshold(i, currentMode) << correctionFactorNew);
    }
}

void hal_SetPrescalerCounterWrapPower(UInt8 val)
{
    GP_WB_WRITE_PWM_PRESCALER_COUNTER_WRAP_POWER(val);
}

void hal_ConfigPWM( UInt8 counterResolution, UInt8 carrier, Bool wordmode, Bool thresholdupdate )
{
    GP_WB_WRITE_PWM_NEXT_THRESHOLD(0x1ff);
    GP_WB_WRITE_U8(GP_WB_PWM_PWM2_THRESHOLD_UPDATE_ADDRESS, 0xff);

    // Disable counters during initialisation
    GP_WB_WRITE_PWM_PRESCALER_ENABLE(0);

    // setup prescaler so we get 16 MHz prescale clock
    GP_WB_WRITE_PWM_PRESCALER_COUNTER_WRAP_POWER(0);

    // setup main counter to get 8 bits of resolution
    GP_WB_WRITE_PWM_MAIN_COUNTER_WRAP_VALUE(counterResolution);  // 255

    // Now set the carrier to get the desired update ratio
    GP_WB_WRITE_PWM_CARRIER_COUNTER_WRAP_POWER(carrier);  // for ~8KHz update // 3

    // Now reset counters
    GP_WB_WRITE_PWM_NEXT_MAIN_COUNTER(0);
    GP_WB_PWM_MAIN_COUNTER_UPDATE();
    GP_WB_PWM_CARRIER_COUNTER_RESET();

    // set fifo size
    GP_WB_WRITE_PWM_NEXT_THRESHOLD_FIFO_SIZE(wordmode); // byte mode - 0

    // configure the threshold updating
    GP_WB_WRITE_PWM_AUTO_SHIFT_THRESHOLD_UPDATE_ON_CARRIER_COUNTER_WRAP(thresholdupdate); // false
    // And kick off the counters
    GP_WB_WRITE_PWM_PRESCALER_ENABLE(1);
}
