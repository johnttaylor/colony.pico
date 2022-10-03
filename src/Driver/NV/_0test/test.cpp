/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "colony_config.h"
#include "test.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/FString.h"
#include <memory.h>

#ifndef OPTION_MAX_PAGE_SIZE
#define OPTION_MAX_PAGE_SIZE        1024
#endif

///
using namespace Driver::NV;


#define TEST_FAILED     1
#define TEST_PASSED     0


#define SECT_           "_0test"

static uint8_t pageBuffer_[OPTION_MAX_PAGE_SIZE];
static uint8_t expectedBuffer_[OPTION_MAX_PAGE_SIZE];
static uint8_t expectedBuffer2_[OPTION_MAX_PAGE_SIZE];

static Cpl::Text::FString<1024> tmpBuffer_;

static void printBuffer( const char* header, uint8_t* buffer, size_t numberBytes )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("BUFFER: %s", header) );
    while ( numberBytes )
    {
        size_t nbytes = numberBytes > 16 ? 16 : numberBytes;

        Cpl::Text::bufferToAsciiHex( buffer, nbytes, tmpBuffer_ );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("  %s", tmpBuffer_.getString()) );
        numberBytes -= 16;
        buffer += 16;
    }
}

///////////////////////////////////////////////////////////////////////
int runtests( Driver::NV::Api& uut,
              size_t           expectedPages,
              size_t           expectedBytesPerPage )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Starting test for the NV RAM tests") );

    // Verify page info
    if ( uut.getNumPages() != expectedPages )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Incorrect number of pages. expected=%ld, actual=%lu", expectedPages, uut.getNumPages()) );
        return TEST_FAILED;
    }
    if ( uut.getPageSize() != expectedBytesPerPage )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Incorrect page size. expected=%ld, actual=%lu", expectedBytesPerPage, uut.getPageSize()) );
        return TEST_FAILED;
    }


    // Start the NV driver
    if ( !uut.start() )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Failed to start the NV driver") );
        return TEST_FAILED;
    }

    // Attempt to write pass the end of the memory
    size_t offset = uut.getPageSize() * uut.getNumPages() - 10;
    size_t len    = 11;
    if ( uut.write( offset, expectedBuffer_, len ) )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED writing passed end of memory. offset=%lu, len=%lu...", offset, len) );
        return TEST_FAILED;
    }

    // Attempt to read pass the end of the memory
    offset = uut.getPageSize() * uut.getNumPages() - 10;
    len    = 11;
    if ( uut.read( offset, pageBuffer_, len ) )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED reading passed end of memory. offset=%lu, len=%lu...", offset, len) );
        return TEST_FAILED;
    }

    // Erase the entire NV storage
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Erasing...") );
    memset( expectedBuffer_, 0xFF, uut.getPageSize() );
    unsigned long startTime = Cpl::System::ElapsedTime::milliseconds();
    for ( size_t i=0; i < uut.getNumPages(); i++ )
    {
        if ( !uut.write( i * uut.getPageSize(), expectedBuffer_, uut.getPageSize() ) )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED to erase page %d", i) );
            return TEST_FAILED;
        }
    }
    unsigned long endTime = Cpl::System::ElapsedTime::milliseconds();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Erase completed. duration = %lu ms", Cpl::System::ElapsedTime::deltaMilliseconds( startTime, endTime )) );

    // Verify the erase
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Verifying the erase...") );
    startTime = Cpl::System::ElapsedTime::milliseconds();
    for ( size_t i=0; i < uut.getNumPages(); i++ )
    {
        memset( pageBuffer_, 0x01, uut.getPageSize() );
        if ( !uut.read( i * uut.getPageSize(), pageBuffer_, uut.getPageSize() ) )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED to read page %d", i) );
            return TEST_FAILED;
        }
        if ( memcmp( pageBuffer_, expectedBuffer_, uut.getPageSize() ) != 0 )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Page read contents bad! (%d).", i) );
            printBuffer( "PageBuffer", pageBuffer_, uut.getPageSize() );
            printBuffer( "ExpectedBuffer", expectedBuffer_, uut.getPageSize() );
            return TEST_FAILED;
        }
    }
    endTime = Cpl::System::ElapsedTime::milliseconds();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Verify completed. duration = %lu ms", Cpl::System::ElapsedTime::deltaMilliseconds( startTime, endTime )) );

    // Non page aligned writes
    size_t   totalBytes         = uut.getPageSize() + 3;
    size_t   startingOffset     = uut.getPageSize() / 2 + 10;
    size_t   page0Bytes         = uut.getPageSize() - startingOffset;
    size_t   page1Bytes         = totalBytes - page0Bytes;
    size_t   expectedStartPage0 = 0;
    size_t   expectedStartPage1 = expectedStartPage0 + uut.getPageSize();
    uint8_t  startVal           = 0xAA;
    memset( expectedBuffer_, startVal, uut.getPageSize() );
    memset( expectedBuffer2_, 0xFF, uut.getPageSize() );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Writing non-aligned buffer (start=%d, val=%02X)...", startingOffset, startVal) );
    if ( !uut.write( startingOffset, expectedBuffer_, uut.getPageSize() ) )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED Writing non-aligned buffer (start=%d, val=%02X)...", startingOffset, startVal) );
        return TEST_FAILED;
    }
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Verifying non-aligned buffer - Check1 (start=%d, val=%02X)...", startingOffset, startVal) );
    if ( !uut.read( expectedStartPage0, pageBuffer_, uut.getPageSize() ) )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED reading back for non-aligned verify - Check1 (start=%d, val=%02X)...", startingOffset, startVal) );
        return TEST_FAILED;
    }
    // Verify the write contents on the 1st page
    if ( memcmp( pageBuffer_ + startingOffset, expectedBuffer_, page0Bytes ) != 0 )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED verify (Check1) for non-aligned verify: 1st page content (start=%d, val=%02X)...", startingOffset, startVal) );
        printBuffer( "PageBuffer", pageBuffer_, uut.getPageSize() );
        printBuffer( "ExpectedBuffer", expectedBuffer_, uut.getPageSize() );
        return TEST_FAILED;
    }
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Verified non-aligned buffer - Check1 (start=%d, val=%02X).", startingOffset, startVal) );
    // Verity that no wrap around happened on the 1st page
    if ( memcmp( pageBuffer_, expectedBuffer2_, uut.getPageSize() - page0Bytes ) != 0 )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED verify (Check1) for non-aligned verify: 1st page wrap-around (start=%d, val=%02X)...", startingOffset, startVal) );
        printBuffer( "PageBuffer", pageBuffer_, uut.getPageSize() );
        printBuffer( "ExpectedBuffer", expectedBuffer_, uut.getPageSize() );
        return TEST_FAILED;
    }
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Verified non-aligned buffer - Check1 (start=%d, val=%02X).", startingOffset, startVal) );

    CPL_SYSTEM_TRACE_MSG( SECT_, ("Verifying non-aligned buffer - Check2 (start=%d, val=%02X)...", startingOffset, startVal) );
    if ( !uut.read( expectedStartPage1, pageBuffer_, uut.getPageSize() ) )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED reading back for non-aligned verify - Check2 (start=%d, val=%02X)...", startingOffset, startVal) );
        return TEST_FAILED;
    }
    // Verify the write contents on the 2nd page
    if ( memcmp( pageBuffer_, expectedBuffer_ + page0Bytes, page1Bytes ) != 0 )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED verify (Check2) for non-aligned verify: 2nd page content (start=%d, val=%02X)...", startingOffset, startVal) );
        printBuffer( "PageBuffer", pageBuffer_, uut.getPageSize() );
        printBuffer( "ExpectedBuffer", expectedBuffer_, uut.getPageSize() );
        return TEST_FAILED;
    }
    // Verity that no extra writes on the 2nd page
    if ( memcmp( pageBuffer_ + page1Bytes, expectedBuffer2_, uut.getPageSize() - page1Bytes ) != 0 )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED verify (Check2) for non-aligned verify: 2nd page no-extras (start=%d, val=%02X)...", startingOffset, startVal) );
        printBuffer( "PageBuffer", pageBuffer_, uut.getPageSize() );
        printBuffer( "ExpectedBuffer", expectedBuffer_, uut.getPageSize() );
        return TEST_FAILED;
    }
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Verified non-aligned buffer - Check2 (start=%d, val=%02X).", startingOffset, startVal) );


    CPL_SYSTEM_TRACE_MSG( SECT_, ("\n*** ALL TESTS PASSED ***\n") );
    return TEST_PASSED;
}


