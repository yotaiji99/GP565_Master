/*
 * Copyright (c) 2012-2013, GreenPeak Technologies
 *
 * hal.c
 *   Extended interface for RAM patching of the applications. Provides patch hoock mechanism.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/halXap5/inc/hal_ExtendedPatch.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _HAL_EXTENDEDPATCH_H_
#define _HAL_EXTENDEDPATCH_H_


/** 
 * @file hal_ExtendedPatch.h
 *
 * @brief This file contains API for adding patch hooks
 *
 * The concept of patching the application is applicable to the host/device arangement where the host has capability 
 * to upload patch information into the RAM memory of the device and by doing that to extend the functionality of the
 * device application. It is foreseen to use gpRamProgram component in the host application to fill the patch information
 * in the device application.
 * The patches are functions that can be connected to the device application via the hooks that must preexist in the
 * original application. This implies that the device application designer has to be aware of the patches that may need
 * to be implemented in the future.
 * In order to facilitate the implementation of the patch functions and reduce their footprint, the device application 
 * should expose some logging and low latency serial communication functionality that could be used by the patch functions. 
 * This exposure is done via the registering addresses of communication handlers to structure defined here.
 * The patch hook implemented here acts as a function call pointer table which if uninitialized causes minimal delay
 * in processing empty hooks.
 * The patch hooks and patch codes are contained in RAM buffer that has location and size
 * specific to particular build of application that is patchable. This means that the patch functions containing absolute
 * references have to be rebuilt every time the patch buffer moves position in the RAM memory. Patch packets containing
 * patches with relative references only and appropriate size may be used accross different builds of the same application.
 */
 
/**
 * @defgroup HAL_PATCH_USE Patch component use description
 * @brief This section describes adding patch capabilities to the application.
 *
 * As already mentioned in the module description, the patches are functions of arbitrary prototype being executed (if loaded)
 * by the hook calls. Application that needs to be patched has to be written by adding several extra steps:
 * - Application build script has to define the size of the patch buffer (i.e. set GP_SHMEM_FREERAMBUFSIZE=500 )
 *   Make note that the actual size of the RAM allocated for patch information is slightly larger if hooks are defined in the application.
 * - Application build script has to define indexes for all of the patches present in the application along with matching number of patch hooks. Example:
 *   set _UWCFLAGS2=%_UWCFLAGS2% -DHAL_PATCH_A=0
 *   set _UWCFLAGS2=%_UWCFLAGS2% -DHAL_PATCH_B=1
 *   set _UWCFLAGS2=%_UWCFLAGS2% -DHAL_PATCH_C=2
 *   set _UWCFLAGS2=%_UWCFLAGS2% -DHAL_PATCH_D=3
 *   set _UWCFLAGS2=%_UWCFLAGS2% -DHAL_NUMBER_OF_PATCH_HOOKS=4
 *   defines 4 patch hooks in the application. They all have to have unique positive values starting from 0 and without unused value ranges.
 *
 * A set of macros and defines have to be declared for each patch to be used. Examples are given for 4 different cases. Note that the build 
 * script controls not only the position of the hook in the table by defining HAL_PATCH_X macro but also if this macro is not defined, the 
 * hook is disabled even if the patch buffer is loaded. This is useful during development to select only a subset of patches that are hooked.
 * Macro MAIN_CHOOK_X is the hook X casted to the particular function type. Patch functions returning value have to have defined fallback value
 * that is used as return value if the hook is not connected to a patch:
 * - Patch function having no parameters and no return value.
 *   #ifdef HAL_PATCH_A
 *   #define MAIN_HOOK_A     HAL_HOOK(HAL_PATCH_A)
 *   #else
 *   #define MAIN_HOOK_A     NULL
 *   #endif
 *   typedef void (* patchAfn)(void);
 *   #define MAIN_CHOOK_A    ((patchAfn) MAIN_HOOK_A)
 *   #define MAIN_PATCH_A()  HAL_PATCH_NORETVAL(MAIN_CHOOK_A)
 * - Patch function having no parameters but having return value:
 *   #ifdef HAL_PATCH_B
 *   #define MAIN_HOOK_B     HAL_HOOK(HAL_PATCH_B)
 *   #else
 *   #define MAIN_HOOK_B     NULL
 *   #endif
 *   typedef Bool (* patchBfn)(void);
 *   #define MAIN_CHOOK_B    ((patchBfn) MAIN_HOOK_B)
 *   #define MAIN_FALLBACK_RV_B   false
 *   #define MAIN_PATCH_B()  HAL_PATCH_RETVAL(MAIN_CHOOK_B, MAIN_FALLBACK_RV_B)
 * - Patch having parameters but not having return value:
 *   #ifdef HAL_PATCH_C
 *   #define MAIN_HOOK_C     HAL_HOOK(HAL_PATCH_C)
 *   #else
 *   #define MAIN_HOOK_C     NULL
 *   #endif
 *   typedef void (* patchCfn)(void *, int, Bool);
 *   #define  MAIN_CHOOK_C    ((patchCfn) MAIN_HOOK_C)
 *   #define MAIN_PATCH_C(a, b, c)    HAL_PATCH_NORETVAL(MAIN_CHOOK_C, a, b, c)
 * - Patch having both parameters and return value:
 *   #ifdef HAL_PATCH_D
 *   #define MAIN_HOOK_D     HAL_HOOK(HAL_PATCH_D)
 *   #else
 *   #define MAIN_HOOK_D     NULL
 *   #endif
 *   typedef int (* patchDfn)(void *, int, Bool);
 *   #define  MAIN_CHOOK_D    ((patchDfn) MAIN_HOOK_D)
 *   #define MAIN_FALLBACK_RV_D   0
 *   #define MAIN_PATCH_D(a, b, c)  HAL_PATCH_RETVAL(MAIN_CHOOK_D, MAIN_FALLBACK_RV_D, a, b, c)
 *
 * Device application supporting patches has to instantiate patch buffer hal_patch as follows:
 * hal_Patch_t hal_patch;
 * The name and type of the patch buffer are fixed and it has to be instantiated as globaly visible symbol. 
 * Any source code implementing patch hook has to include this header file.
 */

