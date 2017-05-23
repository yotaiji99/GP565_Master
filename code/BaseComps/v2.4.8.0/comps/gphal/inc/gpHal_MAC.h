/*
 * Copyright (c) 2008-2014, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/inc/gpHal_MAC.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */



#ifndef _GPHAL_MAC_H_
#define _GPHAL_MAC_H_

/** @file gpHal_MAC.h
 *  @brief This file contains all the functions needed for MAC functionality.
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gp_global.h"
#include "gpHal_reg.h"
#include "gpHal_HW.h"
#include "gpPd.h"
#include "gpPad.h"

// inclusion for number of pbms
#include "gpHal_Pbm.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/** @brief Default value of the time to live setting of a PBM entry. */
#define GPHAL_TTL_START_VALUE           20

/** @brief Maximum length of a payload that can be written into a PBM entry. */
#define GPHAL_MAX_PBM_LENGTH            125UL

/** @brief Maximum amount of channels used to do multi channel retries */
#define GP_HAL_MULTICHANNEL_MAX_CHANNELS 3

/** @brief  Define to ignore channel used in the multiChannel options struct */
#define GP_HAL_MULTICHANNEL_INVALID_CHANNEL 0xFF

/** @brief  Offset in IEEE packet to check if Ack Request is required */
#define GPHAL_ACK_REQ_LSB                  5

// the result from formula 14 from the IEEE-802.15.4 spec is 31776 ms ==> 0x7C2 symbols
#define GPHAL_POLL_REQ_MAX_WAIT_TIME    0x7C2

#define GP_PBM_INVALID_HANDLE           0xFF

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
//------------------------------------------------------------------------------
// ENUMERATIONS
//------------------------------------------------------------------------------

/** @name gpHal_TxPower_t*/
//@{
/** @brief Default transmit power configured by gpHal_SetDefaultTransmitPowers. */
#define gpHal_DefaultTransmitPower     0x7f
/** @typedef gpHal_TxPower_t
 *  @brief The gpHal_TxPower_t type defines the transmit power in dBm or the default define.
*/
typedef Int8 gpHal_TxPower_t;


/** @name gpHal_CCAMode_t*/
//@{
/** @brief CCA based on Energy Detect will be performed. */
#define gpHal_CCAModeEnergy                 GPHAL_ENUM_PHY_CCA_MODE_ENERGY
/** @brief CCA based on detection of modulated carrier will be performed. */
#define gpHal_CCAModeModulatedCarrier       GPHAL_ENUM_PHY_CCA_MODE_CARRIER
/** @brief CCA based on an AND of Energy and Modulated Carrier Detect will be performed. */
#define gpHal_CCAModeEnergyAndModulated     GPHAL_ENUM_PHY_CCA_MODE_ENERGY_AND_CARRIER
/** @typedef gpHal_CCAMode_t
 *  @brief The gpHal_CCAMode_t type defines the CCA measurement method.
*/
typedef UInt8 gpHal_CCAMode_t;
//@}

/** @name gpHal_CollisionAvoidanceMode_t */
//@{
/** @brief No CCA nor CSMA will be performed. */
#define gpHal_CollisionAvoidanceModeNoCCA   0
/** @brief Only CCA will be performed. */
#define gpHal_CollisionAvoidanceModeCCA     1
/** @brief CSMA will be performed. */
#define gpHal_CollisionAvoidanceModeCSMA    2
/** @typedef gpHal_CollisionAvoidanceMode_t
 *  @brief The gpHal_CollisionAvoidanceMode_t type defines the CSMA/CA method in TX.
*/
typedef UInt8 gpHal_CollisionAvoidanceMode_t;
//@}

/** @name gpHal_MultiChannelOptions_t */
//@{
typedef struct {
/** @brief Array of channels, the first one must be valid to perform a correct transmission */
    UInt8 channel[GP_HAL_MULTICHANNEL_MAX_CHANNELS];
/** @typedef gpHal_MultiChannelOptions_t
 *  @brief The gpHal_MultiChannelOptions_t struct contains all parameter to do multichannel retries.
*/
} gpHal_MultiChannelOptions_t;
//@}

//Antenna selection options
/** @name gpHal_AntennaSelection_t */
//@{
/** @brief Force antenna 0 (termed RF Port 1 or Ant1 in datasheet) to be used*/
#define gpHal_AntennaSelection_Ant0     0x0
/** @brief Force antenna 1 (termed RF Port 2 or Ant2 in datasheet) to be used*/
#define gpHal_AntennaSelection_Ant1     0x1
/** @brief Automatic antenna seletion based on BBP-RX for RX and on MAC settings for TX */
#define gpHal_AntennaSelection_Auto     0x2
/** @brief Not possible to dedect which antenna is selected (error condition) */
#define gpHal_AntennaSelection_Unknown  0x3
/** @typedef gpHal_AntennaSelection_t
 *  @brief The gpHal_AntennaSelection_t type defines the antenna selection mode.
*/
typedef UInt8 gpHal_AntennaSelection_t;
//@}

