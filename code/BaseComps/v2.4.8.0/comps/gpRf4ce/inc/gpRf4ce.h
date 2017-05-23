/*
 * Copyright (c) 2008-2014, GreenPeak Technologies
 *
 * gpRf4ce.h
 *   This file contains the definitions of the public functions and enumerations of the gpRf4ce.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpRf4ce/inc/gpRf4ce.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */


#ifndef _GP_RF4CE_H_
#define _GP_RF4CE_H_


#define GP_RF4CE_DIVERSITY_DISCOVERY_ORIGINATOR
#define GP_RF4CE_DIVERSITY_PAIR_ORIGINATOR
#define GP_RF4CE_DIVERSITY_UNPAIR_ORIGINATOR




/**
 * @file gpRf4ce.h
 *
 *
 * @defgroup NLDE_DATA NLDE-DATA primitives
 * This module groups the primitives for data transmission.
 *
 * To transmit a data package the origniator needs to:
 * - use gpPd_GetPd() to claim memory that will be used to write the data payload into the radio chip
 * - trigger gpRf4ce_DataRequest() with the claimed memory handle
 * - wait until the gpRf4ce_cbDataConfirm() callback has been called
 * - use gpPd_FreePd(pdHandle) to free the claimed memory
 *
 * The receiving device should handle an incomming packet as follows:
 * - gpRf4ce_cbDataIndication() callback indicating the data was received
 * - after reading the data from the radio chip with the gpPd_ReadByte((pdHandle, nsduOffset) function,
 *   use gpPd_FreePd(pdHandle) to free the memory
 *
 * @image latex NLDE_DATA.png "NLDE-DATA flow overview" width=12cm
 *
 * @remark It's good practice not to call a request function from a confirm funcion. Instead, update the application state
 *  so that the request is called the next time the application state machines is triggered. Nested calls of any request
 *  function in the confirm callback function should be avoided because:
 * - the confirm callback is triggered from the interrupt service routine
 * - this would create deep call trees which aren't supported on all embedded processors
 * - the rf4ce network layer doesn't queue requests
 *
 *
 *
 * @defgroup NLME_DISCOVERY NLME-DISCOVERY primitives
 * This module groups the primitives for the discovery procedure.
 *
 * A device (originator) that initiates a discovery procedure needs to:
 * - trigger gpRf4ce_DiscoveryRequest()
 * - wait until the gpRf4ce_cbDiscoveryConfirm() callback is called. During this wait period the device broadcasts
 *   multiple discovery requests on the 3 supported channel. The callback function contains a pointer to a list of received replies.
 *
 * Only target devices can reply on a discovery:
 * - when triggered by the gpRf4ce_cbDiscoveryIndication callback
 * - if wanted the device can answer any received discovery request by calling gpRf4ce_DiscoveryResponse()
 * - the status of the transmitted discovery response is returned by gpRf4ce_cbCommStatusIndication()
 *
 * @image latex NLME_DISCOVERY.png "NLME-DISCOVERY flow overview" width=12cm
 *
 * Alternatively a target device can start an auto discovery period:
 * - call gpRf4ce_AutoDiscoveryRequest()
 * - if during the defined period 2 identical discovery requests which fullfill the specified conditions are received, the target
 *   replies automatically by sending a discovery response packet.
 * - the autodiscovery window is closed if the gpRf4ce_cbAutoDiscoveryConfirm() callback is called
 *
 * @image latex NLME_AUTODISCOVERY.png "NLME-AUTODISCOVERY flow overview" width=12cm
 *
 * @remark It's good practice not to call a request function from a confirm funcion. Instead, update the application state
 *  so that the request is called the next time the application state machines is triggered. Nested calls of any request
 *  function in the confirm callback function should be avoided because:
 * - the confirm callback is triggered from the interrupt service routine
 * - this would create deep call trees which aren't supported on all embedded processors
 * - the rf4ce network layer doesn't queue requests
 *
 *
 *
 * @defgroup NLME_PAIR NLME-PAIR primitives
 * This module groups the primitives for the pairing procedure.
 *
 * A device (originator) that initiates a pairing procedure needs to:
 * - trigger gpRf4ce_PairRequest()
 * - during this procedure the security key is exchanged if required
 * - the status of the pair request is returned by the gpRf4ce_cbPairConfirm() callback function.
 *
 * Only target devices can reply on a pairing:
 * - when triggered by the gpRf4ce_cbPairIndication() callback
 * - if wanted the device can reply on a pair request by calling gpRf4ce_PairResponse()
 * - the status of the transmitted pairing response is returned by gpRf4ce_cbCommStatusIndication()
 *
 * @image latex NLME_PAIR.png "NLME-PAIR flow overview" width=12cm
 *
 * @remark It's good practice not to call a request function from a confirm funcion. Instead, update the application state
 *  so that the request is called the next time the application state machines is triggered. Nested calls of any request
 *  function in the confirm callback function should be avoided because:
 * - the confirm callback is triggered from the interrupt service routine
 * - this would create deep call trees which aren't supported on all embedded processors
 * - the rf4ce network layer doesn't queue requests
 *
 *
 *
 * @defgroup NLME_COMMSTAT NLME-COMM-STAT primitive
 * The NLME-COMM-STAT primitive is used in discovery and pairing procedures. It indicates the status of the response message transmissions.
 *
 *
 *
 * @defgroup NLME_UNPAIR NLME-UNPAIR primitives
 * This module groups the primitives for the unpairing procedure.
 *
 * A device (originator) that initiates a unpairing procedure needs to:
 * - trigger gpRf4ce_UnpairRequest(), this will remove the specified pairing entry form the pairing table
 * - the status of the pair request is returned by the gpRf4ce_cbUnpairConfirm() callback function.
 *
 * The paired device (target or controller):
 * - will be informed of the unpair action by the gpRf4ce_cbUnpairIndication() callback
 * - can decide to also remove the corresponding entry in it's own pairing tabel by calling gpRf4ce_UnpairResponse().
 *   As this action doesn't require packets to be send back to the originator of this unpair request, there is no
 *   confirmation of the response with a comm status indication.
 *
 * @image latex NLME_UNPAIR.png "NLME-UNPAIR flow overview" width=12cm
 *
 *
 *
 * @defgroup NLME_ATTRIBUTE NLME-GET and NLME-SET primitives
 * This module groups the primitives to access the NIB attributes.
 *
 * @image latex NLME_GET_SET.png "NLME-GET and NLME-SET flow overview" width=12cm
 *
 * @remark As these attribute access functions can immediatly return with a status and in case of a get with the
 *  requested data, there is no need to overload this attribute access with confirm versions of the primitive. The
 *  status is returned as return value, the requested data is passed back via a pointer argument.
 *
 *
 *
 * @defgroup NLME_RESET NLME-RESET primitives
 * This module groups the primitives for the reset procedure.
 *
 * A device can reset the rf4ce network layer by calling gpRf4ce_ResetRequest().
 *
 * @image latex NLME_RESET.png "NLME-RESET flow overview" width=5cm
 *
 *
 *
 * @defgroup NLME_START NLME-START primitives
 * This module groups the primitives for the start procedure.
 *
 * At startup a device needs to launch/init the rf4ce network by calling gpRf4ce_StartRequest(). A confirm of this start is passed by the gpRf4ce_cbStartConfirm().
 *
 * @image latex NLME_START.png "NLME-START flow overview" width=5cm
 *
 *
 *
 * @defgroup NLME_RXENABLE NLME-RX-ENABLE primitives
 * This module groups the primitives for the rx-enable procedure.
 *
 * A device can put itself to receive mode by calling gpRf4ce_RxEnableRequest().
 *
 * @image latex NLME_RXENABLE.png "NLME-RX-ENABLE flow overview" width=5cm
 *
 *
 *
 * @defgroup NLME_UPDATEKEY NLME-UPDATE-KEY primitive
 * This module groups the primitives for the key update procedure.
 *
 * A device can overwrite the used security key by calling gpRf4ce_UpdateKeyRequest(). A confirm of update action is passed by the gpRf4ce_cbUpdateKeyConfirm().
 *
 * @image latex NLME_UPDATEKEY.png "NLME-UPDATE-KEY flow overview" width=5cm
 *
 *
*/
/**
 * @defgroup INIT Initialization
 *
 *
*/



/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <global.h>
#include "gpMacCore.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


//RF4CE constants

#ifndef GP_RF4CE_NWKC_CHANNEL_1
/** First RF4CE channel */
#define GP_RF4CE_NWKC_CHANNEL_1                      15
#endif

#ifndef GP_RF4CE_NWKC_CHANNEL_2
/** Second RF4CE channel */
#define GP_RF4CE_NWKC_CHANNEL_2                      20
#endif

#ifndef GP_RF4CE_NWKC_CHANNEL_3
/** Third RF4CE channel */
#define GP_RF4CE_NWKC_CHANNEL_3                      25
#endif

/** The amount that needs to be added to the frame counter if a device is reset. */
#define GP_RF4CE_NWKC_FRAME_COUNTER_WINDOW              1024
/** The length, in octets, of the MAC beacon payload field, as used by the RF4CE protocol. */
#define GP_RF4CE_NWKC_MAC_BEACON_PAYLOAD_LENGTH         2
/** The maximum duty cycle in MAC symbols, permitted for a power saving device.(1s) */

