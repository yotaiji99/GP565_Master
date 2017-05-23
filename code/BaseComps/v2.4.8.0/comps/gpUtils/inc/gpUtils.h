/*
 * Copyright (c) 2008-2014, GreenPeak Technologies
 *
 * gpUtils.h
 *
 * This file contains the API of the Utils component.  This component implements the
 * following features:
 *
 *        - Link Lists (LL)
 *        - Array Lists (AL)
 *        - Circular buffer (CircB)
 *        - Stack control (stack)
 *        - CRC calculation (crc)
 *        - CRC32 calculation (crc32)
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpUtils/inc/gpUtils.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */


#ifndef _GPUTILS_H_
#define _GPUTILS_H_


/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "hal.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_UTILS_LL_MEMORY_DECLARATION(elemType, nrOfElements)      \
    typedef struct {                                                \
        gpUtils_Link_t link;                                        \
        elemType elem;                                              \
    }gpUtils_ll_sub_##elemType##_t;                                 \
                                                                    \
    typedef struct {                                                \
        gpUtils_ll_sub_##elemType##_t sub_##elemType##_array[nrOfElements]; \
    }gpUtils_ll_##elemType##_t

#define GP_UTILS_LL_MEMORY_ALOCATION(elemType, memoryName)  \
    gpUtils_ll_##elemType##_t memoryName[1]


#define GP_UTILS_AL_MEMORY_DECLARATION(elemType, nrOfElements)      \
    typedef struct {                                                \
        gpUtils_ArrayListHdr_t arrayListHdr;                                        \
        elemType elem;                                              \
    }gpUtils_al_sub_##elemType##_t;                                 \
                                                                    \
    typedef struct {                                                \
        gpUtils_ArrayInfo_t bufInfo;                                             \
        gpUtils_al_sub_##elemType##_t sub_##elemType##_array[nrOfElements];  \
    }gpUtils_al_##elemType##_t

#define GP_UTILS_AL_MEMORY_ALOCATION(elemType, memoryName)  \
    gpUtils_al_##elemType##_t memoryName[1]

#define GP_UTILS_LL_SIZE_OF(elem_type)        ((UInt32)sizeof(gpUtils_ll_sub_##elem_type##_t))
#define GP_UTILS_AL_SIZE_OF(elem_type)        ((UInt32)sizeof(gpUtils_al_sub_##elem_type##_t))
#define GP_UTILS_AL_SIZE_BUF_INFO             ((UInt32)sizeof(gpUtils_ArrayInfo_t))

#define GP_UTILS_LL_GET_ELEM(plnk)            (void*)((UIntPtr)plnk + sizeof(gpUtils_Link_t))
#define GP_UTILS_LL_GET_LINK(pelem)           (gpUtils_Link_t*)((UIntPtr)pelem - sizeof(gpUtils_Link_t))

#define GP_UTILS_AL_GET_ELEM(phdr)            (void*)((UIntPtr)phdr + sizeof(gpUtils_ArrayListHdr_t))
#define GP_UTILS_AL_GET_HDR(pelem)            (gpUtils_ArrayListHdr_t*)((UIntPtr)pelem - sizeof(gpUtils_ArrayListHdr_t))
#define GP_UTILS_AL_GET_NEXT_HDR(phdr, pfre)  (gpUtils_ArrayListHdr_t *)((UIntPtr)phdr + (UInt16)pfre->pbuf_info->elem_size)
#define GP_UTILS_AL_GET_PREV_HDR(phdr, pfre)  (gpUtils_ArrayListHdr_t *)((UIntPtr)phdr - (UInt16)pfre->pbuf_info->elem_size)

#define GP_UTILS_LL_IS_LIST_EMPTY(plist)         (plist->plnk_first == NULL)