//@{
/** @name gpHal_MacScenario_t */
// MacScenarios
#define gpHal_MacDefault     0x0
/** gpHal_MacSPollReq Mac Scenario to send a Poll Req*/
#define gpHal_MacPollReq     0x1
/** @typedef gpHal_MacScenario_t
 *  @brief The gpHal_MacScenario_t type defines the Mac Scenario as defined in the databook.
*/
typedef UInt8 gpHal_MacScenario_t;
//@}

//@{
/** @struct gpHal_DataReqOptions_t
 *  @param gpHal_MacScenario_t
 *  @brief These options dictate the way a data packet should be transmitted
*/
typedef struct gpHal_DataReqOptions_s{
    gpHal_MacScenario_t             macScenario;
}gpHal_DataReqOptions_t;
//@}

// Explain?? 
#define gpHal_RxModeNormal               0x1
#define gpHal_RxModeLowPower             0x2
#define gpHal_RxModeAttenuatorDiverse    0x3
#define gpHal_RxModeMultiChannel         0x4
typedef UInt8 gpHal_RxMode_t;

#define gpHal_AntennaModeDiversity       0x0
#define gpHal_AntennaModeAntenna0        0x1
#define gpHal_AntennaModeAntenna1        0x2
typedef UInt8 gpHal_AntennaMode_t;

#define gpHal_AttenuatorModeFixed0       0x0
#define gpHal_AttenuatorModeFixed4       0x1
#define gpHal_AttenuatorModeFixed10      0x2
#define gpHal_AttenuatorModeDiversity    0x3
typedef UInt8 gpHal_AttenuatorMode_t;

typedef UInt8 gpHal_Channel_t;



/** @typedef gpHal_RxModeConfig_t
 *  @brief   The gpHal_RxModeConfig_t typedef defines the structure used to pass Receiver Mode configurations. */
 typedef struct gpHal_RxModeConfig_s {
    gpHal_RxMode_t                  rxMode;   
    gpHal_AntennaMode_t             antennaMode; 
    gpHal_AttenuatorMode_t          attenuationMode;
    gpHal_Channel_t                 channelList[6];
    gpHal_AttenuatorMode_t           attenuatorLow;
    gpHal_AttenuatorMode_t          attenuatorHigh;
 } gpHal_RxModeConfig_t; 

//Src Identifier
/** @name gpHal_SourceIdentifier_t */
//@{
/** @brief Identifier for first Pan (pan 0) */
#define gpHal_SourceIdentifier_0    0x0
/** @brief Identifier for second Pan (pan 1) */
#define gpHal_SourceIdentifier_1    0x1
/** @brief Identifier for invalid value */
#define gpHal_SourceIdentifier_Inv    0xFF
/** @typedef gpHal_SourceIdentifier_t
 *  @brief The gpHal_SourceIdentifier_t type defines the source pan.
*/
typedef UInt8 gpHal_SourceIdentifier_t;
//@}




/** @typedef gpHal_DataIndicationCallback_t
 *  @brief   The gpHal_DataIndicationCallback_t callback type definition defines the callback prototype of the DataIndication.*/
typedef void (*gpHal_DataIndicationCallback_t)(gpPd_Loh_t pdLoh, gpHal_RxInfo_t *rxInfo);

/** @typedef gpHal_DataConfirmCallback_t
 *  @brief   The gpHal_DataConfirmCallback_t callback type definition defines the callback prototype of the DataConfirm.*/
typedef void (*gpHal_DataConfirmCallback_t)(UInt8 status, gpPd_Loh_t pdLoh, UInt8 lastChannelUsed);

/** @typedef gpHal_EDConfirmCallback_t
 *  @brief   The gpHal_EDConfirmCallback_t callback typedef defines the callback prototype of the EDConfirm.
 *
 *  The parameter protoED isn't the real energy level.  The real energy level needs to be calculated with the function gpHal_CalculateED().
 */
typedef void (*gpHal_EDConfirmCallback_t)(UInt16 channelMask, UInt8 *protoED);

/** @typedef gpHal_BusyTXCallback_t
 *  @brief   The gpHal_BusyTXCallback_t callback type definition defines the callback prototype of the BusyTX interrupt. */
typedef void (*gpHal_BusyTXCallback_t)(void);

