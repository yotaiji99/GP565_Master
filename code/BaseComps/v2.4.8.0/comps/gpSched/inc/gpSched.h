/*
 * Copyright (c) 2008-2014, GreenPeak Technologies
 *
 * gpSched.h
 *   This file contains the definitions of the scheduler, wich is the operating system.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpSched/inc/gpSched.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */
/**
 * @file gpSched.h
 * @brief The PeakNet scheduler handles the time of a PeakNode.
 *
 * The time base in this scheduler has the following structure:
 *
 * The base period is split into a number of intervals (see function gpSched_SetPeriod()).  Each interval is
 * split into 2 subintervals.
 *
 * The timing of the intervals and subintervals is defined by the asynchronious timer of the AtMega,
 * i.e. the asynchronious timer interrupt is used to indicate the beginning of the interval or subinterval.
 * The hardware Timer1 (Atmega) is used to measure the time within one interval.  This time measurement
 * restart in the beginning of each interval (not reset on the second subinterval).  The timer1 has a
 * discontinuity at the end of each interval.  The Timer1 is started in the ISR of the asynchronious timer.
 *
 * A function call that should be triggered by the scheduler in the future is called 'event'.  Two different
 * types of events exists: slotted and unslotted events.
 *
 * Unslotted event (see function gpSched_ScheduleEvent() and gpSched_ScheduleEventArg()) are executed
 * from the main loop and have the lowest priority.  Slotted events are triggered by an interrupt and
 * have a higher priority (see gpSched_ScheduleSlottedEvent() and gpSched_ScheduleSlottedEventArg()).
 * The interrupt frequency is 320 us.
 *
 * <h2>Drift prediction</h2>
 *
 * The drift prediction is used to handle the clock drift of the asynchronious timer between different devices.
 * The following assumptions are made:
 *
 * The Timer1 runs on the same (absolute) frequency on all devices and the measurement made based on this timer
 * are accurate.  The asynchronious timer does not run at the same frequency on all devices.
 *
 * The drift of the asynchronious timer is measured using a synchronisation signal.  To compensate the drift,
 * a correction is applied to the timer1 offset and to the asynchronious timer itself in the ISR of the
 * asynchronious timer before starting timer1.
 *
 */

#ifndef _GPSCHED_H_
#define _GPSCHED_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/**
 * System Time.
 * The Scheduler works with 32us resolution.
 */
#define GP_SCHED_TIME_RES_ORDER    5
#define GP_SCHED_TIME_RES          (1<<GP_SCHED_TIME_RES_ORDER)    // system time resolution = 32 us
#define GP_SCHED_US2TIME(us)       (us >> GP_SCHED_TIME_RES_ORDER )// convert us to system time resolution

#define GP_SCHED_EVENT_TIME_NOW                     ((UInt32) -1L)

/** @brief Compares times from the chip's timebase - check if t1 < t2 (in us)
*/
#define GP_SCHED_TIME_COMPARE_LOWER_US(t1,t2)          (!((UInt32)((t1)  - (t2))/*&(0xFFFFFFFF)*/ < (0x80000000LU)))

/** @brief Allow sleep only if no events are pending
*/
#define GP_SCHED_NO_EVENTS_GOTOSLEEP_THRES  ((UInt32)(0xFFFFFFFF))

/** @brief Default time between events before going to sleep is considered */
#ifndef GP_SCHED_DEFAULT_GOTOSLEEP_THRES
#define GP_SCHED_DEFAULT_GOTOSLEEP_THRES GP_SCHED_NO_EVENTS_GOTOSLEEP_THRES
#endif

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef void (*gpSched_EventCallback_t)( void* );

typedef Bool (* gpSched_GotoSleepCheckCallback_t) ( void );

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the scheduler
 *
 * This function intializes the scheduler.
 *
 */
GP_API void gpSched_Init(void);
GP_API void gpSched_DeInit(void);

/**
 * @brief Sets the minimum time required between events for going to sleep.
 *
 * This function sets a threshold in us, that is used for determining if we can go to sleep.
 * If the next scheduled event is at a time later than the current time + the defined threshold (- wakeup threshold)
 * then going to sleep is allowed
 *
 *  @param Threshold     threshold in us
 */
