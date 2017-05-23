/*
 * Copyright (c) 2008-2014, GreenPeak Technologies
 *
 * hal.c
 *   Hardware Abstraction Layer for XAP5 devices.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/halXap5/inc/hal.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _HAL_H_
#define _HAL_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include <machine/xap.h>
#include "gpBsp.h"
#ifdef HAL_IR_RT_CONFIG
#include "hal_ExtendedIr.h"
#endif /* HAL_IR_RT_CONFIG */
/*****************************************************************************
 *                    Function and Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    STACK DEBUG
 *****************************************************************************/

UInt8* hal_GetStackStartAddress(void);
UInt8* hal_GetStackEndAddress(void);

#define HAL_STACK_LOW_TO_HIGH  0
#define HAL_STACK_START_ADDRESS     hal_GetStackStartAddress()
#define HAL_STACK_END_ADDRESS       hal_GetStackEndAddress()

/*****************************************************************************
 *                    INTERRUPTS
 *****************************************************************************/

#define HAL_LONG_INTERRUPT_TIME_IN_US      (750)

/*****************************************************************************
 *                    CLK frequency
 *****************************************************************************/


/*****************************************************************************
 *                    GPIO
 *****************************************************************************/
typedef struct
{
    UInt8 mask;
    UInt8 regoffset;
} hal_gpiodsc_t;
extern const hal_gpiodsc_t gpios[];

Bool hal_gpioGetSetClr(hal_gpiodsc_t gpio);
void hal_gpioSet(hal_gpiodsc_t gpio);
void hal_gpioClr(hal_gpiodsc_t gpio);
Bool hal_gpioGet(hal_gpiodsc_t gpio);

void hal_gpioModePP(hal_gpiodsc_t gpio, Bool enable);
Bool hal_gpioGetModePP(hal_gpiodsc_t gpio);
void hal_gpioModePU(UInt8 gpio, Bool enable);

/*****************************************************************************
 *                    Timer
 *****************************************************************************/

void hal_Waitus(UInt16 us);
void hal_Waitms(UInt16 ms);

#define HAL_WAIT_US hal_Waitus
#define HAL_WAIT_MS hal_Waitms

/*****************************************************************************
 *                    DEBUG
 *****************************************************************************/
#ifdef HAL_DEBUG_SIF_AVAILABLE
void hal_EnableDebugMode(void);
void hal_DisableDebugMode(void);

#define HAL_ENABLE_DEBUG_MODE()     hal_EnableDebugMode()
#define HAL_DISABLE_DEBUG_MODE()    hal_DisableDebugMode()
#else
#define HAL_ENABLE_DEBUG_MODE()     do {} while(false)
#define HAL_DISABLE_DEBUG_MODE()    do {} while(false)
#endif //HAL_DEBUG_SIF_AVAILABLE

/*****************************************************************************
 *                    LED
 *****************************************************************************/

#ifndef GP_DIVERSITY_NO_LED

#define HAL_LED_INIT() HAL_LED_INIT_LEDS()

//Led is driven by open drain GPIO - set and clr by driving 0 or acting as input
//Assume GPIO output value is initialized @ zero
#define HAL_LED_SET(x)   HAL_LED_SET_##x()
#define HAL_LED_CLR(x)   HAL_LED_CLR_##x()
#define HAL_LED_TST(x)   HAL_LED_TST_##x()
#define HAL_LED_TGL(x)   HAL_LED_TGL_##x()

#define HAL_USERLED_INIT()

#define HAL_USERLED_SET(x)
#define HAL_USERLED_CLR(x)
#define HAL_USERLED_TST(x)
#define HAL_USERLED_TGL(x)

#else

#define HAL_LED_INIT()
#define HAL_LED_CLR(led)
#define HAL_LED_SET(led)
#define HAL_LED_TST(led)            false
#define HAL_LED_TGL(led)

#define HAL_USERLED_INIT()

#define HAL_USERLED_SET(x)
#define HAL_USERLED_CLR(x)
#define HAL_USERLED_TST(x)
#define HAL_USERLED_TGL(x)

#endif //GP_DIVERSITY_NO_LED

/*****************************************************************************
 *                    Button
 *****************************************************************************/


#define HAL_BTN_INIT()        HAL_BTN_INIT_BTNS()
#define HAL_BTN_PRESSED(btn)  (BUTTON_##btn)


/*****************************************************************************
 *                    Init Function
 *****************************************************************************/

void hal_Init (void);

#define HAL_INIT()  hal_Init()


/*****************************************************************************
 *                    Atomic
 *****************************************************************************/

void hal__AtomicOff (void);
void hal__AtomicOn (void);

extern UInt8 l_n_atomic;

#define HAL_FORCE_ENABLE_GLOBAL_INT()   { l_n_atomic = 0; xap_irqe(); } /* Reset counter - starting anew */
#define HAL_FORCE_DISABLE_GLOBAL_INT()  xap_irqd()
#define HAL_INITIALIZE_GLOBAL_INT()     { HAL_FORCE_DISABLE_GLOBAL_INT(); l_n_atomic = 1; }

