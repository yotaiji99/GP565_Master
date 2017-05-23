/*
 * Copyright (c) 2012-2014, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpMacCore/src/gpMacCore_defs.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GPMAC_CORE_DEFS_H_
#define _GPMAC_CORE_DEFS_H_
/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpUtils.h"
#include "gpMacCore.h"
#include "gpRxArbiter.h"
#include "gpHal.h"
#include "gpStat.h"

//All macro's and typedefs
#include "gpMacCore_defsDefines.h"

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
extern MacCore_AssocRsp_t *gpMacCore_pAssocRsp;
extern gpMacCore_ScanResult_t *gpMacCore_pScanResult;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#if defined (GP_MACCORE_DIVERSITY_SCAN_ACTIVE_ORIGINATOR) || defined (GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT)
STATIC void MacCore_DoActiveScan(void);
#endif // #if defined (GP_MACCORE_DIVERSITY_SCAN_ACTIVE_ORIGINATOR) || defined (GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT)
#if defined (GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR)
STATIC void MacCore_DoEDScan(void);
#endif // #if defined (GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR)
#if defined GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR || defined GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT
STATIC void MacCore_DoOrphanScan(void);
#endif // #if defined GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR || defined GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT

//Assocation Response related
/** @fn MacCore_cbAssocResp(void)
 *  @brief This function constructs and sends the association response
 *
 *  @param void*                    data
 *  @param gpMacCore_AddressInfo_t *dstAddrInfo
 *
 *  @return void
 */
#if defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT)
STATIC void MacCore_cbAssocResp(void);
STATIC void MacCore_AssociateResponseTimeOut(void);
#endif // defined(GP_MACCORE_DIVERSITY_FFD) || defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT)
#if defined(GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR)
//Assocation Request related
STATIC void MacCore_AssociateSendCommandDataRequest(void);
STATIC void MacCore_HandleAssocConf( void );
#endif // defined(GP_MACCORE_DIVERSITY_FFD) || defined(GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR)

#ifdef GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
extern MacCore_PollReqArgs_t *gpMacCore_pPollReqArgs;
#endif // GP_MACCORE_DIVERSITY_POLL_ORIGINATOR


STATIC void MacCore_WriteMacHeaderInPd(gpMacCore_FrameType_t type, gpMacCore_AddressInfo_t* pSrcAddrInfo, const gpMacCore_AddressInfo_t* pDstAddrInfo, UInt8 txOptions, UInt8 securityLevel,  gpPd_Loh_t *pPdLoh , gpMacCore_StackId_t stackId );
STATIC void MacCore_InitSrcAddressInfo(gpMacCore_AddressInfo_t *srcAddressInfo, gpMacCore_AddressMode_t addrMode, gpMacCore_StackId_t stackId);
STATIC Bool MacCore_AllocatePdLoh(gpPd_Loh_t* p_PdLoh);

#if defined(GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR) || defined(GP_MACCORE_DIVERSITY_POLL_ORIGINATOR)
STATIC gpMacCore_Result_t MacCore_SendCommand( gpMacCore_AddressInfo_t* pDestAddrInfo, gpMacCore_AddressInfo_t* pSrcAddrInfo, UInt8 txOptions, UInt8 *pData, UInt8 len , gpMacCore_StackId_t stackId, gpHal_MacScenario_t scenario );
STATIC gpMacCore_Result_t MacCore_SendCommandDataRequest(gpMacCore_AddressInfo_t * pCoordAddrInfo, Bool associationTrigger, gpMacCore_StackId_t stackId);
#endif

STATIC void MacCore_InitPad(gpMacCore_StackId_t stackId);
STATIC void MacCore_SetDefaultStackValues(Bool resetPib, gpMacCore_StackId_t stackId);
STATIC void MacCore_StopRunningRequests(gpMacCore_StackId_t stackId);
#ifdef GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR
STATIC void MacCore_HandleOrphanScanEnd(gpMacCore_Result_t result);
#endif // GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR
// security functions


// pd functions
STATIC void MacCore_ReadStreamAndUpdatePd(UInt8* bytes, UInt8 length, gpPd_Loh_t* p_PdLoh);
STATIC void MacCore_PrependStreamAndUpdatePd(UInt8* bytes, UInt8 length, gpPd_Loh_t* p_PdLoh);


#ifdef GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
STATIC void MacCore_DelayedPollConfirm(void);
#endif // GP_MACCORE_DIVERSITY_POLL_ORIGINATOR

#if (GP_MACCORE_SCAN_RXOFFWINDOW_TIME_US != 0)
STATIC void MacCore_ScanRxOffWindow(void);
#endif // (GP_MACCORE_SCAN_RXOFFWINDOW_TIME_US != 0)

#ifdef GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR
STATIC void MacCore_ResetMinInterferenceLevels(void) ;
#endif // GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR

/** @brief This function returns the stackId based on the panId.
 *  @param panId         returns MACCORE_STACK_UNDEFINED when stackId was not found - else a value below GP_DIVERSITY_NR_OF_STACKS
*/

#if GP_DIVERSITY_NR_OF_STACKS > 1
STATIC gpMacCore_StackId_t MacCore_GetStackId( gpMacCore_AddressInfo_t *pAddressInfo );
#else
#define MacCore_GetStackId(pAddressInfo)    0x0
#endif //  GP_DIVERSITY_NR_OF_STACKS > 1

STATIC gpPad_Handle_t MacCore_GetPad(UInt8 stackId);



#endif //_GPMAC_CORE_DEFS_H_

