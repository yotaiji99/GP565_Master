/*
 * Copyright (c) 2013, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpPad/src/gpPad_ram.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
//#define GP_LOCAL_LOG
#include "gpLog.h"
#include "gpAssert.h"

#include "gpHal_MAC.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

#define PAD_CHECK_HANDLE_VALID(padHandle)      (padHandle < GP_PAD_NR_OF_HANDLES)
#define PAD_CHECK_HANDLE_IN_USE(padHandle)     (gpPad_Handles[padHandle])
#define PAD_CHECK_HANDLE_ACCESSIBLE(padHandle) (PAD_CHECK_HANDLE_VALID(padHandle) && PAD_CHECK_HANDLE_IN_USE(padHandle))

#define PAD_HANDLE_CLAIM(padHandle)           do { gpPad_Handles[padHandle] = true; } while(false);
#define PAD_HANDLE_FREE(padHandle)            do { gpPad_Handles[padHandle] = false; } while(false);

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
typedef gpPad_Attributes_t gpPad_Descriptor_t;

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

#ifdef USE_PRAGMA
#pragma USER_DATA_MAPPING GEN_EXTRAM_SECTION
#endif // USE_PRAGMA
gpPad_Descriptor_t gpPad_Descriptors[GP_PAD_NR_OF_HANDLES] GP_EXTRAM_SECTION_ATTR;
#ifdef USE_PRAGMA
#pragma USER_DATA_MAPPING
#endif // USE_PRAGMA

//Array for handle administration
Bool gpPad_Handles[GP_PAD_NR_OF_HANDLES] = {false};


/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static void Pad_Init(void);

//Descriptor handling
static gpPad_Handle_t Pad_GetPad(gpPad_Attributes_t* pInitAttributes);
static void           Pad_FreePad(gpPad_Handle_t padHandle);
static gpPad_Result_t Pad_CheckPadValid(gpPad_Handle_t padHandle);

//Set handling
static void Pad_SetAttributes(gpPad_Handle_t padHandle, gpPad_Attributes_t* pAttributes);

static void Pad_SetTxChannels(gpPad_Handle_t padHandle, UInt8* channels);
static void Pad_SetTxPower(gpPad_Handle_t padHandle, Int8 txPower);
static void Pad_SetTxAntenna(gpPad_Handle_t padHandle, UInt8 antenna);
static void Pad_SetTxMinBE(gpPad_Handle_t padHandle, UInt8 minBE);
static void Pad_SetTxMaxBE(gpPad_Handle_t padHandle, UInt8 maxBE);
static void Pad_SetTxMaxCsmaBackoffs(gpPad_Handle_t padHandle, UInt8 maxCsmaBackoffs);
static void Pad_SetTxMaxFrameRetries(gpPad_Handle_t padHandle, UInt8 maxFrameRetries);
static void Pad_SetTxCsmaMode(gpPad_Handle_t padHandle, gpHal_CollisionAvoidanceMode_t csma);
static void Pad_SetCcaMode(gpPad_Handle_t padHandle, UInt8 cca);

//Get handling
static void Pad_GetAttributes(gpPad_Handle_t padHandle, gpPad_Attributes_t* pAttributes);

static void Pad_GetTxChannels(gpPad_Handle_t padHandle, UInt8* channels);
static Int8 Pad_GetTxPower(gpPad_Handle_t padHandle);
static UInt8 Pad_GetTxAntenna(gpPad_Handle_t padHandle);
static UInt8 Pad_GetTxMinBE(gpPad_Handle_t padHandle);
static UInt8 Pad_GetTxMaxBE(gpPad_Handle_t padHandle);
static UInt8 Pad_GetTxMaxCsmaBackoffs(gpPad_Handle_t padHandle);
static UInt8 Pad_GetTxMaxFrameRetries(gpPad_Handle_t padHandle);
static UInt8 Pad_GetTxCsmaMode(gpPad_Handle_t padHandle);
static UInt8 Pad_GetCcaMode(gpPad_Handle_t padHandle);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

static void Pad_Init(void)
{
    UInt8 i;
    for(i = 0; i<GP_PAD_NR_OF_HANDLES; i++)
    {
        gpPad_Handles[i]=false;
        MEMSET(&gpPad_Descriptors[i], 0, sizeof(gpPad_Descriptor_t));
    }
}
    
static gpPad_Handle_t Pad_GetPad(gpPad_Attributes_t* pInitAttributes)
{
    gpPad_Handle_t padHandle = 0;

    for(padHandle = 0; padHandle < GP_PAD_NR_OF_HANDLES; padHandle++)
    {
        if(!PAD_CHECK_HANDLE_IN_USE(padHandle))
        {
            GP_ASSERT_DEV_EXT(pInitAttributes);
            PAD_HANDLE_CLAIM(padHandle);
            GP_LOG_PRINTF("G %i",0,(UInt16)padHandle);

            Pad_SetAttributes(padHandle, pInitAttributes);
            return padHandle;
        }
    }
    GP_LOG_PRINTF("G 0xFF",0);
    return GP_PAD_INVALID_HANDLE;
}

static void Pad_FreePad(gpPad_Handle_t padHandle)
{
    GP_LOG_PRINTF("F %i",0,(UInt16)padHandle);

    if(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle))
    {
        PAD_HANDLE_FREE(padHandle);
    }
    else
    {
        GP_ASSERT_DEV_EXT(false); //Make no fun about freeing unused handles!
    }
}

static gpPad_Result_t Pad_CheckPadValid(gpPad_Handle_t padHandle)
{
    gpPad_Result_t result = gpPad_ResultValidHandle;
    
    if(!PAD_CHECK_HANDLE_VALID(padHandle))
    {
        result = gpPad_ResultInvalidHandle;
    }
    else if(!PAD_CHECK_HANDLE_IN_USE(padHandle))
    {
        result = gpPad_ResultNotInUse;
    }
    GP_LOG_PRINTF("Chck %i %i",0,(UInt16)padHandle, (UInt16)result);
    
    return result;
}

//Attributes
void Pad_SetAttributes(gpPad_Handle_t padHandle, gpPad_Attributes_t* pAttributes)
{
    GP_ASSERT_DEV_EXT(pAttributes);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    
    MEMCPY(&gpPad_Descriptors[padHandle],  pAttributes, sizeof(gpPad_Attributes_t));
#define pAttr  (&gpPad_Descriptors[padHandle])
    GP_LOG_PRINTF("ch:%u,%u,%u a:%u mBE:%u/%u",0, \
                                        pAttr->channels[0], pAttr->channels[1], pAttr->channels[2], \
                                        pAttr->antenna, \
                                        pAttr->maxBE, pAttr->minBE);
    GP_LOG_PRINTF("mBO:%u mFR:%u Pow:%i cs:%i/%i",0,\
                                        pAttr->maxCsmaBackoffs, pAttr->maxFrameRetries, \
                                        pAttr->txPower, \
                                        pAttr->csma, pAttr->cca);
#undef pAttr
}

void Pad_GetAttributes(gpPad_Handle_t padHandle, gpPad_Attributes_t* pAttributes)
{
    GP_ASSERT_DEV_EXT(pAttributes);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    
    MEMCPY(pAttributes, &gpPad_Descriptors[padHandle], sizeof(gpPad_Attributes_t));
}

//Separate set's
void Pad_SetTxChannels(gpPad_Handle_t padHandle, UInt8* channels)
{
    GP_ASSERT_DEV_EXT(channels);
    GP_LOG_PRINTF("[%i] ch:%i,%i,%i",0,padHandle,channels[0],channels[1],channels[2]);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    MEMCPY(gpPad_Descriptors[padHandle].channels, channels, GP_PAD_MAX_CHANNELS);
}
void Pad_SetTxPower(gpPad_Handle_t padHandle, Int8 txPower)
{
    GP_LOG_PRINTF("[%i] txp:%i",0,padHandle,txPower);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    gpPad_Descriptors[padHandle].txPower = txPower;
}
void Pad_SetTxAntenna(gpPad_Handle_t padHandle, UInt8 antenna)
{
    GP_LOG_PRINTF("[%i] ant:%i",0,padHandle,antenna);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));    
    gpPad_Descriptors[padHandle].antenna = antenna;
}
void Pad_SetTxMinBE(gpPad_Handle_t padHandle, UInt8 minBE)
{
    GP_LOG_PRINTF("[%i] minBE:%i",0,padHandle,minBE);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));    
    gpPad_Descriptors[padHandle].minBE = minBE;
}
void Pad_SetTxMaxBE(gpPad_Handle_t padHandle, UInt8 maxBE)
{
    GP_LOG_PRINTF("[%i] maxBE:%i",0,padHandle,maxBE);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));    
    gpPad_Descriptors[padHandle].maxBE = maxBE;
}
void Pad_SetTxMaxCsmaBackoffs(gpPad_Handle_t padHandle, UInt8 maxCsmaBackoffs)
{
    GP_LOG_PRINTF("[%i] maxBO:%i",0,padHandle,maxCsmaBackoffs);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));    
    gpPad_Descriptors[padHandle].maxCsmaBackoffs = maxCsmaBackoffs;
}
void Pad_SetTxMaxFrameRetries(gpPad_Handle_t padHandle, UInt8 maxFrameRetries)
{
    GP_LOG_PRINTF("[%i] maxFR:%i",0,padHandle,maxFrameRetries);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));    
    gpPad_Descriptors[padHandle].maxFrameRetries = maxFrameRetries;
}
void Pad_SetTxCsmaMode(gpPad_Handle_t padHandle, UInt8 csma)
{
    GP_LOG_PRINTF("[%i] csma:%i",0,padHandle,csma);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));    
    gpPad_Descriptors[padHandle].csma = csma;
}
void Pad_SetCcaMode(gpPad_Handle_t padHandle, UInt8 cca)
{
    GP_LOG_PRINTF("[%i] cca:%i",0,padHandle,cca);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));    
    gpPad_Descriptors[padHandle].cca = cca;
}

//Separate get's
void Pad_GetTxChannels(gpPad_Handle_t padHandle, UInt8* channels)
{
    GP_ASSERT_DEV_EXT(channels);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    
    MEMCPY(channels, gpPad_Descriptors[padHandle].channels, 3);
}
Int8 Pad_GetTxPower(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    return gpPad_Descriptors[padHandle].txPower;
}
UInt8 Pad_GetTxAntenna(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    return gpPad_Descriptors[padHandle].antenna;
}
UInt8 Pad_GetTxMinBE(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    return gpPad_Descriptors[padHandle].minBE;
}
UInt8 Pad_GetTxMaxBE(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    return gpPad_Descriptors[padHandle].maxBE;
}
UInt8 Pad_GetTxMaxCsmaBackoffs(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    return gpPad_Descriptors[padHandle].maxCsmaBackoffs;
}
UInt8 Pad_GetTxMaxFrameRetries(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    return gpPad_Descriptors[padHandle].maxFrameRetries;
}
UInt8 Pad_GetTxCsmaMode(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));    
    return gpPad_Descriptors[padHandle].csma;
}
UInt8 Pad_GetCcaMode(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));   
    return gpPad_Descriptors[padHandle].cca;
}

//Gphal calls
void Pad_DataRequest(UInt8 pbmHandle, gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    gpHal_FillInTxOptions(pbmHandle, &gpPad_Descriptors[padHandle]);
}

