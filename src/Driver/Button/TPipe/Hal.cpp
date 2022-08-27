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

#include "Driver/Button/Hal.h"
#include "Hal.h"
#include "Driver/TPipe/RxFrameHandler.h"
#include "Cpl/Text/Tokenizer/Basic.h"
#include "Cpl/Text/FString.h"
#include "Cpl/System/Mutex.h"
#include <memory.h>


class ButtonFrameHandler : public Driver::TPipe::RxFrameHandler
{
public:
    ///
    struct Button_T
    {
        Cpl::Text::FString<OPTION_DRIVER_BUTTON_HAL_TPIPE_SIZE_BUTTON_NAME> buttonName;
        bool                                                                pressed;
    };
    ///
    Button_T            m_buttons[OPTION_DRIVER_BUTTON_HAL_TPIPE_MAX_BUTTONS];
    Cpl::System::Mutex  m_lock;

    ///
    ButtonFrameHandler( Cpl::Container::Map<Driver::TPipe::RxFrameHandlerApi>& tpipeRxFrameHandlerList )
        : RxFrameHandler( tpipeRxFrameHandlerList, OPTION_DRIVER_BUTTON_HAL_TPIPE_COMMAND_VERB )
    {
    }

    ///
    void execute( char* decodedFrameText ) noexcept
    {
        Cpl::Text::Tokenizer::Basic tokens( decodedFrameText );

        // Must have at least 3 tokens - and individual buttons are always 2 tokens
        unsigned buttonIdx = 0;
        unsigned numTokens = tokens.numTokens() - 1;
        while( numTokens >= 2 )
        {
            m_buttons[buttonIdx].buttonName = tokens.getToken( 1 + buttonIdx * 2 );
            m_buttons[buttonIdx].pressed    = *(tokens.getToken( 1 + buttonIdx * 2 + 1 )) == 'D' ? true : false;
            buttonIdx++;
            numTokens -= 2;
        }
    }

    /// 
    bool getButtonState( const char* buttonName )
    {
        Cpl::System::Mutex::ScopeBlock lock( m_lock );
        for ( int i=0; i < OPTION_DRIVER_BUTTON_HAL_TPIPE_MAX_BUTTONS; i++ )
        {

            // Exit early if the end-of-list is found
            if ( m_buttons[i].buttonName.length() == 0 )
            {
                break;
            }

            if ( m_buttons[i].buttonName == buttonName )
            {
                return m_buttons[i].pressed;
            }
        }

        // If I get the button name was found in the List of buttons supplied by the TPipe
        return false;
    }
};


///////////////////////////////////////////////////
static ButtonFrameHandler* rxFrameHandler_;

void driverButtonHalTPipe_initialize( Cpl::Container::Map<Driver::TPipe::RxFrameHandlerApi>& tpipeRxFrameHandlerList )
{
    rxFrameHandler_ = new(std::nothrow) ButtonFrameHandler( tpipeRxFrameHandlerList );
}

bool driverButtonHalTPipe_getRawPressState( const char* buttonName )
{
    if ( rxFrameHandler_ )
    {
        return rxFrameHandler_->getButtonState( buttonName );
    }

    return false;
}