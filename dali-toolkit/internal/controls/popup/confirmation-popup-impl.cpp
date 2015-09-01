/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// CLASS HEADER
#include "confirmation-popup-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <cstring>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

/*
 * This struct is used to define all details required about a dynamically created signal.
 */
struct ControlDetailType
{
  const char* signalName;
  const char* controlName;
  const char* connectSignalPropertyName;
};

/* A table of all control details. These details are kept in one place for maintainability.
 *  Name of the signal     | Name of the control  | Name of the property which lets the
 *  the app-developer      | which will provide   | app developer choose which signal
 *  can connect to.        | the signal.          | within the control to connect to.    */
const ControlDetailType ControlDetails[] = {
  { "control-signal-ok",     "control-ok",          "connect-signal-ok-selected"     },
  { "control-signal-cancel", "control-cancel",      "connect-signal-cancel-selected" },
};
const unsigned int ControlDetailsCount = sizeof( ControlDetails ) / sizeof( ControlDetails[0] );

// To give sensible default behaviour to save the connect signal properties being set.
const char* const DEFAULT_CONNECT_SIGNAL_NAME = "clicked";

BaseHandle Create()
{
  return Toolkit::ConfirmationPopup::New();
}

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::ConfirmationPopup, Toolkit::Popup, Create )

DALI_PROPERTY_REGISTRATION( Toolkit, ConfirmationPopup, ControlDetails[0].connectSignalPropertyName, STRING, CONNECT_SIGNAL_OK_SELECTED     )
DALI_PROPERTY_REGISTRATION( Toolkit, ConfirmationPopup, ControlDetails[1].connectSignalPropertyName, STRING, CONNECT_SIGNAL_CANCEL_SELECTED )

// Note: We do not use the macros for signal registration as we do not want to redefine the signal name strings.
// We have predefined them for optimal signal name to control name lookup.
SignalConnectorType signalConnector1( typeRegistration, ControlDetails[0].signalName, &Toolkit::Internal::ConfirmationPopup::DoConnectSignal );
SignalConnectorType signalConnector2( typeRegistration, ControlDetails[1].signalName, &Toolkit::Internal::ConfirmationPopup::DoConnectSignal );

DALI_TYPE_REGISTRATION_END()

} // Unnamed namespace

Dali::Toolkit::ConfirmationPopup ConfirmationPopup::New()
{
  // Create the implementation, temporarily owned on stack.
  IntrusivePtr< ConfirmationPopup > internalConfirmationPopup = new ConfirmationPopup();

  // Pass ownership to CustomActor
  Dali::Toolkit::ConfirmationPopup confirmationPopup( *internalConfirmationPopup );

  // Second-phase initialisation of the implementation.
  // This can only be done after the CustomActor connection has been made...
  internalConfirmationPopup->Initialize();

  return confirmationPopup;
}

ConfirmationPopup::ConfirmationPopup()
: Toolkit::Internal::Popup()
{
  mControlSignals.reserve( MAXIMUM_NUMBER_OF_CONTROLS );
  mControlSignalNames[ Toolkit::ConfirmationPopup::CONTROL_OK ] = DEFAULT_CONNECT_SIGNAL_NAME;
  mControlSignalNames[ Toolkit::ConfirmationPopup::CONTROL_CANCEL ] = DEFAULT_CONNECT_SIGNAL_NAME;
}

ConfirmationPopup::~ConfirmationPopup()
{
  for( SignalContainerType::iterator i = mControlSignals.begin(); i != mControlSignals.end(); ++i )
  {
    delete ( i->second );
  }
  mControlSignals.clear();
}

void ConfirmationPopup::SetProperty( BaseObject* object, Property::Index propertyIndex, const Property::Value& value )
{
  Toolkit::ConfirmationPopup popup = Toolkit::ConfirmationPopup::DownCast( Dali::BaseHandle( object ) );

  if ( popup )
  {
    ConfirmationPopup& popupImpl( GetDerivedImplementation( popup ) );

    switch ( propertyIndex )
    {
      case Toolkit::ConfirmationPopup::Property::CONNECT_SIGNAL_OK_SELECTED:
      {
        popupImpl.SetControlSignalName( Toolkit::ConfirmationPopup::CONTROL_OK, value.Get< std::string >() );
        break;
      }
      case Toolkit::ConfirmationPopup::Property::CONNECT_SIGNAL_CANCEL_SELECTED:
      {
        popupImpl.SetControlSignalName( Toolkit::ConfirmationPopup::CONTROL_CANCEL, value.Get< std::string >() );
        break;
      }
    }
  }
}

