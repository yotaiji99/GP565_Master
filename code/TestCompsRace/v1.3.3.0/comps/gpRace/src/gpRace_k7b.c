/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 * gpRace.c
 *
 * The file contains the function that allows remote autenticated code execution.
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
 *     0!                         $Header: //depot/release/Embedded/TestCompsRace/v1.3.3.0/sw/comps/gpRace/src/gpRace_k7b.c#2 $
 *    M'   GreenPeak              $Change: 72952 $
 *   0'         Technologies      $DateTime: 2015/11/04 13:47:48 $
 *  F
 */


/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define COMPLETE_MAP

#include "global.h"
#include "gpRace.h"
#include "gpVersion.h"
#include "gphal.h"
#include "gpAssert.h"
#include "gpReset.h"


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_RACE

#ifndef GP_RACE_ON_PACKET_DOWN_COUNT_IN_MS
#define GP_RACE_ON_PACKET_DOWN_COUNT_IN_MS (UInt16) (30000)
#endif
#ifndef GP_RACE_CONTROL_CHANNEL
#define GP_RACE_CONTROL_CHANNEL         (UInt8)(12)
#endif
#ifndef GP_RACE_PAN_ID
#define GP_RACE_PAN_ID                  (UInt16)(0x8ACE)
#endif
#ifndef GP_RACE_SHORT_ADDR
#define GP_RACE_SHORT_ADDR              (UInt16)(0x9BDF)
#endif
#ifndef GP_RACE_TX_ANTENNA
#define GP_RACE_TX_ANTENNA              (gpHal_AntennaSelection_Ant0)
#endif

#define GP_RACE_TX_MAC_FRAME_CONTROL    (UInt16)(0xCC61) /*dataframe, acknowledged, pan id compression, long source addressing, long destination addressing*/

#define GP_RACE_MAC_FRAME_CONTROL_DESTINATION_ADDRESS_MODE_MASK      0x0C00
#define GP_RACE_MAC_FRAME_CONTROL_DESTINATION_ADDRESS_MODE_BROADCAST 0x0C00

#define GP_RACE_LED_BLINK_BACK          (Bool)(false)

//#define GP_RACE_ENABLE_ENCRYPTION

#define GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(pbmEntry)    ((gpHal_Address_t)(GP_MM_RAM_PBM_OPTS_START + ((gpHal_Address_t)(pbmEntry))*GP_MM_RAM_PBM_OPTS_OFFSET))
/*rap_pbe2database(UInt8 pbe)*/
#define GP_HAL_PBM_ENTRY2ADDR_DATA_BASE(pbmEntry)   ((gpHal_Address_t)(GP_MM_RAM_PBM_0_DATA_START + ((gpHal_Address_t)(pbmEntry))*GP_MM_RAM_PBM_0_DATA_SIZE))

#define GP_RACE_MIC_LENGTH      4

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct {
   UInt16 frameControl;
   UInt8  sequenceNumber;
   UInt16 panID;
   union
   {
      struct {
         UInt16 destinationAddress;
         MACAddress_t sourceAddress;
         UInt8  entryPointOffset;
         UInt8  code[];
      } WithShortDestinationAddress;
      struct {
         MACAddress_t destinationAddress;
         MACAddress_t sourceAddress;
         UInt8  entryPointOffset;
         UInt8  code[];
      } WithLongDestinationAddress;
   };
} Race_RxPacketDescription_t;

typedef struct{
    UInt16 frameControl;
    UInt8  sequenceNumber;
    UInt16 panID;
    MACAddress_t destinationAddress;
    MACAddress_t sourceAddress;
    UInt8  payload[];
} Race_TxPacketDescription_t;

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
const ROM UInt8 FLASH_PROGMEM gpRace_Key[16]
        = {0x91, 0xC5, 0x72, 0xA9, 0x02, 0xEC, 0xC6, 0x82, 0x10, 0xF0, 0x5C, 0xA3, 0xEB, 0x14, 0xB8, 0xB7};

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

#if defined(GP_RACE_STIMULI_DEBUG)
extern UInt16 entryPoint(UInt16 txPBMAddress, UInt16 rxPBMAddress, UInt8 txPBMEntry, UInt8 rxPBMEntry, UInt16 codeStart);
#endif // defined(GP_RACE_STIMULI_DEBUG)

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