#ifndef GP_RF4CE_NWKC_MAX_DUTYCYCLE
#define GP_RF4CE_NWKC_MAX_DUTYCYCLE                     62500UL
#endif //GP_RF4CE_NWKC_MAX_DUTYCYCLE
#define GP_RF4CE_NWKC_MAX_DUTYCYCLE_MS                  GP_RF4CE_NWKC_MAX_DUTYCYCLE*GP_MACCORE_SYMBOL_DURATION
#define GP_RF4CE_NWKC_MAX_KEY_SEED_WAIT_TIME            3750L
/** The minimum receiver on time, in MAC symbols, permitted for a power saving device.(16.8ms) */
#define GP_RF4CE_NWKC_MIN_ACTIVE_PERIOD                 1050
/** The minimum number of pairing table entries that a controller device shall support. */
#define GP_RF4CE_NWKC_MIN_CONTROLLER_PAIRING_TABLE_SIZE 1
/** The minimum number of entries that must be supported in the node descriptor table generated through the discovery process. */
#define GP_RF4CE_NWKC_MIN_NODE_DESC_TABLE_SIZE          3
/** The minimum overhead the NWK layer adds to an application payload */
#define GP_RF4CE_NWKC_MIN_NWK_HEADER_OVERHEAD           5
/** The maximum size for the application payload of a gpRf4ce_DataRequest */
#define GP_RF4CE_NWKC_MAX_PAYLOAD_SIZE                  (GP_MACCORE_MAX_MAC_SAVE_PAYLOAD_SIZE - GP_RF4CE_NWKC_MIN_NWK_HEADER_OVERHEAD)
/** The minimum number of pairing table entries that a target device shall support. */
#define GP_RF4CE_NWKC_MIN_TARGET_PAIRING_TABLE_SIZE     5
/** The identifier of the NWK layer protocol being used by this device. */
#define GP_RF4CE_NWKC_PROTOCOL_IDENTIFIER               0xCE
/** The version of the RF4CE protocol implemented on this device.  */
#define GP_RF4CE_NWKC_PROTOCOL_VERSION                  1
/** The maximum acceptable power (in dBm) at which commands relating to security should be sent. */
#define GP_RF4CE_NWKC_MAX_SEC_CMD_TX_POWER              -15
/** The default response wait time*/
#define GP_RF4CE_NWKC_DEFAULT_RESPONSE_WAIT_TIME        0x186a

//Implementation specific RF4CE constants

/** The maximum number of entries supported in the pairing table. */
#ifndef GP_RF4CE_NWKC_MAX_PAIRING_TABLE_ENTRIES
#define GP_RF4CE_NWKC_MAX_PAIRING_TABLE_ENTRIES 1
#endif


/** Maximum vendor string length */
#define GP_RF4CE_MAX_VENDOR_STRING_LENGTH               7
/** Maximum user string length */
#define GP_RF4CE_MAX_USER_STRING_LENGTH                 15

/** The length of the authentication tag used in RF4CE = 32 bit  */
#define GP_RF4CE_SEC_MIC_LENGTH                         4


//implementation dependend max size constants for arrays
#ifndef GP_RF4CE_MAX_DISCOVERY_PROFILEID_LIST_SIZE
#define GP_RF4CE_MAX_DISCOVERY_PROFILEID_LIST_SIZE 7
#endif // !GP_RF4CE_MAX_DISCOVERY_PROFILEID_LIST_SIZE
#ifndef GP_RF4CE_MAX_DISCOVERY_DEVTYPE_LIST_SIZE
#define GP_RF4CE_MAX_DISCOVERY_DEVTYPE_LIST_SIZE 3
#endif // !GP_RF4CE_MAX_DISCOVERY_DEVTYPE_LIST_SIZE
#ifndef GP_RF4CE_MAX_NODE_DESCRIPTOR_LIST_SIZE
#define GP_RF4CE_MAX_NODE_DESCRIPTOR_LIST_SIZE 1
#endif // !GP_RF4CE_MAX_NODE_DESCRIPTOR_LIST_SIZE
#ifndef GP_RF4CE_MAX_PANID_CANDIDATES
#define GP_RF4CE_MAX_PANID_CANDIDATES 4
#endif // !GP_RF4CE_MAX_PANID_CANDIDATES



/** This function sets bits in the frame control field of the RF4CE header.
 *
 * @param  bf  The buffer that contains the frame control byte.
 * @param  bm  The shifted bit mask of the bits to set.
 * @param  idx The bit index of the bits to set.
 * @param  v   The value to set.
 */
#define BF_SET(bf,bm,idx,v)     bf &= ~bm; bf |= (v << idx)
/** This function sets bits in the frame control field of the RF4CE header.
 *
 * @param  bf  The buffer that contains the frame control byte.
 * @param  bm  The shifted bit mask of the bits to get.
 * @param  idx The bit index of the bits to get.
 * @param  v   The value to check.
 */
#define BF_IS(bf,bm,idx,v)      ((bf & bm) == (v << idx))


//Addr Modes
/** Network address mode (short address) */
#define GP_RF4CE_ADDR_MODE_NWK  0x2
/** IEEE address mode (extended address) */
#define GP_RF4CE_ADDR_MODE_IEEE 0x3

//TxOptions
/** TX options transmission mode index */
#define GP_RF4CE_TXOPTION_TRANS_MODE_IDX                    0
/** TX options addressing mode index */
#define GP_RF4CE_TXOPTION_DST_ADDR_MODE_IDX                 1
/** TX options acknowledging mode index */
#define GP_RF4CE_TXOPTION_ACK_MODE_IDX                      2
/** TX options security mode index */
#define GP_RF4CE_TXOPTION_SECURITY_MODE_IDX                 3
/** TX options channel agility mode index */
#define GP_RF4CE_TXOPTION_CHANNEL_AGILITY_MODE_IDX          4
/** TX options channel normalization mode index */
#define GP_RF4CE_TXOPTION_CHANNEL_NORMALIZATION_MODE_IDX    5
/** TX options payload mode index */
#define GP_RF4CE_TXOPTION_PAYLOAD_MODE_IDX                  6

/** TX options transmission mode bit mask */
#define GP_RF4CE_TXOPTION_TRANS_MODE_BM                     0x01
/** TX options addressing mode bit mask */
#define GP_RF4CE_TXOPTION_DST_ADDR_MODE_BM                  0x02
/** TX options acknowledging mode bit mask */
#define GP_RF4CE_TXOPTION_ACK_MODE_BM                       0x04
/** TX options security mode bit mask */
#define GP_RF4CE_TXOPTION_SECURITY_MODE_BM                  0x08
/** TX options channel agility mode bit mask */
#define GP_RF4CE_TXOPTION_CHANNEL_AGILITY_MODE_BM           0x10
/** TX options channel normalization mode bit mask */
#define GP_RF4CE_TXOPTION_CHANNEL_NORMALIZATION_MODE_BM     0x20
/** TX options payload mode bit mask */
#define GP_RF4CE_TXOPTION_PAYLOAD_MODE_BM                   0x40
/** TX options indirect tx bit mask */
#define GP_RF4CE_TXOPTION_INDIRECT_TX_BM                    0x80

/** Check for broadcast transmission in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_IS_BROADCAST(o)                   (!!(o & GP_RF4CE_TXOPTION_TRANS_MODE_BM))
/** Check for unicast transmission in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_IS_UNICAST(o)                     !(o & GP_RF4CE_TXOPTION_TRANS_MODE_BM)
/**  Check for IEEE destination address in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_IS_IEEE_DST_ADDR(o)               (!!(o & GP_RF4CE_TXOPTION_DST_ADDR_MODE_BM))
/**  Check for network destination address in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_IS_NWK_DST_ADDR(o)                !(o & GP_RF4CE_TXOPTION_DST_ADDR_MODE_BM)
/**  Check for acknowledge mode in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_IS_ACK(o)                         (!!(o & GP_RF4CE_TXOPTION_ACK_MODE_BM) && GP_RF4CE_TXOPTION_IS_UNICAST(o))
/**  Check for security mode in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_IS_SECURE(o)                      (!!(o & GP_RF4CE_TXOPTION_SECURITY_MODE_BM) && GP_RF4CE_TXOPTION_IS_UNICAST(o))
/**  Check for single channel mode in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_IS_SINGLE_CHANNEL(o)              (!!(o & GP_RF4CE_TXOPTION_CHANNEL_AGILITY_MODE_BM))
/**  Check for multiple channel mode in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_IS_MULTIPLE_CHANNEL(o)            !(o & GP_RF4CE_TXOPTION_CHANNEL_AGILITY_MODE_BM)
/**  Check for valid channel designator in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_IS_CHANNEL_DESIGNATOR(o)          (!!(o & GP_RF4CE_TXOPTION_CHANNEL_NORMALIZATION_MODE_BM))
/**  Check for vendor specific data in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_IS_VENDOR_SPECIFIC(o)             (!!(o & GP_RF4CE_TXOPTION_PAYLOAD_MODE_BM))
/**  Check for inderect transmission in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_IS_INDIRECT_TX(o)             (!!(o & GP_RF4CE_TXOPTION_INDIRECT_TX_BM))

/** Set broadcast transmission in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_SET_BROADCAST(o)                  o |= GP_RF4CE_TXOPTION_TRANS_MODE_BM
/** Set unicast transmission in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_SET_UNICAST(o)                    o &= ~GP_RF4CE_TXOPTION_TRANS_MODE_BM
/**  Set IEEE destination address in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_SET_IEEE_DST_ADDR(o)              o |= GP_RF4CE_TXOPTION_DST_ADDR_MODE_BM
/**  Set network destination address in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_SET_NWK_DST_ADDR(o)               o &= ~GP_RF4CE_TXOPTION_DST_ADDR_MODE_BM
/**  Enable acknowledge mode in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_ENABLE_ACK(o)                     o |= GP_RF4CE_TXOPTION_ACK_MODE_BM
/**  Disable for acknowledge mode in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_DISABLE_ACK(o)                    o &= ~GP_RF4CE_TXOPTION_ACK_MODE_BM
/** Enable security in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_ENABLE_SECURITY(o)                o |= GP_RF4CE_TXOPTION_SECURITY_MODE_BM
/** Disable security in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_DISABLE_SECURITY(o)               o &= ~GP_RF4CE_TXOPTION_SECURITY_MODE_BM
/**  Set single channel mode in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_SET_SINGLE_CHANNEL(o)             o |= GP_RF4CE_TXOPTION_CHANNEL_AGILITY_MODE_BM
/**  Set multiple channel mode in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_SET_MULTIPLE_CHANNEL(o)           o &= ~GP_RF4CE_TXOPTION_CHANNEL_AGILITY_MODE_BM
/** Enable channel designator in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_ENABLE_CHANNEL_DESIGNATOR(o)      o |= GP_RF4CE_TXOPTION_CHANNEL_NORMALIZATION_MODE_BM
/** Disable channel designator in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_DISABLE_CHANNEL_DESIGNATOR(o)     o &= ~GP_RF4CE_TXOPTION_CHANNEL_NORMALIZATION_MODE_BM
/** Enable vendor specific mode in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_ENABLE_VENDOR_SPECIFIC(o)         o |= GP_RF4CE_TXOPTION_PAYLOAD_MODE_BM
/** Disable vendor specific mode in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_DISABLE_VENDOR_SPECIFIC(o)        o &= ~GP_RF4CE_TXOPTION_PAYLOAD_MODE_BM
/** Enable indirect tx mode in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_ENABLE_INDIRECT_TX(o)             o |= GP_RF4CE_TXOPTION_INDIRECT_TX_BM
/** Disable indirect tx mode in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_DISABLE_INDIRECT_TX(o)            o &= ~GP_RF4CE_TXOPTION_INDIRECT_TX_BM
/**  Set short multiple channel mode in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_SET_SHORT_MULTIPLE_CHANNEL(o)      o |= GP_RF4CE_TXOPTION_SHORT_CHANNEL_AGILITY_MODE_BM))
/**  Set normal multiple channel mode in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_TXOPTION_SET_NORMAL_MULTIPLE_CHANNEL(o)       o &= ~GP_RF4CE_TXOPTION_SHORT_CHANNEL_AGILITY_MODE_BM))

//RxFlags
/** Receice mode bit index in RX flags */
#define GP_RF4CE_RXFLAGS_RX_MODE_IDX                        0
/** Security mode bit index in RX flags */
#define GP_RF4CE_RXFLAGS_SECURITY_MODE_IDX                  1
/** Payload mode bit index in RX flags */
#define GP_RF4CE_RXFLAGS_PAYLOAD_MODE_IDX                   2

