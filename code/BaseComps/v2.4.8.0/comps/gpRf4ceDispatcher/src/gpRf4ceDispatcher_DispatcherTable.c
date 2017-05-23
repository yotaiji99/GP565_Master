/*
 * Copyright (c) 2011-2014, GreenPeak Technologies
 *
 * gpRf4ceDispatcher_DispatcherTable.c
 *
 * The file defines the callback mapping tables for the dispatcher.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpRf4ceDispatcher/src/gpRf4ceDispatcher_DispatcherTable.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#define GP_COMPONENT_ID GP_COMPONENT_ID_RF4CEDISPATCHER

#include "global.h"
#include "gpBaseComps.h"
#include "gpRf4ceDispatcher.h"

#include "gpRf4ceBindValidation.h"

#include "gpRf4ceUserControl.h"
#include "gpRf4ceRib.h"
#include "gpRf4ceKickOutLru.h"



//keep gpRf4ceKickOutLru map-entry first to allow kickout analysis
GP_RF4CE_DISPATCHER_DATA_DECLARATION(gpRf4ceKickOutLru);
//keep gpRf4ceRib map-entry before gpRf4ceBindValidation (rib requests shouldn't be rejected in starting/ending validation period)
GP_RF4CE_DISPATCHER_DATA_DECLARATION(gpRf4ceRib);
GP_RF4CE_DISPATCHER_DATA_DECLARATION(gpRf4ceBindValidation);
GP_RF4CE_DISPATCHER_BIND_DECLARATION(gpRf4ceBindValidation);
GP_RF4CE_DISPATCHER_DATA_DECLARATION(gpRf4ceUserControl);
GP_RF4CE_DISPATCHER_DATA_DECLARATION(gpApp);
GP_RF4CE_DISPATCHER_BIND_DECLARATION(gpApp);

GP_RF4CE_DISPATCHER_DATA_MAP_BEGIN()
//keep gpRf4ceKickOutLru map-entry first to allow kickout analysis
GP_RF4CE_DISPATCHER_DATA_MAP_ENTRY(GP_COMPONENT_ID_RF4CEKICKOUTLRU,gpRf4ceKickOutLru)
//keep gpRf4ceRib map-entry before gpRf4ceBindValidation (rib requests shouldn't be rejected in starting/ending validation period)
GP_RF4CE_DISPATCHER_DATA_MAP_ENTRY(GP_COMPONENT_ID_RF4CERIB,gpRf4ceRib)
GP_RF4CE_DISPATCHER_DATA_MAP_ENTRY(GP_COMPONENT_ID_RF4CEBINDVALIDATION,gpRf4ceBindValidation)
GP_RF4CE_DISPATCHER_DATA_MAP_ENTRY(GP_COMPONENT_ID_RF4CEUSERCONTROL,gpRf4ceUserControl)
GP_RF4CE_DISPATCHER_DATA_MAP_ENTRY(GP_COMPONENT_ID_APP,gpApp)
GP_RF4CE_DISPATCHER_DATA_MAP_END()


GP_RF4CE_DISPATCHER_BIND_MAP_BEGIN()
GP_RF4CE_DISPATCHER_BIND_MAP_ENTRY(GP_COMPONENT_ID_RF4CEBINDVALIDATION,gpRf4ceBindValidation)
GP_RF4CE_DISPATCHER_BIND_MAP_ENTRY(GP_COMPONENT_ID_APP,gpApp)
GP_RF4CE_DISPATCHER_BIND_MAP_END()

