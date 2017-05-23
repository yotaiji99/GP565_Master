/*
 * Copyright (c) 2008-2014, GreenPeak Technologies
 *
 * gpHal_MISC.h
 *
 * This file contains miscellaneous functions for GPIO and OTP functionality
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/inc/gpHal_MISC.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _HAL_GP_MISC_H_
#define _HAL_GP_MISC_H_

/** @file gpHal_MISC.h
 *  @brief This file contains miscellaneous functions for GPIO and OTP functionality
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gp_global.h"
#include "gpHal_reg.h"

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/


/** @name gpHal_ADCSource_t */
//@{
/** @brief The ADC reference voltage is selected as input for the ADC block. The measurement will return the value corresponding to this reference value. */
#define gpHal_ADCSourceReference    GPHAL_ENUM_ADCIF_CHANNEL_VREF
/** @brief The external voltage applied on ANIO3 is selected as input for the ADC block. The measurement will return the converted value.*/
#define gpHal_ADCSourceExternal     GPHAL_ENUM_ADCIF_CHANNEL_ANIO3
/** @brief The internal battery voltage is selected as input for the ADC block. The measurement will return the converted value.*/
#define gpHal_ADCSourceBattery      GPHAL_ENUM_ADCIF_CHANNEL_VBATT

/** @typedef gpHal_ADCSource_t
 *  @brief The gpHal_ADCSource_t type defines the source for an ADC measurement.
*/
typedef UInt8 gpHal_ADCSource_t;
//@}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

//Register functions for callback

// GPIO functions

/**
 * @brief Sets the direction of the GPIO pins.
 *
 * To put f.i. GPIO3 in output a 0x4 must be used as parameter for this function.
 * To make the GPIO pins input a zero must be written.
 * @param direction Sets pins 1..7 on in or output.
 */
#define gpHal_SetGPIODirection(direction)          GP_HAL_SET_GPIO_DIRECTION(direction)
/**
 * @brief Gets the current direction settings of the GPIO pins.
 */
#define gpHal_GetGPIODirection()                   GP_HAL_GET_GPIO_DIRECTION()
/**
 * @brief Sets the selected GPIO pins to their alternative output.
 *
 * This function sets the selected GPIO pins to their alternative output.
 * @param output Sets pins 1..7 to alternative or normal output.
 */
#define gpHal_SetGPIOAlternativeOutput(output)      GP_HAL_SET_GPIO_ALTERNATIVEOUTPUT(output)
/**
 * @brief Gets the current alternative output settings of the GPIO pins.
 */
#define gpHal_GetGPIOAlternativeOutput()            GP_HAL_GET_GPIO_ALTERNATIVEOUTPUT()
/**
 * @brief Gets the current input value at the GPIO pins.
 */
#define gpHal_GetGPIOInput()                        GP_HAL_GET_GPIO_INPUT()

/**
 * @brief Sets an output value to the GPIO pins.
 *
 * This function sets an output value to the GPIO pins.
 * @param output the bits 1..7 to be put on GPIO 1..7.
 */
#define gpHal_SetGPIOOutputs(output)                GP_HAL_SET_GPIO_OUTPUTS(output)

/**
 * @brief Converts the analog signal at the ANIO1 pin of the GP chip to a digital value.
 *
 * This function returns a 8-bit value according to the analog signal applied at the ANIO1 pin.
 * The internal reference voltage can also be measured using the gpHal_ADCSourceReference enumeration.
 *
 * @param source enumeration that selects the analog input source for the ADC.
 * @param measurementConfig bitfield specifying the settings of the adc measurement: bit[1:0] indicating the voltage range; bit[6] indicating the  XXX; bit[7] indicating a optimized measurement; bit[5:2] reserved.
 */
GP_API UInt8 gpHal_GetADCValue(gpHal_ADCSource_t source, UInt8 measurementConfig);

/**
 * @brief Returns a 8-bit random value.
 *
 * This function returns a 8-bit random value using samples of the GP chip radio I and Q signals.
 */
GP_API UInt8 gpHal_GetRandomSeed(void);

#define gpHal_GetAttrResultDataError         0x8000
#define gpHal_GetAttrResultUnkownAttr        0x8001
#define gpHal_GetAttrResultBufferTooSmall    0x8002
#define gpHal_GetAttrResultBufferTooBig      0x8003

typedef Int16 gpHal_GetAttrResult_t;

/**
 * @brief Parser function for attributes.
 *
 * This function accesses the GreenPeak intrenal memory data.
 */
GP_API gpHal_GetAttrResult_t gpHal_ParseAttr(UInt8 attrId, Int8 length, UInt8 *pData);

#endif //_HAL_GP_MISC_H_


