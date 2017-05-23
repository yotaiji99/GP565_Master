/*
 * Copyright (c) 2011-2014, GreenPeak Technologies
 *
 *   This file contains the definitions of the public functions and enumerations of the gpRf4ceDispatcher.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpRf4ceDispatcher/inc/gpRf4ceDispatcher.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */
 
#ifndef _GP_RF4CE_DISPATCHER_H_
#define _GP_RF4CE_DISPATCHER_H_


/// @file gpRf4ceDispatcher.h
///
/// @defgroup INIT Initialization
///
/// @defgroup RESET Reset
/// Any functional block (incl. application) can request the dispatcher to reset itself and the underlying RF4CE network.
/// The initiating application issues the ResetRequest to the dispatcher. The dispatcher will reset the RF4CE network layer, and upon confirmation of this 
/// it informs all registered functional blocks through a ResetIndication. Last a ResetConfirmation is given to the functional block that requested the reset.
/// @image latex Reset.pdf "Reset Procedure" width=12cm
///
/// @defgroup LOCK Lock the Access to the RF4CE Network Layer
/// Before a functional block can perform a request to the RF4CE network layer it has to claim the access to it via the dispatcher.
/// When a block has claimed the access, other blocks will not be able to access the RF4CE network layer. 
/// When the functional block is ready, it has to release the access to the RF4CE network layer.
/// See the next figure:
/// @image latex LockRequest.pdf "Lock and Release Access to the RF4CE Network Layer" width=12cm
///
/// @defgroup RADIO Radio Control
///
/// @defgroup CALLBACK RF4CE Callback Functions
/// This chapter describes the confirm and indication primitives that originate in the RF4CE network layer and are routed through the dispatcher to the functional block(s).
/// For the description and parameters of these 'gpRf4ceDispatcher_cb' primitives please refer to the corresponding 'gpRf4ce_cb' primitives in the RF4CE API Reference Manual (GP_P207_IS_01237).
/// The function documentation below describes only the dispatcher-specific aspects.
///
/// <h2>Routing of Confirm Primitives</h2>
/// The confirm primitives are forwarded only to the functional block that currently holds the lock on the RF4CE network layer access.
///
/// <h2>Routing of DataIndication Primitives</h2>
/// The dataIndications are provided to each functional block in a prioritized way. 
/// The data is first provided to the functional block with the highest priority. 
/// If this block does not handle the data it will be provided to the next block. 
/// This continues until a block handles the data. If no functional block handles the data it will be dropped.
/// @image latex IndicationHandling.pdf "Data Indication Handling" width=15cm
/// <h2>Routing of DiscoveryIndication Primitives</h2>
/// The discoveryIndication is handled the same way as the dataIndication; offered to each block till one handles it.
/// <h2>Routing of PairIndication and UnpairIndication Primitives</h2>
/// The pairIndication and unpairIndication are handled the same way as the dataIndication. 
/// If there is no functional block that handles the pairIndication, the dispatcher will respond to the pairIndication with status "not permitted".
/// <h2>Routing of CommStatusIndication Primitives</h2>
/// This callback can be considered a confirm on a response. Therefor this indication is handled the same way as a confirm is handled. 
/// The commStatusIndication will be forwarded to the functional block that currently holds the lock on the RF4CE network layer access.
///
/// @defgroup GENERAL General
/// It is good practice not to call a request function from a confirm function. 
/// Instead, update the functional block's state so that the request is called the next time the block's state machine is triggered. 
/// Nested calls of any request function in the confirm callback function should be avoided because:
/// - the confirm callback is triggered from the interrupt service routine
/// - deep call trees are created which are not supported on all embedded processors
/// - the dispatcher and other functional blocks do not queue requests
///
/// @defgroup REGISTER Dispatcher Registrations
/// <h2>Registering Functional Blocks with the Dispatcher</h2>
/// Functional blocks are registered with the dispatcher through statically defined maps.
/// These maps are typically defined in the gpBaseComps_DispatcherTable.c file.
/// The order in which functional blocks are listed in these maps defines their priority for the indication primitives.
/// An indication is offered first to the first block listed in the map.
/// Two maps exist:
/// - Data Map: Defines all the functional blocks that need to be able to use the data-related RF4CE functions and indications.
/// The Data Map is defined using the following macros:<br>
/// GP_RF4CE_DISPATCHER_DATA_MAP_BEGIN(),<br>
/// GP_RF4CE_DISPATCHER_DATA_MAP_ENTRY() - one per functional block,<br>
/// GP_RF4CE_DISPATCHER_DATA_MAP_END().<br>
/// - Bind Map: Defines the functional blocks that need to be able to use the Binding-related RF4CE functions and indications. 
/// Typically this is the one profile-dependent Bind functional block.
/// The Bind Map is defined using the the following macros:<br>
/// GP_RF4CE_DISPATCHER_BIND_MAP_BEGIN(),<br>
/// GP_RF4CE_DISPATCHER_BIND_MAP_ENTRY() - one per functional block,<br>
/// GP_RF4CE_DISPATCHER_BIND_MAP_END().<br>
///
/// <h2>Defining Callback Functions</h2>
/// Each functional block registered with the dispatcher needs to define its callback functions in dispatcher Bind / data blocks:
/// - Data Block: Defines the data-related callback functions.
/// To define the data callback functions using the standard function names, use the following macros:<br>
/// GP_RF4CE_DISPATCHER_DATA_DECLARATION(functionalBlock);<br>
/// GP_RF4CE_DISPATCHER_DATA_IMPLEMENTATION(functionalBlock);<br>
/// - Bind Block: Defines the Binding-related callback functions.
/// To define the Bind callback functions using the standard function names, use the following macros:<br>
/// GP_RF4CE_DISPATCHER_BIND_DECLARATION(functionalBlock);<br>
/// GP_RF4CE_DISPATCHER_BIND_IMPLEMENTATION(functionalBlock);<br>
///
/// See the structure definitions for the specified callback functions.
/// 
/// <h2><i>ADVANCED USAGE</i></h2>
/// The above Bind / data block macros require all callback functions to be defined, and with standard names.
/// To be able to specify different names or NULL pointers (for callback functions that are not used, saving code),
/// one can also use the GP_RF4CE_DISPATCHER_CONST definition with the gpRf4ceDispatcher_DataCallbacks_t or gpRf4ceDispatcher_BindCallbacks_t structure instead of the above macros.
/// 
 
