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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpVersion/src/gpVersion_versionInfo.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include "gpVersion.h"
#include "gpVersion_defs.h"
#include "gpLog.h"
#include "gpAssert.h"
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_VERSION
/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/
void Version_Init(void);

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

#define CONTROLBITFIELD 0x00

// changelist is 3 bytes and LSB is reserved 
const ROM gpVersion_SoftwareInfo_t FLASH_PROGMEM gpVersionSw LINKER_SECTION(".version_sw") 
        = {{GP_VERSIONINFO_GLOBAL_VERSION},CONTROLBITFIELD,0,0,((0x00FFFFFFUL & GP_CHANGELIST)<<8)};

const ROM gpVersion_ReleaseInfo_t FLASH_PROGMEM gpVersionBc = {GP_VERSIONINFO_BASE_COMPS};

const ROM gpVersion_ReleaseInfo_t FLASH_PROGMEM gpVersionRace LINKER_SECTION(".version_race") = {GP_VERSIONINFO_RACE_VERSION};


/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpVersion_GetSoftwareVersion(gpVersion_ReleaseInfo_t* swVersion)
{
    MEMCPY_P(swVersion, &(gpVersionSw.version), sizeof(gpVersion_ReleaseInfo_t));
}

void gpVersion_GetSoftwareInfo(gpVersion_SoftwareInfo_t* swInfo)
{
    MEMCPY_P(swInfo, &gpVersionSw, sizeof(gpVersion_SoftwareInfo_t));
}

void gpVersion_GetBcVersion(gpVersion_ReleaseInfo_t* bcInfo)
{
    MEMCPY_P(bcInfo, &gpVersionBc, sizeof(gpVersion_ReleaseInfo_t));
}

void gpVersion_GetRaceVersion(gpVersion_ReleaseInfo_t* raceInfo)
{
    MEMCPY_P(raceInfo, &gpVersionRace, sizeof(gpVersion_ReleaseInfo_t));
}


UInt32 gpVersion_GetChangelist(void)
{
    return GP_CHANGELIST;
}

void gpVersion_DumpVersion(gpVersion_ReleaseInfo_t swVersion)
{
   GP_LOG_SYSTEM_PRINTF("Version is: v%i.%i.%i.%i",0, swVersion.major, swVersion.minor, swVersion.revision, swVersion.patch);
}    

