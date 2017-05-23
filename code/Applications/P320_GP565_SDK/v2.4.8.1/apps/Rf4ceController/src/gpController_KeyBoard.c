/*
 * Copyright (c) 2015, GreenPeak Technologies
 *
 * gpController_KeyBoard.c
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/apps/Rf4ceController/src/gpController_KeyBoard.c#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */


/*******************************************************************************
 *                      Include Files
 ******************************************************************************/
#include "gpController_KeyBoard.h"
#include "gpSched.h"
#include "gpKeyScan.h"
/*******************************************************************************
 *                      Defines
 ******************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_APP

#define KEYBOARD_NUMBER_OF_ROWS           6
#define KEYBOARD_NUMBER_OF_COLUMNS        5

#define KEYBOARD_MATRIX_SIZE     (   KEYBOARD_NUMBER_OF_ROWS * \
                                                    KEYBOARD_NUMBER_OF_COLUMNS)

/* setup key behaviour */
#define KEYBOARD_SETUP_SIMULTANEOUS_DELTA_TIME    200000UL
#define KEYBOARD_SETUP_KEYS_COUNT                 1

#define KEYBOARD_SETUP_KEYS                       {11, 0xFF}

#define KEYBOARD_SETUP_HOLD_TIME                  3 /* secondes */
#define KEYBOARD_SETUP_SENDS_MESSAGE              true

#define INVALID_KEY                         0xFF
#define KEYBOARD_STUCK_KEY_TIMEOUT          30000000UL
#define GP_KEYBOARD_MAX_KEYS_INDICATED      GP_RF4CECMD_DIVERSITY_CONCURRENT_ACTIONS


/*******************************************************************************
 *                      Data Structure Definitions
 ******************************************************************************/
static ROM gpKeyboardBasic_Attr_t gpControllerKeyboardAttr =
{
    .size               = KEYBOARD_MATRIX_SIZE,
    .maxKeyIndication   = GP_KEYBOARD_MAX_KEYS_INDICATED,
    .stuckKeyTimoutSecs = 30
};


static ROM UInt8 gpController_SetupKeys[2] = KEYBOARD_SETUP_KEYS;

/*******************************************************************************
 *                      Static Defines
 ******************************************************************************/
static Bool                                         readyToGotoSetupMode = false;
static gpKeyboardBasic_Attr_t keyboardAttr;
static UInt8 keyboardByteLen;
static Bool disableStuckKeyHandling;
/*******************************************************************************
 *                      Static Function Definitions
 ******************************************************************************/
static Bool Keyboard_HandleSetup(UInt8, const gpKeyboardBasic_pKeyInfo_t);
static void gpKeyboardBasic_Init(gpKeyboardBasic_Attr_t* pKbAttr);
static void gpKeyboardBasic_cbKeyIndication(UInt8 numKeys, gpKeyboardBasic_pKeyInfo_t pKeys);
static void gpKeyboardBasic_EnableKeyScan(void);
static void gpKeyboardBasic_DisableKeyScan(void);
static void gpKeyboardBasic_SetAttr(gpKeyboardBasic_Attr_Type_t attrType, void* pAttrVal);
/*******************************************************************************
 *                      Public Functions
 ******************************************************************************/
void gpController_KeyBoard_Init(void)
{
    gpKeyboardBasic_Init(&gpControllerKeyboardAttr);
}

void gpController_KeyBoard_Msg( gpController_KeyBoard_MsgId_t msgId,
                                gpController_KeyBoard_Msg_t *pMsg)
{
    switch(msgId)
    {
        case gpController_KeyBoard_MsgId_KeyScanEnable:
        {
            gpKeyboardBasic_EnableKeyScan();
            break;
        }
        case gpController_KeyBoard_MsgId_KeyScanDisable:
        {
            gpKeyboardBasic_DisableKeyScan();
            break;
        }
        default:
        {
            break;
        }
    }
}
/*******************************************************************************
 *                      Basic Keyboard Callback
 ******************************************************************************/
void gpKeyboardBasic_cbKeyIndication(UInt8 nKeysPrsd, gpKeyboardBasic_pKeyInfo_t pKeyIndList)
{
    if (!Keyboard_HandleSetup(nKeysPrsd, pKeyIndList))
    {

        UInt8 i = 0;
        gpController_KeyBoard_Msg_t msg;
        gpController_KeyBoard_Keys_t keys;

        keys.count = nKeysPrsd;

        for(i=0; i<nKeysPrsd; i++)
        {
            keys.indices[i] = pKeyIndList[i].keyIndex;
        }

        msg.keys = keys;
        gpController_KeyBoard_cbMsg(gpController_KeyBoard_MsgId_KeysPressedIndication, &msg);

    }
}