/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
 
#include <global.h>
#include "gpMacCore.h"
#include "gpRf4ce.h"
#include "gpPd.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_RF4CE_DISPATCHER_CONST       const

/** @brief The invalid functional block ID.  This ID is used to indicate the end of the bind/data map (see GP_RF4CE_DISPATCHER_BIND_MAP_END) */
#define GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID       (0xFF)
 
 /** @brief This macro defines the begin of the bind map. */
#define GP_RF4CE_DISPATCHER_BIND_MAP_BEGIN()  \
const gpRf4ceDispatcher_BindMapping_t ROM gpRf4ceDispatcher_BindMapping[] FLASH_PROGMEM = \
{

/** @brief This macro adds a functional block to the bind map.*/
#define GP_RF4CE_DISPATCHER_BIND_MAP_ENTRY(functionalBlockID,functionalBlock)   \
    { functionalBlockID , &functionalBlock##_BindCallbacks },

/** @brief This macro defines the end of the bind map. */
#define GP_RF4CE_DISPATCHER_BIND_MAP_END()  \
    { GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID  , NULL }\
};

/** @brief This macro declares a bind block. */
#define GP_RF4CE_DISPATCHER_BIND_DECLARATION(functionalBlock) \
    extern GP_RF4CE_DISPATCHER_CONST gpRf4ceDispatcher_BindCallbacks_t ROM functionalBlock##_BindCallbacks FLASH_PROGMEM

/** @brief This macro implements a bind block using standard names for the callback functions.  This macro is required in the bind block body file (.c file). */
#define GP_RF4CE_DISPATCHER_BIND_IMPLEMENTATION(functionalBlock)\
    GP_RF4CE_DISPATCHER_BIND_IMPLEMENTATION_EXT(functionalBlock, gpRf4ceDispatcher_cbMEDiscoveryCommStatusIndication, gpRf4ceDispatcher_cbMEPairCommStatusIndication, gpRf4ceDispatcher_cbMEDiscoveryIndication, gpRf4ceDispatcher_cbMEDiscoveryConfirm, gpRf4ceDispatcher_cbMEDiscoveryResponseNotifyIndication, gpRf4ceDispatcher_cbMEAutoDiscoveryConfirm, gpRf4ceDispatcher_cbMEPairIndication, gpRf4ceDispatcher_cbMEPairConfirm, gpRf4ceDispatcher_cbMEUnpairIndication, gpRf4ceDispatcher_cbMEUnpairConfirm)

#define GP_RF4CE_DISPATCHER_BIND_IMPLEMENTATION_EXT(functionalBlock, cbMEDiscoveryCommStatusIndication, cbMEPairCommStatusIndication, cbMEDiscoveryIndication, cbMEDiscoveryConfirm, cbMEDiscoveryResponseNotifyIndication, cbMEAutoDiscoveryConfirm, cbMEPairIndication, cbMEPairConfirm, cbMEUnpairIndication, cbMEUnpairConfirm,cbGenericCmdConfirm,cbGenericCmdIndication) \
    static void cbMEDiscoveryConfirm(gpRf4ce_Result_t status, UInt8 numNodes);\
    static void cbMEDiscoveryResponseNotifyIndication(gpRf4ce_NodeDesc_t *pNodeDesc, Bool *ignored);\
    static void cbMEPairConfirm(gpRf4ce_Result_t status, UInt8 pairingRef, gpRf4ce_VendorId_t recVendorId, gpPd_Offset_t recVendorStringOffset, UInt8 recAppCapabilities, gpPd_Offset_t recUserStringOffset, gpPd_Offset_t recDevTypeListOffset, gpPd_Offset_t recProfileIdListOffset, gpPd_Handle_t pdHandle);\
    static Bool cbMEUnpairIndication(UInt8 pairingRef);\
    static void cbMEUnpairConfirm(gpRf4ce_Result_t status, UInt8 pairingRef);\
    static void cbGenericCmdConfirm( gpRf4ce_Result_t result , gpRf4ce_CommandId_t genericCmdId );\
    static Bool cbGenericCmdIndication( UInt8 pairingRef , gpRf4ce_PANID_t srcPANId, MACAddress_t* pSrcAddr , UInt8 rxFlags , gpRf4ce_CommandId_t genericCmdId , UInt8 cmdPayloadLength , UInt8 *pCmdPayload );\
    \
    GP_RF4CE_DISPATCHER_CONST gpRf4ceDispatcher_BindCallbacks_t ROM functionalBlock##_BindCallbacks FLASH_PROGMEM = {\
            cbMEDiscoveryConfirm,\
            cbMEDiscoveryResponseNotifyIndication,\
            cbMEPairConfirm,\
            cbMEUnpairIndication,\
            cbMEUnpairConfirm,\
            cbGenericCmdConfirm,\
            cbGenericCmdIndication\
        }

 /** @brief This macro defines the begin of the data map. */