/** Receice mode bit mask in RX flags */
#define GP_RF4CE_RXFLAGS_RX_MODE_BM                         0x01
/** Security mode bit mask in RX flags */
#define GP_RF4CE_RXFLAGS_SECURITY_MODE_BM                   0x02
/** Payload mode bit mask in RX flags */
#define GP_RF4CE_RXFLAGS_PAYLOAD_MODE_BM                    0x04

/** Check for broadcast transmission in receive flags
 *  @param f Receive flags
 */
#define GP_RF4CE_RXFLAGS_IS_BROADCAST(f)                    (!!(f & GP_RF4CE_RXFLAGS_RX_MODE_BM))
/** Check for unicast transmission in receive flags
 *  @param f Receive flags
 */
#define GP_RF4CE_RXFLAGS_IS_UNICAST(f)                      !(f & GP_RF4CE_RXFLAGS_RX_MODE_BM)
/** Check for secured transmission in receive flags
 *  @param f Receive flags
 */
#define GP_RF4CE_RXFLAGS_IS_SECURED(f)                      (!!(f & GP_RF4CE_RXFLAGS_SECURITY_MODE_BM))
/** Check for vendor specific transmission in receive flags
 *  @param f Receive flags
 */
#define GP_RF4CE_RXFLAGS_IS_VENDOR_SPECIFIC(f)              (!!(f & GP_RF4CE_RXFLAGS_PAYLOAD_MODE_BM))

/** Set broadcast transmission in receive flags
 *  @param f Receive flags
 */
#define GP_RF4CE_RXFLAGS_SET_BROADCAST(f)                   f |= GP_RF4CE_RXFLAGS_RX_MODE_BM
/** Set unicast transmission in receive flags
 *  @param f Receive flags
 */
#define GP_RF4CE_RXFLAGS_SET_UNICAST(f)                     f &= ~GP_RF4CE_RXFLAGS_RX_MODE_BM
/** Enable secured transmission in receive flags
 *  @param f Receive flags
 */
#define GP_RF4CE_RXFLAGS_ENABLE_SECURITY(f)                 f |= GP_RF4CE_RXFLAGS_SECURITY_MODE_BM
/** Disable secured transmission in receive flags
 *  @param f Receive flags
 */
#define GP_RF4CE_RXFLAGS_DISABLE_SECURITY(f)                f &= ~GP_RF4CE_RXFLAGS_SECURITY_MODE_BM
/** Enable vendor specific transmission in receive flags
 *  @param f Receive flags
 */
#define GP_RF4CE_RXFLAGS_ENABLE_VENDOR_SPECIFIC(f)          f |= GP_RF4CE_RXFLAGS_PAYLOAD_MODE_BM
/** Disable vendor specific transmission in receive flags
 *  @param f Receive flags
 */
#define GP_RF4CE_RXFLAGS_DISABLE_VENDOR_SPECIFIC(f)         f &= ~GP_RF4CE_RXFLAGS_PAYLOAD_MODE_BM

//Node Capabilities
/** Type bit index in node capabilities */
#define GP_RF4CE_NODE_CAPABILITY_NODE_TYPE_IDX              0
/** Power source bit index in node capabilities */
#define GP_RF4CE_NODE_CAPABILITY_POWER_SRC_IDX              1
/** Security bit index in node capabilities */
#define GP_RF4CE_NODE_CAPABILITY_SECURITY_IDX               2
/** Channel normalization bit index in node capabilities */
#define GP_RF4CE_NODE_CAPABILITY_CHANNEL_NORMALIZATION_IDX  3

/** Type bit mask in node capabilities */
#define GP_RF4CE_NODE_CAPABILITY_NODE_TYPE_BM               0x01
/** Power source bit mask in node capabilities */
#define GP_RF4CE_NODE_CAPABILITY_POWER_SRC_BM               0x02
/** Security bit mask in node capabilities */
#define GP_RF4CE_NODE_CAPABILITY_SECURITY_BM                0x04
/** Channel normalization bit mask in node capabilities */
#define GP_RF4CE_NODE_CAPABILITY_CHANNEL_NORMALIZATION_BM   0x08

/** Check for target in node capabilities
 *  @param c Node capabilities
 */
#define GP_RF4CE_NODE_CAPABILITY_IS_TARGET(c)               (!!(c & GP_RF4CE_NODE_CAPABILITY_NODE_TYPE_BM))
/** Check for controller in node capabilities
 *  @param c Node capabilities
 */
#define GP_RF4CE_NODE_CAPABILITY_IS_CONTROLLER(c)           !(c & GP_RF4CE_NODE_CAPABILITY_NODE_TYPE_BM)
/** Check for power source in node capabilities
 *  @param c Node capabilities
 */
#define GP_RF4CE_NODE_CAPABILITY_IS_POWERED(c)              (!!(c & GP_RF4CE_NODE_CAPABILITY_POWER_SRC_BM))
/**  Check for security in node capabilities
 *  @param c Node capabilities
 */
#define GP_RF4CE_NODE_CAPABILITY_IS_SECURED(c)              (!!(c & GP_RF4CE_NODE_CAPABILITY_SECURITY_BM))
/** Check for channel normalization in node capabilities
 *  @param c Node capabilities
 */
#define GP_RF4CE_NODE_CAPABILITY_IS_CHANNEL_NORMALIZED(c)   (!!(c & GP_RF4CE_NODE_CAPABILITY_CHANNEL_NORMALIZATION_BM))

//Application Capabilities
/** User string bit index in application capabilities */
#define GP_RF4CE_APP_CAPABILITY_USER_STRING_IDX             0
/** Device bit index in application capabilities */
#define GP_RF4CE_APP_CAPABILITY_NBR_DEVICES_IDX             1
/** Number of profiles bit index in application capabilities */
#define GP_RF4CE_APP_CAPABILITY_NBR_PROFILES_IDX            4

/** User string bit mask in application capabilities */
#define GP_RF4CE_APP_CAPABILITY_USER_STRING_BM              0x01
/** Device bit mask in application capabilities */
#define GP_RF4CE_APP_CAPABILITY_NBR_DEVICES_BM              0x06
/** Number of profiles bit mask in application capabilities */
#define GP_RF4CE_APP_CAPABILITY_NBR_PROFILES_BM             0x70

/** Enable user string in application capabilities
 *  @param c Application capabilities
 */
#define GP_RF4CE_APP_CAPABILITY_ENABLE_USER_STRING(c)       c |= GP_RF4CE_APP_CAPABILITY_USER_STRING_BM
/** Disable user string in application capabilities
 *  @param c Application capabilities
 */
#define GP_RF4CE_APP_CAPABILITY_DISABLE_USER_STRING(c)      c &= ~GP_RF4CE_APP_CAPABILITY_USER_STRING_BM
/** Check for user string in application capabilities
 *  @param c Application capabilities
 */
#define GP_RF4CE_APP_CAPABILITY_HAS_USER_STRING(c)          BF_IS(c,GP_RF4CE_APP_CAPABILITY_USER_STRING_BM,GP_RF4CE_APP_CAPABILITY_USER_STRING_IDX,1)
/** Set number of devices in application capabilities
 *  @param c Application capabilities
 *  @param v value to set
 */
#define GP_RF4CE_APP_CAPABILITY_SET_NBR_DEVICES(c,v)        BF_SET(c,GP_RF4CE_APP_CAPABILITY_NBR_DEVICES_BM,GP_RF4CE_APP_CAPABILITY_NBR_DEVICES_IDX,v)
/** Get number of devices in application capabilities
 *  @param c Application capabilities
 */
#define GP_RF4CE_APP_CAPABILITY_GET_NBR_DEVICES(c)          BF(c,GP_RF4CE_APP_CAPABILITY_NBR_DEVICES_BM,GP_RF4CE_APP_CAPABILITY_NBR_DEVICES_IDX)
/** Set number of profiles in application capabilities
 *  @param c Application capabilities
 *  @param v value to set
 */
#define GP_RF4CE_APP_CAPABILITY_SET_NBR_PROFILES(c,v)       BF_SET(c,GP_RF4CE_APP_CAPABILITY_NBR_PROFILES_BM,GP_RF4CE_APP_CAPABILITY_NBR_PROFILES_IDX,v)
/** Get number of profiles in application capabilities
 *  @param c Application capabilities
 */