extern UInt8 *gpShmem_pQueuedCmdBufRaw;
extern UIntPtr gpShmem_patchHooks[];
extern volatile Bool  gpShmem_patchHooksValid;
extern UInt8 *gpShmem_pLowLatencyMsg;
extern volatile UInt8 gpShmem_lowLatencyMsgSize;
extern volatile UInt8 gpShmem_lowLatencyId;
 
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define HAL_HOOK(hookId)                        NULL
#define HAL_PATCH_NORETVAL(chook, ...)          do{}while(false)
#define HAL_PATCH_RETVAL(chook, fallback, ...)  fallback

/*****************************************************************************
 *                    XAP5 ISR Patch Defines
 *****************************************************************************/
/* Macros defining hook for HAL_ISR patch (multiplexed patch over all of the ISRs) */
#define HAL_FALLBACK_RV_ISR true
#define HAL_HOOK_ISR        NULL
typedef Bool (* hal_Isr_Patch) (UInt8);
#define HAL_CHOOK_ISR       ((hal_Isr_Patch) HAL_HOOK_ISR)
#define HAL_ISR(id)         HAL_PATCH_RETVAL(HAL_CHOOK_ISR, HAL_FALLBACK_RV_ISR, id)

#define HAL_ISR_PIO_LP_PRE_ID               0
#define HAL_ISR_PIO_LP_POST_ID              1
#define HAL_ISR_PIO_HP_PRE_ID               2
#define HAL_ISR_PIO_HP_POST_ID              3
#define HAL_ISR_ES_LP_PRE_ID                4
#define HAL_ISR_ES_LP_POST_ID               5
#define HAL_ISR_ES_HP_PRE_ID                6
#define HAL_ISR_ES_HP_POST_ID               7
#define HAL_ISR_TRC_PRE_ID                  8
#define HAL_ISR_TRC_POST_ID                 9
#define HAL_ISR_STANDBY_PRE_ID              10
#define HAL_ISR_STANDBY_POST_ID             11
#define HAL_ISR_KEYP_PRE_ID                 12
#define HAL_ISR_KEYP_POST_ID                13
#define HAL_ISR_TIP_PRE_ID                  14
#define HAL_ISR_TIP_POST_ID                 15
#define HAL_ISR_UART_PRE_ID                 16
#define HAL_ISR_UART_POST_ID                17
#define HAL_ISR_IPC_LP_PRE_ID               18
#define HAL_ISR_IPC_LP_POST_ID              19
#define HAL_ISR_IPC_HP_PRE_ID               20
#define HAL_ISR_IPC_HP_POST_ID              21
#define HAL_ISR_MRI_PRE_ID                  22
#define HAL_ISR_MRI_POST_ID                 23
#define HAL_ISR_GPIO_LP_PRE_ID              24
#define HAL_ISR_GPIO_LP_POST_ID             25
#define HAL_ISR_GPIO_HP_PRE_ID              26
#define HAL_ISR_GPIO_HP_POST_ID             27
#define HAL_ISR_I2C_MASTER_PRE_ID           28
#define HAL_ISR_I2C_MASTER_POST_ID          29
#define HAL_ISR_PHY_PRE_ID                  30
#define HAL_ISR_PHY_POST_ID                 31

#endif /* _HAL_EXTENDEDPATCH_H_ */