#define GP_RF4CE_DISPATCHER_DATA_MAP_BEGIN()  \
const gpRf4ceDispatcher_DataMapping_t ROM gpRf4ceDispatcher_DataMapping[] FLASH_PROGMEM = \
{

/** @brief This macro adds a functional block to the data map.*/
#define GP_RF4CE_DISPATCHER_DATA_MAP_ENTRY(functionalBlockID,functionalBlock)   \
    { functionalBlockID , &functionalBlock##_DataCallbacks },

/** @brief This macro defines the end of the data map. */
#define GP_RF4CE_DISPATCHER_DATA_MAP_END()  \
    { GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID  , NULL }\
};

/** @brief This macro declare a data block. */
#define GP_RF4CE_DISPATCHER_DATA_DECLARATION(functionalBlock) \
    extern GP_RF4CE_DISPATCHER_CONST gpRf4ceDispatcher_DataCallbacks_t ROM functionalBlock##_DataCallbacks FLASH_PROGMEM

/** @brief This macro implements a data block using standard names for the callback functions.  This macro (or the next) is required in the data block body file (.c file). */
#define GP_RF4CE_DISPATCHER_DATA_IMPLEMENTATION(functionalBlock) \
    GP_RF4CE_DISPATCHER_DATA_IMPLEMENTATION_EXT(functionalBlock, gpRf4ceDispatcher_cbResetIndication, gpRf4ceDispatcher_cbDEDataIndication, gpRf4ceDispatcher_cbDEDataConfirm , gpRf4ceDispatcher_cbPollIndication, gpRf4ceDispatcher_cbPollConfirm)

/** @brief This macro does the same as the macro GP_RF4CE_DISPATCHER_DATA_IMPLEMENTATION(), but it allows to specify different callback function names. */
#define GP_RF4CE_DISPATCHER_DATA_IMPLEMENTATION_EXT(functionalBlock, cbResetIndication, cbDEDataIndication, cbDEDataConfirm , cbPollIndication , cbPollConfirm ) \
    static void cbResetIndication(Bool setDefault);\
    static Bool cbDEDataIndication(UInt8 pairingRef, gpRf4ce_ProfileId_t profileId, gpRf4ce_VendorId_t vendorId, UInt8 rxFlags, UInt8 nsduLength, gpPd_Offset_t nsduOffset, gpPd_Handle_t pdHandle);\
    static void cbDEDataConfirm(gpRf4ce_Result_t status, UInt8 pairingRef, gpPd_Handle_t pdHandle);\
    static Bool cbPollIndication( UInt8 pairingRef);\
    static void cbPollConfirm( gpRf4ce_Result_t status, UInt8 pairingRef );\
    \
    GP_RF4CE_DISPATCHER_CONST gpRf4ceDispatcher_DataCallbacks_t ROM functionalBlock##_DataCallbacks FLASH_PROGMEM = { \
            cbResetIndication, \
            cbDEDataIndication,\
            cbDEDataConfirm, \
            cbPollIndication, \
            cbPollConfirm \
        }
        