/*******************************************************************************
 *                      Static Functions
 ******************************************************************************/
static void Keyboard_SetupKeyPressed( void )
{
    readyToGotoSetupMode = true;
    gpController_KeyBoard_cbMsg(gpController_KeyBoard_MsgId_SetupEnteredIndication, NULL);
}

static void simul_key_press(void)
{
}

static Bool Keyboard_HandleSetup( UInt8 nKeysPrsd, gpKeyboardBasic_pKeyInfo_t pKeyIndList )
{
    Bool keysHandled = false;

    if(KEYBOARD_SETUP_KEYS_COUNT == 1)
    {
        if(nKeysPrsd == 1)
        {
            /* Single key setup */
            if(pKeyIndList[0].keyIndex == gpController_SetupKeys[0])
            {
                /* Setup key must be pressed for a certain time, so start the timer*/
                gpSched_ScheduleEvent( (1000000UL * KEYBOARD_SETUP_HOLD_TIME) , Keyboard_SetupKeyPressed );

                gpController_KeyBoard_cbMsg(gpController_KeyBoard_MsgId_SetupKeysPressedIndication, NULL);

                if (KEYBOARD_SETUP_SENDS_MESSAGE)
                {
                    /* Setup key is a dedicated key, only handle in setup mode */
                    keysHandled = true;
                }
                else
                {
                    /* Setup key is a normal key, to be also handled by the application */
                    keysHandled = false;
                }
            }
            else
            {
                keysHandled = false;
            }
        }
    }
    if(KEYBOARD_SETUP_KEYS_COUNT == 2 )
    {
        /* Setup key combo is pressed */
        if (nKeysPrsd == 1)
        {
            if ( (pKeyIndList[0].keyIndex == gpController_SetupKeys[0] || pKeyIndList[0].keyIndex == gpController_SetupKeys[1] ))
            {
                if (readyToGotoSetupMode)
                {
                    keysHandled = true;
                }
                else
                {
                    /* If either of the setup keys are pressed then
                     * Start a timeout for accepting a simultaneous key press (200ms)
                     * Let the key be passed as a normal key press via gpKeyboard_cbKeyIndication
                     */
                    gpSched_ScheduleEvent(KEYBOARD_SETUP_SIMULTANEOUS_DELTA_TIME, simul_key_press);
                    keysHandled = false;
                }
            }
            else
            {
                /* It is a regular keys */
                keysHandled = false;
            }

        }
        if(nKeysPrsd == 2)
        {
            /* Multiple key setup */
            if((pKeyIndList[0].keyIndex == gpController_SetupKeys[0] && pKeyIndList[1].keyIndex == gpController_SetupKeys[1])
                    || (pKeyIndList[0].keyIndex == gpController_SetupKeys[1] && pKeyIndList[1].keyIndex == gpController_SetupKeys[0]))
            {
                if (gpSched_UnscheduleEvent(simul_key_press))
                {
                    /* Setup key combo was pressed within the time (200ms) */

                    /* Setup key must be pressed for a certain time, so start the timer*/
                    gpSched_ScheduleEvent((1000000UL * KEYBOARD_SETUP_HOLD_TIME), Keyboard_SetupKeyPressed);

                    /* Release the key pressed first */
                    gpController_KeyBoard_cbMsg(gpController_KeyBoard_MsgId_KeysReleasedIndication, NULL);
                    gpController_KeyBoard_cbMsg(gpController_KeyBoard_MsgId_SetupKeysPressedIndication, NULL);

                    keysHandled = true;
                }
                else
                {
                /* Setup key combo was pressed after the time (200ms)
                 * This will be passed a multiple key press to gpKeyboard_cbKeyIndication
                 */
                    keysHandled = false;
                }
            }
        }
    }
    if(nKeysPrsd == 0)
    {
        if (readyToGotoSetupMode)
        {
            readyToGotoSetupMode = false;
            gpController_KeyBoard_cbMsg(gpController_KeyBoard_MsgId_SetupKeysReleasedIndication, NULL);
            keysHandled = true;
        }
        if( gpSched_UnscheduleEvent(Keyboard_SetupKeyPressed) )
        {
            /* Setup key is released before the SETUP_TIMEOUT. */
            gpController_KeyBoard_cbMsg(gpController_KeyBoard_MsgId_SetupKeysReleasedIndication, NULL);
            keysHandled = true;
        }
    }

    return keysHandled;
}