/** @typedef gpHal_EmptyQueueCallback_t
 *  @brief   The gpHal_EmptyQueueCallback_t callback type definition defines the callback prototype of the EmptyQueue interrupt. */
typedef void (*gpHal_EmptyQueueCallback_t)(void);

/** @typedef gpHal_CmdDataReqCallback_t
 *  @brief   The gpHal_CmdDataReqCallback_t callback type definition defines the callback prototype of the Cmd Data Req interrupt. */
typedef void (*gpHal_CmdDataReqCallback_t)(void);

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/


/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

/** range check for offsets into a pbm frame */
#define GP_HAL_IS_VALID_PBM_FRAME_OFFSET(offset) ((UInt16)(offset) < 128)
/** @brief Macro for calculation of PBM buffer address.
 *
 *  This macro calculates the base address of the PBM buffer defined by the parameter address.
 *  This macro should be called before accessing any byte of the PBM buffer.
 *
 *  @param  PBMentry The index of the PBM buffer to tranmsit
 *  @return The base address of the PBM buffer.
 */
#define GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS(entry) (GP_HAL_PBM_ENTRY2ADDR(entry)+ GPHAL_REGISTER_PBM_FORMAT_T_FRAME_0)

/** @brief Macro for writing data to a PBM buffer.
 *
 *  This macro writes data to a PBM buffer defined by the parameter address.
 *  This macro should be used in order to update the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the data should be written to.  This value should be calculated
 *                 with the macro GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS().
 *  @param pData   The pointer to the data that will be written.
 *  @param length  The number of bytes that will be written.
 *  @param offset  The offset from the base address of the PBM where the data should be written to.
 */
#define GP_HAL_WRITE_DATA_IN_PBM(address,pData,length,offset) do {      \
        UInt16 offset_l = (offset); /*avoid multiple evaluation of argument*/  \
        GP_ASSERT_DEV_EXT(GP_HAL_IS_VALID_PBM_FRAME_OFFSET(offset_l) );                \
        GP_ASSERT_DEV_EXT(GP_HAL_IS_VALID_PBM_FRAME_OFFSET(offset_l + (length) - 1) ); \
        GP_HAL_WRITE_BYTE_STREAM(( (address) + offset_l ), (pData), (length)); \
    } while (false)

/** @brief Macro for writing one byte to a PBM buffer.
 *
 *  This macro writes one byte to a PBM buffer defined by the parameter address.
 *  This macro should be used in order to update the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the data should be written to.  This value should be calculated
 *                 with the macro GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS().
 *  @param byte    The value that will be written.
 *  @param offset  The offset from the base address of the PBM where the byte should be written to.
 */
#define GP_HAL_WRITE_BYTE_IN_PBM(address,byte,offset) do {                    \
        UInt8 offset_l = (offset); /*avoid multiple evaluation of argument*/  \
        GP_ASSERT_DEV_EXT(GP_HAL_IS_VALID_PBM_FRAME_OFFSET(offset_l));                \
        GP_HAL_WRITE_REG((address) + offset_l, byte); \
    } while (false)

/** @brief Macro for reading data from a PBM buffer.
 *
 *  This macro reads data from a PBM buffer defined by the parameter address.
 *  This macro should be used in order to read back the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the data should be read from.  This value should be otained
 *                 from an interrupt callback function
 *  @param pData   The pointer where the read data will be written to.
 *  @param length  The number of bytes that will be read.
 *  @param offset  The offset from the base address of the PBM where the data is read from.
 */
#define GP_HAL_READ_DATA_IN_PBM(address,pData,length,offset)  do {             \
        UInt16 offset_l = (offset); /*avoid multiple evaluation of argument*/  \
        GP_ASSERT_DEV_EXT(GP_HAL_IS_VALID_PBM_FRAME_OFFSET(offset_l) );                \
        GP_ASSERT_DEV_EXT(GP_HAL_IS_VALID_PBM_FRAME_OFFSET(offset_l + (length) - 1) ); \
        GP_HAL_READ_BYTE_STREAM(( (address) + offset_l ) , (pData) , (length) ); \
    } while (false)

/** @brief Macro for reading one byte from a PBM buffer.
 *
 *  This macro reads data from a PBM buffer defined by the parameter address.
 *  This macro should be used in order to read back one byte of the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the byte should be read from.  This value should be obtained 
 *                 from an interrupt callback function
 *  @param offset  The offset from the base address of the PBM where the byte is read from.
 *  @return        The read value.
 */
#define GP_HAL_READ_BYTE_IN_PBM(address,offset)  GP_HAL_READ_REG((address)+ (offset))

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

// see gpHal_DEFS.h
// MAC public functions

