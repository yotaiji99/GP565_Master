
/*
 * Copyright (c) 2009-2013, GreenPeak Technologies
 *
 * gpNvm.h
 *
 * This file defines the Non Volatilie Memory component api
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpNvm/inc/gpNvm.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */


#ifndef _GPNVM_H_
#define _GPNVM_H_

//DOCUMENTATION GENERATION: no @file required as all documented items are refered to a group

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <global.h>

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_NVM_CONST       const

//---------------------
//General NVM functions
//---------------------

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @file gpNvm.h
 *
 *
 * @defgroup GEN_NVM General NVM functions
 * @brief The general NVM functionality is implemented in these functions
 *
 * Checking and clearing the NVM is made possible by these functions.
 * The implementation of these functions can be unique for each type of NVM used.
 */

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/** @brief  Wildcard used to select all nvm-sections in a backup/restore/clear call. */
#define gpNvm_AllComponents             0xEE
/** @brief Wildcard used to select all tags of the specified nvm-sections in a backup/restore/clear call. */
#define gpNvm_AllTags                   0xEE

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
 
/* @name gpNvm_UpdateFrequency_t */
//@{
/** @brief  Updated more often, typically used for attributes can get updated during normal user usage */
#define gpNvm_UpdateFrequencyHigh       0
/** @brief  Updated in rare condition, typically used for attributes updated if a new RF4CE link is established */
#define gpNvm_UpdateFrequencyLow        1
/** @brief  Updated only during initialisation phase, typically used of attributes which are only set once */
#define gpNvm_UpdateFrequencyInitOnly   2
/** @brief  Set during configuration, read-only information */
#define gpNvm_UpdateFrequencyReadOnly   3
/** @typedef gpNvm_UpdateFrequency_t
 *  @brief The gpNvm_UpdateFrequency_t type defines indicating the frequency a tag will require a new backup into NVM.
*/
typedef UInt8 gpNvm_UpdateFrequency_t;
//@}

/** @struct gpNvm_Tag
 *  @brief The gpNvm_Tag structure specifies the fields that describe an nvm-tag */
/** @typedef gpNvm_Tag_t
 *  @brief The gpNvm_Tag_t type describes an nvm-tag */
typedef struct gpNvm_Tag {
/** @brief A pointer to the corresponding RAM memory of the attributed to be backuped in nvm */
    UInt8*                      pRamLocation;
/** @brief The number of bytes of the nvm-tag */
    UInt16                      size;
/** @brief An indication of the update frequency */
    gpNvm_UpdateFrequency_t     updateFrequency;
/** @brief A pointer to flash memory containing the default values for the tag, 0xFF will be used as default values if pointer is specified as NULL */
    GP_NVM_CONST ROM UInt8*                pDefaultValues;
} gpNvm_Tag_t;

/** @typedef gpNvm_cbCheckConsistency_t
 *  @brief The gpNvm_cbCheckConsistency_t defines the callback type for an additional section specific check-consistency routine (see gpNvm_RegisterSection())
*/
typedef Bool (* gpNvm_cbCheckConsistency_t) (void);

typedef UInt16 gpNvm_VersionCrc_t;

typedef struct gpNvm_Section {
    UInt8                                   componentId;
    const ROM gpNvm_Tag_t* FLASH_PROGMEM    tags;
    UInt8                                   nbrOfTags;
    UInt8*                                  baseAddr;
    gpNvm_cbCheckConsistency_t              cbCheckConsistency;
} gpNvm_Section_t;

/** @typedef gpNvm_cbUpdate_t
 *  @brief The gpNvm_cbUpdate_t defines the callback type for the up or downgrade routine between NVM versions
*/
typedef Bool (*gpNvm_cbUpdate_t)(void);

/** @struct gpNvm_CompatibilityEntry
 *  @brief The gpNvm_CompatibilityEntry structure specifies the information for compatible NVM versions */
/** @typedef gpNvm_CompatibilityEntry_t
 *  @brief The gpNvm_CompatibilityEntry_t type describes a compatibility entry */
typedef struct gpNvm_CompatibilityEntry {
    UInt16  crc;
    gpNvm_cbUpdate_t upgradeCb;
    gpNvm_cbUpdate_t downgradeCb;
} gpNvm_CompatibilityEntry_t;



/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

 /**@ingroup GEN_NVM
 * @brief Initializes the NVM software block
 *
 *  Initializes the NVM software block.
 *  After execution of this function data can be read or written to NVM.
 */
void gpNvm_Init(void);
void gpNvm_DeInit(void);

 /**@ingroup GEN_NVM
 * @brief Reserves part of space on local NVM for remote NVM usage.
 *
 *  This function should be called at most once. If not called, no space is reserved for remote NVM.
 * @param Size of the NVM to be used remotely.
 * @return Offset within local NVM where remote NVM may store sections
 */
UInt16 gpNvm_ReserveRemoteNvmSpace(UInt16 requiredSize);

/**@ingroup GEN_NVM
 * @brief Clears the complete NVM
 *
 *  This function clears the complete NVM. This includes all tags of all registerd nvm-sections and all legacy implementations of the component specific interfaces described in the other sections.
 *  Readonly tags will not be erased unless the NVM was unlocked using gpNvm_SetLock().
*/
void gpNvm_ClearNvm(void);

