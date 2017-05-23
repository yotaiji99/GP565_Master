/*
 * Copyright (c) 2008-2014, GreenPeak Technologies
 *
 * The GreenPeak Hardware Abstraction Layer (HAL) implements the API to control the GP chip
 *
 * The HAL code is subdivided in different C-files as the chip itself consists of different blocks.
 * In this way the user can quickly use the code of the block he needs.
 *
 *  The file gpHal.h contains the general functions of the HAL (init, reset, interrupts).
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/inc/gpHal.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _HAL_GP_H_
#define _HAL_GP_H_

/**
 * @file gpHal.h
 *
 * The general functions of the HAL including initialization, reset and interrupt mask control and MCU settings
 * are included in the file gpHal.h
 *
 * @brief The file gpHal.h contains the general functions of the HAL (init, reset, interrupts).
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#ifdef GP_HAL_DIVERSITY_SINGLE_ANTENNA
#if (GP_HAL_DIVERSITY_SINGLE_ANTENNA!=0) && (GP_HAL_DIVERSITY_SINGLE_ANTENNA!=1)
#error error: gpHal GP_HAL_DIVERSITY_SINGLE_ANTENNA is unsupported single antenna
#endif
#endif

#include "global.h"
#include "gp_global.h"
#include "gpHal_reg.h"

//Different GP blocks
#include "gpHal_HW.h"
#include "gpHal_MAC.h"          //all RF related functions
#include "gpHal_MAC_Ext.h"
#include "gpHal_ES.h"           //Event Scheduler functions
#include "gpHal_SEC.h"          //Security functions
#include "gpHal_MISC.h"         //Miscellaneous functions



#include "gpHal_DP.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

//Chip version checking

/** @brief Indicates the GP chip identifier
*/
#ifndef GP_HAL_EXPECTED_CHIP_ID
extern UInt8 gpHal_ChipId;
#else
#define gpHal_ChipId      GP_HAL_EXPECTED_CHIP_ID
#endif //GP_HAL_EXPECTED_CHIP_ID

/** @brief Indicates the GP chip version
*/
#ifndef GP_HAL_EXPECTED_CHIP_VERSION
extern UInt8 gpHal_ChipVersion;
#else
#define gpHal_ChipVersion GP_HAL_EXPECTED_CHIP_VERSION
#endif //GP_HAL_EXPECTED_CHIP_VERSION

/** @brief Indicates if GP chip is emulated
*/
#ifndef GP_HAL_EXPECTED_CHIP_EMULATED
extern Bool  gpHal_ChipEmulated;
#else
#define gpHal_ChipEmulated GP_HAL_EXPECTED_CHIP_EMULATED
#endif //GP_HAL_EXPECTED_CHIP_EMULATED

#define GP_HAL_DEFAULT_TIMEOUT  10000UL //10ms

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

/** @brief Compares times from the chip's timebase - check if t1 < t2
*/
#define GP_HAL_TIME_COMPARE_LOWER(t1,t2)          (!((UInt32)((t1)  - (t2))/*&(0xFFFFFFFF)*/ < (0x80000000LU)))
/** @brief Compares times from the chip's timebase - check if t1 >= t2
*/
#define GP_HAL_TIME_COMPARE_BIGGER_EQUAL(t1,t2)   ( ((UInt32)((t1)  - (t2))/*&(0xFFFFFFFF)*/ < (0x80000000LU)))
/** @brief Compares times from the chip's timebase - check if t1 > t2
*/
#define GP_HAL_TIME_COMPARE_BIGGER(t1,t2)         (!((UInt32)((t2)  - (t1))/*&(0xFFFFFFFF)*/ < (0x80000000LU)))
/** @brief Compares times from the chip's timebase - check if t1 <= t2
*/
#define GP_HAL_TIME_COMPARE_LOWER_EQUAL(t1,t2)    ( ((UInt32)((t2)  - (t1))/*&(0xFFFFFFFF)*/ < (0x80000000LU)))

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/** @brief Reads a register of the GP chip.
 *
 *  Reads a register of the GP chip.
 *  @param Register The register address to read data from.
*/
GP_API UInt8 gpHal_ReadReg(gpHal_Address_t Register);
/** @brief Reads a block of registers of the GP chip.
 *
 *  Reads a block of registers of the GP chip.
 *  @param Address The register address where the block read starts.
 *  @param pBuffer The pointer to a byte buffer where the read data will be stored.
 *  @param Length  The number of bytes to be read.
*/
GP_API void gpHal_ReadRegs(gpHal_Address_t Address, void* pBuffer, UInt8 Length);

/** @brief Write a register of the GP chip.
 *
 *  Write a register of the GP chip.
 *  @param Register The register address to write to.
 *  @param Data     The data to write to the register.
*/
GP_API void gpHal_WriteReg(gpHal_Address_t Register, UInt8 Data);
/**
 *  @brief Writes a block of registers to the GP chip.
 *
 *  Writes a block of registers to the GP chip.
 *  @param Address The register address where the block write starts.
 *  @param pBuffer The pointer to a byte buffer where the data to be written are stored.
 *  @param Length  The number of bytes to be written.
*/
GP_API void gpHal_WriteRegs(gpHal_Address_t Address, void* pBuffer, UInt8 Length);

// AND register contents with inverted mask, OR in data, and write the result
/** @brief Reads a register, modifies the data with a certain mask and data, writes the register back. */
GP_API void gpHal_ReadModifyWriteReg(gpHal_Address_t Register, UInt8 Mask, UInt8 Data);

/** @brief Checks if MSI communication is possible and correct by reading a known register. */
GP_API Bool gpHal_CheckMsi(void);

/**
 *  @brief Returns the version information of the chip
 *
 *  This function returns the version information of the chip. It indicates the current revision of the chip and other information
 *
*/
GP_API UInt16 gpHal_GetHWVersionId(void);

/**
 * @brief Initializes HAL variables and sets basic GP chip register values.
 *
 * The function has to be called at the beginning of the application as it initializes variables that are used throughout operation.
 * At startup a decision is made to use the timed operation of the GP chip or not, by setting the parameter timedMAC.
 *
 * @param timedMAC If set to true, the GP chip will be used with a timed MAC.
 *                 When a timed MAC is used all transmission is done using scheduled triggers from the Event Scheduler (ES).
 *
*/
GP_API void gpHal_Init(Bool timedMAC);

/**
 * @brief Initializes Advanced HAL variables
 *
 * The function has to be called at the end of the application initialization. In comparison with the basic gpHal_Init()
 * method, the settings triggered by this initialization could interfere with initializations of other components.
 *
*/
GP_API void gpHal_AdvancedInit(void);

/**
 * @brief This function performs a reset of the chip.
 *
 * This function performs a reset of the functional registers. All registers are reverted to their default value and all memories are cleared.
 *
*/
GP_API void gpHal_Reset(void);
/**
 * @brief This function checks if radio is awake
 * @return Returns if value is awake. If value is 0, chip is asleep else the device is awake.
 *
*/
GP_API UInt8 gpHal_IsRadioAccessible(void);
/**
 * @brief This function detects if a reset of the GP chip has occurred.
 *
 * This function will return true if the GP chip has resetted since its first startup.
 *
*/
GP_API Bool gpHal_DidGPReset(void);
/**
 * @brief Thie functions enables the interrupt line of the GP chip.
 *
 * Sets the main interrupt mask of the GP chip.
 *
 * @param enable If set to true: main interrupt mask is switched on.
*/
#define gpHal_EnableInterrupts(enable) GP_HAL_ENABLE_INTERRUPTS(enable)
// Main interrupt service routine - to be registered by user using HAL_GP_INTERRUPT_INIT()
/**
 * @brief The interrupt service routine to be called when the interrupt of the GP chip is seen.
 *
 * This ISR needs to be called when the interrupt line (INTOUTn) goes low (active low signal).
 * It will handle the pending interrupt according to the callback functions registered to the different sources.
 *
 * @param ID  no functionality.
*/
#ifdef GP_DIVERSITY_GPHAL_EXTERN
GP_API void gpHal_Interrupt(UInt8 ID);
GP_API void gpHal_ISR_Interrupt(UInt8 ID);
#else
GP_API void gpHal_Interrupt(void);
#endif

/**
 * @brief This function regulates the GP chip sleep behaviour
 *
 * This function enables the sleep mode of the GP chip when the GP chip is idle.
 * Enabling GoToSleepWhenIdle will cause the GP chip to go to sleep mode when nothing is busy (TX/RX/Receiver on/...)
 * Be aware that the GP chip remains in sleep if no wakeup trigger or event is enabled and registered.
 *
 * It also regulates the GP chip sleep behaviour by keeping track of a "stay awake counter".
 * When the counter reaches '0' the GP chip is put into sleep.
 * It always needs to be used in pairs, one to wake up the GP chip when its needed and one to set it to sleep when the GP chip is no longer of use.
 * In this way different functions can use the sleep functionality without overriding each others settings.
 *
 * @param enable
 *                  - if set to true : The GP chip will be put to sleep if no other wake requests are pending, counter decremented
 *                  - if set to false: The GP chip will be kept awake, counter incremented
*/
GP_API void gpHal_GoToSleepWhenIdle(Bool enable);


#endif //_HAL_GP_H_