#define GP_RF4CE_APP_CAPABILITY_GET_NBR_PROFILES(c)         BF(c,GP_RF4CE_APP_CAPABILITY_NBR_PROFILES_BM,GP_RF4CE_APP_CAPABILITY_NBR_PROFILES_IDX)



#define GP_RF4CE_PAIRING_NVM_START      offsetof(gpRf4ce_PairingTableEntryTotal_t,pub.dstLogicalChannel)
//Only public pairing entry needs to be stored
#define GP_RF4CE_PAIRING_NVM_END        sizeof(gpRf4ce_PairingTableEntry_t)

#define GP_RF4CE_MAX_FILTERS       3
#define GP_RF4CE_MAX_FILTER_LENGTH 10

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/** @name gpRf4ce_Result_t */
//@{
/** @brief The requested operation was completed successfully. */
#define gpRf4ce_ResultSuccess                   0x00
/** @brief A pairing link cannot be established since the originator node has reached its maximum number of entries in its pairing table. */
#define gpRf4ce_ResultNoOrgCapacity             0xb0
/** @brief A pairing link cannot be established since the recipient node has reached its maximum number of entries in its pairing table. */
#define gpRf4ce_ResultNoRecCapacity             0xb1
/** @brief A pairing table entry could not be found that corresponds to the supplied pairing reference. */
#define gpRf4ce_ResultNoPairing                 0xb2
/** @brief A response frame was not received within nwkResponseWaitTime. */
#define gpRf4ce_ResultNoResponse                0xb3
/** @brief A pairing request was denied by the recipient node or an attempt to update a security link key was not possible due to one or more nodes not supporting security. */
#define gpRf4ce_ResultNotPermitted              0xb4
/** @brief A duplicate pairing table entry was detected following the receipt of a pairing command frame. */
#define gpRf4ce_ResultDuplicatePairing          0xb5
/** @brief The frame counter has reached its maximum value. */
#define gpRf4ce_ResultFrameCounterExpired       0xb6
/** @brief Too many unique discovery request or response command frames were received than requested. */
#define gpRf4ce_ResultDiscoveryError            0xb7
/** @brief No discovery request or response command frames were received during discovery. */
#define gpRf4ce_ResultDiscoveryTimeout          0xb8
/** @brief The security link key exchange or recovery procedure did not complete within the required time. */
#define gpRf4ce_ResultSecurityTimeout           0xb9
/** @brief A security link key was not successfully established between both ends of a pairing link. */
#define gpRf4ce_ResultSecurityFailure           0xba
/** @brief The beacon was lost following a synchronization request. */
#define gpRf4ce_ResultBeaconLoss                0xe0
/** @brief A transmission could not take place due to activity on the channel, i.e., the CSMA-CA mechanism has failed. */
#define gpRf4ce_ResultChannelAccessFailure      0xe1
/** @brief The GTS request has been denied by the PAN coordinator. */
#define gpRf4ce_ResultDenied                    0xe2
/** @brief The attempt to disable the transceiver has failed. */
#define gpRf4ce_ResultDisableTrxFailure         0xe3
/** @brief The received frame induces a failed security check according to the security suite. */
#define gpRf4ce_ResultFailedSecurityCheck       0xe4
/** @brief The frame resulting from secure processing has a length that is greater than aMACMaxFrameSize. */
#define gpRf4ce_ResultFrameTooLong              0xe5
/** @brief The requested GTS transmission failed because the specified GTS either did not have a transmit GTS direction or was not defined. */
#define gpRf4ce_ResultInvalidGTS                0xe6
/** @brief A request to purge an MSDU from the transaction queue was made using an MSDU handle that was not found in the transaction table. */
#define gpRf4ce_ResultInvalidHandle             0xe7
/** @brief A parameter in the primitive is either not supported or is out of the valid range. */
#define gpRf4ce_ResultInvalidParameter          0xe8
/** @brief No acknowledgment was received after aMaxFrameRetries. */
#define gpRf4ce_ResultNoAck                     0xe9
/** @brief A scan operation failed to find any network beacons. */
#define gpRf4ce_ResultNoBeacon                  0xea
/** @brief No response data was available following a request. */
#define gpRf4ce_ResultNoData                    0xeb
/** @brief The operation failed because a short address was not allocated. */
#define gpRf4ce_ResultNoShortAddress            0xec
/** @brief A request to enable the receiver was unsuccessful because it could not be completed within the CAP. */
#define gpRf4ce_ResultOutOfCAP                  0xed
/** @brief A PAN identifier conflict has been detected and communicated to the PAN coordinator. */
#define gpRf4ce_ResultPanIdConflict             0xee
/** @brief A coordinator realignment command has been received. */
#define gpRf4ce_ResultRealignment               0xef
/** @brief The transaction has expired and its information is discarded. */
#define gpRf4ce_ResultTransactionExpired        0xf0
/** @brief There is no capacity to store the transaction. */
#define gpRf4ce_ResultTransactionOverflow       0xf1
/** @brief The transceiver was transmitting when the receiver was requested to be enabled. */
#define gpRf4ce_ResultTxActive                  0xf2
/** @brief The appropriate key is not available in the ACL. */
#define gpRf4ce_ResultUnavailableKey            0xf3
/** @brief A SET/GET request was issued with the identifier of a PIBattribute that is not supported. */
#define gpRf4ce_ResultUnsupportedAttribute      0xf4
/** @brief An attempt to write to a NIB attribute that is in a table failed because the specified table index was out of range. */
#define gpRf4ce_ResultInvalidIndex              0xf9
/** @typedef gpRf4ce_Result_t
 *  @brief The gpRf4ce_Result_t type defines the result as specified in the RF4Ce specification.
*/
typedef UInt8 gpRf4ce_Result_t;
//@}

/** @name gpRf4ce_Attribute_t */
//@{
/** @brief The active period of a device in MAC symbols. */
#define gpRf4ce_AttributeNwkActivePeriod                        0x60
/** @brief The logical channel that was chosen when the RC PAN was formed. (Target device only) */
#define gpRf4ce_AttributeNwkBaseChannel                         0x61
/** @brief The LQI threshold below which discovery requests will be rejected. */
#define gpRf4ce_AttributeNwkDiscoveryLQIThreshold               0x62
/** @brief The interval at which discovery attempts are made on all channels. */
#define gpRf4ce_AttributeNwkDiscoveryRepetitionInterval         0x63
/** @brief The duty cycle of a device in MAC symbols. A value of 0x000000 indicates the device is not using power saving. */
#define gpRf4ce_AttributeNwkDutyCycle                           0x64
/** @brief The frame counter added to the transmitted NPDU. */
#define gpRf4ce_AttributeNwkFrameCounter                        0x65
/** @brief Indicates whether the ME indicates the reception of discovery request command frames to the application. TRUE indicates that the ME notifies the application.  */
#define gpRf4ce_AttributeNwkIndicateDiscoveryRequests           0x66
/** @brief The power save mode of the node. TRUE indicates that the device is operating in power save mode. */
#define gpRf4ce_AttributeNwkInPowerSave                         0x67
/** @brief The pairing table managed by the device. */
#define gpRf4ce_AttributeNwkPairingTable                        0x68
/** @brief The maximum number of discovery attempts made at the nwkDiscoveryRepetitionInterval rate. */
#define gpRf4ce_AttributeNwkMaxDiscoveryRepetitions             0x69
/** @brief The maximum number of backoffs the MAC CSMACA algorithm will attempt before declaring a channel access failure for the first transmission attempt. */
#define gpRf4ce_AttributeNwkMaxFirstAttemptCSMABackoffs         0x6a
/** @brief The maximum number of MAC retries allowed after a transmission failure for the first transmission attempt. */
#define gpRf4ce_AttributeNwkMaxFirstAttemptFrameRetries         0x6b
/** @brief The maximum number of node descriptors that can be obtained before reporting to the application. */
#define gpRf4ce_AttributeNwkMaxReportedNodeDescriptors          0x6c
/** @brief The maximum time in MAC symbols, a device shall wait for a response command frame following a request command frame. */
#define gpRf4ce_AttributeNwkResponseWaitTime                    0x6d
/** @brief A measure of the duration of a scanning operation. */
#define gpRf4ce_AttributeNwkScanDuration                        0x6e
/** @brief The user defined character string used to identify this node. */
#define gpRf4ce_AttributeNwkUserString                          0x6f
/** @typedef gpRf4ce_Attribute_t
 *  @brief The gpRf4ce_Attribute_t type defines the attribte type.
*/
typedef UInt8 gpRf4ce_Attribute_t;
//@}

/** @name gpRf4ce_CommandId_t */
//@{
//Command Identifiers
#define gpRf4ce_CommandIdDiscoveryRequest       0x01
#define gpRf4ce_CommandIdDiscoveryResponse      0x02
#define gpRf4ce_CommandIdPairRequest            0x03
#define gpRf4ce_CommandIdPairResponse           0x04
#define gpRf4ce_CommandIdUnpairRequest          0x05
#define gpRf4ce_CommandIdKeySeed                0x06
#define gpRf4ce_CommandIdPingRequest            0x07
#define gpRf4ce_CommandIdPingResponse           0x08
/** @typedef gpRf4ce_CommandId_t
 *  @brief The gpRf4ce_CommandId_t type defines the RF4CE command.
*/
typedef UInt8 gpRf4ce_CommandId_t;
//@}

