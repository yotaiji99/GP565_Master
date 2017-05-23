/*
 * Copyright (c) 2008-2013, GreenPeak Technologies
 *
 * gpUtils_LLext.c
 *   
 * This file contains the less used functions of the Linked Lists
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpUtils/src/gpUtils_LLext.c#1 $
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

extern void Utils_InsertBound(gpUtils_Link_t * plnk, gpUtils_LinkList_t * plst);

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void
gpUtils_LLInsert (void * pelem, void * pelem_cur, gpUtils_LinkList_t * plst) {
    gpUtils_Link_t* plnk;
    gpUtils_Link_t* plnk_cur;
    // Insert the element at the current position
    // UNTESTED
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    //HAL_DISABLE_GLOBAL_INT();
    plnk = GP_UTILS_LL_GET_LINK(pelem);
    plnk_cur = GP_UTILS_LL_GET_LINK(pelem_cur);
    // Set the link pointers
#if 0
    plnk->plnk_prv = pelem_cur;
#else
    plnk->plnk_prv = plnk_cur;
#endif
    plnk->plnk_nxt = plnk_cur->plnk_nxt;
    // Insert the element
    Utils_InsertBound (plnk, plst);
    //HAL_ENABLE_GLOBAL_INT();
}

void
gpUtils_LLInsertSorted        (void * pelem, gpUtils_LinkList_t * plst, gpUtils_ElemCompare_t callback)
{
    gpUtils_Link_t* plnk;
    void* pelemFound = NULL;

    GP_ASSERT_DEV_EXT(pelem);

    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    //HAL_DISABLE_GLOBAL_INT();

    // Search position in the sorted list
    for (plnk = plst->plnk_first; plnk; plnk = plnk->plnk_nxt)
    {
        pelemFound = GP_UTILS_LL_GET_ELEM(plnk);
        if (callback(pelem,pelemFound))
            break;
    }
    if (plnk)
    {
        gpUtils_LLInsertBefore(pelem, pelemFound, plst);
    }
    else
    {
        gpUtils_LLAdd(pelem,plst);
    }
}

void*
gpUtils_LLNewSorted (void * pelem, gpUtils_LinkList_t * plst, gpUtils_LinkFree_t * pfre, gpUtils_ElemCompareDelete_t callback)
{
    void* preturn = gpUtils_LLNew(pfre);
    Bool betterElemThanLast = true;
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    //HAL_DISABLE_GLOBAL_INT();

    // If last element is better than last, element can always be added (it can alays replace the last element)
    // If it is not better, then it can only be added if a free element exists in the free list
    if (!preturn)
    {
        gpUtils_Link_t* plnk;
        void* pelem_last;
        Bool canBeDeleted;
        for (plnk = plst->plnk_last; plnk; plnk = plnk->plnk_prv)
        {
            pelem_last = GP_UTILS_LL_GET_ELEM(plnk);

            betterElemThanLast = callback(pelem,pelem_last,&canBeDeleted);
            if (betterElemThanLast)
            {
                if (canBeDeleted)
                {
                    // return last element
                    preturn = pelem_last;
                    gpUtils_LLUnlink(preturn, plst);
                    break;
                }
            }
            else
                break;
        }
    }
    return preturn;
}

void
gpUtils_LLRelease (void * pelem, gpUtils_LinkList_t * plst, gpUtils_LinkFree_t * pfre) {
    // Remove the element from the list and return to the free-list
    gpUtils_LLUnlink (pelem, plst);
    gpUtils_LLFree (pelem, pfre);
}

void  gpUtils_LLDump     (gpUtils_LinkList_t * plst)
{
    gpUtils_Link_t* plnk;
    HAL_DISABLE_GLOBAL_INT();
    GP_LOG_PRINTF("Dump LL:",0);
    GP_LOG_PRINTF("First:%x, Last:%x",4,plst->plnk_first,plst->plnk_last);
    for (plnk = plst->plnk_first;plnk;plnk = plnk->plnk_nxt)
    {
        GP_LOG_PRINTF("this:%x/%x,prev:%x,nxt:%x,data:%i",10,plnk,GP_UTILS_LL_GET_ELEM(plnk),plnk->plnk_prv,plnk->plnk_nxt,*(Char*)(GP_UTILS_LL_GET_ELEM(plnk)));
    }
    GP_LOG_PRINTF("End Dump LL:",0);
    HAL_ENABLE_GLOBAL_INT();
}

void  gpUtils_LLDumpFree (gpUtils_LinkFree_t * pfree)
{
    UInt8 Counter = 0;
    gpUtils_Link_t* plnk;
    HAL_DISABLE_GLOBAL_INT();
    GP_LOG_PRINTF("DumpFree LL:",0);
    GP_LOG_PRINTF("First:%x, Last:%x",4,pfree->plnk_free,pfree->plnk_free_last);
    for (plnk = pfree->plnk_free;plnk && Counter<10;plnk = plnk->plnk_nxt)
    {
        GP_LOG_PRINTF("this:%x/%x,prev:%x,nxt:%x,data:%i",10,plnk,GP_UTILS_LL_GET_ELEM(plnk),plnk->plnk_prv,plnk->plnk_nxt,*(Char*)(GP_UTILS_LL_GET_ELEM(plnk)));
        Counter++;
    }
    GP_LOG_PRINTF("End Dump LL:",0);
    HAL_ENABLE_GLOBAL_INT();
}

void
gpUtils_LLDumpMemory (void * buf, UInt32 n_size_cell, UInt32 n_nr_of_elements, gpUtils_LinkList_t * plst, gpUtils_LinkFree_t * pfre ) {
    UInt32 i;
    gpUtils_Link_t* plnk = (gpUtils_Link_t*)buf;

    NOT_USED(pfre);
    NOT_USED(plst);

    GP_LOG_PRINTF("Lf%x Ll%x Ff%x Fl%x",8,plst->plnk_first,plst->plnk_last,pfre->plnk_free,pfre->plnk_free_last);
    for (i = 0; i < n_nr_of_elements; i++) {
        GP_LOG_PRINTF("@%x <%x %x>",6,plnk,plnk->plnk_prv, plnk->plnk_nxt);
        GP_LOG_PRINTF("d:%x %x %x %x %x",10, plnk->plnk_nxt[1], plnk->plnk_nxt[2], plnk->plnk_nxt[3], plnk->plnk_nxt[4], plnk->plnk_nxt[5]);
        plnk = (gpUtils_Link_t *) ((UIntPtr) plnk + (UInt16)n_size_cell);
    }
}

Bool gpUtils_LLCheckConsistency(void * buf, UInt32 n_size_cell, UInt32 n_nr_of_elements, gpUtils_LinkList_t * plst, gpUtils_LinkFree_t * pfre)
{
    UInt32 ElementCounter = 0;
    gpUtils_Link_t* plnk;
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    if (plst)
    {
        for (plnk = plst->plnk_first; plnk; plnk = plnk->plnk_nxt)
        {
            ElementCounter++;
            if (ElementCounter > n_nr_of_elements)
            {
                GP_LOG_PRINTF("Err1 %li %i",6,ElementCounter,n_nr_of_elements);
                gpUtils_LLDumpMemory(buf, n_size_cell, n_nr_of_elements, plst, pfre);
                return false;
            }
        }
    }
    if (pfre)
    {
        ElementCounter = 0;
        for (plnk = pfre->plnk_free; plnk; plnk = plnk->plnk_nxt)
        {
            ElementCounter++;
            if (ElementCounter > n_nr_of_elements)
            {
                GP_LOG_PRINTF("Err2 %li %i",6,ElementCounter,n_nr_of_elements);
                gpUtils_LLDumpMemory(buf, n_size_cell, n_nr_of_elements, plst, pfre);
                return false;
            }
        }
    }
    return true;
}

void* gpUtils_LLGetNext(void* pelem)
{
    gpUtils_Link_t* plnk = (gpUtils_Link_t*)((UIntPtr)pelem - sizeof(gpUtils_Link_t));
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
#if 0
    return plnk->plnk_nxt;
#else
    return plnk->plnk_nxt?GP_UTILS_LL_GET_ELEM(plnk->plnk_nxt):NULL;
#endif
}

void* gpUtils_LLGetPrev(void* pelem)
{
    gpUtils_Link_t* plnk = (gpUtils_Link_t*)((UIntPtr)pelem - sizeof(gpUtils_Link_t));
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
#if 0
    return plnk->plnk_prv;
#else
    return plnk->plnk_prv?GP_UTILS_LL_GET_ELEM(plnk->plnk_prv):NULL;
#endif
}

gpUtils_Link_t* gpUtils_LLGetLink(void* pelem)
{
    gpUtils_Link_t* plnk = (gpUtils_Link_t*)((UIntPtr)pelem - sizeof(gpUtils_Link_t));
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    return pelem?plnk:NULL;
}

void* gpUtils_LLGetElem(gpUtils_Link_t* plnk)
{
    void* pelem = (void*)((UIntPtr)plnk + sizeof(gpUtils_Link_t));
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    return plnk?pelem:NULL;
}

void* gpUtils_LLGetLastElem(gpUtils_LinkList_t* plst)
{
    void* pelem = (void*)((UIntPtr)plst->plnk_last + sizeof(gpUtils_Link_t));
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    return plst->plnk_last?pelem:NULL;
}



