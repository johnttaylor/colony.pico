#ifndef Cpl_System_PeriodicScheduler_h_
#define Cpl_System_PeriodicScheduler_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/System/ElapsedTime.h"

///
namespace Cpl {
///
namespace System {

/** This concrete class is a 'policy' object that is used to add polled based,
    cooperative monotonic scheduling to a Runnable object.  Monotonic is this
    content means that if a 'interval' method is scheduled to execute at a
    periodic rate of 200Hz, then the method will be called on every 5ms boundary
    of the system time.

    The scheduler makes its best attempt at being monotonic and deterministic,
    but because the scheduling is polled and cooperative (i.e. its is the
    application's responsibility to not-overrun/over-allocate the processing
    done during each interval) the actual the timing cannot be guaranteed.
    That said, the schedule will detect and report when the interval timing
    slips.

    The usage of this class is to implement (or extended) a Cpl::System::Runnable
    object, i.e. invoked inside the 'forever' loop of the Runnable object's appRun()
    method.  The scheduler can be use within a 'bare' forever loop and it can be
    used with existing event based Runnable objects to add periodic scheduling.
    See Cpl::Dm::PeriodicScheduler for example extending an event based Runnable
    object to include periodic scheduling.
 */
class PeriodicSchedular
{
public:
    /** Definition for an interval method.  This method is called when the
        period time has expired for the interval.

        Where:
            currentTick     - is current system when the interval method is called
            currentInterval - is the deterministic interval boundary that is being
                              logically executed.
            context         - Application defined argument/value passed to the
                              interval method.

        Example:
            Given a interval method is scheduled to execute at 10Hz and the
            current system time in the Runnable object's forever loop is 10.212
            seconds when the interval method is called THEN:

                currentTick:=      10212 ms
                currentInterval:=  10200 ms
      */
    typedef void (*IntervalCallbackFunc_T)(Cpl::System::ElapsedTime::Precision_T currentTick,
                                            Cpl::System::ElapsedTime::Precision_T currentInterval,
                                            void* context);

    /** Defines an interval.  The application should treat this data struct as
        an opaque structure.

        NOTE: Use the CPL_SYSTEM_PERIODIC_SCHEDULER_DEFINE_INTERVAL macro to
        statically create (and initialize) an Interval instance
     */
    typedef struct
    {
        IntervalCallbackFunc_T                callbackFunc; //!< Callback function pointer
        unsigned long                         periodMs;     //!< The number of milliseconds in the Interval's period.
        void*                                 context;      //!< Optional Context for the callback.  The callback function is required to 'understand' the actual type of the context pointer being passed to it.
        Cpl::System::ElapsedTime::Precision_T timeMarker;   //!< Internal Use ONLY: Marks the last time the interval executed    
    } Interval_T;

    /** Defines the method that is used to report to the Application when an
        Interval does not execute 'on time'

        Where:
            intervalThatSlipped  - Reference to the interval that did not execute on time
            currentTick          - The system time when the scheduler executed the interval
            missedInterval       - The interval boundary that was missed
            intervalContext      - Application defined argument/value that was
                                   passed to the interval method.

     */
    typedef void (*ReportSlippageFunc_T)(Interval_T& intervalThatSlipped,
                                          Cpl::System::ElapsedTime::Precision_T currentTick,
                                          Cpl::System::ElapsedTime::Precision_T missedInterval,
                                          void* intervalContext);

    /** Defines the optional method for an Application callback that is called
        every the executeScheduler() is called.
     */
    typedef void (*IdleFunc_T)(bool atLeastIntervalExecuted);


public:
    /** Constructor. The application provides a variable length array of interval
        definitions that will be scheduled.  The last entry in the
        array must contain a 'null' Interval_T definition (i.e. all fields
        set to zero).  The Scheduler assumes that each Interval_T definition
        has a unique period time.

        The individual 'intervals' MUST be initialized (either statically or
        by calling initializeInterval()) before creating the scheduler.

        Notes:
            - When extending a event based (i.e. inherits from EventLoop) runnable
              object with scheduling, the Application should define all of the
              scheduled Intervals to have period times that are multiples of
              the EventLoop's 'timeOutPeriodInMsec' constructor value.
     */
    PeriodicSchedular( Interval_T*          intervals[],
                       ReportSlippageFunc_T slippageFunc = nullptr,
                       IdleFunc_T           idleFunc     = nullptr );

    /// Virtual destructor
    virtual ~PeriodicSchedular() {};


public:
    /** This method is used to invoke the scheduler.  When called zero or more
        Interval definitions will be executed.  The method returns true if at
        least one Interval was executed.  The caller is responsible for
        providing the current system time.

        If a scheduled Interval does not execute 'on time', then the reportSlippage()
        method will called.  It is the Application's to decide (what if anything)
        is done when there is slippage in the scheduling. The slippage is reported
        AFTER the Interval's IntervalCallbackFunc_T is called.
     */
    bool executeScheduler( Cpl::System::ElapsedTime::Precision_T currentTick = Cpl::System::ElapsedTime::precision() );


public:
    /** Method for the Application to initialize a Interval at runtime.
        NOTE: Use the CPL_SYSTEM_PERIODIC_SCHEDULER_DEFINE_INTERVAL
        macro to statically create (and initialize) an Interval instance
     */
    inline Interval_T& initializeInterval( Interval_T&            intervalToInit,
                                           IntervalCallbackFunc_T callbackFunc,
                                           unsigned long          periodMs,
                                           void*                  context  = nullptr )
    {
        intervalToInit.callbackFunc = callbackFunc;
        intervalToInit.periodMs     = periodMs;
        intervalToInit.context      = context;
        intervalToInit.timeMarker   = 0;
        return intervalToInit;
    }

protected:
    /// List of Intervals
    Interval_T*             m_intervals;

    /// Report slippage method
    ReportSlippageFunc_T    m_reportSlippage;

    /// Idle callback
    IdleFunc_T              m_idleFunc;

};

};      // end namespaces
};
#endif  // end header latch