static void Race_gpHal_SetRxChannel(UInt8 channel)
{
    /*from gpHalMac_CalibrateChannel(UInt8 channel)*/
    //Calibrate channel before enabling receiver
    GP_WB_WRITE_RIB_CHANNEL_IDX(0);
    GP_WB_WRITE_RIB_CHANNEL_NR(channel-11);

    GP_WB_RIB_CHANNEL_CHANGE_REQUEST();
    while (GP_WB_READ_RIB_CHANNEL_CHANGE_BUSY());
}
 
/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpRace_Start(UInt16 initialTimeOutInMs, gpRace_ActionMapHandlerCallback_t cbActionMapHandler)
{
    //**********************
    //Assumptions
    //*watchdog is active
    //*radio is disabled
    //**********************

    UInt8  previousSequenceNumber = 0xFF;
    UInt16 downCounter            = initialTimeOutInMs;
    Bool   resetOnExit            = false;

    UInt16 originalPANID;
    UInt16 originalShortAddress;
    UInt16 originalChannel;
    Bool   originalRxOnWhenIdle;

    //**********************
    //Disable interrupts
    //**********************

    HAL_DISABLE_GLOBAL_INT();

    //**********************
    //Call BSP testmode init to perform platform specifics
    //**********************

#if defined(HAL_BSP_INIT_TESTMODE)
    HAL_BSP_INIT_TESTMODE();
#endif // defined(HAL_BSP_INIT_TESTMODE)
        
    //**********************
    //Prepare the TX PBM
    //**********************

    UInt8           txPBMEntry        = gpHal_GetHandle();
    gpHal_Address_t txPBMStart        = GP_HAL_PBM_ENTRY2ADDR_DATA_BASE(txPBMEntry);
    gpHal_Address_t txPBMOptions      = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(txPBMEntry);
    Race_TxPacketDescription_t* pTxPacket = (Race_TxPacketDescription_t*)txPBMStart;

    if (txPBMEntry != 0xFF)
    {
        pTxPacket->frameControl  = GP_RACE_TX_MAC_FRAME_CONTROL;
        pTxPacket->panID         = GP_RACE_PAN_ID;
        //pTxPacket->sequenceNumber = 0x0; - no more automatic insertion of DSN?
        gpHal_GetExtendedAddress(&pTxPacket->sourceAddress);

        //clear options
        MEMSET((UInt8*)txPBMOptions, 0, GP_MM_RAM_PBM_OPTS_SIZE);

        GP_WB_WRITE_PBM_FORMAT_T_GP_ACKED_MODE(txPBMOptions, true);
        //gpHal_CollisionAvoidanceModeCSMA
        GP_WB_WRITE_PBM_FORMAT_T_GP_CSMA_CA_ENABLE(txPBMOptions, true);

        GP_WB_WRITE_PBM_FORMAT_T_MIN_BE(txPBMOptions, 0x3);
        GP_WB_WRITE_PBM_FORMAT_T_MAX_BE(txPBMOptions, 0x5);
        GP_WB_WRITE_PBM_FORMAT_T_MAX_CSMA_BACKOFFS(txPBMOptions, 0x4);
        GP_WB_WRITE_PBM_FORMAT_T_MAX_FRAME_RETRIES(txPBMOptions, 0x3);

        GP_WB_WRITE_PBM_FORMAT_T_GP_ANTENNA(txPBMOptions,GP_RACE_TX_ANTENNA);
        GP_WB_WRITE_PBM_FORMAT_T_GP_VQ_SEL(txPBMOptions, GP_WB_ENUM_PBM_VQ_UNTIMED);

        GP_WB_WRITE_PBM_FORMAT_T_TX_POWER(txPBMOptions, 1); //New
    }

    //**********************
    //Switch to 8Mhz clk frequency to have accurate wait times
    //**********************

    //K7 doesn't switch clock frequency

    //**********************
    //Address and channel configuration for RACE
    //**********************

    originalPANID =        gpHal_GetPanId(gpHal_SourceIdentifier_0);
    originalShortAddress = gpHal_GetShortAddress(gpHal_SourceIdentifier_0);
    originalChannel =      gpHal_GetRxChannel(gpHal_SourceIdentifier_0);

    gpHal_SetPanId       ( GP_RACE_PAN_ID, gpHal_SourceIdentifier_0);
    gpHal_SetShortAddress( GP_RACE_SHORT_ADDR, gpHal_SourceIdentifier_0 );

    //**********************
    //Enable Radio
    //**********************

    originalRxOnWhenIdle = gpHal_GetRxOnWhenIdle();
    gpHal_SetRxOnWhenIdle(gpHal_SourceIdentifier_0, true, GP_RACE_CONTROL_CHANNEL);

#if defined(GP_RACE_STIMULI_DEBUG)
    //**********************
    //Execute stimuli directly for debug purposes
    //**********************

    UInt16 actionMap = entryPoint(txPBMStart, 0, txPBMEntry, 0, 0);

    if (!!(actionMap & GP_RACE_TRANSMIT_PACKET_MASK))
    {
        UInt8 len = GP_WB_READ_PBM_FORMAT_T_FRAME_LEN(GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(txPBMEntry));

        gpLog_PrintBuffer(len, (UInt8*)txPBMStart);
    }

    NOT_USED(downCounter);
    NOT_USED(previousSequenceNumber);
#else // defined(GP_RACE_STIMULI_DEBUG)
    //**********************
    //Wait for packet to be received
    //**********************

    while(downCounter)
    {

        //**********************
        //Wait 1 ms
        //**********************

        HAL_WAIT_MS(1);



        //**********************
        //Decrement down counter if not infite
        //**********************

        if (downCounter != GP_RACE_INFINITE_DOWN_COUNT)
        {
           downCounter--;
        }

        //**********************
        //Kick watchdog
        //**********************

        hal_ResetWatchdog();

        //**********************
        //Check if frame is received
        //**********************

        if (!!GP_WB_READ_RIB_UNMASKED_INT_DATA_IND())
        {
            //GP_LOG_SYSTEM_PRINTF("Rx!",0);
            //**********************
            //PAN ID and Sequence Number filtering
            //**********************
            UInt8  rxPBMEntry                = GP_WB_READ_RIB_PBETE_DATA_IND();
            gpHal_Address_t rxPBMStart       = GP_HAL_PBM_ENTRY2ADDR_DATA_BASE(rxPBMEntry);
            gpHal_Address_t rxPbmOptsBase    = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(rxPBMEntry);
            Race_RxPacketDescription_t* pRxPacket = (Race_RxPacketDescription_t*)rxPBMStart;
            UInt8  currentSequenceNumber     = pRxPacket->sequenceNumber;
            UInt16 currentPANID              = pRxPacket->panID;
            Bool   longDestinationAddressing = ((pRxPacket->frameControl & GP_RACE_MAC_FRAME_CONTROL_DESTINATION_ADDRESS_MODE_MASK) == GP_RACE_MAC_FRAME_CONTROL_DESTINATION_ADDRESS_MODE_BROADCAST);
            UInt8 micData[GP_RACE_MIC_LENGTH];

            GP_ASSERT_DEV_INT(rxPBMEntry != 0xFF);

            if (currentPANID == GP_RACE_PAN_ID && currentSequenceNumber != previousSequenceNumber)
            {
                if (GP_RACE_LED_BLINK_BACK)
                {
                    HAL_LED_SET(RED);
                    HAL_WAIT_MS(200);
                    HAL_LED_CLR(RED);
                    HAL_WAIT_MS(200);
                }

                //**********************
                //Prepare CCM Operation
                //**********************

                //nonce and frame counter are not used here
                //so same key/nonce pair is used multiple times and replay attacks are not prevented
                //this is done for ease of operation
                const UInt8 zeros[13] = {0};
                UInt32 tmpPtr; //avoid warnings for 24-bit registers

                UInt8 frame_length = GP_WB_READ_PBM_FORMAT_R_FRAME_LEN(rxPbmOptsBase);

                // nonce
                tmpPtr = (gpHal_Address_t)zeros;
                GP_WB_WRITE_SSP_NONCE_PTR(tmpPtr);
                // security key
                GP_WB_WRITE_SSP_KEY_PTR((UInt16)gpRace_Key);
                GP_WB_WRITE_SSP_KEY_LEN(GP_WB_ENUM_SSP_KEY_LEN_KEY_128);

#ifdef GP_RACE_ENABLE_ENCRYPTION
                // To setup for decryption and authentication
                // fixme: implement
#else
                tmpPtr = rxPBMStart;
                GP_WB_WRITE_SSP_A_PTR(tmpPtr);
                GP_WB_WRITE_SSP_A_LEN(frame_length-GP_RACE_MIC_LENGTH);
                tmpPtr+= frame_length;
                GP_WB_WRITE_SSP_MSG_PTR(tmpPtr);
                GP_WB_WRITE_SSP_MSG_OUT_PTR(tmpPtr);
                GP_WB_WRITE_SSP_MSG_LEN(0);
                tmpPtr-= GP_RACE_MIC_LENGTH;
                GP_WB_WRITE_SSP_MIC_PTR(tmpPtr);
                GP_WB_WRITE_SSP_MIC_LEN(GP_RACE_MIC_LENGTH >> 1);
                GP_WB_WRITE_SSP_MODE(GP_WB_ENUM_SSP_MODE_DECRYPT);
#endif
                //**********************
                //Do CCM Operation: Check authentication
                //**********************

                // start encryption
                GP_WB_SSP_START_ENCRYPT();
                //Polled wait for encryption to finish & assert when memory error occured
                while ( 1 == GP_WB_READ_SSP_BUSY() );
                GP_ASSERT_DEV_EXT(0 == GP_WB_READ_SSP_MEM_ERR());

                gpHal_ReadDataInPBMCyclic(rxPBMStart+frame_length-GP_RACE_MIC_LENGTH,0,micData, GP_RACE_MIC_LENGTH);
                Bool result = gpHal_ResultSuccess;
                for (UInt8 i = 0; i < 4; ++i)
                {
                    if (0 != micData[i])
                    {
                        //Decryption failed
                        result= gpHal_ResultInvalidParameter;
                        break;
                    }
                }

                if (result==gpHal_ResultSuccess) // passes CCM* authentication check
                {
                    gpHal_Address_t codeStart;
                    UInt32 entryPoint; //UInt32 since it is an address in the function table
                    UInt16 actionMap = 0;

                    //**********************
                    //Blink green LED once on successfull decryption and authentication check
                    //**********************

                    if (GP_RACE_LED_BLINK_BACK)
                    {
                        HAL_LED_SET(GRN);
                        HAL_WAIT_MS(200);
                        HAL_LED_CLR(GRN);
                        HAL_WAIT_MS(200);
                    }

                    if (longDestinationAddressing)
                    {
                        //**********************
                        //Preparing transmit PBM
                        //**********************
                        if (txPBMEntry != 0xFF)
                        {
                            pTxPacket->destinationAddress = pRxPacket->WithLongDestinationAddress.sourceAddress;
                            GP_WB_WRITE_PBM_FORMAT_T_FRAME_LEN(txPBMOptions, sizeof(Race_TxPacketDescription_t));
                        }

                        //**********************
                        //Preparing code execution
                        //**********************
                        codeStart   = (gpHal_Address_t)(pRxPacket->WithLongDestinationAddress.code);
                        entryPoint  = pRxPacket->WithLongDestinationAddress.entryPointOffset;
                    }
                    else
                    {
                        //**********************
                        //Preparing transmit PBM
                        //**********************
                        if (txPBMEntry != 0xFF)
                        {
                            pTxPacket->destinationAddress = pRxPacket->WithShortDestinationAddress.sourceAddress;
                            GP_WB_WRITE_PBM_FORMAT_T_FRAME_LEN(txPBMOptions, sizeof(Race_TxPacketDescription_t));
                        }

                        //**********************
                        //Preparing code execution
                        //**********************
                        codeStart   = (gpHal_Address_t)(pRxPacket->WithShortDestinationAddress.code);
                        entryPoint  = pRxPacket->WithShortDestinationAddress.entryPointOffset;
                    }
                    entryPoint += (gpHal_Address_t)(codeStart);
                    resetOnExit = true;

                    //**********************
                    //Executing the code
                    //**********************
                    //Aaargh - near mode is a pain - results in bsr.m that requires an entry in the function table
                    //"Real" is XAP terminology - don't blame me
                    {
                        typedef UInt16 (*Race_EntryPointReal_t) (UInt16 txPBMStart, UInt16 rxPBMStart, UInt8 txPBMEntry, UInt8 rxPBMEntry, UInt16 codeStart);
                        // in near mode, take address of entrypoint
                        Race_EntryPointReal_t entryPointReal = (Race_EntryPointReal_t)(&entryPoint); //function table entry
                        actionMap = entryPointReal(txPBMStart, rxPBMStart, txPBMEntry, rxPBMEntry, codeStart);
                    }

                    //**********************
                    //Handling the action map
                    //**********************

                    if (cbActionMapHandler)
                    {
                        cbActionMapHandler(actionMap);
                    }
                    if (!!(actionMap & GP_RACE_STOP_MASK))
                    {
                        //fall through the for loop
                        downCounter            = 0;
                    }
                    else if (!!(actionMap & GP_RACE_INFINITE_MASK))
                    {
                        //stay forever in the for loop
                        downCounter            = GP_RACE_INFINITE_DOWN_COUNT;
                    }
                    else
                    {
                        //use default value for thhe downcounter used in the for loop
                        downCounter            = GP_RACE_ON_PACKET_DOWN_COUNT_IN_MS;
                    }

                    if (!!(actionMap & GP_RACE_RETURN_TO_CONTROL_CHANNEL_MASK))
                    {
                        Race_gpHal_SetRxChannel( GP_RACE_CONTROL_CHANNEL );
                    }

                    if (!!(actionMap & GP_RACE_TRANSMIT_PACKET_MASK))
                    {
                        UInt8 txDownCounter = 255;

                        GP_WB_WRITE_QTA_PBEFE_DATA_REQ(txPBMEntry);

                        while (!!txDownCounter && !GP_WB_READ_RIB_UNMASKED_INT_DATA_CNF_0());
                        {
                            //txDownCounter spans 255 ms;
                            HAL_WAIT_MS(1);
                        }

                        GP_ASSERT_DEV_INT(!!txDownCounter);
                        {
                            UInt8 txCnfPBMEntry = GP_WB_READ_RIB_PBETE_DATA_CNF_0();
                            NOT_USED(txCnfPBMEntry);
                            GP_ASSERT_DEV_INT(txCnfPBMEntry == txPBMEntry);
                        }
                    }

                    //**********************
                    //Sequence number update
                    //**********************

                    previousSequenceNumber = currentSequenceNumber;
                }
            }

           //**********************
           //Return received PBM
           //**********************

           gpHal_FreeHandle(rxPBMEntry);
        }
    }
#endif // defined(GP_RACE_STIMULI_DEBUG)

    //**********************
    //Reset in case of possibly polluted state
    //**********************

    if(resetOnExit)
    {
        gpReset_ResetByWatchdog();
    }

    //**********************
    //Disable Radio
    //**********************

    gpHal_SetRxOnWhenIdle(gpHal_SourceIdentifier_0, originalRxOnWhenIdle, originalChannel);

    //**********************
    //Restore
    //**********************

    gpHal_SetPanId( originalPANID, gpHal_SourceIdentifier_0);
    gpHal_SetShortAddress( originalShortAddress, gpHal_SourceIdentifier_0);
    //K7 doesn't set clock frequency

    //**********************
    //Return PBM used for transmit
    //**********************

    if (txPBMEntry != 0xFF)
    {
       gpHal_FreeHandle(txPBMEntry);
    }

    //**********************
    //Call BSP testmode deinit to perform platform specifics
    //**********************

#if defined(HAL_BSP_DEINIT_TESTMODE)
    HAL_BSP_DEINIT_TESTMODE();
#endif // defined(HAL_BSP_DEINIT_TESTMODE)

    //**********************
    //Blink red LED twice on leaving RACE
    //**********************

    if (GP_RACE_LED_BLINK_BACK)
    {
       HAL_LED_SET(RED);
       HAL_WAIT_MS(200);
       HAL_LED_CLR(RED);
       HAL_WAIT_MS(200);
       HAL_LED_SET(RED);
       HAL_WAIT_MS(200);
       HAL_LED_CLR(RED);
       HAL_WAIT_MS(200);
    }

    //**********************
    //Re-enable interrupts
    //**********************

    HAL_ENABLE_GLOBAL_INT();
}