Property::Value ConfirmationPopup::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Property::Value value;

  Toolkit::ConfirmationPopup popup = Toolkit::ConfirmationPopup::DownCast( Dali::BaseHandle( object ) );

  if ( popup )
  {
    ConfirmationPopup& popupImpl( GetDerivedImplementation( popup ) );

    switch ( propertyIndex )
    {
      case Toolkit::ConfirmationPopup::Property::CONNECT_SIGNAL_OK_SELECTED:
      {
        value = popupImpl.GetControlSignalName( Toolkit::ConfirmationPopup::CONTROL_OK );
        break;
      }
      case Toolkit::ConfirmationPopup::Property::CONNECT_SIGNAL_CANCEL_SELECTED:
      {
        value = popupImpl.GetControlSignalName( Toolkit::ConfirmationPopup::CONTROL_CANCEL );
        break;
      }
    }
  }

  return value;
}

void ConfirmationPopup::SetControlSignalName( const unsigned int controlNumber, const std::string& signalName )
{
  if( controlNumber < ControlDetailsCount )
  {
    mControlSignalNames[ controlNumber ] = signalName;
  }
}

std::string ConfirmationPopup::GetControlSignalName( unsigned int controlNumber ) const
{
  if( controlNumber < ControlDetailsCount )
  {
    return mControlSignalNames[ controlNumber ];
  }

  return "";
}

bool ConfirmationPopup::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );
  Toolkit::ConfirmationPopup popup = Toolkit::ConfirmationPopup::DownCast( handle );

  // Look up the requested signal, attempting to create it dynamically if it doesn't exist.
  SignalDelegate* signalDelegate = Dali::Toolkit::GetDerivedImplementation( popup ).GetControlSignal( signalName );
  if( signalDelegate )
  {
    // The signal delegate was created successfully, attempt to connect it to a callback if specified.
    // If none is specified, the creation is still successful as the signal delegate can connect at a later time.
    if( functor )
    {
      signalDelegate->Connect( tracker, functor );
    }
    return true;
  }

  // The signal could not be created.
  return false;
}

SignalDelegate* ConfirmationPopup::GetControlSignal( const std::string& signalName )
{
  // Check if the specified signal name already exists.
  SignalContainerType::iterator end = mControlSignals.end();
  for( SignalContainerType::iterator iter = mControlSignals.begin(); iter != end; ++iter )
  {
    // Find the first non-connected signal by matching signal name.
    if( ( signalName == iter->first ) && ( !iter->second->IsConnected() ) )
    {
      // The requested signal (delegate) already exists, just return it.
      return iter->second;
    }
  }

  // The signal doesn't exist, or it does but it's already connected to something else.
  // To make a new connection to an existing signal, we need a new delegate,
  // as delegates house a signal connection functor each.
  // Check the signal name is valid and if so create the signal dynamically.
  for( unsigned int i = 0; i < ControlDetailsCount; ++i )
  {
    if( 0 == strcmp( signalName.c_str(), ControlDetails[ i ].signalName ) )
    {
      // The signal name is valid, check the respective actor to connect to exists.
      Actor connectActor = Self().FindChildByName( ControlDetails[ i ].controlName );
      if( connectActor )
      {
        // The actor exists, set up a signal delegate that will allow the application developer
        // to connect the actor signal directly to their callback.
        // Note: We don't use the GetControlSignalName() here for speedup, as we know the array bound is capped.
        SignalDelegate* signalDelegate = new SignalDelegate( connectActor, mControlSignalNames[ i ] );

        // Store the delegate with the signal name so we know what signals have been dynamically created so far.
        mControlSignals.push_back( std::make_pair( signalName, signalDelegate ) );

        // Return the delegate to allow connection to the newly created signal.
        return signalDelegate;
      }

      // Signal name valid but could not connect to the control,
      return NULL;
    }
  }

  // Signal name was not found (invalid).
  return NULL;
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali
