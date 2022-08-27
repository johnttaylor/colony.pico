#ifndef Driver_TPipe_Pipe_h_
#define Driver_TPipe_Pipe_h_
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

#include "Cpl/Container/Map.h"
#include "Driver/TPipe/Tx.h"
#include "Driver/TPipe/RxFrameHandlerApi.h"
#include "Cpl/Text/Frame/StreamDecoder.h"
#include "Cpl/Text/Frame/StreamEncoder.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/Io/Input.h"
#include "Cpl/Io/Output.h"

///
namespace Driver {
///
namespace TPipe {

/** This concrete class provides the implementation of TPipe.

    The TPipe assumes that it executes in the context of a single thread.
    All transmissions and reception of frames is done the TPipe's thread.
    It is APPLICATION's responsibility to provide thread safety for its Receive
    Frame Handlers.

    The TPipe dynamically allocates memory for its incoming frame buffer on
    start-up.  The memory is freed when the TPipe is shutdown.

    The TPipe also assumes that the runnable object for its thread is a
    Cpl::System::PeridiocScheduler.  The Periodic Scheduler semantics impose
    the following constraints on the Application (who is responsible for creating
    the Periodic Scheduler)

        - The application MUST call the TPipe's start() method in the 'beginThreadProcessing'
          function for the thread/PeriodicScheduler.

        - The application MUST call the TPipe's stop() method in the 'endThreadProcessing'
          function for the thread/PeriodicScheduler

        - The application MUST call the TPipe's poll() method in the 'idleProcessing'
          function for the thread/PeriodicScheduler
 */
class Pipe : public Tx
{
public:
    /** Constructor.

        @param rxFrameHdlrs     The set of received frame handlers.  Note: frame
                                handler's self register
        @param deframer         Frame decoder used to identify individual command
                                strings within the raw Input stream
        @param framer           Frame encoder used to encapsulate the output of
                                command in the Output stream.
        @param rxBufferSize     The size, in bytes, of the buffer used to buffer
                                incoming data.  The behavior of what happens if
                                the incoming data exceeds the buffer is defined
                                by the 'deframer' (typically this is to discard the
                                current frame and being looking for a new SOF)
        @param verbDelimiters   The delimiter characters used to separate the
                                command verb from the rest of commands tokens/data.
                                This string must stay in scope for of TPipe instance.
     */
    Pipe( Cpl::Container::Map<RxFrameHandlerApi>&   rxFrameHdlrs,
          Cpl::Text::Frame::StreamDecoder&          deframer,
          Cpl::Text::Frame::StreamEncoder&          framer,
          size_t                                    rxBufferSize,
          const char*                               verbDelimiters=" "
    );

    /// Destructor
    ~Pipe();

public:
    /** This method performs the in-thread initialization of the TPipe. It
        MUST be called in the 'beginThreadProcessing' for the Periodic Scheduler
        of which the TPipe executes in.

        Note: Until the setStreams() method is called, the TPipe will NOT transmit
              or accept incoming frames.
     */
    void start( Cpl::Io::Input& inStream, Cpl::Io::Output& outStream ) noexcept;

    /** This method performs the in-thread shutdown of the TPipe. It
        MUST be called in the 'endThreadProcessing' for the Periodic Scheduler
        of which the TPipe executes in.
     */
    void stop() noexcept;

    /** This method provides the TPipe CPU/Execution time.  It MUST be called
        in the 'idleFunction' for the Periodic Scheduler of which the TPipe
        executes in.
     */
    void poll() noexcept;

public:
    /// See Driver::TPipe::Tx
    bool sendCommand( const char* completeCommandText, size_t numBytes ) noexcept;

public:
    /** This method is to provide the TPipe with its input/output streams.  This
        method is thread safe and can be called from any thread
     */
    void setStreams( Cpl::Io::Input& inStream, Cpl::Io::Output& outStream ) noexcept;

protected:
    /// List of Frame handlers
    Cpl::Container::Map<RxFrameHandlerApi>& m_rxHandlers;

    /// Frame Decoder
    Cpl::Text::Frame::StreamDecoder&        m_deframer;
    
    /// Frame Encoder
    Cpl::Text::Frame::StreamEncoder&        m_framer;

    /// Frame buffer
    char*                                   m_frameBuffer;
    
    /// Lock for thread safety and atomic transmits
    Cpl::System::Mutex                      m_lock;

    /// Frame buffer size (not including the null terminator)
    size_t                                  m_bufSize;

    /// Delimiter(s) to find the end of the command verb
    const char*                             m_verbDelimiters;
};

};      // end namespaces
};
#endif  // end header latch