/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
 
typedef void (*gpRf4ceDispatcher_cbResetIndication_t)(Bool setDefault);

typedef Bool (*gpRf4ceDispatcher_cbDEDataIndication_t)(UInt8 pairingRef, gpRf4ce_ProfileId_t profileId, gpRf4ce_VendorId_t vendorId, UInt8 rxFlags, UInt8 nsduLength, gpPd_Offset_t nsduOffset, gpPd_Handle_t pdHandle);

typedef void (*gpRf4ceDispatcher_cbDEDataConfirm_t)(gpRf4ce_Result_t status, UInt8 pairingRef, gpPd_Handle_t pdHandle);

typedef Bool (*gpRf4ceDispatcher_cbPollIndication_t)(UInt8 pairingRef);

typedef void (*gpRf4ceDispatcher_cbPollConfirm_t)(gpRf4ce_Result_t status, UInt8 pairingRef );

typedef void (*gpRf4ceDispatcher_cbMEDiscoveryCommStatusIndication_t)( gpRf4ce_Result_t status, gpRf4ce_PANID_t dstPANId, MACAddress_t* pDstAddr );

typedef void (*gpRf4ceDispatcher_cbMEPairCommStatusIndication_t)( gpRf4ce_Result_t status , UInt8 PairingRef, gpRf4ce_PANID_t dstPANId, UInt8 dstAddrMode, gpMacCore_Address_t *pDstAddr );

typedef Bool (*gpRf4ceDispatcher_cbMEDiscoveryIndication_t)(gpRf4ce_Result_t status, MACAddress_t *pSrcIEEEAddr, UInt8 orgNodeCapabilities, gpRf4ce_VendorId_t orgVendorId, gpPd_Offset_t orgVendorStringOffset, UInt8 orgAppCapabilities, gpPd_Offset_t orgUserStringOffset, gpPd_Offset_t orgDevTypeListOffset, gpPd_Offset_t orgProfileIdListOffset, gpRf4ce_DeviceType_t searchDevType, gpPd_Handle_t pdHandle);

typedef void (*gpRf4ceDispatcher_cbMEDiscoveryConfirm_t)(gpRf4ce_Result_t status, UInt8 numNodes);

typedef void (*gpRf4ceDispatcher_cbMEDiscoveryResponseNotifyIndication_t)(gpRf4ce_NodeDesc_t *pNodeDesc, Bool *ignored);

typedef void (*gpRf4ceDispatcher_cbMEAutoDiscoveryConfirm_t)(gpRf4ce_Result_t status , MACAddress_t *pSrcIEEEAddr );

