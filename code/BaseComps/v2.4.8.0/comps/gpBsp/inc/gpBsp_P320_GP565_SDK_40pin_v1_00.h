
/*
 *
 *   GP565 SDK 40 pin
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpBsp/inc/gpBsp_P320_GP565_SDK_40pin_v1_00.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */
 
 
#ifndef _GPBSP_P320_GP565_SDK_40PIN_V1_00_H_
#define _GPBSP_P320_GP565_SDK_40PIN_V1_00_H_

/*****************************************************************************
 *                    BSP configuration interface
 *****************************************************************************/

#define GP_BSP_RESET_PIN_USED() 0
// Is a 32kHz crystal mounted?
#define GP_BSP_32KHZ_CRYSTAL_AVAILABLE() 0
// Is the watchdog timer used?
#define GP_BSP_USE_WDT_TIMER() 1

/*****************************************************************************
 *                    GPIO - LED
 *****************************************************************************/

#include "gpHal_kx_regprop.h"
#include "gpHal_kx_MSI_basic.h"
#include "gpHal_kx_enum.h"

#define HAL_LED_ACTIVE_LOW

#ifdef GP_DIVERSITY_XSIF_DEBUG_ENABLED
#define GRN 1 // GPIO21 - LED Active when low
#define HAL_LED_GRN_MAX_OUTPUT  255
#define HAL_LED_GRN_FADE_IN_UNIT     11 /* 50 ms */ 
#define HAL_LED_GRN_FADE_OUT_UNIT     11 /* 50 ms */ 
#define RED 0 // GPIO20 - LED Active when low
#define HAL_LED_RED_MAX_OUTPUT  255

#define HAL_LED_SET_GRN() do {  GP_WB_WRITE_LED_ENABLE_1(1); } while(false)
#define HAL_LED_CLR_GRN() do {  GP_WB_WRITE_LED_ENABLE_1(0); } while(false)
#define HAL_LED_TST_GRN() GP_WB_READ_LED_ENABLE_1()
#define HAL_LED_TGL_GRN() do { if(HAL_LED_TST_GRN()) { HAL_LED_CLR_GRN(); } else { HAL_LED_SET_GRN(); }; } while(false)

#define HAL_LED_SET_RED() do {  GP_WB_WRITE_LED_ENABLE_0(1); } while(false)
#define HAL_LED_CLR_RED() do {  GP_WB_WRITE_LED_ENABLE_0(0); } while(false)
#define HAL_LED_TST_RED() GP_WB_READ_LED_ENABLE_0()
#define HAL_LED_TGL_RED() do { if(HAL_LED_TST_RED()) { HAL_LED_CLR_RED(); } else { HAL_LED_SET_RED(); }; } while(false)

#define HAL_LED_INIT_LEDS() do { \
    /*Initialize LED driver block 2 for GPIO 20*/ \
    GP_WB_WRITE_LED_THRESHOLD_0(HAL_LED_RED_MAX_OUTPUT); \
    GP_WB_WRITE_LED_LED0_OUTPUT_PINMAP(GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_E); \
    GP_WB_WRITE_LED_LED0_OUTPUT_INVERT(1); \
    GP_WB_READ_IOB_GPIO_20_23_DRIVE_STRENGTH(); \
    HAL_LED_CLR(RED); \
    /*Initialize LED driver block 3 for GPIO 21*/ \
    GP_WB_WRITE_LED_THRESHOLD_1(HAL_LED_GRN_MAX_OUTPUT); \
    GP_WB_WRITE_LED_LED1_OUTPUT_PINMAP(GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_E); \
    GP_WB_WRITE_LED_LED1_OUTPUT_INVERT(1); \
    GP_WB_READ_IOB_GPIO_20_23_DRIVE_STRENGTH(); \
    HAL_LED_CLR(GRN); \
} while(false)

#else

