/*
 * Copyright (c) 2016, GreenPeak Technologies
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
 *     0!                         $Header: //depot/main/Embedded/BaseComps/vlatest/sw/comps/gpAudio/src/gpAudio_cx20823.c#1 $
 *    M'   GreenPeak              $Change: 79997 $
 *   0'         Technologies      $DateTime: 2016/04/30 15:17:21 $
 *  F
 */

//#define GP_LOCAL_LOG

/*****************************************************************************
 *                    Include Definitions
 *****************************************************************************/
#define  GP_COMPONENT_ID  GP_COMPONENT_ID_AUDIO
#include "gpAudio_cx20823.h"

 /*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
/* Codec register values */
static UInt16 Audio_CodecRegisterValues[] = { CX20823_INIT_REG_VALUES };

/* PCM audio data buffer */
UInt8  pcmAudioData[256];

/* ADPCM audio data buffer */
UInt8  adpcmAudioData[ADPCM_DATA_ARRAY_LENGTH];
UInt8  adpcmAudioDataIndex = 0;
gpCompression_Compression_State_t Audio_AdpcmCompressionState;

/* DMA variables */
static UInt8 Audio_DMABuffer[GP_AUDIO_DMA_BUF_LENGTH];
UInt16 DMA_LastReadIndex;
UInt16 DMA_ReadIndex;
UInt8  DMA_ReadWrap;
UInt8  DMA_BufferState;

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
static void INLINE Audio_Init(void)
{
    /* Initialize I2C for codec configuration */
    hal_InitTWI();

    /* Initialize Conexant audio codec */
    Audio_CodecInit();
}

static void Audio_SSPI_PinConfig(void)
{
    HAL_DISABLE_GLOBAL_INT();

    /* Disable MSI interface */
    GP_WB_WRITE_MSI_SERIAL_ITF_SELECT(GP_WB_ENUM_SERIAL_ITF_SELECT_NO_INTERFACE);
    GP_WB_WRITE_SPI_SL_TX_LATENCY(0); /* Not needed though */
    /* Config SSn */
    GP_WB_WRITE_IOB_GPIO_4_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP);
    /* Config MOSI */
    GP_WB_WRITE_IOB_GPIO_2_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP);
    /* Wait for pull-ups to be active */
    HAL_WAIT_US(50); 
    /* SSPI slave pin config - only one mapping (A)*/
    GP_WB_WRITE_SPI_SL_PINMAP(GP_WB_ENUM_GENERIC_SINGLE_PINMAP_MAPPING_A);
    /* Ensure RX FIFO is empty */
    while (GP_WB_READ_SPI_SL_UNMASKED_RX_NOT_EMPTY_INTERRUPT())
    {
        GP_WB_READ_SPI_SL_RX_DATA();
    }
    /* clear SPI Slave INT */
    GP_WB_SPI_SL_CLEAR_TX_UNDERRUN_INTERRUPT();
    GP_WB_SPI_SL_CLEAR_RX_OVERRUN_INTERRUPT();

    HAL_ENABLE_GLOBAL_INT();
}

static Bool INLINE Audio_StartProcessing(void)
{
    Bool wordmode_16bit = false; /* use 8 bit mode */
    MEMSET(Audio_DMABuffer, 0, GP_AUDIO_DMA_BUF_LENGTH);
    adpcmAudioDataIndex = 0;

	Audio_Init();

    /* Pin configuration of SPI Slave device */
    Audio_SSPI_PinConfig();

    /* Active codec */
    Audio_CodecWakeup();

    /* Lock DMA */
    if(hal_LockDMA())
    {
        DMA_BufferState   = gpAudio_DmaBufferStateEmpty;
        DMA_ReadIndex     = 0;
        DMA_LastReadIndex = 0;
        DMA_ReadWrap      = 0;
        /* Setup SPI slave and DMA */
        hal_ConfigureDmaForI2S(wordmode_16bit, GP_AUDIO_DMA_BUF_LENGTH, (UInt8*)Audio_DMABuffer);
        /* Enable DMA */
        HAL_ENABLE_DMA_INT(true);
        return true;
    }
    GP_LOG_SYSTEM_PRINTF("Could not lock DMA",0);
    return false;
}

static void INLINE Audio_StopProcessing(void)
{
    Audio_CodecSleep();
    HAL_ENABLE_DMA_INT(false);
    hal_ReleaseDMA();
    hal_DisableSPI();
    /* Disable SPI Slave and enable keyboard scan on GPIO1 */
    GP_WB_WRITE_MSI_SERIAL_ITF_SELECT(GP_WB_ENUM_SERIAL_ITF_SELECT_NO_INTERFACE); 
    GP_WB_WRITE_SPI_SL_PINMAP(GP_WB_ENUM_GENERIC_SINGLE_PINMAP_NOT_MAPPED);
    GP_WB_WRITE_IOB_GPIO_1_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP);