#define GP_UTILS_LL_ACQUIRE_LOCK(plist)     HAL_ACQUIRE_MUTEX(plist->lock)
#define GP_UTILS_LL_RELEASE_LOCK(plist)     HAL_RELEASE_MUTEX(plist->lock)
#define GP_UTILS_LL_FREE_ACQUIRE_LOCK(pfre) HAL_ACQUIRE_MUTEX(pfre->lock)
#define GP_UTILS_LL_FREE_RELEASE_LOCK(pfre) HAL_RELEASE_MUTEX(pfre->lock)

#ifdef GP_DIVERSITY_STACK_DBG
#if !defined(HAL_STACK_START_ADDRESS)
#error not implemented
#endif

#define GP_UTILS_STACK_PATTERN      0x66
#define GP_UTILS_START_OF_STACK     HAL_STACK_START_ADDRESS
#define GP_UTILS_END_OF_STACK       HAL_STACK_END_ADDRESS
#define GP_UTILS_STACK_LOW_TO_HIGH  HAL_STACK_LOW_TO_HIGH

//Stack can grow from low to high (IAR) or high to low addresses (Atmel)
#if GP_UTILS_STACK_LOW_TO_HIGH
#define GP_UTILS_FROM_A     GP_UTILS_START_OF_STACK
#define GP_UTILS_TO_B       GP_UTILS_END_OF_STACK
#else
#define GP_UTILS_FROM_A     GP_UTILS_END_OF_STACK
#define GP_UTILS_TO_B       GP_UTILS_START_OF_STACK
#endif // GP_UTILS_STACK_LOW_TO_HIGH

#define GP_UTILS_INIT_STACK()  do \
    {                                                         \
        UInt8* myPointer;                                     \
        for (myPointer=GP_UTILS_FROM_A; myPointer<=GP_UTILS_TO_B; myPointer++)    \
        {                                                     \
            *myPointer = GP_UTILS_STACK_PATTERN;              \
        }                                                     \
    } while (false)
#define GP_UTILS_CHECK_STACK_PATTERN() do {GP_ASSERT_SYSTEM(gpUtils_CheckStackPattern()); } while (false)
#define GP_UTILS_CHECK_STACK_POINTER() do {GP_ASSERT_SYSTEM(gpUtils_CheckStackPointer()); } while (false)
#define GP_UTILS_CHECK_STACK_USAGE()   do {gpUtils_GetMaxStackUsage(); } while(false)
#define GP_UTILS_DUMP_STACK_POINTER()    do \
        {                                                           \
        UInt8* p_this_call = gpUtils_GetStackPointer();       \
        GP_LOG_SYSTEM_PRINTF("stack %x %x %x %x %x",10,p_this_call,p_this_call[0],p_this_call[1],p_this_call[2],p_this_call[3]);    \
    } while (false)

#define GP_UTILS_DUMP_STACK_POINTER2(x)    do \
        {                                                                                             \
        UInt8* pStack,*p_this_call;                            \
        gpUtils_GetStackPointer2(&p_this_call,&pStack);       \
        GP_LOG_SYSTEM_PRINTF("stack%x %x %x",6,x,p_this_call,pStack);  \
    } while (false)

#ifdef GP_DIVERSITY_LOG
#define GP_UTILS_DUMP_STACK_TRACK()     gpUtils_DumpStackTrack()
#define GP_UTILS_RESET_STACK_TRACK()    gpUtils_ResetStackTrack()
#define GP_UTILS_STACK_TRACK()          gpUtils_StackTrack()
#else
#define GP_UTILS_DUMP_STACK_TRACK()
#define GP_UTILS_RESET_STACK_TRACK()
#define GP_UTILS_STACK_TRACK()
#endif // GP_DIVERSITY_LOG

#else //GP_DIVERSITY_STACK_DBG

#define GP_UTILS_END_OF_STACK
#define GP_UTILS_STACK_PATTERN
#define GP_UTILS_END_OF_ISRAM
#define GP_UTILS_INIT_STACK()

