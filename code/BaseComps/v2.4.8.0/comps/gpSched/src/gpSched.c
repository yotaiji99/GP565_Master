/*
 * Copyright (c) 2008-2015, GreenPeak Technologies
 *
 * gpSched.c
 *   This file contains the implementation of the scheduler, which is the operating system.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpSched/src/gpSched.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_SCHED

#include "hal.h"

#include "gpSched.h"
#include "gpUtils.h"
#include "gpLog.h"
#ifdef GP_COMP_COM
#include "gpCom.h"
#endif
#include "hal_WB.h"
#include "hal_ExtendedPatch.h"
#include "gpAssert.h"

#include "gpWmrk.h"             // Watermarker component

#include "gpSched_defs.h"


/*****************************************************************************
 *                    Precompiler checks
 *****************************************************************************/

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define TIMER_MASK (((UInt32)1 << (32-GP_SCHED_TIME_RES_ORDER))-1)

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/
//Macro's use 27-bit range
#define GP_SCHED_TIME_COMPARE_LOWER(t1,t2)          (!((((t1)  - (t2))&((0x80000000LU>>(GP_SCHED_TIME_RES_ORDER-1))-1)) < (0x80000000LU>>GP_SCHED_TIME_RES_ORDER)))
#define GP_SCHED_TIME_COMPARE_BIGGER_EQUAL(t1,t2)   ( ((((t1)  - (t2))&((0x80000000LU>>(GP_SCHED_TIME_RES_ORDER-1))-1)) < (0x80000000LU>>GP_SCHED_TIME_RES_ORDER)))
#define GP_SCHED_TIME_COMPARE_BIGGER(t1,t2)         (!((((t2)  - (t1))&((0x80000000LU>>(GP_SCHED_TIME_RES_ORDER-1))-1)) < (0x80000000LU>>GP_SCHED_TIME_RES_ORDER)))
#define GP_SCHED_TIME_COMPARE_LOWER_EQUAL(t1,t2)    ( ((((t2)  - (t1))&((0x80000000LU>>(GP_SCHED_TIME_RES_ORDER-1))-1)) < (0x80000000LU>>GP_SCHED_TIME_RES_ORDER)))

/*void             Sched_ExecEvent(gpSched_Event_t* pevt);*/
#define Sched_ExecEvent(pevt) \
do { \
    if (pevt->arg) pevt->func.callbackarg(pevt->arg);\
    else           pevt->func.callback(); \
} while(false)

/*void             Sched_ReleaseEvent(gpSched_Event_t * pevt );*/
#define Sched_ReleaseEvent(pevt) \
do {                             \
    HAL_DISABLE_GLOBAL_INT();    \
    _ReleaseEvent(pevt);         \
    HAL_ENABLE_GLOBAL_INT();     \
} while(false)

/* Macros defining hook for SCHED_MAIN_BODY patch */
#define SCHED_HOOK_MAIN_BODY                 NULL
typedef void (*Sched_MainBody_Patch)(gpSched_Event_t*);
#define SCHED_CHOOK_MAIN_BODY                ((Sched_MainBody_Patch)SCHED_HOOK_MAIN_BODY)
#define SCHED_MAIN_BODY(pE)             HAL_PATCH_NORETVAL(SCHED_CHOOK_MAIN_BODY, pE)

#define SCHED_ACQUIRE_EVENT_LIST() do { \
    /*GP_LOG_SYSTEM_PRINTF("Acq list %u",0,__LINE__);*/\
    GP_UTILS_LL_ACQUIRE_LOCK(gpSched_EventList);\
} while(false)

#define SCHED_RELEASE_EVENT_LIST() do { \
    /*GP_LOG_SYSTEM_PRINTF("Rel list %u",0,__LINE__);*/\
    GP_UTILS_LL_RELEASE_LOCK(gpSched_EventList);\
} while(false)

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
STATIC UInt32 gpSched_GoToSleepTreshold;

//Event list definitions
gpUtils_LinkFree_t gpSched_EventFree[1];
gpUtils_LinkList_t gpSched_EventList[1];

STATIC UInt8 gpSched_GoToSleepDisableCounter;
STATIC gpSched_GotoSleepCheckCallback_t gpSched_cbGotoSleepCheck;



/*****************************************************************************
 *                    External Function Prototypes
 *****************************************************************************/