#ifdef DUMP_PCM_DATA
    dumpPCMBufferIndex = 0;
    gpSched_ScheduleEvent(100000,Audio_DumpPCMData);
#endif /* DUMP_PCM_DATA */
}

#ifdef DUMP_PCM_DATA
static void Audio_DumpPCMData(void)
{
    GP_LOG_SYSTEM_PRINTF("  0x%x, 0x%x, 0x%x, 0x%x,",4,
                         pcmBuffer[dumpPCMBufferIndex], 
                         pcmBuffer[dumpPCMBufferIndex+1],
                         pcmBuffer[dumpPCMBufferIndex+2],
                         pcmBuffer[dumpPCMBufferIndex+3]);
    if(dumpPCMBufferIndex< AUDIO_RAMBUFFER_SIZE-10 && (dumpPCMBufferIndex < pcmBufferIndex-5))
    {
        dumpPCMBufferIndex+=4;
        gpSched_ScheduleEvent(100000,Audio_DumpPCMData);
    }
}
#endif /* DUMP_PCM_DATA */

Int16 Audio_DmaReadBuffer(UInt8* pRxData)
{
    UInt16 rptr = DMA_LastReadIndex;
    UInt16 maxReadBytes, readBytes = 0;
    UInt16 word;

    if(pRxData == NULL)
    {
        return -1;
    }

    /* Read data in the inactive period */
    SSPI_WAIT_UNTIL_INACTIVE();

    if(gpAudio_DmaBufferStateFull == DMA_BufferState)
    {
        /* If buffer full then empty, reset pointers and adjust the read ptr */ 
        /* Reset cyclic buffer ptr */
        GP_WB_DMA_RESET_POINTERS();
        maxReadBytes = GP_AUDIO_DMA_BUF_LENGTH;
    }
    else if (gpAudio_DmaBufferStateActive == DMA_BufferState)
    {
        maxReadBytes = GP_AUDIO_DMA_BUF_THRESHOLD;
    }
    else
    {
        GP_LOG_SYSTEM_PRINTF("DMA buffer not full",0);
        return -1;
    }

    while(readBytes < maxReadBytes)
    {
        /* Handle read ptr wrap when reading FULL buffer */
        MEMCPY(&word, (UInt8*)&Audio_DMABuffer[(rptr + readBytes) & 0xFF], CX20823_CODEC_WORD_SIZE);
        /*  8 bit tranfer ops only in SSPI, in LJM convert the word in little-endian */
        word      = GP_SWAP_UINT16_CONVERSION(word);
        MEMCPY(pRxData, &word, CX20823_CODEC_WORD_SIZE);
        pRxData   += CX20823_CODEC_WORD_SIZE;
        readBytes += CX20823_CODEC_WORD_SIZE;
    }
    
    if(gpAudio_DmaBufferStateFull == DMA_BufferState)
    {
        /* Set the DMA read ptr
         * Make the ptr and wrap reflect the buffer is empty
         */
        /* Update read ptr */
        DMA_ReadIndex     = 0;
        DMA_ReadWrap      = 1 - DMA_ReadWrap;
        DMA_LastReadIndex = 0;
        GP_WB_WRITE_DMA_BUFFER_PTR_VALUE(DMA_ReadIndex);
        GP_WB_WRITE_DMA_BUFFER_PTR_WRAP_VALUE(DMA_ReadWrap);
        GP_WB_DMA_SET_SRC_WRITE_PTR();
    }
    return readBytes;
}

static void Audio_ProcessSamples(UInt8* pOutBuffer, gpCompression_Compression_State_t *pState)
{    
    /* Copy ADPCM data to pOutBuffer */
    MEMCPY(pOutBuffer,adpcmAudioData,ADPCM_DATA_ARRAY_LENGTH);
    MEMCPY(pState,&Audio_AdpcmCompressionState, sizeof(gpCompression_Compression_State_t));
    adpcmAudioDataIndex = 0;
}

