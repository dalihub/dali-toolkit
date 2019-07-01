/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/controls/text-controls/autofill-container-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/controls/text-controls/text-field-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

#if defined ( DEBUG_ENABLED )
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_AUTOFILL_CONTAINER");
#endif

// Type registration
BaseHandle Create()
{
  return Toolkit::AutofillContainer::New( "" );
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::AutofillContainer, Dali::BaseHandle, Create );

DALI_SIGNAL_REGISTRATION( Toolkit, AutofillContainer, "serviceShown",   SIGNAL_SERVICE_SHOWN )
DALI_SIGNAL_REGISTRATION( Toolkit, AutofillContainer, "listShown",      SIGNAL_LIST_SHOWN )

DALI_TYPE_REGISTRATION_END()

} // namespace

Toolkit::AutofillContainer AutofillContainer::New( const std::string& name )
{
  // Create the implementation, temporarily owned by this handle on stack
  Internal::AutofillContainer* impl = new AutofillContainer();

  // Pass ownership to CustomActor handle
  Toolkit::AutofillContainer handle( impl );
  impl->Initialize( name );

  return handle;
}

void AutofillContainer::Initialize( const std::string& name )
{
  mAutofillManager = Dali::AutofillManager::Get();
  mAutofillGroup = mAutofillManager.CreateAutofillGroup( name );

  // If the authentication is needed, AuthenticationReceivedSignal would be emitted.
  mAutofillManager.AuthenticationReceivedSignal().Connect( mSlotDelegate, &AutofillContainer::OnAutofillAuthReceived );

  // If the data to be filled is present, FillResponseReceivedSignal would be emitted.
  mAutofillManager.FillResponseReceivedSignal().Connect( mSlotDelegate, &AutofillContainer::OnDataFillReceived );

  // If the values to be filled are multiple, ListEventSignal would be emitted.
  mAutofillManager.ListEventSignal().Connect( mSlotDelegate, &AutofillContainer::OnListReceived );
}

void AutofillContainer::AddAutofillItem( Dali::Toolkit::Control control, Dali::Property::Index propertyIndex, const std::string& id, const std::string& label, Dali::AutofillItem::Hint hint, bool isSensitive )
{
  if( control )
  {
    Dali::AutofillItem item = mAutofillManager.CreateAutofillItem( id, label, static_cast<Dali::AutofillItem::Hint>(hint), isSensitive );

    Internal::Control& controlImpl = GetImplementation( control );
    Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( controlImpl );
    controlDataImpl.SetAutofillContainer( this );
    controlDataImpl.SetAutofillEnabled( true );
    controlDataImpl.SetAutofillItemHandle( item ); // TODO : This instance would be here as... std::map or pair

    mAutofillGroup.AddAutofillItem( item );

    mPropertyIndex = propertyIndex;

    // TODO : Remove the mControlItemList below and replace it.
    mControlList.push_back( std::pair< Dali::Toolkit::Control, Dali::AutofillItem >( control, item ) );

    // Push back a Control to the list
    mControlItemList.push_back( control );
  }
}

void AutofillContainer::RemoveAutofillItem( Dali::Toolkit::Control control )
{
  if( control )
  {
    Internal::Control& controlImpl = GetImplementation( control );
    Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( controlImpl );
    controlDataImpl.SetAutofillEnabled( false );
    // TODO : Put the control out the list
  }
}

Dali::AutofillGroup AutofillContainer::GetAutofillGroup()
{
  return mAutofillGroup;
}

void AutofillContainer::SetFocusedControl( Toolkit::Control focused )
{
  mCurrentFocused = focused;
}

Toolkit::Control AutofillContainer::GetFocusedControl()
{
  return mCurrentFocused;
}

void AutofillContainer::SaveAutofillData()
{
  for( std::vector<Dali::Toolkit::Control>::iterator iter = mControlItemList.begin(), endIter = mControlItemList.end(); iter != endIter; ++iter )
  {
    std::string controlValue = (*iter).GetProperty<std::string>( mPropertyIndex );
    Internal::Control& controlImpl = GetImplementation( *iter );
    Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( controlImpl );
    controlDataImpl.GetAutofillItemHandle().SetSaveValue( controlValue );
  }

  // Sends request to save the current autofill data
  mAutofillGroup.SaveAutofillData();
  mAutofillManager.SaveAutofillData( mAutofillGroup );
}

void AutofillContainer::RequestFillData()
{
  // Sends fill request to fill out the data.
  mAutofillGroup.SendFillRequest();
}

void AutofillContainer::SetAutofillServiceName( const std::string& serviceName )
{
  mAutofillServiceName = serviceName;
}

const std::string& AutofillContainer::GetAutofillServiceName() const
{
  return mAutofillServiceName;
}

void AutofillContainer::SetAutofillServiceMessage( const std::string& serviceMessage )
{
  mAutofillServiceMessage = serviceMessage;
}

const std::string& AutofillContainer::GetAutofillServiceMessage() const
{
  return mAutofillServiceMessage;
}