/**@ingroup GEN_NVM
 * @brief Requests a forced flush of the write buffers to NVM
 *
 * This function request the nvm component to flush all pending write data to NVM. If the
 * NVM implementation doesn't use buffers, the function should be stubbed.
 *
*/
void gpNvm_Flush(void);

/**@ingroup GEN_NVM
 * @brief Dumps the content NVM.
 *
 * This function dumps the content of the specified tag of the specified component. It will also indicate with a '*' that this content is not
 * in sync with the current values of the corresponding ram variables
 *
 *  @param componentId          The unique idenditfier nvm-section of the section to be restored, gpNvm_AllComponents can be used as wildcard.
 *  @param tagId                The sequence number of the tag to be restored, gpNvm_AllTags can be used as wildcard.
*/
void gpNvm_Dump(UInt8 componentId, UInt8 tagId);

/**@ingroup GEN_NVM
 * @brief Dumps the NVM structure
 *
 * This function dumps the structure of the NVM tags and NVM CRC
*/
void gpNvm_DumpStructure(void);

/**@ingroup GEN_NVM
 * @brief Callback triggered if consistency check failed.
 *
 *  This callback function will be called when the NVM consistency check fails.
*/
void gpNvm_cbFailedCheckConsistency(void);

/**@ingroup GEN_NVM
 * @brief Registers a component to the NVM software block
 *
 *  Registers a component to the NVM software block. This will define a NVM section. The function should be called from the _init
 *  function of the component.
 *
 *  @param componentId          An unique idenditfier of the registered section, which will be used further on to refer to this section. The GP_COMPONENT_ID should be used for this.
 *  @param gpNvm_Tag_t          A pointer to a gpNvmTag_t array allocated in flash contining the information of all nvm-tags of the registered section.
 *  @param nbrOfTags            The number of tags in the gpNvm_Tag_t array tags.
 *  @param cbCheckConsistency   A function pointer to a callback which will be called by the nvm-component if it's performing a consistency check. This allows specific checks on the
 *                              content of the section which are only known by the component. If specified as NULL, this additional check will be ommitted.
 */
void gpNvm_RegisterSection(UInt8 componentId, const ROM gpNvm_Tag_t *tags, UInt8 nbrOfTags, gpNvm_cbCheckConsistency_t cbCheckConsistency);


/**@ingroup GEN_NVM
 * @brief Backup the content of the specified NVM-tag.
 *
 *  The content of the RAM memory (pRamLocation parameter) specified by the combination of the component and tag will be written to the non-volitale memory. 
 *
 *  @param componentId          The unique idenditfier nvm-section of the section to be backuped, gpNvm_AllComponents can be used as wildcard.
 *  @param tagId                The sequence number of the tag to be backuped
 *  @param pRamLocation         Pointer to the data that should be backuped.  When this pointer is NULL, the internal stored pointer is used.
 */
void gpNvm_Backup(UInt8 componentId, UInt8 tagId, UInt8* pRamLocation);

/**@ingroup GEN_NVM
 * @brief Restore the content of the specified NVM-tag using the provided RAM location.
 *
 *  The content of the RAM memory (pRamLocation parameter) specified by the combination of the component and tag will be updated with the corresponding copy in the the non-volitale memory. 
 *
 *  @param componentId          The unique idenditfier nvm-section of the section to be restored, gpNvm_AllComponents can be used as wildcard.
 *  @param tagId                The sequence number of the tag to be restored.
 *  @param pRamLocation         Pointer to the data that should be restored.  When this pointer is NULL, the internal stored pointer is used.
 */
void gpNvm_Restore(UInt8 componentId, UInt8 tagId, UInt8* pRamLocation);

/**@ingroup GEN_NVM
 * @brief Clears the content of the specified NVM-tag using the provided RAM location.
 *
 *  The non-volatile memory specified by the combination of the component and tag will be updated to the default values (pDefaultValues). If there are no
 *  default values specified the content of the non-volatile memory will be set to 0xFF.
 *
 *  @param componentId          The unique idenditfier nvm-section of the section to be cleared, gpNvm_AllComponents can be used as wildcard.
 *  @param tagId                The sequence number of the tag to be cleard, gpNvm_AllTags can be used as wildcard.
 */
void gpNvm_Clear(UInt8 componentId, UInt8 tagId);

/**@ingroup GEN_NVM
 * @brief Locks or unlocks the possibility to write or clear read-only elements.
 *
 * Default the NVM area will be unable to write or clear read-only elements (tags with update frequency gpNvm_UpdateFrequencyReadOnly). 
 * One can 'unlock' the area by calling this function with false.
 * Re-lock the area after the needed manipulations by calling this function with true.
 *
 * @param lock                  Lock (true) or Unlock (false) the NVM area.
 */
void gpNvm_SetLock(Bool lock);

/**@ingroup GEN_NVM
 * @brief Checks if NVM is consistent.
 *
 *  The function checks if the NVM is consistent. If not, NVM is cleared.
 */
void gpNvm_CheckConsistency(void);


/**@ingroup GEN_NVM
 * @brief Checks if NVM is accessible.
 *
 *  The function checks if the NVM is accessible
 *  @return Boolean that retunrns the result of the check.
 */
Bool gpNvm_CheckAccessible(void);




#ifdef __cplusplus
}
#endif //__cplusplus

#endif // _GPNVM_H_