#define GRN 2 // GPIO17 - LED Active when low
#define HAL_LED_GRN_MAX_OUTPUT  255
#define HAL_LED_GRN_FADE_IN_UNIT     11 /* 50 ms */ 
#define HAL_LED_GRN_FADE_OUT_UNIT     11 /* 50 ms */ 
#define RED 3 // GPIO16 - LED Active when low
#define HAL_LED_RED_MAX_OUTPUT  255

#define HAL_LED_SET_RED() do {  GP_WB_WRITE_LED_ENABLE_3(1); } while(false)
#define HAL_LED_CLR_RED() do {  GP_WB_WRITE_LED_ENABLE_3(0); } while(false)
#define HAL_LED_TST_RED() GP_WB_READ_LED_ENABLE_3()
#define HAL_LED_TGL_RED() do { if(HAL_LED_TST_GRN()) { HAL_LED_CLR_GRN(); } else { HAL_LED_SET_GRN(); }; } while(false)

#define HAL_LED_SET_GRN() do {  GP_WB_WRITE_LED_ENABLE_2(1); } while(false)
#define HAL_LED_CLR_GRN() do {  GP_WB_WRITE_LED_ENABLE_2(0); } while(false)
#define HAL_LED_TST_GRN() GP_WB_READ_LED_ENABLE_2()
#define HAL_LED_TGL_GRN() do { if(HAL_LED_TST_RED()) { HAL_LED_CLR_RED(); } else { HAL_LED_SET_RED(); }; } while(false)

#define HAL_LED_INIT_LEDS() do { \
    /*Initialize LED driver block 2 for GPIO 16*/ \
    GP_WB_WRITE_LED_THRESHOLD_2(HAL_LED_RED_MAX_OUTPUT); \
    GP_WB_WRITE_LED_LED2_OUTPUT_PINMAP(GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_B); \
    GP_WB_WRITE_LED_LED2_OUTPUT_INVERT(1); \
    GP_WB_READ_IOB_GPIO_0_3_DRIVE_STRENGTH(); \
    HAL_LED_CLR(GRN); \
    /*Initialize LED driver block 3 for GPIO 17*/ \
    GP_WB_WRITE_LED_THRESHOLD_3(HAL_LED_GRN_MAX_OUTPUT); \
    GP_WB_WRITE_LED_LED3_OUTPUT_PINMAP(GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_B); \
    GP_WB_WRITE_LED_LED3_OUTPUT_INVERT(1); \
    GP_WB_READ_IOB_GPIO_0_3_DRIVE_STRENGTH(); \
    HAL_LED_CLR(RED); \
} while(false)

#endif
/*****************************************************************************
 *                    GPIO - BTN - No buttons used
 *****************************************************************************/

#define HAL_BTN_INIT_BTNS() do {  \
} while(false)

/*****************************************************************************
 *                    GPIO - ALTERNATIVE - not used
 *****************************************************************************/


/*****************************************************************************
 *                    UART
 *****************************************************************************/

#ifdef GP_DIVERSITY_XSIF_DEBUG_ENABLED
#define HAL_UART_MAPPING_TX           GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_C //Mapping C - pin 21 - GPIO0
#define UART_TX_GPIO                  0 //GPIO0

#ifndef GP_BSP_UART_COM_BAUDRATE
#define GP_BSP_UART_COM_BAUDRATE    57600
#endif //GP_BSP_UART_COM_BAUDRATE

#ifndef GP_BSP_UART_SCOM_BAUDRATE
#define GP_BSP_UART_SCOM_BAUDRATE   57600
#endif //GP_BSP_UART_SCOM_BAUDRATE
#endif

/*****************************************************************************
 *                    XSIF
 *****************************************************************************/
#ifdef GP_DIVERSITY_XSIF_DEBUG_ENABLED
#define HAL_XSIF_ENABLED

#define HAL_SIF_MAPPING_MISO           GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_A//Mapping D - pin 31 - GPIO10
#define HAL_SIF_MAPPING_MOSI           GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_A //Mapping D - pin 32 - GPIO11
#define HAL_SIF_MAPPING_SCLK           GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_A //Mapping D - pin 33 - GPIO12
#define HAL_SIF_MAPPING_LOADB           GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_A //Mapping D - pin 34 - GPIO13
#endif
/*****************************************************************************
 *                    IR
 *****************************************************************************/

