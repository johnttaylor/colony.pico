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

#include "Pipe.h"

using namespace Driver::TPipe;


//////////////////////////////////////////////////
Pipe::Pipe( Cpl::Container::Map<RxFrameHandlerApi>&   rxFrameHdlrs,
            Cpl::Text::Frame::StreamDecoder&          deframer,
            Cpl::Text::Frame::StreamEncoder&          framer,
            size_t                                    rxBufferSize,
            const char*                               verbDelimiters
)
    : m_rxHandlers( rxFrameHdlrs )
    , m_deframer( deframer )
    , m_framer( framer )
    , m_frameBuffer( nullptr )
    , m_bufSize( rxBufferSize )
    , m_verbDelimiters( verbDelimiters )
{
}

Pipe::~Pipe()
{
    stop();
}

//////////////////////////////////////////////////
void Pipe::start( Cpl::Io::Input& inStream, Cpl::Io::Output& outStream ) noexcept
{
    m_frameBuffer = new (std::nothrow) char[m_bufSize + 1]; // add space for the null terminator
}

void Pipe::stop() noexcept
{
    if ( m_frameBuffer != nullptr )
    {
        delete[] m_frameBuffer;
        m_frameBuffer = nullptr;
    }
}

void Pipe::setStreams( Cpl::Io::Input& inStream, Cpl::Io::Output& outStream ) noexcept
{
    Cpl::System::Mutex::ScopeBlock lock( m_lock );
    m_framer.setOutput( outStream );
    m_deframer.setInput( inStream );
}

//////////////////////////////////////////////////
bool Pipe::sendCommand( const char* completeCommandText, size_t numBytes ) noexcept
{
    bool success = false;
    Cpl::System::Mutex::ScopeBlock lock( m_lock );

    // Fail the send if start() has not been called
    if ( m_frameBuffer != nullptr )
    {
        success = true;
        success &= m_framer.startFrame();
        success &= m_framer.output( completeCommandText, numBytes );
        success &= m_framer.endFrame();
    }
    
    return success;
}

void Pipe::poll() noexcept
{
}