/* DMA INT has higher priority than the PWM INT */
INTERRUPT_H void handle_dma_int(void)
{
    HAL_WDT_RESET();
    Bool cb = false;
    /* Disable DMA INT */
    HAL_ENABLE_DMA_INT(false);

    if (GP_WB_READ_INT_CTRL_MASKED_DMA_CPY_ERR_INTERRUPT())
    {
        GP_WB_DMA_CLR_CPY_ERR_INTERRUPT();
    }
    if (GP_WB_READ_INT_CTRL_MASKED_DMA_DEST_OVERFLOW_INTERRUPT())
    {
        GP_WB_DMA_CLR_DEST_OVERFLOW_INTERRUPT();
        DMA_LastReadIndex = DMA_ReadIndex;
        DMA_BufferState = gpAudio_DmaBufferStateFull;
        cb = true;
    }
    if (GP_WB_READ_INT_CTRL_MASKED_DMA_SRC_UNDERRUN_INTERRUPT())
    {
        GP_WB_DMA_CLR_SRC_UNDERRUN_INTERRUPT();
    }
    if (GP_WB_READ_INT_CTRL_MASKED_DMA_DEST_ALMOST_FULL_INTERRUPT())
    {
        /* Save current read index */
        DMA_LastReadIndex = DMA_ReadIndex;
        DMA_ReadIndex += GP_AUDIO_DMA_BUF_THRESHOLD;

        if(DMA_ReadIndex >= GP_AUDIO_DMA_BUF_LENGTH)
        {            
            DMA_ReadIndex = 0;
            DMA_ReadWrap = 1 - DMA_ReadWrap;
        }

        GP_WB_WRITE_DMA_BUFFER_PTR_VALUE((UInt8)DMA_ReadIndex);
        GP_WB_WRITE_DMA_BUFFER_PTR_WRAP_VALUE(DMA_ReadWrap);
        GP_WB_DMA_SET_DEST_READ_PTR();

        DMA_BufferState = gpAudio_DmaBufferStateActive;
        cb = true;
    }
    
    if(cb)
    {
        /* Read PCM data from DMA buffer */
        Int16 ret = Audio_DmaReadBuffer((UInt8*)&pcmAudioData[0]);
        if(ret != -1)
        {
            UInt16 i = 0;
            UInt16 j = 0;
            Int16 RecBuf[128];
            /* Convert PCM data from UInt8 into signed Int16 */
            for(i=0; i<ret; i+=2)
            {
                // convert UInt8 into Int16
                Int16 data1 = (Int16)(pcmAudioData[i + 1]);
                Int16 data2 = (Int16)(pcmAudioData[i]) & 0xFFFF;
                RecBuf[j] = ((data1 << 8) & 0xFF00) | data2;
                j++;
            }
            /* Compress PCM data into ADPCM data */
            for(i=0;i<j;i+=2)
            {
                if(adpcmAudioDataIndex<ADPCM_DATA_ARRAY_LENGTH)
                {
                    // Encode the first 16-bit sample
                    UInt8 code = gpCompression_ADPCMEncode(RecBuf[i], &Audio_AdpcmCompressionState);
                    // Store the first 4-bit sample - reversing nibbles to match the SOX default.
                    adpcmAudioData[adpcmAudioDataIndex] = code<< 0;
                    // Encode the second 16-bit sample
                    code = gpCompression_ADPCMEncode(RecBuf[i+1], &Audio_AdpcmCompressionState);
                    // Store the second 4-bit sample - One adpcm_byte contains two 4-bit ADPCM samples
                    adpcmAudioData[adpcmAudioDataIndex] |= (code << 4);
                    adpcmAudioDataIndex++;
                }
                else
                {
                    GP_LOG_SYSTEM_PRINTF("Overflow %d",1,adpcmAudioDataIndex); gpLog_Flush();
                }
            }
            if(adpcmAudioDataIndex>=ADPCM_DATA_ARRAY_LENGTH)
            {
                if(!gpSched_ExistsEvent(Audio_getADPCM))
                {
                    gpSched_ScheduleEvent(0,Audio_getADPCM);
                    adpcmAudioDataIndex = 0;
                }
            }
        }
    }
    /* Enable DMA INT */    
     HAL_ENABLE_DMA_INT(true);
}

/*****************************************************************************
 *                Codec Specific Function Definitions
 *****************************************************************************/
static void Audio_ReadRegisters(void)
{
    UIntLoop i;
    UIntLoop j;
    UInt8 nbReg = sizeof(Audio_CodecRegisterValues) / sizeof(UInt16);

    for(i=0;i < nbReg; )
    {
        HAL_WDT_RESET();
        HAL_WAIT_MS(5);
        UInt8 txBuffer[2];
        UInt8 rxBuffer[2];
        // Process register address
        txBuffer[0] = (UInt8) (Audio_CodecRegisterValues[i]) & 0x00FF;
        i++;
        i++;

        for (j = 0; j < TWI_WRITE_MAX_RETRIES; j++)
        {
            if (hal_WriteReadTWI(CX20823_DEVICE_ADDRESS, 1, &txBuffer[0], 1, &rxBuffer[0]))
            {
                GP_LOG_PRINTF("R 0x44 0x%x 0x%x",2,(UInt8)txBuffer[0], (UInt8)rxBuffer[0]);
                break;
            }
        }
    }
}