static void getKeyMapping(const UInt8 * bitmap, gpKeyboardBasic_pKeyInfo_t pKeyIndexes, UInt8 *pNumKeys)
{
    UInt8 n = 0;
    UInt8 bitPos = 0;

    do
    {
        UInt8 keyBits = bitmap[n];

        /* Keys pressed in this byte */
        if(keyBits)
        {
            bitPos = 0;
            do
            {
                /* Key pressed */
                if(keyBits & 0x1)
                {
                    pKeyIndexes[*pNumKeys].keyIndex = bitPos+n*8;
                    *pNumKeys+=1;

                    /* Max key count reached. Stop */
                    if(*pNumKeys == keyboardAttr.maxKeyIndication)
                    {
                        return;
                    }
                }
                keyBits = keyBits >> 1;
                bitPos++;

                if((bitPos + n*8) == keyboardAttr.size)
                {
                    return;
                }
            } while(keyBits);
        }

        n++;

    } while(n != keyboardByteLen);
}

static void gpKeyboardBasic_Init(gpKeyboardBasic_Attr_t* pKbAttr)
{
    keyboardAttr.size = pKbAttr->size;

    gpKeyboardBasic_SetAttr(ATTR_KB_MAX_KEY_INDICATION, &pKbAttr->maxKeyIndication );

    if (keyboardAttr.size % 8)
    {
        keyboardByteLen = keyboardAttr.size/8 + 1;
    }
    else
    {
        keyboardByteLen = keyboardAttr.size/8;
    }

    gpKeyboardBasic_SetAttr(ATTR_KB_STUCK_KEY_TIMEOUT, &pKbAttr->stuckKeyTimoutSecs);
}

/* This function is called from the keyscan.
 * A bit in the matrix is set when the key is pressed
 * If the bit matrix is returned as NULL then  all the key released
 */
void gpKeyScan_cbScanIndication( const UInt8 * pBitmaskMatrix)
{
    gpKeyboardBasic_KeyInfo_t   keyIndexList[GP_KEYBOARD_MAX_KEYS_INDICATED];
    UInt8                       nKeysPrsd = 0;
    gpKeyboardBasic_pKeyInfo_t  pKeyIndList = NULL;

    MEMSET(keyIndexList, -1, sizeof(keyIndexList));

    if(pBitmaskMatrix)
    {
        getKeyMapping(pBitmaskMatrix, keyIndexList, &nKeysPrsd);
        pKeyIndList = keyIndexList;
    }
    else
    {
        nKeysPrsd = 0;
        pKeyIndList = NULL;
    }

    /* Stuck keys handling
     * Schedule a timeout at every key press, release when key released
     */
    if(nKeysPrsd == 0)
    {
        gpSched_UnscheduleEvent(gpKeyScan_HandleStuckKey);
    }
    else
    {
        /* Handle stuck key */
        if(!disableStuckKeyHandling
            && !gpSched_ExistsEvent(gpKeyScan_HandleStuckKey))
        {
            gpSched_ScheduleEvent(keyboardAttr.stuckKeyTimoutSecs * 1000000UL, gpKeyScan_HandleStuckKey );
        }
    }

    gpKeyboardBasic_cbKeyIndication(nKeysPrsd, pKeyIndList);
}

static void gpKeyboardBasic_EnableKeyScan(void)
{
    /* Enable key scanning, in interrupt mode */
    gpKeyScan_EnableAutomaticKeyScanning();
}

static void gpKeyboardBasic_DisableKeyScan(void)
{
    /* Disable key scan */
    gpKeyScan_DisableAutomaticKeyScanning();
}

static void gpKeyboardBasic_SetAttr(gpKeyboardBasic_Attr_Type_t attrType, void* pAttrVal)
{
    switch(attrType)
    {
        case ATTR_KB_MAX_KEY_INDICATION:
        {
            if (*((UInt8*)pAttrVal) <= GP_KEYBOARD_MAX_KEYS_INDICATED)
            {
                keyboardAttr.maxKeyIndication = *((UInt8*)pAttrVal);
            }
            else
            {
                /* Can't indicate more than max key indication define */
                keyboardAttr.maxKeyIndication = GP_KEYBOARD_MAX_KEYS_INDICATED;
            }
            break;
        }
        case ATTR_KB_STUCK_KEY_TIMEOUT:
        {
            /* Both values are considered to disable stuck key handling */
            if (*((UInt8*)pAttrVal) == 0xFF || *((UInt8*)pAttrVal) == 0)
            {
                disableStuckKeyHandling = true;
                break;
            }

            disableStuckKeyHandling = false;
            keyboardAttr.stuckKeyTimoutSecs = *((UInt8*)pAttrVal);
            break;
        }
        default:
            break;
    }
}