GP_API Bool gpSched_SetGotoSleepThreshold (UInt32 Threshold /*us*/);
GP_API void gpSched_SetGotoSleepCheckCallback( gpSched_GotoSleepCheckCallback_t gotoSleepCheckCallback );
GP_API void gpSched_SetGotoSleepEnable( Bool enable );

GP_API void gpSched_GoToSleep( void );
GP_API UInt32 Sched_CanGoToSleep(void);

/**
 * @brief Starts the time base.
 *
 * Starts the time base, i.e. the timer interrupts are enabled.
 */
GP_API void gpSched_StartTimeBase(void);

/**
 * @brief Returns the current time of the time base.
 *
 * Returns the current time in time base units of 32us.
 */
GP_API UInt32 gpSched_GetTime( void );

GP_API Bool gpSched_TimeCompareLower (UInt32 n_time1, UInt32 n_time2);

/**
 * @brief Clears the event queue.
 *
 * Clears the event queue.
 *
 */
GP_API void           gpSched_Clear( void );

/**
 * @brief Returns the event queue status.
 *
 * Returns the next event to execute.  The return pointer is NULL when no event is pending yet.  This function
 * should be called in a polling loop in the main thread.
 *
 * @return  Event queue status:
 *              - true: Queue is empty
 *              - false: Still events in the queue
 */
GP_API Bool         gpSched_EventQueueEmpty( void );

/**
 * @brief Schedules a scheduled event.
 *
 * Schedules an event.  The event is inserted into the event queue.
 *
 * @param  rel_time     Relative execution time (delay) in us.  If the delay equals 0, the function is directly called.
 * @param  callback     Callback function.
 * @param  arg_buf      Pointer to the argument buffer.  This parameter is ignored, if arg_length parameter is 0.
 * @param  arg_length   Argument buffer length.
 */
GP_API void         gpSched_ScheduleEvent           (UInt32 rel_time, void_func callback);
GP_API void         gpSched_ScheduleEventArg        (UInt32 rel_time, gpSched_EventCallback_t callback, void* arg);

/**
 * @brief Unschedule a scheduled event.
 *
 * Unschedules a scheduled event before its execution.  The event is identified with the callback pointer and the
 * arguments content.
 *
 * @param  callback Scheduled event callback.
 * @param  arg      Pointer to the the arguments used when scheduling. Use NULL to skip matching the arg pointer.
 */
GP_API Bool         gpSched_UnscheduleEvent(void_func callback);
GP_API Bool         gpSched_ExistsEvent(void_func callback);
GP_API UInt32       gpSched_GetRemainingTime(void_func callback);

GP_API Bool         gpSched_UnscheduleEventArg(gpSched_EventCallback_t callback, void* arg);
GP_API Bool         gpSched_ExistsEventArg(gpSched_EventCallback_t callback, void* arg);
GP_API UInt32       gpSched_GetRemainingTimeArg(gpSched_EventCallback_t callback, void* arg);

GP_API void gpSched_DumpList( void );

/**
 * @brief the MAIN_FUNCTION_NAME define will be used as entry point for the gpSched scheduler
 * 
 * When defining MAIN_FUNCTION_NAME to a certain function, another main() implementation is expected
 * To start the normal gpSched operation, the MAIN_FUNCTION_NAME can be called.
 * gpSched will then loop infinitely, executing any scheduled event and servicing stack functionality
*/
#ifdef MAIN_FUNCTION_NAME
GP_API MAIN_FUNCTION_RETURN_TYPE MAIN_FUNCTION_NAME(void);
#define GP_SCHED_OS_USED
#else //MAIN_FUNCTION_NAME
#define MAIN_FUNCTION_NAME main
#endif //MAIN_FUNCTION_NAME

GP_API void gpSched_Main_Init(void);
GP_API void gpSched_Main_Body(void);

#ifdef __cplusplus
}
#endif

#endif // _GPSCHED_H_