/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static void RescheduleEvent(UInt32 delay, gpSched_Event_t * pevt);

static void _ScheduleEvent(UInt32 rel_time, void_func callback);
static void _ScheduleEventArg(UInt32 rel_time, gpSched_EventCallback_t callback, void* arg);
void _ReleaseEvent(gpSched_Event_t* pevt );

gpSched_Event_t* _FindEvent(gpUtils_LinkList_t* plst , void_func callback, Bool unscheduleWhenFound);
static gpSched_Event_t* _FindEventArg(gpUtils_LinkList_t* plst , gpSched_EventCallback_t callback, void* arg, Bool unscheduleWhenFound );


static gpSched_Event_t* Sched_GetEvent( void );

static UInt32           Sched_GetEventIdlePeriod(void);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

void RescheduleEvent (UInt32 delay, gpSched_Event_t * pevt)
{
    gpUtils_LinkList_t* pList = gpSched_EventList;
    gpSched_Event_t* pevt_nxt = NULL;
    gpUtils_Link_t* plnk = NULL;
    UInt32 time;

    GP_ASSERT_DEV_INT(pevt); //pevt pointer should be != NULL

    HAL_TIMER_GET_CURRENT_TIME(time);
    if (delay == GP_SCHED_EVENT_TIME_NOW)
    {
        UInt32 first_time;

        if (pList->plnk_first)
        {
            pevt_nxt = (gpSched_Event_t*)GP_UTILS_LL_GET_ELEM(pList->plnk_first);
            first_time = pevt_nxt->time;
        }
        else
        {
            first_time = time;
        }
        pevt->time = GP_SCHED_TIME_COMPARE_LOWER_EQUAL (first_time, time)?
                first_time - 1L
            :
                time - 1L;
    }
    else
    {
        pevt->time = time + GP_SCHED_US2TIME (delay);
    }

    // Handle wrap around
    pevt->time &= TIMER_MASK;

    // Scroll pointer
    // Insert event
    // Duration : 8us per loop
    {
        // Search for first item, that should be executed later as the current one
        // or that will be executed as last
        for (plnk = pList->plnk_first;
             plnk;
             plnk = plnk->plnk_nxt)
        {
            pevt_nxt = (gpSched_Event_t*)GP_UTILS_LL_GET_ELEM(plnk);
            if (GP_SCHED_TIME_COMPARE_BIGGER( pevt_nxt->time , pevt->time ))
            {
                break;
            }
        }
    }

    if (plnk)
    {
        gpUtils_LLInsertBefore( pevt, pevt_nxt, pList);
    }
    else
    {
        gpUtils_LLAdd( pevt , pList );
    }
}

void _ScheduleEvent(UInt32 rel_time, void_func callback)
{
    gpSched_Event_t* pevt;

    GP_UTILS_LL_FREE_ACQUIRE_LOCK(gpSched_EventFree);
    pevt = (gpSched_Event_t*)gpUtils_LLNew(gpSched_EventFree);
    GP_UTILS_LL_FREE_RELEASE_LOCK(gpSched_EventFree);

    if (!pevt)
    {
        GP_ASSERT_SYSTEM(false);
    }
    else
    {
        pevt->arg = NULL; //Normal scheduling - no arg pointer
        pevt->func.callback = callback;
        RescheduleEvent( rel_time,  pevt);

    }
}

void _ScheduleEventArg(UInt32 rel_time, gpSched_EventCallback_t callback, void* arg)
{
    gpSched_Event_t* pevt;

    GP_ASSERT_DEV_EXT(arg); //arg pointer should be != NULL

    GP_UTILS_LL_FREE_ACQUIRE_LOCK(gpSched_EventFree);
    pevt = (gpSched_Event_t*)gpUtils_LLNew(gpSched_EventFree);
    GP_UTILS_LL_FREE_RELEASE_LOCK(gpSched_EventFree);

    if (!pevt)
    {
        GP_ASSERT_SYSTEM(false);
    }
    else
    {
        if (arg)
        {
            pevt->arg = arg;
        }
        pevt->func.callbackarg = callback;
        RescheduleEvent( rel_time,  pevt);

    }
}

void _ReleaseEvent(gpSched_Event_t* pevt )
{
    // Free event (Unlink has been done by GetEvent)
    GP_UTILS_LL_FREE_ACQUIRE_LOCK(gpSched_EventFree);
    gpUtils_LLFree(pevt,gpSched_EventFree);
    GP_UTILS_LL_FREE_RELEASE_LOCK(gpSched_EventFree);
}