/** @name gpRf4ce_DeviceType_t */
//@{
#define gpRf4ce_DeviceTypeReserved                      0
#define gpRf4ce_DeviceTypeRemoteControl                 1
#define gpRf4ce_DeviceTypeTelevision                    2
#define gpRf4ce_DeviceTypeProjector                     3
#define gpRf4ce_DeviceTypePlayer                        4
#define gpRf4ce_DeviceTypeRecorder                      5
#define gpRf4ce_DeviceTypeVideoPlayerRecorder           6
#define gpRf4ce_DeviceTypeAudioPlayerRecorder           7
#define gpRf4ce_DeviceTypeAudioVideoRecorder            8
#define gpRf4ce_DeviceTypeSetTopBox                     9
#define gpRf4ce_DeviceTypeHomeTheaterSystem             10
#define gpRf4ce_DeviceTypeMediaCenterPC                 11
#define gpRf4ce_DeviceTypeGameConsole                   12
#define gpRf4ce_DeviceTypeSatelliteRadioReceiver        13
#define gpRf4ce_DeviceTypeIRExtender                    14
#define gpRf4ce_DeviceTypeMonitor                       15
#define gpRf4ce_DeviceTypeGeneric                       0xfe
#define gpRf4ce_DeviceTypeWildcard                      0xff
/** @typedef gpRf4ce_DeviceType_t
 *  @brief The gpRf4ce_DeviceType_t type defines the RF4CE device type.
 */
typedef UInt8 gpRf4ce_DeviceType_t;
//@}

/** @name gpRf4ce_ProfileId_t */
//@{
#define gpRf4ce_ProfileIdGdp            0x00
#define gpRf4ce_ProfileIdZrc            0x01
#define gpRf4ce_ProfileIdZid            0x02
#define gpRf4ce_ProfileIdZrc2_0         0x03
// When adding a GreenPeak specific profile - please add this one to the wiki also:
// http://wiki.greenpeak.com/index.php/GreenPeak-specific_RF4CE_Profile_ID
#define gpRf4ce_ProfileIdMso            0xC0
#define gpRf4ce_ProfileIdMsoVoice       0xC1
#define gpRf4ce_ProfileIdVoice          gpRf4ce_ProfileIdMsoVoice
#define gpRf4ce_ProfileIdMsoOta         0xC2
#define gpRf4ce_ProfileIdWildcard       0xFF
/** @typedef gpRf4ce_ProfileId_t
 *  @brief The gpRf4ce_ProfileId_t type defines the RF4CE profile identifier.
 */
typedef UInt8 gpRf4ce_ProfileId_t;
//@}

/** @name gpRf4ce_VendorId_t */
//@{
#define gpRf4ce_VendorIdReserved                    0x0000
#define gpRf4ce_VendorIdAlps                        0x10AE
#define gpRf4ce_VendorIdComcast                     0x109D
#define gpRf4ce_VendorIdEchoStart                   0x10B1
#define gpRf4ce_VendorIdFreescale                   0x0005
#define gpRf4ce_VendorIdGreenpeak                   0x10D0
#define gpRf4ce_VendorIdPace                        0x10FD
#define gpRf4ce_VendorIdLightspeedTechnologies      0x10AA
#define gpRf4ce_VendorIdMotorola                    0x10BC
#define gpRf4ce_VendorIdNecElectronics              0x109E
#define gpRf4ce_VendorIdOkiSemiconductor            0x0006
#define gpRf4ce_VendorIdPanasonic                   0x0001
#define gpRf4ce_VendorIdPhilips                     0x0004
#define gpRf4ce_VendorIdSamsung                     0x0003
#define gpRf4ce_VendorIdSanJuanSoftware             0x1006
#define gpRf4ce_VendorIdSmk                         0x1081
#define gpRf4ce_VendorIdSony                        0x0002
#define gpRf4ce_VendorIdTexasInstruments            0x0007
#define gpRf4ce_VendorIdToshiba                     0x1086
#define gpRf4ce_VendorIdTrendElectronics            0x1116
#define gpRf4ce_VendorIdTestVendor1                 0xFFF1
#define gpRf4ce_VendorIdTestVendor2                 0xFFF2
#define gpRf4ce_VendorIdTestVendor3                 0xFFF3
#define gpRf4ce_VendorIdWildCard                    0xFFFF

/** @typedef gpRf4ce_VendorId_t
 *  @brief The gpRf4ce_VendorId_t type defines the RF4CE vendor identifier.
*/
typedef UInt16 gpRf4ce_VendorId_t;
//@}


/** @typedef gpRf4ce_VendorString_t
 *  The gpRf4ce_VendorString_t type specifies the manufacturer specific vendor string.
*/
typedef struct gpRf4ce_VendorString {
/** Vendor string array */
    char str[GP_RF4CE_MAX_VENDOR_STRING_LENGTH];
} gpRf4ce_VendorString_t;

/** @typedef gpRf4ce_UserString_t
 *  The gpRf4ce_UserString_t type specifies the application specific user string.
*/
typedef struct gpRf4ce_UserString {
/** User string array */
    char str[GP_RF4CE_MAX_USER_STRING_LENGTH];
} gpRf4ce_UserString_t;


/** @typedef gpRf4ce_PANID_t
 *  The gpRf4ce_PANID_t type specifies a PANId (Personal Area Network Identifier). The PANId is a unique identifier of the network. The length of the PANId is 2 Bytes.
*/
typedef UInt16 gpRf4ce_PANID_t;


/** @struct gpRf4ce_PairingTableEntry
 *  The gpRf4ce_PairingTableEntry structure specifies the fields that are contained in the pairing table.
 *  @typedef gpRf4ce_PairingTableEntry_t
 *  The gpRf4ce_PairingTableEntry_t type specifies the fields that are contained in the pairing table.
*/
typedef struct gpRf4ce_PairingTableEntry{
/** The frame counter last received from the recipient node. */
    UInt32 frameCounter;
/** The expected channel of the destination device. */
    UInt8 dstLogicalChannel;
/** The node capabilities of the recipient node. */ /*Start of NVM storage defined in GP_RF4CE_PAIRING_NVM_START*/
    UInt8 recipientCapabilities;
/** The network address to be assumed by the source device. */
    UInt16 srcNwkAddr;
/** The network address of the destination device. */
    UInt16 dstNwkAddr;
/** The IEEE address of the destination device. */
    MACAddress_t dstIEEEAddr;
/** The PAN identifier of the destination device. */
    UInt16 dstPANID;
/** The link key to be used to secure this pairing link. */
    UInt32  securityLinkKey[4];
} gpRf4ce_PairingTableEntry_t;


/** @union gpRf4ce_AttributeType
 *  The gpRf4ce_AttributeType union specifies the union that combines all possible types of the RF4CE attributes.
 *  @typedef gpRf4ce_AttributeType_t
 *  The gpRf4ce_AttributeType_t is a union that combines all possible types of the RF4CE attributes
*/
typedef union gpRf4ce_AttributeType {
/** 8 bit unsigned attribute */
    UInt8                                   typeUInt8;
/** 32 bit unsigned attribute */
    UInt32                                  typeUInt32;
/** Pairing table entry attribute */
    gpRf4ce_PairingTableEntry_t             typePairingEntry;
/** User string attribute */
    UInt8                                   typeUserString[GP_RF4CE_MAX_USER_STRING_LENGTH];
} gpRf4ce_AttributeType_t;


/** internal pairing table for non standard entry properties */
typedef struct gpRf4ce_PairingTableEntryInternal{
    Bool                            faGpExtension;
/*End of NVM storage defined in GP_RF4CE_PAIRING_NVM_END*/
    UInt8                           dsnRx;
#ifdef GP_MACCORE_DIVERSITY_POLL_RECIPIENT
    Bool dataPending;
#endif //GP_MACCORE_DIVERSITY_POLL_RECIPIENT
} gpRf4ce_PairingTableEntryInternal_t;

/** Structure for Non-volatile memory */
typedef struct gpRf4ce_PairingTableEntryTotal {
/** Public part according to RF4CE specification */
    gpRf4ce_PairingTableEntry_t             pub;
#if defined(GP_MACCORE_DIVERSITY_POLL_RECIPIENT) //Only gpFa needs non volatile storage
/** Private part containing non standard properties */
    gpRf4ce_PairingTableEntryInternal_t     pri;
#endif
} gpRf4ce_PairingTableEntryTotal_t;

typedef struct gpRf4ce_NodeDesc{
/** The LQI of the discovery request command frame reported by the responding device. */
    UInt8 DiscReqLQI;
/** The logical channel of the responding device. */
    UInt8 logicalChannel;
/** The PAN identifier of the responding device. */
    UInt16 PANId;
/** The IEEE address of the responding device. */
    MACAddress_t IEEEAddr;
/** The capabilities of the responding node. */
    UInt8 nodeCapabilities;
/** The application capabilities of the responding node. */
    UInt8 appCapabilities;
/** The vendor identifier of the responding node. */
    gpRf4ce_VendorId_t vendorId;
/** The vendor string of the responding node. */
    gpRf4ce_VendorString_t vendorString;
/** The user defined identification string of the responding node. This field is present only if the user string specified subfield of the AppCapabilities field is set to one. */
    gpRf4ce_UserString_t userString;
/** The list of device types supported by the responding node. */
    gpRf4ce_DeviceType_t pDevTypeList[3];
/** The list of profile identifiers supported by the responding node. */
    gpRf4ce_ProfileId_t pProfileIdList[7];
/** The status of the discovery */
    gpRf4ce_Result_t status;
} gpRf4ce_NodeDesc_t;

typedef struct gpRf4ce_NIB {
    UInt32  nwkActivePeriod;
    UInt8   nwkBaseChannel;
    UInt8   nwkDiscoveryLQIThreshold;
    UInt32  nwkDiscoveryRepetitionInterval;
    UInt32  nwkDutyCycle;
    UInt32  nwkFrameCounter;
    Bool    nwkIndicateDiscoveryRequests;
    //nwkPairingTable defined as separate variable gpRf4ce_nwkPairingTable
    UInt8   nwkMaxDiscoveryRepetitions;
    UInt8   nwkMaxFirstAttemptCSMABackoffs;
    UInt8   nwkMaxFirstAttemptFrameRetries;
    UInt8   nwkMaxReportedNodeDescriptors;
    UInt32  nwkResponseWaitTime;
    UInt8   nwkScanDuration;
    gpRf4ce_UserString_t nwkUserString;
#if defined( GP_RF4CE_DIVERSITY_PAIR_RECIPIENT) 
    Int8    nwkKeySeedTransmissionPower;
#endif //defined( GP_RF4CE_DIVERSITY_PAIR_RECIPIENT) && defined( GP_RF4CE_DIVERSITY_SECURITY_ENABLED )
} gpRf4ce_NIB_t;