typedef Bool (*gpRf4ceDispatcher_cbMEPairIndication_t)(gpRf4ce_Result_t status, gpRf4ce_PANID_t srcPANId, MACAddress_t *pSrcIEEEAddr, UInt8 orgNodeCapabilities, gpRf4ce_VendorId_t orgVendorId, gpPd_Offset_t orgVendorStringOffset, UInt8 orgAppCapabilities, gpPd_Offset_t orgUserStringOffset, gpPd_Offset_t orgDevTypeListOffset, gpPd_Offset_t orgProfileIdListOffset, UInt8 keyExTransferCount, UInt8 provPairingRef, gpPd_Handle_t pdHandle);

typedef void (*gpRf4ceDispatcher_cbMEPairConfirm_t)(gpRf4ce_Result_t status, UInt8 pairingRef, gpRf4ce_VendorId_t recVendorId, gpPd_Offset_t recVendorStringOffset, UInt8 recAppCapabilities, gpPd_Offset_t recUserStringOffset, gpPd_Offset_t recDevTypeListOffset, gpPd_Offset_t recProfileIdListOffset, gpPd_Handle_t pdHandle);

typedef Bool (*gpRf4ceDispatcher_cbMEUnpairIndication_t)(UInt8 pairingRef);

typedef void (*gpRf4ceDispatcher_cbMEUnpairConfirm_t)(gpRf4ce_Result_t status, UInt8 pairingRef);

typedef Bool (*gpRf4ceDispatcher_cbGenericCmdIndication_t)( UInt8 pairingRef , gpRf4ce_PANID_t srcPANId, MACAddress_t* pSrcAddr , UInt8 rxFlags , gpRf4ce_CommandId_t genericCmdId , UInt8 cmdPayloadLength , UInt8 *pCmdPayload );

typedef void (*gpRf4ceDispatcher_cbGenericCmdConfirm_t)( gpRf4ce_Result_t result , gpRf4ce_CommandId_t genericCmdId );

typedef struct
{
#ifdef GP_RF4CE_DIVERSITY_DISCOVERY_RECIPIENT
    gpRf4ceDispatcher_cbMEDiscoveryCommStatusIndication_t discoveryCommStatusIndicationCallback;
#endif //GP_RF4CE_DIVERSITY_DISCOVERY_RECIPIENT
#ifdef GP_RF4CE_DIVERSITY_PAIR_RECIPIENT
    gpRf4ceDispatcher_cbMEPairCommStatusIndication_t pairCommStatusIndicationCallback;
#endif //GP_RF4CE_DIVERSITY_PAIR_RECIPIENT
#ifdef GP_RF4CE_DIVERSITY_DISCOVERY_RECIPIENT
    gpRf4ceDispatcher_cbMEDiscoveryIndication_t discoveryIndicationCallback;
#endif // GP_RF4CE_DIVERSITY_DISCOVERY_RECIPIENT
    gpRf4ceDispatcher_cbMEDiscoveryConfirm_t discoveryConfirmCallback;
    gpRf4ceDispatcher_cbMEDiscoveryResponseNotifyIndication_t discoveryResponseNotifyIndicationCallback;
#if defined(GP_RF4CE_DIVERSITY_AUTO_DISCOVERY)
    gpRf4ceDispatcher_cbMEAutoDiscoveryConfirm_t autoDiscoveryConfirmCallback;
#endif //GP_DIVERSITY_RF4CE_TARGET
#ifdef GP_RF4CE_DIVERSITY_PAIR_RECIPIENT
    gpRf4ceDispatcher_cbMEPairIndication_t       pairIndicationCallback;
#endif //GP_RF4CE_DIVERSITY_PAIR_RECIPIENT
    gpRf4ceDispatcher_cbMEPairConfirm_t        pairConfirmCallback;
    gpRf4ceDispatcher_cbMEUnpairIndication_t   unpairIndicationCallback;
    gpRf4ceDispatcher_cbMEUnpairConfirm_t      unpairConfirmCallback;
#ifdef GP_RF4CE_DIVERSITY_GENERIC_CMD_RECIPIENT
    gpRf4ceDispatcher_cbGenericCmdIndication_t genericCmdIndicationCallback;
#endif //GP_RF4CE_DIVERSITY_GENERIC_CMD_RECIPIENT
#ifdef GP_RF4CE_DIVERSITY_GENERIC_CMD_ORIGINATOR
    gpRf4ceDispatcher_cbGenericCmdConfirm_t    genericCmdConfirmCallback;
#endif //GP_RF4CE_DIVERSITY_GENERIC_CMD_ORIGINATOR
}gpRf4ceDispatcher_BindCallbacks_t;