UInt32 Sched_GetEventIdlePeriod (void)
{
    UInt32 idleTime = HAL_SLEEP_INDEFINITE_SLEEP_TIME;
    gpSched_Event_t* pevt;

    SCHED_ACQUIRE_EVENT_LIST();
    pevt = (gpSched_Event_t*)gpUtils_LLGetFirstElem(gpSched_EventList);
    if (pevt)
    {
        if(gpSched_GoToSleepTreshold == GP_SCHED_NO_EVENTS_GOTOSLEEP_THRES) //Only sleep if no events are pending
        {
            idleTime = 0;
        }
        else
        {
            UInt32 time_now;
            HAL_TIMER_GET_CURRENT_TIME(time_now);

            if(GP_SCHED_TIME_COMPARE_BIGGER(pevt->time,(time_now + GP_SCHED_US2TIME(gpSched_GoToSleepTreshold)) & TIMER_MASK))
            {
                idleTime = pevt->time - time_now ;
            }
            else
            {
                idleTime = 0;
            }
        }
#if 0
        UInt32 time_now;
        HAL_TIMER_GET_CURRENT_TIME(time_now);
        GP_LOG_SYSTEM_PRINTF("idle %li now %lu-%lu cb 0x%x",10,(gpSched_GoToSleepTreshold == GP_SCHED_NO_EVENTS_GOTOSLEEP_THRES) ? 0x1 : idleTime, time_now, pevt->time,(UIntPtr) pevt->func.callback);
        gpLog_Flush();
#endif
    }
    SCHED_RELEASE_EVENT_LIST();

    return (idleTime);
}