typedef struct gpRf4ce_VendorInfo {
    gpRf4ce_VendorId_t     nwkVendorId;
    gpRf4ce_VendorString_t nwkVendorString;
} gpRf4ce_VendorInfo_t;

typedef struct gpRf4ce_IndicationFilter {
    /** Filter length in bytes */
    UInt8 length;
    /** Array containing filter pattern */
    UInt8 filter[GP_RF4CE_MAX_FILTER_LENGTH];
} gpRf4ce_IndicationFilter_t;

extern const UInt8 ROM gpRf4ce_nwkCMaxPairingTableEntries FLASH_PROGMEM;
extern const gpRf4ce_VendorInfo_t ROM gpRf4ce_VendorInfoDefault FLASH_PROGMEM;

extern UInt8 gpRf4ce_stackId;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup INIT
 *
 *
*/
GP_API void gpRf4ce_Init(void);
GP_API void gpRf4ce_DeInit(void);

/** @ingroup NLDE_DATA
 *
 *  This primitive is used to request the transfer of a data APDU (i.e. NSDU) from a local application entity to a peer application entity.
 *
 *  @param pairingRef       Reference into the pairing table which contains the information required to transmit the NSDU.The address modes to be used for destination and source address.
 *  @param profileId        The identifier of the profile indicating the format of the transmitted data.
 *  @param vendorId         If the TxOptions parameter specifies that the data is vendor specific, this parameter specifies the vendor identifier.
 *  @param nsduLength       The length (in Bytes) of the payload.
 *  @param pNsdu            A pointer to the payload. This is a 16bit absolute address in the radio chip address space.
 *  @param pdHandle         The id of the used memory area.
 *  @param txOptions        Txoptions byte (see RF4CE specification).
 *  @param rfRetryPeriod    Duration of the rf retry period. In case of a multi-channel retransmit, this timeout is only checked at the end of every 3-channel cycle.
*/
GP_API void gpRf4ce_DataRequest(UInt8 pairingRef, gpRf4ce_ProfileId_t profileId, gpRf4ce_VendorId_t vendorId,  UInt8 txOptions, UInt32 rfRetriesPeriod, UInt8 nsduLength, gpPd_Offset_t nsduOffset, gpPd_Handle_t pdHandle);

/** @ingroup NLDE_DATA
 *
 *  This primitive indicates (to the next higher layer) that a data packet is received.
 *
 *  @param pairingRef       Reference into the pairing table which matched the information contained in the received NPDU. A value of 0xff indicates that a broadcast frame was received that does not correspond to an entry in the pairing table.
 *  @param profileId        The identifier of the profile indicating the format of the transmitted data.
 *  @param vendorId         If the RxFlags parameter specifies that the data is vendor specific, this parameter specifies the vendor identifier. If the RxFlags parameter specifies that the data is not vendor specific this parameter is ignored.
 *  @param rxFlags          Reception indication flags for this NSDU (see RF4CE specification).
 *  @param nsduLength       The length (in Bytes) of the payload.
 *  @param nsduOffset       An offset to the data in the pd
 *  @param pdHandle         The handle to the used Packet descriptor.
 *
 * @note The LQI link information is not passed as argument in this callback function. The values is
 * stored in Pd memory area and can be accessed with the gpPd_GetLqi(pdHandle) function. This function returns LQI as a measure of the RSSI of the incoming frame as described in the RF4CE spec.
 */
GP_API void gpRf4ce_cbDataIndication(UInt8 pairingRef, gpRf4ce_ProfileId_t profileId, gpRf4ce_VendorId_t vendorId, UInt8 rxFlags, UInt8 nsduLength, gpPd_Offset_t nsduOffset, gpPd_Handle_t pdHandle);

/** @ingroup NLDE_DATA
 *
 *  This primitive is a confirmation of a dataRequest. It gives info of the success or failure of the dataRequest.
 *
 *  @param status           Status of the NSDU transmission.
 *  @param pairingRef       The pairing table reference for the NSDU being confirmed.
 *  @param pdHandle         The handle to the used Packet descriptor.
*/
GP_API void gpRf4ce_cbDataConfirm(gpRf4ce_Result_t status, UInt8 pairingRef, gpPd_Handle_t pdHandle);

/** @ingroup NLME_COMMSTAT
 *
 *  This primitive allows the ME to notify the application of a communication status.
 *
 *  @param status           The status of the transmission.
 *  @param PairingRef       Reference into the pairing table indicating the recipient node. A value of 0xff indicates that a discovery response frame was sent that does not correspond to an entry in the pairing table.
 *  @param dstPANId         The PAN identifier of the destination device.
 *  @param dstAddrMode      The addressing mode used in the dstAddr parameter.
 *  @param pDstAddr         The address of the destination device.
*/
GP_API void gpRf4ce_cbDiscoveryCommStatusIndication(gpRf4ce_Result_t status, gpRf4ce_PANID_t dstPANId, MACAddress_t* pDstAddr);
GP_API void gpRf4ce_cbPairCommStatusIndication(gpRf4ce_Result_t status, UInt8 pairingRef, gpRf4ce_PANID_t dstPANId, UInt8 dstAddrMode, gpMacCore_Address_t* pDstAddr);

/** @ingroup NLME_DISCOVERY
 *
 *  This primitive allows the application to request the ME to discover other devices of interest operating in the POS of the device.
 *
 *  @param  dstPANId                The PAN identifier of the destination device for the discovery. This value can be set to 0xffff to indicate a wildcard.
 *  @param  dstNwkAddr              The address of the destination device for the discovery. This value can be set to 0xffff to indicate a wildcard.
 *  @param  orgAppCapabilities      The application capabilities of the node issuing this primitive.
 *  @param  pOrgDevTypeList         The list of device types supported by the node issuing this primitive.
 *  @param  pOrgProfileIdList       The list of profile identifiers disclosed as supported by the node issuing this primitive.
 *  @param  searchDevType           The device type to discover.This value can be set to 0xff to indicate a wildcard.
 *  @param  discProfileIdListSize   The number of profile identifiers contained in the DiscProfileIdList parameter.
 *  @param  pDiscProfileIdList      The list of profile identifiers against which profile identifiers contained in received discovery response command frames will be matched for acceptance.
 *  @param  discDuration            The maximum number of MAC symbols to wait for discovery responses to be sent back from potential target nodes on each channel.
*/
GP_API void gpRf4ce_DiscoveryRequest(gpRf4ce_PANID_t dstPANId, UInt16 dstNwkAddr, UInt8 orgAppCapabilities, gpRf4ce_DeviceType_t *pOrgDevTypeList, gpRf4ce_ProfileId_t *pOrgProfileIdList, gpRf4ce_DeviceType_t searchDevType, UInt8 discProfileIdListSize, gpRf4ce_ProfileId_t *pDiscProfileIdList, UInt32 discDuration);

/** @ingroup NLME_DISCOVERY
 *
 *  This primitive allows the ME to notify the application that a discovery request command has been received.
 *
 *  @param status               The status of the pairing table.
 *  @param pSrcIEEEAddr         The IEEE address of the device that requested the discovery.
 *  @param orgNodeCapabilities  The capabilities of the originator of the discovery request.
 *  @param orgVendorId          The vendor identifier of the originator of the discovery request.
 *  @param pOrgVendorString     The vendor string of the originator of the discovery request.
 *  @param orgAppCapabilities   The application capabilities of the originator of the discovery request.
 *  @param pOrgUserString       The user defined identification string of the originator of the discovery request.
 *  @param pOrgDevTypeList      The list of device types supported by the originator of the discovery request.
 *  @param pOrgProfileIdList    The list of profile identifiers supported by the originator of the discovery request.
 *  @param searchDevType        The device type being discovered. If this is 0xff, any type is being requested.
 *  @param rxLinkQuality        LQI value, as passed via the MAC sublayer, of the discovery request command frame. This value is based on the RSSI.
*/
GP_API void gpRf4ce_cbDiscoveryIndication(gpRf4ce_Result_t status, MACAddress_t *pSrcIEEEAddr, UInt8 orgNodeCapabilities, gpRf4ce_VendorId_t orgVendorId, gpPd_Offset_t orgVendorStringOffset, UInt8 orgAppCapabilities, gpPd_Offset_t orgUserStringOffset, gpPd_Offset_t orgDevTypeListOffset, gpPd_Offset_t orgProfileIdListOffset, gpRf4ce_DeviceType_t searchDevType, gpPd_Handle_t pdHandle);

/** @ingroup NLME_DISCOVERY
 *
 *  This primitive allows the application to send a response to the ME on a previously received discovery indication.
 *
 *  @param  status              The status of the discovery request.
 *  @param  pDstIEEEAddr        The IEEE address of the device requesting discovery.
 *  @param  recAppCapabilities  The application capabilities of the node issuing this primitive.
 *  @param  pRecDevTypeList     The list of device types supported by the node issuing this primitive.
 *  @param  pRecProfileIdList   The list of profile identifiers supported by the node issuing this primitive.
 *  @param  discReqLQI          The LQI value from the associated discovery indication.
*/
GP_API void gpRf4ce_DiscoveryResponse(gpRf4ce_Result_t status, MACAddress_t* pDstIEEEAddr, UInt8 recAppCapabilities, gpRf4ce_DeviceType_t* pRecDevTypeList, gpRf4ce_ProfileId_t* pRecProfileIdList, UInt8 discReqLQI);


/** @ingroup NLME_DISCOVERY
 *
 *  This primitive allows the ME to notify the application of the status of its request to perform a network discovery.
 *
 *  @param status           The status of the network discovery attempt.
 *  @param numNodes         The number of discovered nodes.
*/
GP_API void gpRf4ce_cbDiscoveryConfirm(gpRf4ce_Result_t status, UInt8 numNodes);


