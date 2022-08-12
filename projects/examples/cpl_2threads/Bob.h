#ifndef Bob_h
#define Bob_h
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2019  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file

    This file defines the Module Bob that is silly module that blinks a LED
    at specified frequency.  Other modules interact with the Bob module via
    model points.
 */

#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Dm/SubscriberComposer.h"
#include "Cpl/Dm/Mp/Uint32.h"
#include "Cpl/Dm/Mp/Bool.h"
#include "Cpl/System/ElapsedTime.h"


 /** This class blinks a LED at frequency specified by the 'timing' model point
     provided in its constructor.  The frequency will be 2 x 'timing'.  The use
     of the model points provides a thread safe mechanism for other modules to
     interact with Bob.

     The class uses the model point 'verbose' to enable/disable its text output.
     The text output uses the Trace statement to provide thread information and
     time stamps for the output messages.

     The class uses 'periodic scheduling' for the LED timing.  This is NOT the
     recommended way for 'timing logic' (i.e. a Cpl::System::Timer is recommended),
     but is done to provide an example of using the Periodic Scheduling with an
     event based thread.

     At run time - the application is required to call open() and close() on the
     module instance to start and stop the instance.  Note: the open() and close()
     MUST be AFTER scheduling has been enabled.  This is because the class's
     model point change notification subscription MUST be done in the thread context
     that the callback functions will occur in.
  */
class Bob
{
public:
    ///
    Bob( Cpl::Dm::MailboxServer&  myMbox,
         Cpl::Dm::Mp::Uint32&     delayTimeModelPoint,
         Cpl::Dm::Mp::Bool&       verboseModelPoint );

public:
    /// In-thread initialization of the module
    void start();

    /// In-thread shutdown of the module
    void stop();

protected:
    /// This method is called when the Model point changes state/value
    void delayModelPointChange( Cpl::Dm::Mp::Uint32 & mp );

    /// This method is called when the Model point changes state/value
    void verboseModelPointChange( Cpl::Dm::Mp::Bool & mp );

public:
    /// This method is called from the Periodic Scheduler
    static void periodicInterval( Cpl::System::ElapsedTime::Precision_T currentTick,
                                  Cpl::System::ElapsedTime::Precision_T currentInterval,
                                  void*                                 context );

protected:
    /// Reference to my model point
    Cpl::Dm::Mp::Uint32&     m_mpDelayTime;

    /// Reference to my model point
    Cpl::Dm::Mp::Bool&       m_mpVerbose;

    /// Observer for getting change notifications
    Cpl::Dm::SubscriberComposer<Bob, Cpl::Dm::Mp::Uint32>   m_observerDelay;

    /// Observer for getting change notifications
    Cpl::Dm::SubscriberComposer<Bob, Cpl::Dm::Mp::Bool>     m_observerVerbose;

    /// Time, in milliseconds, to delay between toggling the LED
    Cpl::System::ElapsedTime::Precision_T                   m_delayMs;

    /// Time, in milliseconds, of when the LED was last toggled
    Cpl::System::ElapsedTime::Precision_T                   m_timeMarker;

    /// Free running counter that track the number of 'edges'
    uint32_t        m_numEdges;

    /// Verbose flag
    bool            m_verbose;

    /// Keep track of my open/close state
    bool            m_opened;

};


#endif  // end header latch