#define HAL_ENABLE_GLOBAL_INT()        hal__AtomicOff()
#define HAL_DISABLE_GLOBAL_INT()       hal__AtomicOn()

#define HAL_GLOBAL_INT_ENABLED()        (l_n_atomic == 0)

/*****************************************************************************
 *                    RESET UC
 *****************************************************************************/

void hal_Reset(void) __attribute__((noreturn));
#define HAL_RESET_UC( )  hal_Reset()

/*****************************************************************************
 *                    SLEEP
 *****************************************************************************/
//static inline void hal_EnableGotoSleep(void);
//static inline void hal_DisableGotoSleep(void);
void hal_sleep_uc(UInt32 timeToSleep);
void hal_IntHandlerPrologue(void); //not part of uchal but used by gpHal, gpKeyScan

#define HAL_SLEEP_INDEFINITE_SLEEP_TIME   0xFFFFFFFF
#define HAL_SLEEP_MAX_SLEEP_TIME         (0x7FFFFFFF/32UL)

#define HAL_ENABLE_SLEEP_UC()   hal_EnableGotoSleep()
#define HAL_DISABLE_SLEEP_UC()  hal_DisableGotoSleep()
#define HAL_SLEEP_UC(time)      hal_sleep_uc(time)

/*****************************************************************************
 *                    WATCHDOG
 *****************************************************************************/

void hal_EnableWatchdog(UInt16 timeout);
void hal_DisableWatchdog(void);
void hal_ResetWatchdog(void);
void hal_TriggerWatchdog(void);


#define HAL_WDT_ENABLE(timeout) hal_EnableWatchdog(timeout)
#define HAL_WDT_DISABLE()       hal_DisableWatchdog()
#define HAL_WDT_RESET()         hal_ResetWatchdog()
#define HAL_WDT_FORCE_TRIGGER() hal_TriggerWatchdog()

/*****************************************************************************
 *                    Reset Reason
 *****************************************************************************/

#define hal_ResetReason_Unspecified 0x0
#define hal_ResetReason_HWPor       0x1
#define hal_ResetReason_SWPor       0x2
#define hal_ResetReason_BrownOut    0x3
#define hal_ResetReason_Watchdog    0x4
typedef UInt8 hal_ResetReason_t;

hal_ResetReason_t hal_GetResetReason(void);

#define HAL_GET_RESET_REASON()      hal_GetResetReason()

/*****************************************************************************
 *                    UART
 *****************************************************************************/

#include "hal_UART.h"

/*****************************************************************************
 *                    SPI
 *****************************************************************************/

void  hal_InitSPI(UInt32 frequency, UInt8 mode, Bool lsbFirst);
UInt8 hal_WriteReadSPI(UInt8 byte);
void hal_WriteStreamSPI(UInt8 length, UInt8* pData);


#define HAL_SPI_TX(x)   hal_WriteReadSPI(x)
#define HAL_SPI_RX(x)   (x) = hal_WriteReadSPI(0)

/*****************************************************************************
 *                    IR
 *****************************************************************************/

GP_API void hal_InitIR(void);
GP_API void hal_SetIRCarrierPeriod(UInt16 period); //in us
GP_API void hal_MarkEndOfIRCode(void);
GP_API void hal_DisableIRCarrier(void);
GP_API void hal_SendIRSetClr(UInt32 time1, UInt32 time2); //in us
GP_API void hal_TriggerIR(void);
GP_API void hal_SetIRTimingParams (UInt8 internalCpuFreq);

// K7b specific functions
GP_API void hal_cbIrStartOfPattern(void);
GP_API void hal_ConfigureRegisterMode(void);
GP_API void hal_ConfigurePatternMode(UInt8 timeUnit);
GP_API void hal_ConfigureTimeMode(UInt8 timeUnit);
GP_API void hal_SetIRRegisterValue(Bool on);
GP_API void hal_PlaySequenceFromRam(UInt16* pPattern, UInt8 numEntries);
GP_API void hal_SetIRDutyCycle(UInt8 dutyCycle);

#define halIr_InterruptIdStart   0x00
#define halIr_InterruptIdRepeat  0x01
#define halIr_InterruptIdDone    0x02
typedef UInt8 halIr_InterruptId_t;

typedef void (*halIr_IntCallback_t)(halIr_InterruptId_t id);
GP_API void hal_RegisterIrIntCallback(halIr_IntCallback_t callback);


//Bootloader related
GP_API void hal_GoToBootloader(void);


/*****************************************************************************
 *                    TWI
 *****************************************************************************/

GP_API void hal_InitTWI(void);
GP_API Bool hal_WriteReadTWI(UInt8 deviceAddress, UInt8 txLength, UInt8* txBuffer, UInt8 rxLength, UInt8* rxBuffer);
GP_API Bool hal_WasActiveTWI(void);

/*****************************************************************************
 *                    ADC
 *****************************************************************************/

GP_API void   hal_InitADC(void);
GP_API UInt8  hal_MeasureADC_8(UInt8 channel, UInt8 measurementConfig);
GP_API UInt16 hal_MeasureADC_16(UInt8 channel, UInt8 measurementConfig);