/** @ingroup NLME_DISCOVERY
 *
 *  This primitive allows the ME to notify the application of received discovery responses.
 *
 *  @param pNodeDesc        A pointer to the received node descriptor.
 *
 *  @return                 indicates if the reported node is a duplicate (true) or not (false)
*/
GP_API Bool gpRf4ce_cbDiscoveryResponseNotifyIndication(gpRf4ce_NodeDesc_t *pNodeDesc);


/** @ingroup NLME_DISCOVERY
 *
 *  This primitive allows the application to request the ME to automatically handle the receipt of discovery request command frames.
 *
 *  @param recAppCapabilities   The application capabilities of the node issuing this primitive.
 *  @param pRecDevTypeList      The list of device types supported by the node issuing this primitive.
 *  @param pRecProfileIdList    The list of profile identifiers supported by the node issuing this primitive.
 *  @param autoDiscDuration     The maximum number of MAC symbols the ME will be in auto discovery response mode.
*/
GP_API void gpRf4ce_AutoDiscoveryRequest(UInt8 recAppCapabilities, gpRf4ce_DeviceType_t* pRecDevTypeList, gpRf4ce_ProfileId_t* pRecProfileIdList, UInt32 autoDiscDuration);


/** @ingroup NLME_DISCOVERY
 *
 *  This primitive allows the ME to notify the application of the status of its request to enter auto discovery response mode.
 *
 *  @param status           Status of the NSDU transmission.
 *  @param pSrcIEEEAddr     The IEEE address to which the discovery response was sent.
*/
GP_API void gpRf4ce_cbAutoDiscoveryConfirm(gpRf4ce_Result_t status , MACAddress_t *pSrcIEEEAddr );


/** @ingroup NLME_DISCOVERY
 *
 *  This primitive aborts the discovery/AutoDiscovery procedure.
 *
 *  @return           Result of the abort procedure. If the rf4ce layer is doing a discovery, the result will be gpRf4ce_ResultSuccess. If the rf4ce layer is not in discovery mode the result will be gpRf4ce_ResultInvalidParameter.
*/
GP_API gpRf4ce_Result_t gpRf4ce_DiscoveryAbort( void );

/** @ingroup NLME_PAIR
 *
 *  This primitive allows the application to request the ME to pair with another device.
 *
 *  @param  logicalChannel      The logical channel of the device with which to pair.
 *  @param  dstPANId            The PAN identifier of the device with which to pair.
 *  @param  pDstIEEEAddr        The IEEE address of the device with which to pair.
 *  @param  orgAppCapabilities  The application capabilities of the node issuing this primitive.
 *  @param  pOrgDevTypeList     The list of device types supported by the node issuing this primitive.
 *  @param  pOrgProfileIdList   The list of profile identifiers supported by the node issuing this primitive.
 *  @param  keyExTransferCount  The number of transfers the target should use to exchange the link key with the pairing originator. Should be set to 0 when no encryption should be used.
*/
GP_API void gpRf4ce_PairRequest(UInt8 logicalChannel, gpRf4ce_PANID_t dstPANId, MACAddress_t *pDstIEEEAddr, UInt8 orgAppCapabilities, gpRf4ce_DeviceType_t  *pOrgDevTypeList, gpRf4ce_ProfileId_t  *pOrgProfileIdList, UInt8 keyExTransferCount);

/** @ingroup NLME_PAIR
 *
 *  This primitive allows the ME to notify the application of the reception of a pairing request command indicates the application that a discovery request command has been received.
 *
 *  @param status               The status of the provisional pairing.
 *  @param srcPANId             The PAN identifier of the device requesting the pair.
 *  @param pSrcIEEEAddr         The IEEE address of the device requesting the pair.
 *  @param orgNodeCapabilities  The capabilities of the originator of the pair request.
 *  @param orgVendorId          The vendor identifier of the originator of the pair request.
 *  @param pOrgVendorString     The vendor string of the originator of the pair request.
 *  @param orgAppCapabilities   The application capabilities of the originator of the pair request.
 *  @param pOrgUserString       The user defined identification string of the originator of the pair request.
 *  @param pOrgDevTypeList      The list of device types supported by the originator of the pair request.
 *  @param pOrgProfileIdList    The list of profile identifiers supported by the originator of the pair request.
 *  @param provPairingRef       Next free pairing reference that will be used if this pairing request is successful.
*/
GP_API void gpRf4ce_cbPairIndication(gpRf4ce_Result_t status, gpRf4ce_PANID_t srcPANId, MACAddress_t *pSrcIEEEAddr, UInt8 orgNodeCapabilities, gpRf4ce_VendorId_t orgVendorId, gpPd_Offset_t orgVendorStringOffset, UInt8 orgAppCapabilities, gpPd_Offset_t orgUserStringOffset, gpPd_Offset_t orgDevTypeListOffset, gpPd_Offset_t orgProfileIdListOffset, UInt8 keyExTransferCount, UInt8 provPairingRef, gpPd_Handle_t pdHandle);

/** @ingroup NLME_PAIR
 *
 *  This primitive allows the application to request that the ME responds to a pairing request command.
 *
 *  @param   status             The status of the pairing request.
 *  @param   dstPANId           The PAN identifier of the device requesting the pair.
 *  @param   pDstIEEEAddr       The IEEE address of the device requesting the pair.
 *  @param   recAppCapabilities The application capabilities of the node issuing this primitive.
 *  @param   pRecDevTypeList    The list of device types supported by the node issuing this primitive.
 *  @param   pRecProfileIdList  The list of profile identifiers supported by the node issuing this primitive.
 *  @param   provPairingRef     The reference to the provisional pairing entry.
*/
GP_API void gpRf4ce_PairResponse(gpRf4ce_Result_t status, gpRf4ce_PANID_t dstPANId, MACAddress_t *pDstIEEEAddr, UInt8 recAppCapabilities, gpRf4ce_DeviceType_t *pRecDevTypeList, gpRf4ce_ProfileId_t *pRecProfileIdList, UInt8 provPairingRef);

/** @ingroup NLME_PAIR
 *
 *  This primitive allows the ME to notify the application of the status of its request to pair with another device.
 *
 *  @param status               The status of the pair attempt.
 *  @param pairingRef           The pairing table reference for this pairing link. If the Status parameter is not equal to SUCCESS, this value will be equal to 0xff.
 *  @param recVendorId          The vendor identifier of the originator of the pair response.
 *  @param pRecVendorString     The vendor string of the originator of the pair response.
 *  @param recAppCapabilities   The application capabilities of the originator of the pair response.
 *  @param pRecUserString       The user defined identification string of the originator of the pair response.
 *  @param pRecDevTypeList      The list of device types supported by the originator of the pair response.
 *  @param pRecProfileIdList    The list of profile identifiers supported by the originator of the pair response.
*/
GP_API void gpRf4ce_cbPairConfirm(gpRf4ce_Result_t status, UInt8 pairingRef, gpRf4ce_VendorId_t recVendorId, gpPd_Offset_t recVendorStringOffset, UInt8 recAppCapabilities, gpPd_Offset_t recUserStringOffset, gpPd_Offset_t recDevTypeListOffset, gpPd_Offset_t recProfileIdListOffset, gpPd_Handle_t pdHandle);

/** @ingroup NLME_UNPAIR
 *
 *  This primitive allows the application to request the ME removes a pairing link with another device both in the local and remote pairing tables.
 *
 *  @param  pairingRef      The reference into the local pairing table of the entry that is to be removed.
*/
GP_API void gpRf4ce_UnpairRequest(UInt8 pairingRef);


/** @ingroup NLME_UNPAIR
 *
 *  This primitive allows the ME to notify the application of the removal of a pairing link by another device.
 *
 *  @param pairingRef       The pairing table reference that has been removed from the pairing table.
*/
GP_API void gpRf4ce_cbUnpairIndication(UInt8 pairingRef);


/** @ingroup NLME_UNPAIR
 *
 *  This primitive allows the application to notify the ME that the pairing link indicated via unpairIndication primitive can be removed from the pairing table.
 *
 *  @param pairingRef       The reference into the local pairing table of the entry that is to be removed.
*/
GP_API void gpRf4ce_UnpairResponse(UInt8 pairingRef);


/** @ingroup NLME_UNPAIR
 *
 *  This primitive allows the ME to notify the application of the status of its request to remove a pair with another device.
 *
 *  @param status           The status of the unpair attempt.
 *  @param pairingRef       The pairing table reference for this pairing link.
*/
GP_API void gpRf4ce_cbUnpairConfirm(gpRf4ce_Result_t status, UInt8 pairingRef);


/** @ingroup NLME_ATTRIBUTE
 *
 *  This primitive allows the application to request the ME the value of a NIB attribute.
 *
 *  @param nibAttribute         The identifier of the NIB attribute to read.
 *  @param nibAttributeIndex    The index within the table or array of the specified NIB attribute to read. This parameter is valid only for NIB attributes that are tables or arrays.
 *  @param pNibAttributeValue   The pointer to the value of the NIB attribute that will be read. This value has a zero length when the status parameter is not equal to SUCCESS.
 *
 *  @return                     The status of the request for NIB attribute information.
 */
GP_API gpRf4ce_Result_t gpRf4ce_GetRequest(gpRf4ce_Attribute_t nibAttribute, UInt8 nibAttributeIndex, void* pNibAttributeValue);


/** @ingroup NLME_ATTRIBUTE
 *
 *  This primitive allows the application to request the ME to change the value of a NIB attribute.
 *
 *  @param  nibAttribute        The identifier of the NIB attribute to write.
 *  @param  nibAttributeIndex   The index within the table or array of the specified NIB attribute to write. This parameter is valid only for NIB attributes that are tables or arrays.
 *  @param  pNibAttributeValue  The value of the indicated attribute to write.
 *
 *  @return                     The status of the request for NIB attribute information.
*/
GP_API gpRf4ce_Result_t gpRf4ce_SetRequest(gpRf4ce_Attribute_t nibAttribute, UInt8 nibAttributeIndex, void* pNibAttributeValue);


