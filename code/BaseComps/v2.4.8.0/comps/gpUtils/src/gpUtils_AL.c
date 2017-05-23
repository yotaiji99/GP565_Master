/*
 * Copyright (c) 2008-2014, GreenPeak Technologies
 *
 * gpUtils_AL.c
 *   
 * This file contains the array list features of the Utils component.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpUtils/src/gpUtils_AL.c#1 $
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
gpUtils_ALInit(void * buf, UInt32 n_size_cell, UInt32 n_nr_of_elements, gpUtils_ArrayFree_t * pfre)
{
    gpUtils_ArrayInfo_t * pinfo =  (gpUtils_ArrayInfo_t*)buf; // start of buffer will contain info
    gpUtils_ArrayListHdr_t * phdr;
    gpUtils_ArrayListHdr_t * phdr_org;
    UInt32 i;

    // Init the buffer info with function call parmeters
    pinfo->elem_size = (UInt16)n_size_cell;
    pinfo->elem_cnt  = (UInt16)n_nr_of_elements;
    //And now add it to the free list
    pfre->pbuf_info = pinfo;
    // Get the the first array element
    phdr = (gpUtils_ArrayListHdr_t*)((UIntPtr)pinfo + sizeof(gpUtils_ArrayInfo_t));
    phdr_org = phdr; // remember start of elements
    // Init the the Array list - Set the header field to ELEM_FREE)
    for (i = 0; i < (n_nr_of_elements - 1); i++) {
        *phdr = GP_UTILS_AL_ELEM_FREE;                    // set element to not used
        phdr  = GP_UTILS_AL_GET_NEXT_HDR(phdr, pfre); // go to next element
    }
    //Clear the last element
    *phdr = GP_UTILS_AL_ELEM_FREE;
    // Init the free list
    pfre->pfree     = phdr_org;
    pfre->plast     = phdr;
}

void*
gpUtils_ALNew(gpUtils_ArrayFree_t* pfre)
{
    gpUtils_ArrayListHdr_t * phdr;

    // Get a fresh element from the free-list
    // Check if atomic on    
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    phdr = NULL;
    // check if the freelist exists
    if (pfre) {
        //check that the free pointer is not pointing to NULL - meaning all elements in use
        if (pfre->pfree) {
            phdr = pfre->pfree;
            if(phdr != NULL)
            {
                //check that the element the free pointer is pointing to is in fact free.
                //If this is not the case run through the list until a free element is found
                while (*phdr!= GP_UTILS_AL_ELEM_FREE) {
                    if (phdr == pfre->plast) {
                        phdr = NULL;
                        break;
                    } else {
                        phdr  = GP_UTILS_AL_GET_NEXT_HDR(phdr, pfre); // go to next element
                    }
                }
            }
        }
    }
    else
    {
        return NULL;
    }

    if (phdr) {
        *phdr = GP_UTILS_AL_ELEM_RESERVED; // Mark buffer element as "Reserved"
        if (phdr == pfre->plast) {
            pfre->pfree = NULL;
        } else {
            pfre->pfree = GP_UTILS_AL_GET_NEXT_HDR(phdr, pfre);
        }
        return GP_UTILS_AL_GET_ELEM(phdr);
    } else {
        // List is full so set free pointer to 0
        if (pfre) {
            pfre->pfree = NULL;
        }
    }

    return NULL;
}


void
gpUtils_ALFree(void * pelem, gpUtils_ArrayFree_t * pfre) {
    // the header pointer
    gpUtils_ArrayListHdr_t * phdr;
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    //Check if the gven element is a valid pointer !
    if (!pelem)
    {
        GP_ASSERT_DEV_EXT(false);
    }
    //Get the header from the pelem pointer
    phdr = GP_UTILS_AL_GET_HDR(pelem);
    //Check if the pointer value is correctly aligned and in the right range
    if (!(((UInt8 *) phdr >= (UInt8 *)pfre->pbuf_info + GP_UTILS_AL_SIZE_BUF_INFO)&&((UInt8 *)phdr <= (UInt8 *)pfre->plast)
        &&(((UInt8 *)phdr - (UInt8 *)pfre->pbuf_info - GP_UTILS_AL_SIZE_BUF_INFO)%pfre->pbuf_info->elem_size==0))) {
        GP_ASSERT_DEV_EXT(false);
    }
    else
    {
        // Now free the buffer element- Should we check for correct hdr alignment ??
        *phdr = GP_UTILS_AL_ELEM_FREE;
        // Reset the free pointer if needed
        if ((pfre->pfree==NULL) || (phdr < pfre->pfree)) {
            pfre->pfree = phdr;
        }
    }
}

void
gpUtils_ALClear(gpUtils_ArrayList_t * plst , gpUtils_ArrayListHdr_t type , gpUtils_ArrayFree_t * pfre)
{
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    if (type >GP_UTILS_AL_ELEM_RESERVED) {
        plst->type = type;
    } else {
        GP_LOG_PRINTF("Invalid type for Array List in gpUtils_ALClear",0);
        GP_ASSERT_DEV_EXT(false);
    }
    if (pfre) {
        plst->pbuf_info = pfre->pbuf_info;
    } else {
        GP_LOG_PRINTF("Invalid free list for Array List in gpUtils_ALClear",0);
        GP_ASSERT_DEV_EXT(false);
    }
    plst->pfirst = plst->plast = NULL;
}

void
gpUtils_ALAdd(void * pelem , gpUtils_ArrayList_t * plst)
{
    gpUtils_ArrayListHdr_t* phdr;
    // check if atomic
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    // Now  add the element to the list
    //Check that the element is not NULL
    GP_ASSERT_DEV_EXT(pelem);
    // Get the header
    phdr = GP_UTILS_AL_GET_HDR(pelem);
    // Put the type into the header field
    GP_ASSERT_DEV_EXT(*phdr==GP_UTILS_AL_ELEM_RESERVED); // check that that the element was correctly obtained by a new
    *phdr = plst->type;
    // Adapt the first and last pointers if necessary
    if (phdr > plst->plast) {
        plst->plast = phdr;
    }
    if ((phdr < plst->pfirst)||(!plst->pfirst)) {
        plst->pfirst = phdr;
    }
}

void
gpUtils_ALUnlink(void * pelem, gpUtils_ArrayList_t * plst)
{

    gpUtils_ArrayListHdr_t* phdr = GP_UTILS_AL_GET_HDR(pelem);
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    //HAL_DISABLE_GLOBAL_INT();
    if ((!pelem)||(!plst))
    {
        GP_ASSERT_DEV_EXT(false);
    }
    if (*phdr!=plst->type)
    {
        GP_ASSERT_DEV_EXT(false);
    }
    // check if header pointer is within the correct range
    GP_ASSERT_DEV_EXT(((phdr>=plst->pfirst)&&(phdr<=plst->plast)));
    //Now remove the element from the list
    // Take the element of the list, but keep it reserved ... until it is freed
    *phdr= GP_UTILS_AL_ELEM_RESERVED;
    //Now adapt the first and last pointers when necessary
    phdr = plst->pfirst;
    if(phdr != NULL)
    {
        while (*phdr!=plst->type) {
            if (phdr!=plst->plast) {
                phdr=GP_UTILS_AL_GET_NEXT_HDR(phdr, plst);
            } else {
                phdr = NULL;
                break;
            }
        }
    }
    plst->pfirst = phdr;
    if (phdr != NULL)  {
        phdr = plst->plast;
        while (*phdr != plst->type) {
            if (phdr != plst->pfirst) {
                phdr=GP_UTILS_AL_GET_PREV_HDR(phdr, plst);
            } else {
                phdr = NULL;
                break;
            }
        }
    }
    plst->plast=phdr;

}

void
gpUtils_ALRelease(void * pelem, gpUtils_ArrayList_t * plst, gpUtils_ArrayFree_t * pfre)
{
    //Remove the element from the list and free it
    gpUtils_ALUnlink(pelem, plst);
    gpUtils_ALFree(pelem, pfre);
}

void* gpUtils_ALGetNext(gpUtils_ArrayList_t * plst , void * pelem) {
    gpUtils_ArrayListHdr_t* phdr   = GP_UTILS_AL_GET_HDR(pelem);
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    if (!plst) {
        return NULL;
    }
    if (pelem) {
        // Check if we are doing a getNext on an element of the correct list
        GP_ASSERT_DEV_EXT(*phdr==plst->type);
        if (phdr==plst->plast) {
            return NULL;
        }
        // We will loop through the list starting at the given element to find the previous element that matches
        // the type field of the given element.
        for (phdr= GP_UTILS_AL_GET_NEXT_HDR(phdr, plst);phdr<=plst->plast;phdr=GP_UTILS_AL_GET_NEXT_HDR(phdr, plst)) {
            if (*phdr == plst->type) {
                return GP_UTILS_AL_GET_ELEM(phdr);
            }
        }
    }
    return NULL;
}

void* gpUtils_ALGetFirstElem(gpUtils_ArrayList_t * plst) {
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    if (plst && plst->pfirst) 
    {
        void *pelem;
        pelem = GP_UTILS_AL_GET_ELEM(plst->pfirst);
        //check if the element actually belongs to the list
        GP_ASSERT_DEV_EXT(*(plst->pfirst)==plst->type);
        return pelem;

    }
    return NULL;
}