#define HAL_MEASURE_ADC_CONFIG_NO_OPTIMIZE_FLAG   0x80
#define HAL_MEASURE_ADC_CONFIG_VOLTAGE_RANGE_MASK 0x03

#define HAL_MEASURE_ADC_CONFIG_OPTIMISATION_USED(config) (!(config & HAL_MEASURE_ADC_CONFIG_NO_OPTIMIZE_FLAG))
#define HAL_MEASURE_ADC_CONFIG_VOLTAGE_RANGE(config)     (  config & HAL_MEASURE_ADC_CONFIG_VOLTAGE_RANGE_MASK)

#define HAL_ENABLE_HOLD_MAX                 true
#define HAL_DISABLE_HOLD_MAX                false
#define HAL_ENABLE_HOLD_MIN                 true
#define HAL_DISABLE_HOLD_MIN                false
#define HAL_ENABLE_3V6                      true
#define HAL_DISABLE_3V6                     false
#define HAL_DONT_CARE_3V6                   false

GP_API void   hal_StartContinuousADCMeasurement(UInt8 channel, Bool maxHold, Bool minHold, Bool anioRange3V6);
GP_API void   hal_StopContinuousADCMeasurement(UInt8 channel);
GP_API UInt8  hal_GetContinuousADCMeasurement_8(UInt8 channel);
GP_API UInt16 hal_GetContinuousADCMeasurement_16(UInt8 channel);
GP_API void   hal_ClearContinuousADCMeasurement(UInt8 channel);

/*****************************************************************************
 *                    PWM
 *****************************************************************************/

GP_API void hal_InitPWM(void);
GP_API void hal_EnablePwm(Bool enable);
GP_API void hal_SetChannelEnabled(UInt8 channel, Bool enabled);
GP_API void hal_InvertOutput(UInt8 channel, Bool invert);
GP_API void hal_SetDutyCycle(UInt8 channel, UInt16 dutyCycle);
GP_API void hal_SetBalancedMode(Bool enable);
GP_API void hal_SetFrequencyCorrectionFactor(UInt8 factor);

// K7b Features and functions
GP_API void hal_ConfigPWM( UInt8 counterResolution, UInt8 carrier, Bool wordmode, Bool thresholdupdate );
GP_API void hal_SetPrescalerCounterWrapPower(UInt8 val);
GP_API void hal_EnableFreeRunningSPI( Bool wordMode, UInt8 length, UInt8* pData);
GP_API void hal_DisableSPI(void);

GP_API void hal_InitDMA( void );
GP_API Bool hal_LockDMA( void );
GP_API void hal_ReleaseDMA( void );
GP_API void hal_handleAlmostFullDMA( UInt8 *dest_rd_ptr, UInt8 *dest_rd_ptr_wrap, UInt16 dmaBufLength );

GP_API void hal_InitRamRetention(UInt8 ramRetentionSize); //size specified in KB

/*****************************************************************************
 *                    OTP debug settings
 *****************************************************************************/

GP_API void hal_DebugInit(void);
GP_API void hal_DebugOverrideSettings(void);
GP_API UInt8 hal_DebugGetInfo(void);

// SPI enabled | UART enabled | XSIF enabled | TWI enabled | ...

#define HAL_DEBUG_IS_SPI_ENABLED()   (!!(hal_DebugGetInfo() & BM(0)))
#define HAL_DEBUG_IS_UART_ENABLED()  (!!(hal_DebugGetInfo() & BM(1)))
#define HAL_DEBUG_IS_SLEEP_ENABLED() (!!(hal_DebugGetInfo() & BM(2)))
#define HAL_DEBUG_IS_XSIF_ENABLED()  (!!(hal_DebugGetInfo() & BM(3)))
#define HAL_DEBUG_IS_TWI_ENABLED()   (false)

/*****************************************************************************
 *                    RADIO INTERRUPT
 *****************************************************************************/

GP_API Bool hal_HandleRadioInterrupt(Bool execute);

#define HAL_RADIO_INT_EXEC_IF_OCCURED()    hal_HandleRadioInterrupt(true)
#define HAL_RADIO_INT_CHECK_IF_OCCURED()   hal_HandleRadioInterrupt(false)

/*****************************************************************************
 *                    MUTEX SUPPORT
 *****************************************************************************/

#define HAL_CRITICAL_SECTION_TYPE
#define HAL_CRITICAL_SECTION_DEF(pMutex)
#define HAL_CREATE_MUTEX(pMutex)
#define HAL_DESTROY_MUTEX(pMutex)
#define HAL_ACQUIRE_MUTEX(pMutex)  HAL_DISABLE_GLOBAL_INT()
#define HAL_RELEASE_MUTEX(pMutex)  HAL_ENABLE_GLOBAL_INT()
#define HAL_VALID_MUTEX(pMutex)    true
#define HAL_IS_MUTEX_ACQUIRED(pMutex) (!HAL_GLOBAL_INT_ENABLED())

#endif //_HAL_H_