static void Audio_CodecInit(void)
{
    UIntLoop i;
    UIntLoop j;
    UInt8 nbReg = sizeof(Audio_CodecRegisterValues) / sizeof(UInt16);

    GP_LOG_SYSTEM_PRINTF("Codec configuration",0);

    /* Pull ENABLE line up */
    GP_WB_WRITE_IOB_GPIO_16_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP);
#if 0

	GP_WB_WRITE_GPIO_GPIO16_DIRECTION(1);
	GP_WB_WRITE_GPIO_GPIO_CLOCK_ENABLE(1);
    GP_WB_WRITE_GPIO_GPIO16_OUTPUT_VALUE(0);
    GP_WB_WRITE_GPIO_GPIO16_OUTPUT_VALUE(1);
    GP_WB_WRITE_GPIO_GPIO16_OUTPUT_VALUE(0);
#endif	


	
    /* Enable MCLK of codec */
    GP_WB_WRITE_STANDBY_CLK_SELECT_CPU(GP_WB_ENUM_EXT_CLKFREQ_MHZ8);
    GP_WB_WRITE_IOB_MCU_CLK_PINMAP(GP_WB_ENUM_GENERIC_SINGLE_PINMAP_MAPPING_A);
    GP_WB_WRITE_STANDBY_CLK_ENA_CPU(true);

    HAL_WAIT_MS(30);

    for(i=0;i < nbReg; )
    {
        HAL_WDT_RESET();
#ifndef CX20823_TWO_BYTE_REG_ADDRESS
        UInt8 txBuffer[2];
        // Process register address
        txBuffer[0] = (UInt8) (Audio_CodecRegisterValues[i]) & 0x00FF;
        i++;
        // Process register value
        txBuffer[1] = (UInt8)  (Audio_CodecRegisterValues[i] & 0x00FF);
        i++;

        for (j = 0; j < TWI_WRITE_MAX_RETRIES; j++)
        {
            if (hal_WriteReadTWI(CX20823_DEVICE_ADDRESS, 2, txBuffer, 0, NULL))
            {
                //GP_LOG_PRINTF("W %d %d %d 0x34 0x%x%x",2,j,i,nbReg,(UInt8)txBuffer[0], (UInt8)txBuffer[1]);
                break;
            }
        }
#else // CX20823_TWO_BYTE_REG_ADDRESS
        /* According to section 5.4.1 of the CX20823 datasheet: send high byte followed by low byte */
        UInt8 txBuffer[3];
        // Process register address
        txBuffer[0] = 0x00; /* High byte is always 0, see chapter 7 of the CX20823 datasheet */
        txBuffer[1] = (UInt8) (Audio_CodecRegisterValues[i]) & 0x00FF;
        i++;
        // Process register value
        txBuffer[2] = (UInt8)  (Audio_CodecRegisterValues[i] & 0x00FF);
        i++;

        for (j = 0; j < TWI_WRITE_MAX_RETRIES; j++)
        {
            if (hal_WriteReadTWI(CX20823_DEVICE_ADDRESS, 3, txBuffer, 0, NULL))
            {
                //GP_LOG_PRINTF("W %d %d %d 0x34 0x%x%x",2,j,i,nbReg,(UInt8)txBuffer[0], (UInt8)txBuffer[1]);
                break;
            }
        }
#endif // CX20823_TWO_BYTE_REG_ADDRESS
    }
    //GP_WB_WRITE_STANDBY_CLK_ENA_CPU(false);
    gpSched_ScheduleEvent(0,Audio_ReadRegisters);
}

static void Audio_CodecWakeup(void)
{
#if 1
    /* Pull ENABLE line up */
    GP_WB_WRITE_IOB_GPIO_16_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP);
    /* Enable MCLK of codec */
    GP_WB_WRITE_STANDBY_CLK_SELECT_CPU(GP_WB_ENUM_EXT_CLKFREQ_MHZ8);
    GP_WB_WRITE_IOB_MCU_CLK_PINMAP(GP_WB_ENUM_GENERIC_SINGLE_PINMAP_MAPPING_A);
    GP_WB_WRITE_STANDBY_CLK_ENA_CPU(true);
#endif
}

static void Audio_CodecSleep(void)
{
#if 1
    /* Disable MCLK */
    GP_WB_WRITE_STANDBY_CLK_ENA_CPU(false);
    /* Pull ENABLE line low */
    GP_WB_WRITE_IOB_GPIO_16_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN);
#endif
}