typedef struct
{
    UInt8 functionalBlockID;
    GP_RF4CE_DISPATCHER_CONST ROM gpRf4ceDispatcher_BindCallbacks_t* FLASH_PROGMEM callbacks;
}gpRf4ceDispatcher_BindMapping_t;

typedef struct
{
    gpRf4ceDispatcher_cbResetIndication_t   resetIndicationCallback;
    gpRf4ceDispatcher_cbDEDataIndication_t  dataIndicationCallback;
    gpRf4ceDispatcher_cbDEDataConfirm_t     dataConfirmCallback;
    gpRf4ceDispatcher_cbPollIndication_t    pollIndicationCallback;
    gpRf4ceDispatcher_cbPollConfirm_t       pollConfirmCallback;
}gpRf4ceDispatcher_DataCallbacks_t;

typedef struct
{
    UInt8 functionalBlockID;
    GP_RF4CE_DISPATCHER_CONST ROM gpRf4ceDispatcher_DataCallbacks_t* FLASH_PROGMEM callbacks;
}gpRf4ceDispatcher_DataMapping_t;
    
/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup INIT
 *
 *  This function initializes the RF4CE dispatcher layer. It should be called before calling any other dispatcher request function.
 *  This primitive is to be called once in the system, and is typically called by the gpBaseComps_StackInit() function of the gpBaseComps component.
 *
*/
GP_API void gpRf4ceDispatcher_Init(void);

/** @ingroup RESET
 *
 *  This function resets the RF4CE dispatcher as well as the RF4CE network. It will first reset the RF4CE network layer and than inform all
 *  registered functional blocks with a ResetIndication. Last a ResetConfirmation is given to the functional block that requested the reset.
 *
 *  @param setDefault       Indicates whether or not the internal states should be reset to the default settings. The dispatcher will pass this parameter on to the other functional blocks via the ResetIndication.
 *  @param nwkNodeCapabilities The nodeCapabilities for the RF4CE network layer.
 *
*/
GP_API void gpRf4ceDispatcher_ResetRequest(Bool setDefault , UInt8 nwkNodeCapabilities);

/** @ingroup RESET
 *
 *  This primitive is the response from the dispatcher to notify the initiating functional block that its ResetRequest has been completed.
 *
 *  @param status               The completion status of the reset request.
*/
GP_API void gpRf4ceDispatcher_cbResetConfirm(gpRf4ce_Result_t status);

/** @ingroup LOCK
 *  This function is used to request the lock for the RF4CE network layer access. This lock must be acquired by a functional block before it may access the RF4CE network layer.
 *
 *  @param fbId             The identifier of the functional block that requests the lock.
 *
 *  @return                 Whether this functional block has successfully acquired the lock on the RF4CE network layer access (true) or not (false).
*/
GP_API Bool gpRf4ceDispatcher_LockRequest(UInt8 fbId);

/** @ingroup LOCK
 *  This function is used to release the lock for the RF4CE network layer access.
 *
 *  @param fbId             The identifier of the functional block.
*/
GP_API void gpRf4ceDispatcher_ReleaseRequest(UInt8 fbId);

/** @ingroup LOCK
 *  This function can be used to check whether the functional block already has the lock on the RF4CE network layer access.
 *
 *  @param fbId             The identifier of the functional block.
 *
 *  @return                 Whether this functional block has the lock on the RF4CE network layer access (true) or not (false).
*/
GP_API Bool gpRf4ceDispatcher_LockedByThisFunctionalBlock( UInt8 fbId );

GP_API void gpRf4ceDispatcher_SetFuncBlockIdForPdHandle(UInt8 fbId, gpPd_Handle_t pdHandle);

#ifdef __cplusplus
}
#endif
#endif //_GP_RF4CE_DISPATCHER_H_