#define GP_UTILS_CHECK_STACK_PATTERN()
#define GP_UTILS_CHECK_STACK_POINTER()
#define GP_UTILS_CHECK_STACK_USAGE()
#define GP_UTILS_DUMP_STACK_POINTER()
#define GP_UTILS_DUMP_STACK_POINTER2(x)

#define GP_UTILS_DUMP_STACK_TRACK()
#define GP_UTILS_RESET_STACK_TRACK()
#define GP_UTILS_STACK_TRACK()

#endif // GP_DIVERSITY_STACK_DBG

#define GP_UTILS_AL_ELEM_FREE     0
#define GP_UTILS_AL_ELEM_RESERVED 1

#define GP_UTILS_CRC32_INITIAL_REMAINDER (UInt32)(0xFFFFFFFF)
#define GP_UTILS_CRC32_FINAL_XOR_VALUE   (UInt32)(0xFFFFFFFF)

#define GP_UTILS_LOCK_CLAIM()           true
#define GP_UTILS_LOCK_RELEASE()         true
#define GP_UTILS_LOCK_CHECK_CLAIMED()   false

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct gpUtils_Link {
    struct gpUtils_Link*   plnk_nxt;
    struct gpUtils_Link*   plnk_prv;
} gpUtils_Link_t;

typedef struct {
    gpUtils_Link_t* plnk_free;      // List of free cells
    gpUtils_Link_t* plnk_free_last; // Pointer to the last free cell for circular cell usage
    HAL_CRITICAL_SECTION_DEF(lock)
} gpUtils_LinkFree_t;

typedef struct {
    gpUtils_Link_t * plnk_first;     // The first element in the list
    gpUtils_Link_t * plnk_last;      // The last element in the list
    HAL_CRITICAL_SECTION_DEF(lock)
} gpUtils_LinkList_t;

//Padding issues for arraylists
typedef UInt8 gpUtils_ArrayListHdr_t;  //The header of an arrayelement indicating if the element is in use

typedef struct {
    UInt16 elem_size;    //Array list element size including header
    UInt16 elem_cnt;     // Array list total number of elements
} gpUtils_ArrayInfo_t;

typedef struct {
    gpUtils_ArrayInfo_t *    pbuf_info;      // pointer to Array buffer info at start of buffer block
    gpUtils_ArrayListHdr_t * pfree;          // first known free element in array list
    gpUtils_ArrayListHdr_t * plast;          // Last element pointer of array list (kept for calculation ease)
} gpUtils_ArrayFree_t;

//Simple array lists (not linked)
typedef struct {
    gpUtils_ArrayListHdr_t   type;       // value indentifying a unique ID for the list
    gpUtils_ArrayInfo_t *    pbuf_info;  // pointer to Array buffer info at start of free list buffer block
    gpUtils_ArrayListHdr_t * pfirst;     // Start ptr of Arraylist
    gpUtils_ArrayListHdr_t * plast;      // End ptr of Arraylist
} gpUtils_ArrayList_t;

//Circular buffer
typedef struct {
    UInt8* pBuffer;     // Pointer to the memory block
    UInt16 size;        // Size of the memory block
    UInt16 readIndex;   // Read index
    UInt16 writeIndex;  // Write index
    Bool   full;        // Flag indicating a full buffer
} gpUtils_CircularBuffer_t;

typedef Bool (*gpUtils_ElemCompare_t)( void* , void* );
typedef Bool (*gpUtils_ElemCompareDelete_t)( void* , void* , Bool* );

/*****************************************************************************
 *                    Static Data Definitions
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
#ifdef __cplusplus
extern "C" {
#endif

void gpUtils_WmrkSubscribe(void);

/****************************************************************************
 ****************************************************************************
 **                       Linked lists                                     **
 ****************************************************************************
 ****************************************************************************/

