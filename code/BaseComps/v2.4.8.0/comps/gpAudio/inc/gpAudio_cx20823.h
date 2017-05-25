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
 *     0!                         $Header: //depot/main/Embedded/BaseComps/vlatest/sw/comps/gpAudio/inc/gpAudio_cx20823.h#1 $
 *    M'   GreenPeak              $Change: 79997 $
 *   0'         Technologies      $DateTime: 2016/04/30 15:17:21 $
 *  F
 */
 
#ifndef _GP_AUDIO_CX20823_H_
#define _GP_AUDIO_CX20823_H_

/*****************************************************************************
 *                    Include Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
#define gpAudio_DmaBufferStateEmpty      0
#define gpAudio_DmaBufferStateFull       1    
#define gpAudio_DmaBufferStateActive     2
#define gpAudio_DmaBufferStateInactive   0xFF

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
/* Audio packet defines */
#define GP_AUDIO_PACKET_LENGTH          90
#define ADPCM_DATA_ARRAY_LENGTH         90

/* DMA defines */
#define GP_AUDIO_DMA_BUF_LENGTH         240
#define GP_AUDIO_DMA_BUF_THRESHOLD      ((UInt8)(GP_AUDIO_DMA_BUF_LENGTH / 2))
#define HAL_ENABLE_DMA_INT(en)          do{                                                  \
                                            HAL_DISABLE_GLOBAL_INT();                        \
                                            GP_WB_WRITE_INT_CTRL_MASK_INT_DMA_INTERRUPT(en); \
                                            HAL_ENABLE_GLOBAL_INT();                         \
                                        }while(false)

/* TWI defines */
#define TWI_WRITE_MAX_RETRIES           3

/* SPI Slave macros */
#define SSPI_SSN                        GP_WB_READ_GPIO_GPIO4_INPUT_VALUE()
#define SSPI_ACTIVE()                   (SSPI_SSN == 0)
#define SSPI_INACTIVE()                 (SSPI_SSN != 0)
#define SSPI_WAIT_UNTIL_INACTIVE()      do{                            \
                                            if (SSPI_ACTIVE())         \
                                            {                          \
                                                while (SSPI_ACTIVE()); \
                                            }                          \
                                        }while(false)

/* Debug defines */
//#define DUMP_PCM_DATA
#ifdef  DUMP_PCM_DATA
    /* Use a local RAM buffer of size AUDIO_RAMBUFFER_SIZE and store PCM data in this buffer
     * Dump the content of this buffer on the serial output after the voice session has ended
     * This data can be played back and verified 4096
     */
    #define AUDIO_RAMBUFFER_SIZE        5120
#endif

/* Conexant codec defines */
#define CX20823_DEVICE_ADDRESS           0x88 /* 0x44 << 1 */
//#define CX20823_TWO_BYTE_REG_ADDRESS
#define CX20823_CODEC_WORD_SIZE          2
#define CX20823_INIT_REG_VALUES          \
                        0x01, 0x0D,   \
                        0x02, 0x18,   \
                        0x09, 0x01,   \
                        0x0A, 0x05,   \
                        0x0A, 0xC5,   \
                        0x10, 0x19,   \
                        0x10, 0x99,   \
                        0x11, 0x03,   \
                        0x16, 0x40,   \
                        0x20, 0xB8,   \
                        0x21, 0x02,   \
                        0x30, 0x0D,   \
                        0x31, 0x3E,   \
                        0x32, 0x08,   \
                        0x33, 0x01,   \
                        0x40, 0x1F,   \
                        0x41, 0x01,   \
                        0x60, 0x00,   \
                        0x61, 0x40,   \
                        0x62, 0x11,   \
                        0x63, 0x58,   \
                        0x64, 0x21,   \
                        0x65, 0x0A,   \
                        0x66, 0x00,   \
                        0x67, 0x00,   \
                        0x68, 0x00,   \
                        0x80, 0x03,   \
                        0x83, 0x00

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/
static void Audio_CodecInit(void);
static void Audio_CodecWakeup(void);
static void Audio_CodecSleep(void);
static void Audio_SSPI_PinConfig(void);
Int16 Audio_DmaReadBuffer(UInt8* pRxData);
#ifdef DUMP_PCM_DATA
static void Audio_DumpPCMData(void);
#endif /* DUMP_PCM_DATA */

#endif // _GP_AUDIO_CX20823_H_
