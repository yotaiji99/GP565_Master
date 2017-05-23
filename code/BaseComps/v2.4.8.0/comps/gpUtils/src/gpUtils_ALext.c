/*
 * Copyright (c) 2008-2013, GreenPeak Technologies
 *
 * gpUtils_AL.c
 *
 * This file contains the less used array list features of the Utils component.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpUtils/src/gpUtils_ALext.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_UTILS

#include "hal.h"
#include "gpUtils.h"
#include "gpLog.h"
#include "gpAssert.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    External Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void
gpUtils_ALDumpFree(gpUtils_ArrayFree_t * pfre)
{
    UInt8 Counter = 0;
    gpUtils_ArrayListHdr_t* phdr;

    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    GP_LOG_PRINTF("DumpFree AL:",0);
    //Dump the free and last pointer of the list
    GP_LOG_PRINTF("Free:%x, Last:%x",4,pfre->pfree,pfre->plast);
    phdr = pfre->pfree;
    while ((phdr != NULL) && (Counter < 10)) {
        // If the element is actually free - dump it to log ...
        if (*phdr == GP_UTILS_AL_ELEM_FREE) {
            GP_LOG_PRINTF("Hdr:%x ID:%x dt:%i",6,phdr,*phdr,*(Char*)(GP_UTILS_AL_GET_ELEM(phdr)));
            Counter++;
        }
        //Get the next header pointer
        if (phdr==pfre->plast) {
            phdr = NULL;
            break;
        } else {
            phdr = GP_UTILS_AL_GET_NEXT_HDR(phdr, pfre);
        }
    }
    GP_LOG_PRINTF("End Dump AL:",0);
}

void* gpUtils_ALGetPrev(gpUtils_ArrayList_t * plst , void * pelem) {
    gpUtils_ArrayListHdr_t* phdr   = GP_UTILS_AL_GET_HDR(pelem);
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    if (!plst) {
        return NULL;
    }
    if (pelem) {
        // Check if we are doing a getPrev on an element of the correct list
        GP_ASSERT_DEV_EXT(*phdr==plst->type);
        if (phdr==plst->pfirst) {
            return NULL;
        }
        // We will loop through the list starting at the given element to find the previous element that matches
        // the type field of the given element.
        for (phdr= GP_UTILS_AL_GET_PREV_HDR(phdr, plst);phdr>=plst->pfirst;phdr=GP_UTILS_AL_GET_PREV_HDR(phdr, plst)) {
            if (*phdr == plst->type) {
                return GP_UTILS_AL_GET_ELEM(phdr);
            }
        }
    }
    return NULL;
}


gpUtils_ArrayListHdr_t* gpUtils_ALGetHdr(void * pelem) {
    gpUtils_ArrayListHdr_t* phdr = GP_UTILS_AL_GET_HDR(pelem);
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    return pelem?phdr:NULL;
}

void* gpUtils_ALGetElem(gpUtils_ArrayListHdr_t * phdr) {
    void * pelem = GP_UTILS_AL_GET_ELEM(phdr);
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    return phdr?pelem:NULL;
}

void* gpUtils_ALGetLastElem(gpUtils_ArrayList_t * plst) {
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    if (plst && plst->plast) {
        void *pelem;
        pelem = GP_UTILS_AL_GET_ELEM(plst->plast);
        //check if the element actually belongs to the list
        GP_ASSERT_DEV_EXT(*(plst->plast)==plst->type);
        return pelem;
    }
    return NULL;
}

void
gpUtils_ALDump(gpUtils_ArrayList_t * plst)
{
    gpUtils_ArrayListHdr_t* phdr;

    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    GP_LOG_PRINTF("Dump AL:",0);
    GP_LOG_PRINTF("First:%x, Last:%x",4,plst->pfirst,plst->plast);
    phdr = plst->pfirst;
    if (phdr) {
        do
        {
            if (*phdr == plst->type) {
                GP_LOG_PRINTF("Hdr:%x ID:%x dt:%i",6,phdr,*phdr,*(Char*)(GP_UTILS_AL_GET_ELEM(phdr)));
            }
            phdr = GP_UTILS_AL_GET_NEXT_HDR(phdr, plst);
        } while (phdr <= plst->plast);
    }

    GP_LOG_PRINTF("End Dump AL:",0);
}