/** @ingroup NLME_RESET
 *
 *  This primitive allows the application entity to request a reset of the NWK layer.
 *
 *  @param   setDefaultNIB         If TRUE, the NWK layer is reset and all NIB attributes are set to their default values. If FALSE, the NWK layer is reset but all NIB attributes retain their values prior to the generation of the primitive.
 *  @param   nwkNodeCapabilities   Sets the nodecapabilites of the RF4CE layer. Is only set if setDefaultNIB is true.
*/
GP_API gpRf4ce_Result_t gpRf4ce_ResetRequest(Bool setDefaultNIB , UInt8 nwkNodeCapabilities);


/** @ingroup NLME_START
 *
 *  This primitive allows the application to request the ME to start a network.
*/
GP_API void gpRf4ce_StartRequest(void);


/** @ingroup NLME_START
 *
 *  This primitive allows the ME to notify the application of the status of its request to start a network.
 *
 *  @param status           The status of the start attempt.
*/
GP_API void gpRf4ce_cbStartConfirm(gpRf4ce_Result_t status);


/** @ingroup NLME_RXENABLE
 *
 * This primitive allows the application to request that the receiver is either enabled (for a finite period or until further notice) or disabled.
 *
 *  @param   rxOnDuration       The number of MAC symbols for which the receiver is to be enabled. Only the values 0, 0xFFFFFF and the current value of nwkActivePeriod are supported/allowed. A value 0xFFFFFF will enable the receiver until further notice, while 0x0 will disable the receiver. To start power saving mode, this value should correspond to the value of gpRf4ce_AttributeNwkActivePeriod. Any other value will return 'gpRf4ce_ResultInvalidParameter'.
*/
GP_API gpRf4ce_Result_t gpRf4ce_RxEnableRequest(UInt32 rxOnDuration);

/**  @ingroup NLME_UPDATEKEY
 *
 *  This primitive allows the application to request the ME to change the security link key of an entry in the pairing table.
 *
 *  @param  pairingRef      The reference into the local pairing table of the entry whose key is to be updated.
 *  @param  newLinkKey      The 16 byte security link key to replace the key in the pairing table.
*/
GP_API gpRf4ce_Result_t gpRf4ce_UpdateKey(UInt8 pairingRef, UInt8* newLinkKey);

/**  @ingroup NLDE_DATA
 *
 *  This function gets the LQI value using the LQI mapping mechanism described i n the RF4CE spec
 *
 *  @param PdEntry         The Pd memory area containing the requested data.
*/
GP_API UInt8 gpRf4ce_GetLQI(UInt8 PdEntry );




void gpRf4ce_SetActivePeriod( UInt32 activePeriod );
UInt32 gpRf4ce_GetActivePeriod( void);
void gpRf4ce_SetBaseChannel( UInt8 baseChannel );
UInt8 gpRf4ce_GetBaseChannel( void );
void gpRf4ce_SetDiscoveryLQIThreshold( UInt8 discoveryLQIThreshold );
UInt8 gpRf4ce_GetDiscoveryLQIThreshold( void );
void gpRf4ce_SetDiscoveryRepetitionInterval( UInt32 discoveryRepetitionInterval );
UInt32 gpRf4ce_GetDiscoveryRepetitionInterval( void );
void gpRf4ce_SetDutyCycle( UInt32 dutyCycle );
UInt32 gpRf4ce_GetDutyCycle( void );
void gpRf4ce_SetFrameCounter( UInt32 frameCounter );
UInt32 gpRf4ce_GetFrameCounter( void );
void gpRf4ce_SetIndicateDiscoveryRequests( Bool indicateDiscoveryRequests );
Bool gpRf4ce_GetIndicateDiscoveryRequests( void );
Bool gpRf4ce_GetInPowerSave( void );
void gpRf4ce_SetMaxDiscoveryRepetitions( UInt8 maxDiscoveryRepetitions );
UInt8 gpRf4ce_GetMaxDiscoveryRepetitions( void );
void gpRf4ce_SetMaxFirstAttemptCSMABackoffs( UInt8 maxFirstAttemptCSMABackoffs );
UInt8 gpRf4ce_GetMaxFirstAttemptCSMABackoffs( void );
void gpRf4ce_SetMaxFirstAttemptFrameRetries( UInt8 maxFirstAttemptFrameRetries );
UInt8 gpRf4ce_GetMaxFirstAttemptFrameRetries( void );
void gpRf4ce_SetMaxReportedNodeDescriptors( UInt8 maxReportedNodeDescriptors );
UInt8 gpRf4ce_GetMaxReportedNodeDescriptors( void );
void gpRf4ce_SetResponseWaitTime( UInt32 responseWaitTime );
UInt32 gpRf4ce_GetResponseWaitTime( void );
void gpRf4ce_SetScanDuration( UInt8 scanDuration );
UInt8 gpRf4ce_GetScanDuration( void );
void gpRf4ce_SetUserString( char *pUserString );
void gpRf4ce_GetUserString( char* pUserString );
void gpRf4ce_SetKeySeedTransmissionPower( Int8 keySeedTransmissionPower );
Int8 gpRf4ce_GetKeySeedTransmissionPower( void );
void gpRf4ce_SetPairingTableEntry( UInt8 entry , gpRf4ce_PairingTableEntry_t* pPairingTableEntry );
void gpRf4ce_GetPairingTableEntry( UInt8 entry , gpRf4ce_PairingTableEntry_t* pPairingTableEntry );
Bool gpRf4ce_IsColdStartAdvised(UInt8 nwkNodeCapabilities);
void gpRf4ce_SetStartInfo(UInt16 panID , UInt16 shortAddress );
void gpRf4ce_SetVendorId(gpRf4ce_VendorId_t vendorId);
void gpRf4ce_SetVendorString(gpRf4ce_VendorString_t* pVendorString);
gpRf4ce_VendorId_t gpRf4ce_GetVendorId(void);
void gpRf4ce_GetVendorString(gpRf4ce_VendorString_t* pVendorString);
Bool gpRf4ce_GetCurrentPairingEntryBackup(gpRf4ce_PairingTableEntry_t* pairingEntry);



/**  @ingroup NLME_ATTRIBUTE
 *
 *  This helper function returns the size of an nibAttribute. The function will return 0xFF if the nibAttribute isn't found.
 *
 *  @param nibAttribute     The attribute from which the size will be returned.
 *  @return                 The size in bytes.
*/
GP_API UInt8 gpRf4ce_GetNibAttributeLength(gpRf4ce_Attribute_t nibAttribute);

/**  @ingroup NLME_ATTRIBUTE
 *
 *  This helper function returns if a pairing entry is valid.
 *
 *  @return                 returns true if pairing entry is valid, false if invalid.
*/
GP_API Bool gpRf4ce_IsPairingTableEntryValid( UInt8 pairingRef );

/**  @ingroup NLME_ATTRIBUTE
 *
 *  This helper function returns if a pairing entry is secured.
 *
 *  @return                 returns true if link is secured, false if unsecured.
*/
GP_API Bool gpRf4ce_IsLinkSecure(UInt8 pairingRef);

/**  @ingroup NLME_ATTRIBUTE
 *
 *  This helper function returns if a pairing entry exists based on the IEEE address.
 *
 *  @return                 returns true if pairing entry exists, false if it doesn't exist.
*/
GP_API Bool gpRf4ce_IsNodeInPairingTable(MACAddress_t *pIEEEAddr);

/**  @ingroup NLME_ATTRIBUTE
 *
 *  This helper function returns the index of a pairing entry in the pairing table.
 *
 *  @return                 returns the index of the entry if it exists, 0xFF if it doesn't exist.
*/
GP_API UInt8 gpRf4ce_GetPairingEntryIndex(MACAddress_t *pIEEEAddr);

/**  @ingroup NLME_ATTRIBUTE
 *
 *  This helper function returns the number of valid pairing entries.
 *
 *  @return                 The number of valid pairing entries.
*/
GP_API UInt8 gpRf4ce_GetNrOfPairingEntries(void);



/**  @ingroup NLDE_DATA
 *
 *  This function manipulates the Data Pending Counter in the RF4CE pairing table.
 *
 *  @param pairingRef     A refence within the RF4CE PairingTable
 *  @param dataIsPending  when true, dataPending counter is increased, false decreased
 *  @note When gpRf4ce_cbPollIndication is invoked and Data was pending, it is expected that gpRf4ce_SetDataPending is invoked with dataIsPending set to 'false'
*/
gpRf4ce_Result_t gpRf4ce_SetDataPending( UInt8 pairingRef , Bool dataIsPending);

void gpRf4ce_PollRequest( UInt8 pairingRef , UInt32 maxFrameTotalWaitTime );
void gpRf4ce_cbPollConfirm( gpRf4ce_Result_t status , UInt8 pairingRef );
void gpRf4ce_cbPollIndication( UInt8 pairingRef );

void gpRf4ce_GenericCmdRequest( UInt8 pairingRef , UInt8 logicalChannel , gpRf4ce_PANID_t dstPANId, gpMacCore_Address_t* pDstAddr , UInt8 txOptions, UInt32 rfRetriesPeriod , gpRf4ce_CommandId_t genericCmdId , UInt8 cmdPayloadLength ,UInt8 *pCmdPayload );
void gpRf4ce_cbGenericCmdConfirm( gpRf4ce_Result_t result , gpRf4ce_CommandId_t genericCmdId );
void gpRf4ce_cbGenericCmdIndication( UInt8 pairingRef , gpRf4ce_PANID_t srcPANId, MACAddress_t* pSrcAddr , UInt8 rxFlags , gpRf4ce_CommandId_t genericCmdId , UInt8 cmdPayloadLength , UInt8 *pCmdPayload );

void gpRf4ce_cbResetConfirm( gpRf4ce_Result_t status );

gpRf4ce_Result_t gpRf4ce_SetRxOn(Bool enable);




gpRf4ce_PANID_t gpRf4ce_GetPanId(void);
void gpRf4ce_GetExtendedAddress(MACAddress_t *pMacAddress);

void gpRf4ce_SetTransactionPersistenceTime(UInt16 time);
UInt16 gpRf4ce_GetTransactionPersistenceTime(void);

#ifdef __cplusplus
}
#endif


#endif //_GP_RF4CE_H_


