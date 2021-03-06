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

#include "Catch/catch.hpp"
#include "common.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Api.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/MailboxServer.h"



////////////////////////////////////////////////////////////////////////////////
/* The test app consists of 4 threads:
   - Three client threads, one contains viewers, the other two contain writers
   - One master thread (which the main thread)
*/
// Create my Data Model mailboxes
static MailboxServer     t1Mbox_;
static MailboxServer     t2Mbox_;
static MailboxServer     t3Mbox_;

// Allocate/create my Model Database
static ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Mp::Uint32       mp_apple_( modelDb_, "APPLE" );
static Mp::Uint32       mp_orange_( modelDb_, "ORANGE" );
static Mp::Uint32       mp_cherry_( modelDb_, "CHERRY" );
static Mp::Uint32       mp_plum_( modelDb_, "PLUM" );


#define VIEWER_APPLE1_END_VALUE     10
#define VIEWER_ORANGE1_END_VALUE    7
#define VIEWER_CHERRY1_END_VALUE    11
#define VIEWER_PLUM1_END_VALUE      9
#define GENERIC_VIEWER_END_COUNT    (VIEWER_APPLE1_END_VALUE+VIEWER_ORANGE1_END_VALUE+VIEWER_CHERRY1_END_VALUE)


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "mvc" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox_, "T1" );
    Cpl::System::Thread* t2 = Cpl::System::Thread::create( t2Mbox_, "T2" );

    // Create my viewers, writers
#define NUM_INSTANCES   9
    Viewer viewer_apple1( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_apple_, VIEWER_APPLE1_END_VALUE );
    Writer writer_apple1( t2Mbox_, Cpl::System::Thread::getCurrent(), mp_apple_, 10, 1, VIEWER_APPLE1_END_VALUE, 1 );

    Viewer viewer_orange1( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_orange_, VIEWER_ORANGE1_END_VALUE );
    Writer writer_orange1( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_orange_, 0, 1, VIEWER_ORANGE1_END_VALUE, 1 );

    Viewer viewer_cherry1( t2Mbox_, Cpl::System::Thread::getCurrent(), mp_cherry_, VIEWER_CHERRY1_END_VALUE );
    Writer writer_cherry1( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_cherry_, 100, 1, VIEWER_CHERRY1_END_VALUE, 1 );

    Viewer viewer_plum1( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_plum_, VIEWER_PLUM1_END_VALUE );
    Writer writer_plum1( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_plum_, 100, 1, VIEWER_PLUM1_END_VALUE, 1 );

    GenericViewer generic_viewer( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_apple_, mp_orange_, mp_cherry_, GENERIC_VIEWER_END_COUNT );


    // Open my viewers, writers
    viewer_apple1.open();
    viewer_orange1.open();
    viewer_cherry1.open();
    viewer_plum1.open();
    generic_viewer.open();
    writer_apple1.open();
    writer_orange1.open();
    writer_cherry1.open();
    writer_plum1.open();


    // Wait for everything to finish
    for ( int i=0; i < NUM_INSTANCES; i++ )
    {
        Cpl::System::Thread::wait();
    }

    REQUIRE( viewer_apple1.m_lastValue == VIEWER_APPLE1_END_VALUE );
    REQUIRE( viewer_orange1.m_lastValue == VIEWER_ORANGE1_END_VALUE );
    REQUIRE( viewer_cherry1.m_lastValue == VIEWER_CHERRY1_END_VALUE );
    REQUIRE( viewer_plum1.m_lastValue >= VIEWER_PLUM1_END_VALUE );
    REQUIRE( generic_viewer.m_mpNotificationCount >= (GENERIC_VIEWER_END_COUNT/2) );

    CPL_SYSTEM_TRACE_MSG( SECT_, ("viewer_apple1(%p). m_lastValue=%lu (expected == %lu)", &viewer_apple1, viewer_apple1.m_lastValue, VIEWER_APPLE1_END_VALUE) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("viewer_orange1(%p). m_lastValue=%lu (expected == %lu)", &viewer_orange1, viewer_orange1.m_lastValue, VIEWER_ORANGE1_END_VALUE) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("viewer_cherry1(%p). m_lastValue=%lu (expected == %lu)", &viewer_cherry1, viewer_cherry1.m_lastValue, VIEWER_CHERRY1_END_VALUE) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("viewer_plum1(%p). m_lastValue=%lu (expected >= %lu)", &viewer_plum1, viewer_plum1.m_lastValue, VIEWER_PLUM1_END_VALUE) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("generic_viewer(%p). m_notifCount=%lu (expected >= %lu)", &generic_viewer, generic_viewer.m_mpNotificationCount, (GENERIC_VIEWER_END_COUNT/2)) );

    // Close my viewers, writers, 
    generic_viewer.close();
    viewer_apple1.close();
    viewer_orange1.close();
    viewer_cherry1.close();
    viewer_plum1.close();
    writer_apple1.close();
    writer_orange1.close();
    writer_cherry1.close();
    writer_plum1.close();


    // Shutdown threads
    t1Mbox_.pleaseStop();
    t2Mbox_.pleaseStop();
    Cpl::System::Api::sleep( 100 ); // allow time for threads to stop
    REQUIRE( t1->isRunning() == false );
    REQUIRE( t2->isRunning() == false );

    Cpl::System::Thread::destroy( *t1 );
    Cpl::System::Thread::destroy( *t2 );
    Cpl::System::Api::sleep( 100 ); // allow time for threads to stop BEFORE the runnable object goes out of scope
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