GP_API void  gpUtils_LLInit     (void * buf, UInt32 n_size_cell, UInt32 n_nr_of_elements, gpUtils_LinkFree_t * pfre);
GP_API void  gpUtils_LLClear    (gpUtils_LinkList_t * plst);
GP_API void* gpUtils_LLNew      (gpUtils_LinkFree_t * pfre);
GP_API void  gpUtils_LLAdd      (void * pelem, gpUtils_LinkList_t * plst);
GP_API void  gpUtils_LLInsert   (void * pelem, void * pelem_cur, gpUtils_LinkList_t * plst);
GP_API void  gpUtils_LLInsertBefore (void * pelem, void * pelem_cur, gpUtils_LinkList_t * plst);
GP_API void   gpUtils_LLInsertSorted (void * pelem, gpUtils_LinkList_t * plst, gpUtils_ElemCompare_t callback);
GP_API void*  gpUtils_LLNewSorted    (void * pelem, gpUtils_LinkList_t * plst, gpUtils_LinkFree_t * pfre, gpUtils_ElemCompareDelete_t callback);
GP_API void  gpUtils_LLUnlink   (void * pelem, gpUtils_LinkList_t * plst);
GP_API void  gpUtils_LLFree     (void * pelem, gpUtils_LinkFree_t * pfre);
GP_API void  gpUtils_LLRelease  (void * pelem, gpUtils_LinkList_t * plst, gpUtils_LinkFree_t * pfre);
GP_API void  gpUtils_LLDump     (gpUtils_LinkList_t * plst);
GP_API void  gpUtils_LLDumpFree (gpUtils_LinkFree_t * pfree);
GP_API void  gpUtils_LLDumpMemory (void * buf, UInt32 n_size_cell, UInt32 n_nr_of_elements, gpUtils_LinkList_t * plst, gpUtils_LinkFree_t * pfre);
GP_API Bool  gpUtils_LLCheckConsistency(void * buf, UInt32 n_size_cell, UInt32 n_nr_of_elements, gpUtils_LinkList_t * plst, gpUtils_LinkFree_t * pfre);
GP_API void  gpUtils_LLDeInit(gpUtils_LinkList_t* plst);
GP_API void  gpUtils_LLDeInitFree(gpUtils_LinkFree_t* pfre);

GP_API void* gpUtils_LLGetNext(void* pelem);
GP_API void* gpUtils_LLGetPrev(void* pelem);
GP_API gpUtils_Link_t* gpUtils_LLGetLink(void* pelem);
GP_API void* gpUtils_LLGetElem(gpUtils_Link_t* plnk);
GP_API void* gpUtils_LLGetFirstElem(gpUtils_LinkList_t* plst);
GP_API void* gpUtils_LLGetLastElem(gpUtils_LinkList_t* plst);

