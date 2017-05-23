/*
 * Copyright (c) 2013-2014, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpVersion/inc/gpVersion.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GP_VERSION_H_
#define _GP_VERSION_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <global.h>

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_DIVERSITY_VERSION_MAX_MODULE_TABLE_SIZE  6
#define GP_DIVERSITY_VERSION_MAX_VERSIONS_SIZE      6

#define GP_VERSION_VERSIONS_EQUAL(v1, v2)           ((v1).major == (v2).major && (v1).minor == (v2).minor && (v1).revision == (v2).revision && (v1).patch == (v2).patch)

#define GP_VERSION_MODULE_ID_INVALID            0xFF
#define GP_VERSION_VERSION_INDEX_INVALID        0xFF
/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

#define gpVersion_CompatibilityResultCompatible         0
#define gpVersion_CompatibilityResultMaybeCompatible    1
#define gpVersion_CompatibilityResultNotCompatible      2
#define gpVersion_CompatibilityResultInvalidParameter   0xFF
typedef UInt8 gpVersion_VersionStatusResult_t;

typedef UInt8 gpVersion_VersionNumber_t;

// defines release version
typedef struct {
    gpVersion_VersionNumber_t major;
    gpVersion_VersionNumber_t minor;
    gpVersion_VersionNumber_t revision;
    gpVersion_VersionNumber_t patch;
} gpVersion_ReleaseInfo_t;

// defines software information
typedef struct {
    gpVersion_ReleaseInfo_t version;
    UInt8                   ctrl;
    UInt8                   reserved;
    UInt16                  number;
    UInt32                  changeList;
} gpVersion_SoftwareInfo_t;

typedef struct {
    gpVersion_ReleaseInfo_t         versions[GP_DIVERSITY_VERSION_MAX_VERSIONS_SIZE];
} gpVersion_ModuleVersions_t;

// callback structure for all modules
typedef struct {
    UInt32                              commId;
    UInt8                               moduleId;
    UInt8                               nrOfVersions;
    UInt8                               selectedVersionIndex;
    Bool                                overrule;
    gpVersion_ReleaseInfo_t             versionInfo[GP_DIVERSITY_VERSION_MAX_VERSIONS_SIZE];
    gpVersion_ReleaseInfo_t             serverVersion;
} gpVersion_ModuleVersionInfo_t;

// callback type for wrappers
typedef void (*gpVersion_cbSetModuleVersion)(gpVersion_ModuleVersionInfo_t* moduleVersionInfo);

// structure that contains all useful module information
typedef struct {
    gpVersion_ModuleVersionInfo_t       moduleVersionInfo;
    gpVersion_cbSetModuleVersion        cbSetModuleApiVersion;
} gpVersion_ModuleInfo_t;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpVersion_Init(void);

/* Get functions */
// get the release version
void gpVersion_GetSoftwareVersion(gpVersion_ReleaseInfo_t* swVersion);

// get all info about the software (version, changelist, ...)
void gpVersion_GetSoftwareInfo(gpVersion_SoftwareInfo_t* swInfo);

// get the basecomps version
void gpVersion_GetBcVersion(gpVersion_ReleaseInfo_t* bcInfo);

void gpVersion_GetRaceVersion(gpVersion_ReleaseInfo_t* raceInfo);


UInt32 gpVersion_GetChangelist(void);

/* Register Module to version administration */
void gpVersion_RegisterModule(UInt8 moduleId, gpVersion_ReleaseInfo_t* versions, UInt8 nrOfVersions, gpVersion_cbSetModuleVersion cbSetModuleApiVersion);

/* Dump Functions */
void gpVersion_DumpVersion(gpVersion_ReleaseInfo_t swVersion);


#endif // _GP_VERSION_H_