void AutofillContainer::SetAutofillServiceImagePath( const std::string& serviceImagePath )
{
  mAutofillServiceImagePath = serviceImagePath;
}

const std::string& AutofillContainer::GetAutofillServiceImagePath() const
{
  return mAutofillServiceImagePath;
}

unsigned int AutofillContainer::GetListCount()
{
  Toolkit::Control control = mCurrentFocused;
  Internal::Control& controlImpl = GetImplementation( control );
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( controlImpl );
  return controlDataImpl.GetAutofillItemHandle().GetFillValueCount();
}

const std::string& AutofillContainer::GetListItem( unsigned int index ) const
{
  Toolkit::Control control = mCurrentFocused;
  Internal::Control& controlImpl = GetImplementation( control );
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( controlImpl );
  return controlDataImpl.GetAutofillItemHandle().GetPresentationText( index );
}

void AutofillContainer::SetSelectedItem( const std::string& selected )
{
  Toolkit::Control control = mCurrentFocused;
  Internal::Control& controlImpl = GetImplementation( control );
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( controlImpl );
  unsigned int count = controlDataImpl.GetAutofillItemHandle().GetFillValueCount();
  unsigned int index = 0;
  for( index = 0; index < count; index++ )
  {
    std::string presentationText =  controlDataImpl.GetAutofillItemHandle().GetPresentationText( index );
    if( 0 == strcmp( presentationText.c_str(), selected.c_str() ) )
    {
      break;
    }
  }

  for( std::vector<Dali::Toolkit::Control>::iterator iter = mControlItemList.begin(), endIter = mControlItemList.end(); iter != endIter; ++iter )
  {
    Internal::Control& controlImpl = GetImplementation( *iter );
    Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( controlImpl );
    std::string fillValue = controlDataImpl.GetAutofillItemHandle().GetFillValue( index );
    (*iter).SetProperty( Toolkit::TextField::Property::TEXT, fillValue );
  }
}

//
void AutofillContainer::OnAutofillAuthReceived()
{
  SetAutofillServiceName( mAutofillManager.GetAuthenticationServiceName() );
  SetAutofillServiceMessage( mAutofillManager.GetAuthenticationServiceMessage() );
  SetAutofillServiceImagePath( mAutofillManager.GetAuthenticationServiceImagePath() );

  Dali::Toolkit::AutofillContainer handle( this );
  mAuthenticationEventSignal.Emit( handle );

  DALI_LOG_ERROR(" [ Emit DALi signal to receive the auth information ] \n");

}

void AutofillContainer::OnDataFillReceived( Dali::AutofillItem item )
{
  for( std::vector<Dali::Toolkit::Control>::iterator iter = mControlItemList.begin(), endIter = mControlItemList.end(); iter != endIter; ++iter )
  {
    Internal::Control& controlImpl = GetImplementation( *iter );
    Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( controlImpl );

    if( 0 == strcmp( controlDataImpl.GetAutofillItemHandle().GetId().c_str(), item.GetId().c_str() ) )
    {
      // TODO : Is the index for fill data always 0 in this case?
      std::string itemText = controlDataImpl.GetAutofillItemHandle().GetFillValue( 0 );
      (*iter).SetProperty( Toolkit::TextField::Property::TEXT, itemText );
    }
  }

}

void AutofillContainer::OnListReceived()
{
  mListEventSignal.Emit( mCurrentFocused );

}


AutofillContainer::AutofillContainer()
: mAutofillManager(),
  mAutofillGroup(),
  mControlItemList(),
  mControlList(),
  mSlotDelegate( this ),
  mPropertyIndex( Property::INVALID_INDEX ),
  mAutofillServiceName( "" ),
  mAutofillServiceMessage( "" ),
  mAutofillServiceImagePath( "" ),
  mCurrentFocused()
{
}

AutofillContainer::~AutofillContainer()
{
  mAutofillManager.AuthenticationReceivedSignal().Disconnect( mSlotDelegate, &AutofillContainer::OnAutofillAuthReceived );
  mAutofillManager.FillResponseReceivedSignal().Disconnect( mSlotDelegate, &AutofillContainer::OnDataFillReceived );
  mAutofillManager.ListEventSignal().Disconnect( mSlotDelegate, &AutofillContainer::OnListReceived );
}

// Signals
Toolkit::AutofillContainer::AuthenticationSignalType& AutofillContainer::AutofillServiceShownSignal()
{
  return mAuthenticationEventSignal;
}

Toolkit::AutofillContainer::ListShownSignalType& AutofillContainer::AutofillListShownSignal()
{
  return mListEventSignal;
}

bool AutofillContainer::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::AutofillContainer container = Toolkit::AutofillContainer::DownCast( handle );

  if( container )
  {
    if( 0 == signalName.compare( SIGNAL_SERVICE_SHOWN ) )
    {
      container.AutofillServiceShownSignal().Connect( tracker, functor );
    }
    else if( 0 == signalName.compare( SIGNAL_LIST_SHOWN ) )
    {
      container.AutofillListShownSignal().Connect( tracker, functor );
    }
    else
    {
      // signalName does not match any signal
      connected = false;
    }
  }

  return connected;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