//MAC-SEC interrupts
/**
 * @brief Enables the interrupt line of the MAC and SEC operations.
 *
 * This function sets the interrupt mask of the PIO block.
 *
 * @param enable Enables the interrupt source if true.
*/
#define gpHal_EnablePrimitiveCallbackInterrupt(enable)      GP_HAL_ENABLE_PIO_INT(enable)
/**
 * @brief Enables the interrupt line of the Empty Queue interrupt.
 *
 * This function sets the interrupt mask of the Empty Queue interrupt.
 *
 * @param enable Enables the interrupt source if true.
*/
#define gpHal_EnableEmptyQueueCallbackInterrupt(enable)     GP_HAL_ENABLE_EMPTY_QUEUE_CALLBACK_INTERRUPT(enable)

/**
 * @brief Enables the interrupt line of the BusyTX interrupt.
 *
 * This function sets the interrupt mask of the BusyTX interrupt.
 *
 * @param enable Enables the interrupt source if true.
*/
#define gpHal_EnableBusyTXCallbackInterrupt(enable)     GP_HAL_ENABLE_BUSY_TX_CALLBACK_INTERRUPT(enable)
/**
 * @brief Registers the callback for a DataConfirm.
 *
 * This function registers the callback for a DataConfirm. The callback will be executed on a DataConfirm interrupt.
 * This DataConfirm will be given after a DataRequest is finished.
 *
 * The Primitive interrupt needs to be enabled.
 *
 * @param callback The pointer to the callback function.
*/
GP_API void gpHal_RegisterDataConfirmCallback(gpHal_DataConfirmCallback_t callback);

/**
 * @brief Registers the callback for a DataIndication.
 *
 * This function registers the DataIndication callback. The callback will be executed on a DataIndication interrupt
 * This DataIndication will be given if a packet is received.
 *
 * The Primitive interrupt needs to be enabled.
 *
 * @param callback The pointer to the callback function.
*/
GP_API void gpHal_RegisterDataIndicationCallback(gpHal_DataIndicationCallback_t callback);

/**
 * @brief Registers the callback for a EDConfirm.
 *
 * This function registers the EDConfirm callback. The callback will be executed on a EDConfirm interrupt
 * This EDConfirm will be given after a EDRequest is finished
 *
 * The Primitive interrupt needs to be enabled.
 *
 * @param callback The pointer to the callback function.
*/
GP_API void gpHal_RegisterEDConfirmCallback(gpHal_EDConfirmCallback_t callback);

/**
 * @brief Registers the callback for a BusyTX interrupt.
 *
 * This function registers the BusyTX callback. The callback will be executed on a BusyTX interrupt, i.e.
 * is triggered when the MAC receives a TX trigger while he is already transmitting another packet.
 * The BusyTX interrupt needs to be enabled.
 *
 * @param callback The pointer to the callback function.
*/
GP_API void gpHal_RegisterBusyTXCallback(gpHal_BusyTXCallback_t callback);

/**
 * @brief Registers the callback for Empty Queue interrupt.
 *
 * This function registers the EmptyQueue callback.  The callback will be executed on an Empty Queue interrupt, i.e.
 * is triggered when a TX trigger is given to the MAC when no packet is pending in the TX queue.
 * The Empty Queue interrupt needs to be enabled.
 *
 * @param callback The pointer to the callback function.
*/
GP_API void gpHal_RegisterEmptyQueueCallback(gpHal_EmptyQueueCallback_t callback);

/**
 * @brief Registers the callback for the reception of a Cmd Data Req interrupt.
 *
 * This function registers the CmdDataReq callback. 
 * This function determines whether the Frame Pending bit in an Ack needs to be set.
 *
 * @param callback The pointer to the callback function.
*/
GP_API void gpHal_RegisterCmdDataReqConfirmCallback(gpHal_CmdDataReqCallback_t callback);

/** @brief Start a data transmission.
 *
 *  Performs a DataRequest(according to the IEEE802.15.4 specification).
 *  The DataConfirm function can be registered as a callback using gpHal_RegisterDataConfirmCallback().
 *
 *  Possible results are:
 *          - gpHal_ResultSuccess
 *          - gpHal_ResultBusy              (no packet buffer available)
 *          - gpHal_ResultInvalidParameter
 *
 *  @param dataReqOptions       csma, multiChannelOptions, macScenario
 *  @param pdLoh                The packet descriptor structure that contains length, offset and unique handle of the packet content.
*/
GP_API gpHal_Result_t gpHal_DataRequest(gpHal_DataReqOptions_t *dataReqOptions, gpPad_Handle_t padHandle, gpPd_Loh_t pdLoh);
GP_API void gpHal_FillInTxOptions(UInt8 pbmHandle, gpPad_Attributes_t* pOptions);

