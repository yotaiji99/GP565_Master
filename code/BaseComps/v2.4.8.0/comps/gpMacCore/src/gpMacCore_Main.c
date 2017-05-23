/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 * gpMacCore_Main.c
 *   This file contains all files of the MAC layer.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpMacCore/src/gpMacCore_Main.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#include "gpMacCore.h"
#include "gpMacCore_defs.h"

#include "gpMacCore_Helpers.c"
#include "gpMacCore_Attributes.c"
#include "gpMacCore_Reset.c"
#include "gpMacCore.c"

#if defined(GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR)
#include "gpMacCore_Association_Originator.c"
#endif // defined(GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR)

#if defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT)
#include "gpMacCore_Association_Recipient.c"
#endif // defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT)


#if defined(GP_MACCORE_DIVERSITY_POLL_ORIGINATOR)
#include "gpMacCore_Poll_Originator.c"
#endif // defined(GP_MACCORE_DIVERSITY_POLL_ORIGINATOR)

#if    defined(GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR)\
    || defined(GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR)\
    || defined(GP_MACCORE_DIVERSITY_SCAN_ACTIVE_ORIGINATOR)\
    || defined(GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR)\
    || defined(GP_MACCORE_DIVERSITY_SCAN_RECIPIENT)\
    || defined(GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT)\
    || defined(GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT)
#include "gpMacCore_Scan.c"
#endif //










