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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpVersion/src/gpVersion_defs.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include "gpVersion.h"
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


/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void Version_Init(void);
void Version_UpdateModuleVersion(gpVersion_ModuleInfo_t* moduleInfo, gpVersion_ReleaseInfo_t serverVersion, UInt32 commId, Bool overrule);