/** @brief Returns the listening channel currently used
 *
 *  @param srcId The idx of the rx channel slot.
*/
GP_API UInt8 gpHal_GetRxChannel(gpHal_SourceIdentifier_t srcId);

/** @brief Configure the default transmit power for each channel 
 *
 *  @param pointer to 16 byte array with default transmit power for each IEEE channel (11..26).
 *
*/
GP_API void gpHal_SetDefaultTransmitPowers(Int8* pDefaultTransmitPowerTable );

/** @brief Set the CCA Threshold setting */

GP_API void gpHal_SetCCAThreshold(void);





/** @brief Writes data in the specified pbm address*/
#define gpHal_WriteDataInPbm(address,pData,length,offset)             GP_HAL_WRITE_DATA_IN_PBM(address,pData,length,offset) 

#define gpHal_CalculateTxPbmDataBufferAddress(pbmEntry)            GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS(pbmEntry) 

#define gpHal_CheckPbmValid(pbmEntry)             GP_HAL_CHECK_PBM_VALID(pbmEntry) 

/** @brief Sets the Rx packet in packet mode
 *
 *  The function sets the Rx packet in packet mode on or off
 *
*/
#define gpHal_SetPipMode(pipmode)               GP_HAL_SET_PIP_MODE(pipmode)

/** @brief Returns the number packet in packet mode */
#define gpHal_GetPipMode()                      GP_HAL_GET_PIP_MODE()


//FIXME: add documentation

GP_API void gpHal_SetRxAntennaDiversity(Bool onoff);


/** @brief Returns the current Radio state.
 *
 *  This function returns the state of the radio.
 *  Possible results are:
 *          - gpHal_ResultTrxOff
 *          - gpHal_ResultTxOn
 *          - gpHal_ResultRx0n
 *          - gpHal_ResultBusy
*/
GP_API gpHal_Result_t gpHal_GetRadioState(void);

// MAC ED Request
/** @brief Performs a Energy Detect request according to the IEEE802.15.4 spec
 *
 *  This function triggers an energy detection. The energy value of this function is given in the
 *  EDConfirm callback (to be registered with gpHal_RegisterEDConfirmCallback).
 *
 *  Possible results are :
 *          - gpHal_ResultSuccess
 *          - gpHal_ResultBusy      (no packet buffer available)
 *
*/
GP_API gpHal_Result_t gpHal_EDRequest(UInt32 time_us , UInt16 channelMask);

/** @brief Set the PAN ID
 *
 *  This function sets the PAN ID .
 *  Setting the PAN ID of your network enables the automatic filter of
 *  packets not intended for your device.
 *
 *  @param panId The PAN ID of the network.
 *  @param srcId The PAN src, we want to change the ID from.
*/
GP_API void gpHal_SetPanId(UInt16 panId, gpHal_SourceIdentifier_t srcId);

/** @brief Returns the PAN ID stored 
 * 
 *  @param srcId The src id of the Pan.
 **/
GP_API UInt16 gpHal_GetPanId(gpHal_SourceIdentifier_t srcId);

/** @brief Sets the Short Address.
 *
 *  This functions sets the Short Address.
 *  Setting the Short Address of your device enables the automatic filter of
 *  packets not intended for your device.
 *
 *  @param shortAddress The Short Address of the device.
 *  @param srcId The src id of the pan where we want to change the short address.
*/
GP_API void gpHal_SetShortAddress(UInt16 shortAddress, gpHal_SourceIdentifier_t srcId);

/** @brief Returns the ShortAddress 
 *  
 *  @param srcId The src id of the pan where we want to get the short address.
 */
GP_API UInt16 gpHal_GetShortAddress(gpHal_SourceIdentifier_t srcId);

/** @brief Set the Extended Address
 *
 *  This function sets the Extended Address.
 *  Setting the Extended Address of your device enables the automatic filter of
 *  packets not intended for your device.
 *  @param pExtendedAddress The pointer to the Extended Address of the device.
*/
GP_API void gpHal_SetExtendedAddress(MACAddress_t* pExtendedAddress);

/** @brief Returns the ExtendedAddress stored

    This function returns the extended address stored.
    @param pExtendedAddress pointer where the Extended Address is read back to
*/
GP_API void gpHal_GetExtendedAddress(MACAddress_t* pExtendedAddress);

/** @brief Set the Coordinator Address
 *
 *  Setting the Coordinator Address enables the filtering of
 *  packets not coming from the coordinator.
 *  @param Address The pointer to the Address of the coordinator.
*/
GP_API void gpHal_SetCoordExtendedAddress(MACAddress_t* pCoordExtendedAddress);
GP_API void gpHal_SetCoordShortAddress(UInt16 shortCoordAddress);