static gpSched_Event_t* Sched_GetEvent(void)
{
    gpSched_Event_t* pevt = NULL;

    SCHED_ACQUIRE_EVENT_LIST();
    pevt = (gpSched_Event_t*)gpUtils_LLGetFirstElem(gpSched_EventList);
    if(pevt)
    {
        UInt32 time_now;
        //Check the eventList for overdue events
        HAL_TIMER_GET_CURRENT_TIME(time_now);

        if (GP_SCHED_TIME_COMPARE_BIGGER_EQUAL (time_now, pevt->time))
        {
            gpUtils_LLUnlink(pevt, gpSched_EventList);
        }
        else
        {
            pevt = NULL;
        }
    }
    SCHED_RELEASE_EVENT_LIST();
    return pevt;
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpSched_Init(void)
{
    // Inititialize event lists
    gpSched_InitExtramData(); //Initialize free list and event data buffer
    gpUtils_LLClear(gpSched_EventList);

    gpSched_GoToSleepTreshold = GP_SCHED_DEFAULT_GOTOSLEEP_THRES;

    gpSched_GoToSleepDisableCounter = 0;   // Enable GotoSleep by default
    gpSched_cbGotoSleepCheck = NULL;
} //gpSched_init

void gpSched_DeInit(void)
{
    gpUtils_LLDeInit(gpSched_EventList);
    gpUtils_LLDeInitFree(gpSched_EventFree);
}

Bool gpSched_SetGotoSleepThreshold (UInt32 Threshold /*us*/) {
    //This defines the minimum time between SW events required for allowing to go to sleep
    gpSched_GoToSleepTreshold = Threshold;
    return true;
}

void gpSched_SetGotoSleepCheckCallback( gpSched_GotoSleepCheckCallback_t gotoSleepCheckCallback )
{
    NOT_USED(gotoSleepCheckCallback);
    gpSched_cbGotoSleepCheck = gotoSleepCheckCallback;
}

void gpSched_StartTimeBase( void )
{
    //Startup sync timer

    HAL_DISABLE_GLOBAL_INT();

    //init the timer1
    HAL_TIMER_START( 0 );
    HAL_ENABLE_GLOBAL_INT();
}

UInt32 gpSched_GetTime( void )
{
    // This function should not be called from an interrupt service routine,
    // since it uses atomic_on/off, and this can cause unwanted nested interrupt
    // service routines; instead, HAL_TIMER_GET_CURRENT_TIME() should be used in such cases
    UInt32 time;

    HAL_TIMER_GET_CURRENT_TIME( time );

    return time;
}

Bool gpSched_TimeCompareLower (UInt32 n_time1, UInt32 n_time2)
{
    return GP_SCHED_TIME_COMPARE_LOWER(n_time1,n_time2);
}

void gpSched_Clear( void )
{
    SCHED_ACQUIRE_EVENT_LIST();

    while(gpSched_EventList->plnk_last)
    {
        gpSched_Event_t* pevt;

        pevt = (gpSched_Event_t*)GP_UTILS_LL_GET_ELEM(gpSched_EventList->plnk_last);
        //GP_LOG_SYSTEM_PRINTF("event %x",2,(UInt16)pevt->func.callback);
        gpUtils_LLUnlink(pevt, gpSched_EventList);
        _ReleaseEvent(pevt);
    }


    gpSched_GoToSleepDisableCounter = 0;   // Enable GotoSleep by default
    SCHED_RELEASE_EVENT_LIST();
}

Bool gpSched_EventQueueEmpty( void )
{
    Bool Empty;
    SCHED_ACQUIRE_EVENT_LIST();
    Empty = (!gpSched_EventList->plnk_first);
    SCHED_RELEASE_EVENT_LIST();
    return Empty;
}

void gpSched_ScheduleEventArg(UInt32 rel_time, gpSched_EventCallback_t callback, void* arg)
{
    SCHED_ACQUIRE_EVENT_LIST();
    _ScheduleEventArg(rel_time , callback , arg);
    SCHED_RELEASE_EVENT_LIST();
}

void gpSched_ScheduleEvent(UInt32 rel_time, void_func callback )
{
    SCHED_ACQUIRE_EVENT_LIST();
    _ScheduleEvent( rel_time , callback);
    SCHED_RELEASE_EVENT_LIST();
}

Bool gpSched_UnscheduleEventArg(gpSched_EventCallback_t callback, void* arg)
{
    gpSched_Event_t* pevt;
    SCHED_ACQUIRE_EVENT_LIST();
    pevt=_FindEventArg(gpSched_EventList,callback, arg, true);
    SCHED_RELEASE_EVENT_LIST();
    return pevt?true:false;
}

Bool gpSched_ExistsEventArg(gpSched_EventCallback_t callback, void* arg)
{
    gpSched_Event_t* pevt;
    SCHED_ACQUIRE_EVENT_LIST();
    pevt=_FindEventArg(gpSched_EventList,callback, arg, false);
    SCHED_RELEASE_EVENT_LIST();
    return pevt?true:false;
}

UInt32 gpSched_GetRemainingTimeArg(gpSched_EventCallback_t callback, void* arg)
{
    gpSched_Event_t*        pevt=NULL;
    UInt32                  remainingTime = 0xffffffff;

    SCHED_ACQUIRE_EVENT_LIST();
    pevt=_FindEventArg(gpSched_EventList, callback, arg, false);
    SCHED_RELEASE_EVENT_LIST();

    if(pevt)
    {
        UInt32 time_now;
        HAL_TIMER_GET_CURRENT_TIME(time_now);

        if(GP_SCHED_TIME_COMPARE_BIGGER_EQUAL(pevt->time,(time_now) & TIMER_MASK))
        {
            GP_LOG_PRINTF("1 delta %lu now %lu-%lu cb 0x%x",10, pevt->time-time_now, time_now, pevt->time,(UIntPtr) pevt->func.callback);
            remainingTime = pevt->time - time_now ;
        }
        else
        {
            GP_LOG_PRINTF("2 now %lu-%lu cb 0x%x",10,  time_now, pevt->time,(UIntPtr) pevt->func.callback);
            remainingTime = (TIMER_MASK - time_now) + pevt->time;
        }

        remainingTime *=GP_SCHED_TIME_RES;
    }
    return remainingTime;
}

gpSched_Event_t* _FindEventArg(gpUtils_LinkList_t* plst , gpSched_EventCallback_t callback, void* arg, Bool unscheduleWhenFound)
{
    gpUtils_Link_t* plnk;
    for (plnk = plst->plnk_first;plnk;plnk = plnk->plnk_nxt)
    {
        gpSched_Event_t* pevt;
        pevt = (gpSched_Event_t*)GP_UTILS_LL_GET_ELEM(plnk);
        //using (arg == NULL) as wildcard
        if ((pevt->func.callbackarg == callback)&&((pevt->arg == arg) || (arg == NULL)))
        {
            if (unscheduleWhenFound)
            {
                gpUtils_LLUnlink (pevt, plst);
                _ReleaseEvent(pevt);
            }
            return pevt;
        }
    }

    return NULL;
}

Bool gpSched_UnscheduleEvent(void_func callback)
{
    gpSched_Event_t* pevt;
    SCHED_ACQUIRE_EVENT_LIST();
    pevt=_FindEvent(gpSched_EventList, callback, true);
    SCHED_RELEASE_EVENT_LIST();
    return pevt?true:false;
}

Bool gpSched_ExistsEvent(void_func callback)
{
    gpSched_Event_t* pevt;
    SCHED_ACQUIRE_EVENT_LIST();
    pevt=_FindEvent(gpSched_EventList, callback, false);
    SCHED_RELEASE_EVENT_LIST();
    return pevt?true:false;
}

UInt32 gpSched_GetRemainingTime(void_func callback)
{
    gpSched_Event_t*        pevt=NULL;
    UInt32                  remainingTime = 0xffffffff;

    SCHED_ACQUIRE_EVENT_LIST();
    pevt=_FindEvent(gpSched_EventList, callback, false);
    SCHED_RELEASE_EVENT_LIST();

    if(pevt)
    {
        UInt32 time_now;
        HAL_TIMER_GET_CURRENT_TIME(time_now);

        if(GP_SCHED_TIME_COMPARE_BIGGER_EQUAL(pevt->time,(time_now) & TIMER_MASK))
        {
            GP_LOG_PRINTF("1 delta %lu now %lu-%lu cb 0x%x",10, pevt->time-time_now, time_now, pevt->time,(UIntPtr) pevt->func.callback);
            remainingTime = pevt->time - time_now ;
        }
        else
        {
            GP_LOG_PRINTF("2 now %lu-%lu cb 0x%x",10,  time_now, pevt->time,(UIntPtr) pevt->func.callback);
            remainingTime = (TIMER_MASK - time_now) + pevt->time;
        }

        remainingTime *=GP_SCHED_TIME_RES;
    }
    return  remainingTime;
}

gpSched_Event_t* _FindEvent(gpUtils_LinkList_t* plst , void_func callback, Bool unscheduleWhenFound)
{
    gpUtils_Link_t* plnk;

    for (plnk = plst->plnk_first;plnk;plnk = plnk->plnk_nxt)
    {
        gpSched_Event_t* pevt;
        pevt = (gpSched_Event_t*)GP_UTILS_LL_GET_ELEM(plnk);
        if ((pevt->func.callback == callback)
            &&(pevt->arg == NULL)
            )
        {
            if (unscheduleWhenFound)
            {
                gpUtils_LLUnlink (pevt, plst);
                _ReleaseEvent(pevt);
            }
            return pevt;
        }
    }
    return NULL;
}

UInt32 Sched_CanGoToSleep(void)
{
    UInt32 result = 0;

    HAL_DISABLE_GLOBAL_INT();
    result = Sched_GetEventIdlePeriod();

    if (gpSched_GoToSleepDisableCounter ||
        (gpSched_cbGotoSleepCheck && !gpSched_cbGotoSleepCheck()))
    {
#ifdef GP_SCHED_FREE_CPU_TIME
        if(result > GP_SCHED_FREE_CPU_TIME)
        {
            result = GP_SCHED_FREE_CPU_TIME;
        }
#else
        result = 0;
#endif //GP_SCHED_FREE_CPU_TIME
    }

#ifdef GP_COMP_COM
#endif //GP_COMP_COM

    //Don't go to sleep when Radio interrupt is pending
    if(HAL_RADIO_INT_CHECK_IF_OCCURED())
    {
        result = 0;
    }

    HAL_ENABLE_GLOBAL_INT();
    return result;
}


void gpSched_GoToSleep( void )
{
    if (Sched_CanGoToSleep())
    {
#ifdef GP_COMP_COM
        //Flush out any pending serial data
        gpCom_Flush();
#endif
        HAL_DISABLE_GLOBAL_INT();
        // Disable unneeded interrupts
        HAL_TIMER_STOP(); // Re-enabled by TIMER_RESTART()

        HAL_ENABLE_SLEEP_UC();
        HAL_ENABLE_GLOBAL_INT();

        while (true)
        {
            UInt32 timeTosleep;
            timeTosleep = Sched_CanGoToSleep();
            if(timeTosleep == 0)
            {
                break;
            }
            HAL_SLEEP_UC( timeTosleep );
            HAL_ENABLE_SLEEP_UC();
        }

        //Restart timer of uC without initialization
        //Note: if we have a HAL that supports going to sleep while an event is pending,
        //we have to forward the time of the scheduler when a new event is scheduled from an ISR or when we wake-up to execute the pending event.
        HAL_TIMER_RESTART();

#ifdef GP_COMP_COM
        //Flush out any new serial data
        gpCom_Flush();
#endif
    }
}

void gpSched_SetGotoSleepEnable( Bool enable )
{
    NOT_USED(enable);
    //gpSched_GoToSleepEnable = enable;
    HAL_DISABLE_GLOBAL_INT();
    if (enable)
    {
        GP_ASSERT_DEV_EXT(gpSched_GoToSleepDisableCounter);
        gpSched_GoToSleepDisableCounter--;
    }
    else
    {
        gpSched_GoToSleepDisableCounter++;
    }
    HAL_ENABLE_GLOBAL_INT();
}


/*****************************************************************************
 *                    Main function
 *****************************************************************************/

extern void Application_Init( void );

#ifndef GP_SCHED_EXTERNAL_MAIN
void gpSched_Main_Init(void)
{
    HAL_INITIALIZE_GLOBAL_INT();

    //Hardware initialisation
    HAL_INIT();

    //Radio interrupts that occur will only be handled later on in the main loop
    //Other interrupt source do not trigger any calls to blocks that are not initialized yet
    HAL_ENABLE_GLOBAL_INT();

    Application_Init();

    GP_UTILS_DUMP_STACK_POINTER();
    GP_UTILS_CHECK_STACK_PATTERN();
    GP_UTILS_CHECK_STACK_POINTER();
}
#endif

void gpSched_Main_Body(void)
{
    gpSched_Event_t* pevt;

    // check stack before event (after coming from sleep)
    GP_UTILS_CHECK_STACK_PATTERN();
    GP_UTILS_CHECK_STACK_POINTER();

    GP_ASSERT_DEV_EXT(HAL_GLOBAL_INT_ENABLED());

    HAL_WDT_RESET();

        //Handle radio interrupts
        HAL_RADIO_INT_EXEC_IF_OCCURED();

#ifdef GP_COMP_COM
        //Handle non-interrupt driven actions from gpCom
        gpCom_HandleTx();
#endif //GP_COMP_COM


    // Handle events
    pevt = Sched_GetEvent();
    SCHED_MAIN_BODY( pevt );
    if (pevt)             // get event
    {
        GP_ASSERT_DEV_EXT(HAL_GLOBAL_INT_ENABLED());
            //GP_LOG_SYSTEM_PRINTF("sched: %08lx",0, pevt->func.callback);
            Sched_ExecEvent(pevt);                   // execute event
        // check stack after event
        GP_UTILS_CHECK_STACK_PATTERN();
        GP_UTILS_CHECK_STACK_POINTER();
        if (!HAL_GLOBAL_INT_ENABLED())
        {
            GP_LOG_PRINTF("AtomF %x",2,pevt->func.callback);
            GP_ASSERT_DEV_EXT(HAL_GLOBAL_INT_ENABLED());
        }
        Sched_ReleaseEvent(pevt);                // release event
        // check stack after event release
        GP_UTILS_CHECK_STACK_PATTERN();
        GP_UTILS_CHECK_STACK_POINTER();
        GP_UTILS_CHECK_STACK_USAGE();
    }
#ifdef GP_SCHED_OS_USED //Only needed when system is part of a higher layer OS
#endif //GP_SCHED_OS_USED
    GP_ASSERT_DEV_EXT(HAL_GLOBAL_INT_ENABLED());
}

#ifndef GP_SCHED_EXTERNAL_MAIN
MAIN_FUNCTION_RETURN_TYPE MAIN_FUNCTION_NAME(void)
{
    //the line below destroys the stack. so don't call it from a function!
    GP_UTILS_INIT_STACK();
    gpSched_Main_Init();
    for (;;)
    {
        // Check if the system can go to sleep
        gpSched_GoToSleep();
        gpSched_Main_Body();
    }
    MAIN_FUNCTION_RETURN_VALUE;
}
#endif