#define HAL_IR_ACTIVE_LOW
#define HAL_IR_MAPPING           GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_A //Mapping A - pin 39 - GPIO18
#define IR_GPIO                  18 //GPIO18

/*****************************************************************************
 *                    KEY
 *****************************************************************************/

//Rows 
// Pin 31 - KEY_OUT2_a - GPIO10 
// Pin 32 - KEY_OUT3_a - GPIO11 
// Pin 33 - KEY_OUT4_a - GPIO12 
// Pin 34 - KEY_OUT5_a - GPIO13 
// Pin 35 - KEY_OUT6_a - GPIO14 
// Pin 36 - KEY_OUT7_a - GPIO15 
//Columns 
// Pin 22 - KEY_IN1_a - GPIO1 
// Pin 23 - KEY_IN2_a - GPIO2 
// Pin 24 - KEY_IN3_a - GPIO3 
// Pin 25 - KEY_IN4_a - GPIO4 
// Pin 26 - KEY_IN5_a - GPIO5 

/*****************************************************************************
 *                    GPIO - Unused
 *****************************************************************************/

#ifdef GP_DIVERSITY_XSIF_DEBUG_ENABLED
#define HAL_UNUSED_INIT() do { \
    /* Pull down unused pins*/ \
    GP_WB_WRITE_IOB_GPIO_19_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_22_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_23_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_24_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_25_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_26_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_27_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_28_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_29_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
} while(false)
#else
#define HAL_UNUSED_INIT() do { \
    /* Pull down unused pins*/ \
    GP_WB_WRITE_IOB_GPIO_0_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_6_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_14_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_15_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_19_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_20_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_21_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_22_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_23_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_24_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_25_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_26_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_27_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_28_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_29_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
} while(false)
#endif
/*****************************************************************************
 *                    IO Pending
 *****************************************************************************/

#define HAL_BSP_IO_ACTIVITY_PENDING()                                  \
(                                                                      \
    GP_WB_READ_IR_BUSY()                                               \
)

/*****************************************************************************
 *                    Generic
 *****************************************************************************/

#ifdef GP_DIVERSITY_XSIF_DEBUG_ENABLED
#define HAL_BSP_INIT() do{ \
    /*Disable bus keeper/PU/PD on UART_TX*/ \
    GP_WB_WRITE_IOB_GPIO_0_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT); \
    /*SIF MISO mapping*/ \
    GP_WB_WRITE_IOB_GPIO_15_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_XAP5_GP_SIF_MISO_MAPPING(HAL_SIF_MAPPING_MISO); \
    /*SIF MOSI mapping*/ \
    GP_WB_WRITE_IOB_GPIO_16_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_XAP5_GP_SIF_MOSI_MAPPING(HAL_SIF_MAPPING_MOSI); \
    /*SIF SCLK mapping*/ \
    GP_WB_WRITE_IOB_GPIO_17_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_XAP5_GP_SIF_CLK_MAPPING(HAL_SIF_MAPPING_SCLK); \
    /*SIF LOADB mapping*/ \
    GP_WB_WRITE_IOB_GPIO_14_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP); \
    GP_WB_WRITE_XAP5_GP_SIF_LOADB_MAPPING(HAL_SIF_MAPPING_LOADB); \
    GP_WB_WRITE_IOB_GPIO_21_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP); \
} while(false)
#else
#define HAL_BSP_INIT() do{ \
    /*Disable bus keeper/PU/PD on UART_TX*/ \
    GP_WB_WRITE_IOB_GPIO_0_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT); \
    GP_WB_WRITE_IOB_GPIO_21_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP); \
} while(false)
#endif
#define HAL_BSP_INIT_SLEEP_IO_CONFIG() do{ \
} while(false)

#endif //_GPBSP_P320_GP565_SDK_40PIN_V1_00_H_