/** @brief Set the pan coordinator property
 *
 *  This function sets the pan coordinator property of the device.
 *  
 *  @param panCoordinator true if the device is the pan coordinator, false otherwise.
*/
GP_API void gpHal_SetPanCoordinator(Bool panCoordinator);

/** @brief Returns the pan coordinator property of this device */
GP_API Bool gpHal_GetPanCoordinator(void);

/** @brief Enables/Disables Address Recognition.
 *
 *  This function sets the address recognition options.
 *  @param enable           Possible values are :
 *                           - set to true : destination address of a packet will checked against the address (set by gpHal_SetExtendedAddress and gpHal_SetShortAddress) and destination PAN ID of incoming packets.
 *                           - set to false: address recognition disabled.
 *  @param panCoordinator   Possible values are :
 *                           - set to true : The device is a PAN coordinator.  He will accept messages without a destination address.
 *                           - set to false: Normal filtering will be applied according to recognition settings.
*/
GP_API void gpHal_SetAddressRecognition(Bool enable, Bool panCoordinator);

/** @brief Returns the addressRecognition flag. */
#define gpHal_GetAddressRecognition()   GP_HAL_GET_ADDRESS_RECOGNITION()

/** @brief Sets the FrameType FilterMask.
 *
 *  This function sets the Frame Type filter mask.
 *
 *  Packets can be filter out based on their frametype (BCN, DATA, CMD, RSV).
 *  This can be controlled by setting the filter bitmap with this function.
 *
 *  @param bitmap The FrameType FilterMask.
*/
#define gpHal_SetFrameTypeFilterMask(bitmap)    GP_HAL_SET_FRAME_TYPE_FILTER_MASK((bitmap))
/** @brief Returns the FrameTypeFilter bitmap. */
#define gpHal_GetFrameTypeFilterMask()              GP_HAL_GET_FRAME_TYPE_FILTER_MASK()

/** @brief Sets the property for filtering beacons based on src pan.
 *
 *  This function sets the  property for filtering beacons based on src pan.
 *
 *  @param enable If we want to enable beacon filtering on src pan or not.
*/
GP_API void gpHal_SetBeaconSrcPanChecking(Bool enable);


/** @brief Gets the property for filtering beacons based on src pan.
 *
 *  This function gets the  property for filtering beacons based on src pan.
 *
*/
GP_API Bool gpHal_GetBeaconSrcPanChecking(void);

/** @brief Sets the RxOnWhenIdle flag.
 *
 *  This function sets the RxOnWhenIdle flag.
 *  Turns on the receiver when the device is idle.  Switching between
 *  TX and RX is done automatically.
 *
 *  @param srcId The idx of the rx channel slot.
 *  @param flag Possible values are :
 *              - set to true : RxOnWhenIdle is activated and the radio is turned on.
 *              - set to false: RxOnWhenIdle is deactivated.
 *  @param channel channel to enable radio on
*/
GP_API void gpHal_SetRxOnWhenIdle(gpHal_SourceIdentifier_t srcId, Bool flag, UInt8 channel);
/** @brief Returns the RxOnWhenIdle flag. */
#define gpHal_GetRxOnWhenIdle()                 GP_HAL_GET_RX_ON_WHEN_IDLE()

/** @brief Sets the Auto Acknowledge flag.
 *
 *  This function sets the Auto Acknowledge flag.
 *  All packets addressed to the device (see address recognition) will be automatically acknowledge
 *  (if requested by the MAC headerof the packet).
 *  @param flag
 *              - Set to true : Automatic acknowledgement enabled.
 *              - Set to false: Automatic acknowledgement disabled.
*/
GP_API void gpHal_SetAutoAcknowledge(Bool flag);
/** @brief Returns the AutoAcknowledge flag. */
GP_API Bool gpHal_GetAutoAcknowledge(void);

extern UInt8 gpHal_MacState;

/** @brief Sets the chip to timed MAC mode
 *
 * This function sets the chip to timed MAC mode.
 * When a timed MAC is used all transmission is done using scheduled triggers from the Event Scheduler (ES).
 * This function may only be called once after the initialization of the stack.
 */
GP_API void gpHal_SetTimedMode(Bool timedMode);

/** @brief Calculate the RSSI from the protoRSSI returned by the data indication handler.
 *
 *  This function calculates the RSSI from the protoRSSI value returned by the data indication handler.
 *  @param protoRSSI Value returned by data indication handler.
*/
GP_API Int8 gpHal_CalculateRSSI(UInt8 protoRSSI);

/** @brief Calculate the proto RSSI from the RSSI.
 *
 *  This function calculates the proto RSSI from the RSSI value.
 *  @param protoRSSI Value returned by data indication handler.
*/
GP_API UInt8 gpHal_CalculateProtoRSSI(Int8 protoRSSI);

/** @brief Calculate the ED value from the protoED returned by the ED scan handler, values are conform the ZIP phy testspec.
 *
 *  This function calculates the ED value from the protoED returned by the ED scan handler. The lowest value is 0, which is at -75dBm, the highest value is 0xFF, which is at -35dBm.
 *  @param protoED  Value returned by ED scan handler.
*/
GP_API UInt8 gpHal_CalculateED(UInt8 protoED);

/** @brief Calculate the LQI of a received packet based on RSSI.
 *
 *  This function calculates the LQI of a received packet based on the RSSI. The lowest value is 0 which is at -93dBm. The highest value is 0xFF witch is at -20dBm.
 *  @param RSSI Value returned by data indication handler.
*/
GP_API UInt8 gpHal_CalculateLQIfromRSSI(Int8 rssi);

/** @brief Checks if a NO LOCK was triggerd by the radio.
 *
 *  This function reports if a lock loss was detected by the radio.
 *  @return Result of the check.
*/
GP_API Bool gpHal_CheckNoLock(void);

/** @brief Query the calibrated RSSI value from the specified pbm entry.
 *
 *  This function returns the calibrated RSSI from the specified pbm entry.
 *  @param PBMentry  the pbm entry containing the requested data.
*/
GP_API Int8 gpHal_GetRSSI(UInt8 PBMhandle);

/** @brief Query the calibrated LQI value from the specified pbm entry.
 *
 *  This function returns the calibrated LQI from the specified pbm entry.
 *  LQI is a value from 0 to 0xFF with 0 the lowest value and 0xFF the highest value.
 *  LQI is only based on signal strength not on correlation.
 *  @param PBMentry  the pbm entry containing the requested data.
*/
GP_API UInt8 gpHal_GetLQI(UInt8 PBMhandle);

/** @brief Query the timestamp of a received packet from the specified pbm entry.
 *
 * This function returns the timestamp of a received packet for a specified pbm entry.
 * The timestamp is taken at the beginning of the frame (preamble).
 * @param PBMentry  the pbm entry containing the requested data.
 * @param timeStamp the pointer to which the timestamp will be returned
*/
GP_API void gpHal_GetRxTimestamp(UInt8 PBMentry, UInt32* pTimeStamp);

/** @brief Query the timestamp of a transmitted packet from the specified pbm entry.
 *
 * This function returns the timestamp of a transmitted packet for a specified pbm entry.
 * The timestamp is taken at the beginning of the frame (preamble).
 * @param PBMentry  the pbm entry containing the requested data.
 * @param timeStamp the pointer to which the timestamp will be returned
*/
GP_API void gpHal_GetTxTimestamp(UInt8 PBMentry, UInt32* pTimeStamp);

/** @brief Query the retry counter of a transmitted packet from the specified pbm entry.
 *
 * This function returns the retry counter of a transmitted packet for a specified pbm entry.
 * @param PBMentry  the pbm entry containing the requested data.
 * @note retry counter 0 means - first time succesfully transmitted
*/
GP_API UInt8 gpHal_GetTxRetryCntr(UInt8 PBMhandle);

/** @brief Returns a free handle.
 *
 *  This function allocate a free PBM buffer and returns its handle.
 *  Returns 0xFF if no free buffer is available;
*/
GP_API UInt8 gpHal_GetHandle(void);

/** @brief Releases the given handle
 *
 *  This function releases the PBM buffer associated with the handle.
 *  @param handle  A valid handle which will be released.
*/
GP_API void gpHal_FreeHandle(UInt8 handle);

/** @brief Default Tx Antenna selection
 *
 *  This macros selects the antenna configuration.  By default (macro GP_HAL_DIVERSITY_SINGLE_ANTENNA
 *  not defined), both antennas are used with antenna diversity.
 *  When the macro GP_HAL_DIVERSITY_SINGLE_ANTENNA is defined
 *  through the command line option of the compiler, the single antenna is selected.
*/
#if defined(GP_HAL_DIVERSITY_SINGLE_ANTENNA)
#define GP_HAL_TX_ANTENNA  GP_HAL_DIVERSITY_SINGLE_ANTENNA
#else
#define GP_HAL_TX_ANTENNA 0
#endif

/** @brief Gets the antenna used to transmit a packet
 *
 *  This function queries the antenna used to transmit a packet
 *  @return the antenna used to transmit a packet, possible values defined in enumeration gpHal_AntennaSelection
 */
GP_API gpHal_AntennaSelection_t gpHal_GetTxAntenna(void);

/** @brief Gets the antenna used to receive packets
 *
 *  This function queries the antenna used to receive packets
 *  @return the antenna used to receive packets, possible values defined in enumeration gpHal_AntennaSelection
 */
GP_API gpHal_AntennaSelection_t gpHal_GetRxAntenna(void);

/** @brief Sets the antenna used to transmit and receiving packets
 *
 *  This function selects the used antenna used to transmit a packet
 *  @param antenna The antenna used to transmit and receive packets, possible values defined in enumeration gpHal_AntennaSelection
 */
GP_API void gpHal_SetAntenna(gpHal_AntennaSelection_t antenna);

/** @brief function for writing data to a PBM buffer on a cyclic way.
 *
 *  This function writes data to a PBM buffer defined by the parameter address. The destination is the data segment of the PBM. This segment is handled as a cyclic buffer.
 *  This function should be used in order to update the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the data should be written to.  This value should be calculated
 *                 with the macro GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS().
 *  @param pData   The pointer to the data that will be written.
 *  @param length  The number of bytes that will be written.
 *  @param offset  The offset from the base address of the PBM where the data should be written to.
 */
GP_API void gpHal_WriteDataInPBMCyclic(gpHal_Address_t pbmAddr,UInt8 pbmOffset, UInt8* pData, UInt8 length);

/** @brief Macro for reading data from a PBM buffer. The destination is the data segment of the PBM. This segment is handled as a cyclic buffer.
 *
 *  This function reads data from a PBM buffer defined by the parameter address.
 *  This function should be used in order to read back the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the data should be read from.  This value should be calculated
 *                 with the macro GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS().
 *  @param pData   The pointer where the read data will be written to.
 *  @param length  The number of bytes that will be read.
 *  @param offset  The offset from the base address of the PBM where the data is read from.
 */
GP_API void gpHal_ReadDataInPBMCyclic(gpHal_Address_t pbmAddr,UInt8 pbmOffset, UInt8* pData, UInt8 length);

/** @brief Functino for writing one byte to a PBM buffer. The destination is the data segment of the PBM. This segment is handled as a cyclic buffer.
 *
 *  This function writes one byte to a PBM buffer defined by the parameter address.
 *  This function should be used in order to update the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the data should be written to.  This value should be calculated
 *                 with the macro GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS().
 *  @param byte    The value that will be written.
 *  @param offset  The offset from the base address of the PBM where the byte should be written to.
 */
GP_API void gpHal_WriteByteInPBMCyclic(gpHal_Address_t pbmAddr,UInt8 pbmOffset, UInt8 byte);

/** @brief Function for reading one byte from a PBM buffer. The destination is the data segment of the PBM. This segment is handled as a cyclic buffer.
 *
 *  This function reads data from a PBM buffer defined by the parameter address.
 *  This function should be used in order to read back one byte of the payload of a PBM buffer.
 *
 *  @param address The base address of the PBM where the byte should be read from.  This value should be calculated
 *                 with the macro GP_HAL_CALCULATE_TX_PBM_DATA_BUFFER_ADDRESS().
 *  @param offset  The offset from the base address of the PBM where the byte is read from.
 *  @return        The read value.
 */
GP_API UInt8 gpHal_ReadByteInPBMCyclic(gpHal_Address_t pbmAddr,UInt8 pbmOffset );

/** @brief Function for duplicating a PBM in a new PBM
 *
 *  @param PBMentryOrig The pbm.which will be copied
 *  @param PBMentryDst  The destination pbm. This pbm will have the same data and options as the original pbm.
 */
void gpHal_MakeBareCopyPBM( UInt8 PBMentryOrig , UInt8 PBMentryDst );

/** @brief Enables the promiscuous mode.
 *
 *  In promiscuous mode, all packets will be received. In order to enable the receiver the
 *  RxOnWhenIdle flag must be set.
 *
 *  @param flag Possible values are :
 *              - set to true : Promiscuous mode is enabled and the filters disabled.
 *              - set to false: Normal filtering is applied on incoming packets.
*/
GP_API void gpHal_SetPromiscuousMode(Bool flag);

/** @brief Returns promiscuous mode state.
*/
GP_API Bool gpHal_GetPromiscuousMode(void);

/** @brief Set the default ack frame pending bit. */
GP_API void gpHal_SetFramePendingAckDefault(Bool enable);

/** @brief Returns the default ack frame pending bit. */
GP_API Bool gpHal_GetFramePendingAckDefault(void);



#endif  /* _GPHAL_MAC_H_ */