//Array list basic operations
GP_API void  gpUtils_ALInit       (void * buf, UInt32 n_size_cell, UInt32 n_nr_of_elements, gpUtils_ArrayFree_t * pfre);
GP_API void* gpUtils_ALNew        (gpUtils_ArrayFree_t* pfre);
GP_API void  gpUtils_ALFree       (void * pelem, gpUtils_ArrayFree_t* pfre);
GP_API void  gpUtils_ALDumpFree   (gpUtils_ArrayFree_t* pfre);
GP_API void  gpUtils_ALClear      (gpUtils_ArrayList_t* plst, gpUtils_ArrayListHdr_t type, gpUtils_ArrayFree_t* pfre);
GP_API void  gpUtils_ALAdd        (void * pelem, gpUtils_ArrayList_t* plst);
GP_API void  gpUtils_ALUnlink     (void * pelem, gpUtils_ArrayList_t* plst);
GP_API void  gpUtils_ALRelease    (void * pelem, gpUtils_ArrayList_t * plst, gpUtils_ArrayFree_t * pfre);
GP_API void  gpUtils_ALDump       (gpUtils_ArrayList_t* plst);
//GP_API void  gpUtils_ALDumpMemory (gpUtils_ArrayList_t* plst, gpUtils_ArrayFree_t* pfre);
//Array list functions
GP_API void*  gpUtils_ALGetNext  (gpUtils_ArrayList_t * plst, void* pelem);
GP_API void*  gpUtils_ALGetPrev  (gpUtils_ArrayList_t * plst, void* pelem);
GP_API void*  gpUtils_ALGetElem  (gpUtils_ArrayListHdr_t* phdr);
GP_API gpUtils_ArrayListHdr_t*   gpUtils_ALGetHdr  (void* pelem);
GP_API void*  gpUtils_ALGetFirstElem (gpUtils_ArrayList_t * plst);
GP_API void*  gpUtils_ALGetLastElem  (gpUtils_ArrayList_t * plst);
//Circular buffer basic operations
GP_API void   gpUtils_CircBInit           (gpUtils_CircularBuffer_t* pCircularBuffer, void * pBuffer, UInt16 size);
GP_API void   gpUtils_CircBClear          (gpUtils_CircularBuffer_t* pCircularBuffer);
GP_API UInt16 gpUtils_CircBAvailableData  (gpUtils_CircularBuffer_t* pCircularBuffer);
GP_API UInt16 gpUtils_CircBAvailableSpace (gpUtils_CircularBuffer_t* pCircularBuffer);
GP_API Bool   gpUtils_CircBWriteData      (gpUtils_CircularBuffer_t* pCircularBuffer, UInt8* pData, UInt16 length);
GP_API Bool   gpUtils_CircBWriteByte      (gpUtils_CircularBuffer_t* pCircularBuffer, UInt8 Data);
GP_API Bool   gpUtils_CircBReadData       (gpUtils_CircularBuffer_t* pCircularBuffer, UInt8* pData, UInt16 length);
GP_API Bool   gpUtils_CircBReadByte       (gpUtils_CircularBuffer_t* pCircularBuffer, UInt8* pData);
GP_API Bool   gpUtils_CircBGetData        (gpUtils_CircularBuffer_t* pCircularBuffer, UInt16 index, UInt8* pData, UInt16 length);
GP_API Bool   gpUtils_CircBGetByte        (gpUtils_CircularBuffer_t* pCircularBuffer, UInt16 index, UInt8* pData);

#ifdef GP_DIVERSITY_STACK_DBG
GP_API Bool gpUtils_CheckStackPattern(void);
GP_API Bool gpUtils_CheckStackPointer(void);
GP_API UInt8* gpUtils_GetStackPointer(void);
GP_API void gpUtils_GetStackPointer2(UInt8** pStack, UInt8** pFilledStack);
GP_API void gpUtils_ResetStackTrack(void);
GP_API void gpUtils_StackTrack(void);
GP_API void gpUtils_DumpStackTrack( void );
GP_API UInt16 gpUtils_GetMaxStackUsage(void);
#endif //GP_DIVERSITY_STACK_DBG

//Crc checking
GP_API UInt16 gpUtils_CalculateCrc(UInt8* pData , UInt16 length);
GP_API void   gpUtils_UpdateCrc(UInt16* pCRCValue, UInt8 Data);
GP_API void gpUtils_CalculatePartialCrc(UInt16* pCrcValue, UInt8* pData, UInt16 length);

//Crc32 checking
GP_API UInt32 gpUtils_CalculateCrc32(UInt8* pData , UInt16 length);
GP_API void   gpUtils_UpdateCrc32(UInt32* pCRCValue, UInt8 Data);
GP_API void   gpUtils_CalculatePartialCrc32(UInt32* pCrcValue, UInt8* pData, UInt16 length);

//Global lock
GP_API Bool gpUtils_LockClaim(void);
GP_API Bool gpUtils_LockRelease(void);
GP_API Bool gpUtils_LockCheckClaimed(void);

#ifdef __cplusplus
}
#endif


#endif    // _GPUTILS_H_

