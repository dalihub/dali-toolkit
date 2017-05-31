/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include "control-data-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/debug.h>

#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <cstring>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_CONTROL_VISUALS");
#endif

/**
 *  Finds visual in given array, returning true if found along with the iterator for that visual as a out parameter
 */
bool FindVisual( Property::Index targetIndex, const RegisteredVisualContainer& visuals, RegisteredVisualContainer::Iterator& iter )
{
  for ( iter = visuals.Begin(); iter != visuals.End(); iter++ )
  {
    if ( (*iter)->index ==  targetIndex )
    {
      return true;
    }
  }
  return false;
}




/**
 * Performs actions as requested using the action name.
 * @param[in] object The object on which to perform the action.
 * @param[in] actionName The action to perform.
 * @param[in] attributes The attributes with which to perfrom this action.
 * @return true if action has been accepted by this control
 */
const char* ACTION_ACCESSIBILITY_ACTIVATED = "accessibilityActivated";
static bool DoAction( BaseObject* object, const std::string& actionName, const Property::Map& attributes )
{
  bool ret = false;

  if( object && ( 0 == strcmp( actionName.c_str(), ACTION_ACCESSIBILITY_ACTIVATED ) ) )
  {
    Toolkit::Control control = Toolkit::Control::DownCast( BaseHandle( object ) );
    if( control )
    {
      // if cast succeeds there is an implementation so no need to check
      ret = Internal::GetImplementation( control ).OnAccessibilityActivated();
    }
  }

  return ret;
}

/**
 * Connects a callback function with the object's signals.
 * @param[in] object The object providing the signal.
 * @param[in] tracker Used to disconnect the signal.
 * @param[in] signalName The signal to connect to.
 * @param[in] functor A newly allocated FunctorDelegate.
 * @return True if the signal was connected.
 * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
 */
const char* SIGNAL_KEY_EVENT = "keyEvent";
const char* SIGNAL_KEY_INPUT_FOCUS_GAINED = "keyInputFocusGained";
const char* SIGNAL_KEY_INPUT_FOCUS_LOST = "keyInputFocusLost";
const char* SIGNAL_TAPPED = "tapped";
const char* SIGNAL_PANNED = "panned";
const char* SIGNAL_PINCHED = "pinched";
const char* SIGNAL_LONG_PRESSED = "longPressed";
static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( false );
  Toolkit::Control control = Toolkit::Control::DownCast( handle );
  if ( control )
  {
    Internal::Control& controlImpl( Internal::GetImplementation( control ) );
    connected = true;

    if ( 0 == strcmp( signalName.c_str(), SIGNAL_KEY_EVENT ) )
    {
      controlImpl.KeyEventSignal().Connect( tracker, functor );
    }
    else if( 0 == strcmp( signalName.c_str(), SIGNAL_KEY_INPUT_FOCUS_GAINED ) )
    {
      controlImpl.KeyInputFocusGainedSignal().Connect( tracker, functor );
    }
    else if( 0 == strcmp( signalName.c_str(), SIGNAL_KEY_INPUT_FOCUS_LOST ) )
    {
      controlImpl.KeyInputFocusLostSignal().Connect( tracker, functor );
    }
    else if( 0 == strcmp( signalName.c_str(), SIGNAL_TAPPED ) )
    {
      controlImpl.EnableGestureDetection( Gesture::Tap );
      controlImpl.GetTapGestureDetector().DetectedSignal().Connect( tracker, functor );
    }
    else if( 0 == strcmp( signalName.c_str(), SIGNAL_PANNED ) )
    {
      controlImpl.EnableGestureDetection( Gesture::Pan );
      controlImpl.GetPanGestureDetector().DetectedSignal().Connect( tracker, functor );
    }
    else if( 0 == strcmp( signalName.c_str(), SIGNAL_PINCHED ) )
    {
      controlImpl.EnableGestureDetection( Gesture::Pinch );
      controlImpl.GetPinchGestureDetector().DetectedSignal().Connect( tracker, functor );
    }
    else if( 0 == strcmp( signalName.c_str(), SIGNAL_LONG_PRESSED ) )
    {
      controlImpl.EnableGestureDetection( Gesture::LongPress );
      controlImpl.GetLongPressGestureDetector().DetectedSignal().Connect( tracker, functor );
    }
  }
  return connected;
}

/**
 * Creates control through type registry
 */
BaseHandle Create()
{
  return Internal::Control::New();
}
// Setup signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Control, CustomActor, Create );

// Note: Properties are registered separately below.

SignalConnectorType registerSignal1( typeRegistration, SIGNAL_KEY_EVENT, &DoConnectSignal );
SignalConnectorType registerSignal2( typeRegistration, SIGNAL_KEY_INPUT_FOCUS_GAINED, &DoConnectSignal );
SignalConnectorType registerSignal3( typeRegistration, SIGNAL_KEY_INPUT_FOCUS_LOST, &DoConnectSignal );
SignalConnectorType registerSignal4( typeRegistration, SIGNAL_TAPPED, &DoConnectSignal );
SignalConnectorType registerSignal5( typeRegistration, SIGNAL_PANNED, &DoConnectSignal );
SignalConnectorType registerSignal6( typeRegistration, SIGNAL_PINCHED, &DoConnectSignal );
SignalConnectorType registerSignal7( typeRegistration, SIGNAL_LONG_PRESSED, &DoConnectSignal );

TypeAction registerAction( typeRegistration, ACTION_ACCESSIBILITY_ACTIVATED, &DoAction );

DALI_TYPE_REGISTRATION_END()

} // unnamed namespace


// Properties registered without macro to use specific member variables.
const PropertyRegistration Control::Impl::PROPERTY_1( typeRegistration, "styleName",              Toolkit::Control::Property::STYLE_NAME,                   Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty );
const PropertyRegistration Control::Impl::PROPERTY_2( typeRegistration, "backgroundColor",        Toolkit::Control::Property::BACKGROUND_COLOR,             Property::VECTOR4, &Control::Impl::SetProperty, &Control::Impl::GetProperty );
const PropertyRegistration Control::Impl::PROPERTY_3( typeRegistration, "backgroundImage",        Toolkit::Control::Property::BACKGROUND_IMAGE,             Property::MAP,     &Control::Impl::SetProperty, &Control::Impl::GetProperty );
const PropertyRegistration Control::Impl::PROPERTY_4( typeRegistration, "keyInputFocus",          Toolkit::Control::Property::KEY_INPUT_FOCUS,              Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty );
const PropertyRegistration Control::Impl::PROPERTY_5( typeRegistration, "background",             Toolkit::Control::Property::BACKGROUND,                   Property::MAP,     &Control::Impl::SetProperty, &Control::Impl::GetProperty );



Control::Impl::Impl( Control& controlImpl )
: mControlImpl( controlImpl ),
  mStyleName(""),
  mBackgroundColor(Color::TRANSPARENT),
  mStartingPinchScale( NULL ),
  mKeyEventSignal(),
  mPinchGestureDetector(),
  mPanGestureDetector(),
  mTapGestureDetector(),
  mLongPressGestureDetector(),
  mFlags( Control::ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) ),
  mIsKeyboardNavigationSupported( false ),
  mIsKeyboardFocusGroup( false )
{

}

Control::Impl::~Impl()
{
  // All gesture detectors will be destroyed so no need to disconnect.
  delete mStartingPinchScale;
}

Control::Impl& Control::Impl::Get( Internal::Control& internalControl )
{
  return *internalControl.mImpl;
}

const Control::Impl& Control::Impl::Get( const Internal::Control& internalControl )
{
  return *internalControl.mImpl;
}

// Gesture Detection Methods
void Control::Impl::PinchDetected(Actor actor, const PinchGesture& pinch)
{
  mControlImpl.OnPinch(pinch);
}

void Control::Impl::PanDetected(Actor actor, const PanGesture& pan)
{
  mControlImpl.OnPan(pan);
}

void Control::Impl::TapDetected(Actor actor, const TapGesture& tap)
{
  mControlImpl.OnTap(tap);
}

void Control::Impl::LongPressDetected(Actor actor, const LongPressGesture& longPress)
{
  mControlImpl.OnLongPress(longPress);
}

// Called by a Visual when it's resource is ready
void Control::Impl::ResourceReady( Visual::Base& object)
{

  // go through and check if all the visuals are ready, if they are emit a signal
  for ( RegisteredVisualContainer::ConstIterator visualIter = mVisuals.Begin();
        visualIter != mVisuals.End(); ++visualIter )
  {
    const Toolkit::Visual::Base visual = (*visualIter)->visual;
    const Internal::Visual::Base& visualImpl = Toolkit::GetImplementation( visual );

    // one of the visuals is not ready
    if( !visualImpl.IsResourceReady() )
    {
      return;
    }
  }

  // all the visuals are ready
  Dali::Toolkit::Control handle( mControlImpl.GetOwner() );
  mResourceReadySignal.Emit( handle );

}

bool Control::Impl::IsResourceReady() const
{
  // go through and check all the visuals are ready
  for ( RegisteredVisualContainer::ConstIterator visualIter = mVisuals.Begin();
         visualIter != mVisuals.End(); ++visualIter )
   {
     const Toolkit::Visual::Base visual = (*visualIter)->visual;
     const Internal::Visual::Base& visualImpl = Toolkit::GetImplementation( visual );

     // one of the visuals is not ready
     if( !visualImpl.IsResourceReady()  )
     {
       return false;
     }
   }
  return true;
}

void Control::Impl::RegisterVisual( Property::Index index, Toolkit::Visual::Base& visual )
{
  RegisterVisual( index, visual, true );
}

void Control::Impl::RegisterVisual( Property::Index index, Toolkit::Visual::Base& visual, bool enabled )
{
  bool visualReplaced ( false );
  Actor self = mControlImpl.Self();

  if( !mVisuals.Empty() )
  {
    RegisteredVisualContainer::Iterator iter;
    // Check if visual (index) is already registered.  Replace if so.
    if ( FindVisual( index, mVisuals, iter ) )
    {
      if( (*iter)->visual && self.OnStage() )
      {
        Toolkit::GetImplementation((*iter)->visual).SetOffStage( self );
      }

      StopObservingVisual( (*iter)->visual );
      StartObservingVisual( visual );

      (*iter)->visual = visual;
      visualReplaced = true;
    }
  }

  if( !visualReplaced ) // New registration entry
  {
    mVisuals.PushBack( new RegisteredVisual( index, visual, enabled ) );

    // monitor when the visuals resources are ready
    StartObservingVisual( visual );

  }

  if( visual && self.OnStage() && enabled )
  {
    Toolkit::GetImplementation(visual).SetOnStage( self );
  }

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Control::RegisterVisual() Registered %s(%d), enabled:%s\n",  visual.GetName().c_str(), index, enabled?"T":"F" );
}

void Control::Impl::UnregisterVisual( Property::Index index )
{
   RegisteredVisualContainer::Iterator iter;
   if ( FindVisual( index, mVisuals, iter ) )
   {
     // stop observing visual
     StopObservingVisual( (*iter)->visual );

     Actor self( mControlImpl.Self() );
     Toolkit::GetImplementation((*iter)->visual).SetOffStage( self );
     (*iter)->visual.Reset();
     mVisuals.Erase( iter );
   }
}

Toolkit::Visual::Base Control::Impl::GetVisual( Property::Index index ) const
{
  RegisteredVisualContainer::Iterator iter;
  if ( FindVisual( index, mVisuals, iter ) )
  {
    return (*iter)->visual;
  }

  return Toolkit::Visual::Base();
}

void Control::Impl::EnableVisual( Property::Index index, bool enable )
{
  RegisteredVisualContainer::Iterator iter;
  if ( FindVisual( index, mVisuals, iter ) )
  {
    if (  (*iter)->enabled == enable )
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Control::EnableVisual Visual %s(%d) already %s\n", (*iter)->visual.GetName().c_str(), index, enable?"enabled":"disabled");
      return;
    }

    (*iter)->enabled = enable;
    Actor parentActor = mControlImpl.Self();
    if ( mControlImpl.Self().OnStage() ) // If control not on Stage then Visual will be added when StageConnection is called.
    {
      if ( enable )
      {
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Control::EnableVisual Setting %s(%d) on stage \n", (*iter)->visual.GetName().c_str(), index );
        Toolkit::GetImplementation((*iter)->visual).SetOnStage( parentActor );
      }
      else
      {
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Control::EnableVisual Setting %s(%d) off stage \n", (*iter)->visual.GetName().c_str(), index );
        Toolkit::GetImplementation((*iter)->visual).SetOffStage( parentActor );  // No need to call if control not staged.
      }
    }
  }
}

bool Control::Impl::IsVisualEnabled( Property::Index index ) const
{
  RegisteredVisualContainer::Iterator iter;
  if ( FindVisual( index, mVisuals, iter ) )
  {
    return (*iter)->enabled;
  }
  return false;
}

void Control::Impl::StopObservingVisual( Toolkit::Visual::Base& visual )
{
  Internal::Visual::Base& visualImpl = Toolkit::GetImplementation( visual );

  // Stop observing the visual
  visualImpl.RemoveResourceObserver( *this );
}

void Control::Impl::StartObservingVisual( Toolkit::Visual::Base& visual)
{
  Internal::Visual::Base& visualImpl = Toolkit::GetImplementation( visual );

  // start observing the visual for resource ready
  visualImpl.AddResourceObserver( *this );
}

void Control::Impl::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::Control control = Toolkit::Control::DownCast( BaseHandle( object ) );

  if ( control )
  {
    Control& controlImpl( GetImplementation( control ) );

    switch ( index )
    {
      case Toolkit::Control::Property::STYLE_NAME:
      {
        controlImpl.SetStyleName( value.Get< std::string >() );
        break;
      }

      case Toolkit::Control::Property::BACKGROUND_COLOR:
      {
        DALI_LOG_WARNING( "BACKGROUND_COLOR property is deprecated. Use BACKGROUND property instead\n" );
        controlImpl.SetBackgroundColor( value.Get< Vector4 >() );
        break;
      }

      case Toolkit::Control::Property::BACKGROUND_IMAGE:
      {
        DALI_LOG_WARNING( "BACKGROUND_IMAGE property is deprecated. Use BACKGROUND property instead\n" );
        Image image = Scripting::NewImage( value );
        if ( image )
        {
          controlImpl.SetBackgroundImage( image );
        }
        else
        {
          // An empty image means the background is no longer required
          controlImpl.ClearBackground();
        }
        break;
      }

      case Toolkit::Control::Property::KEY_INPUT_FOCUS:
      {
        if ( value.Get< bool >() )
        {
          controlImpl.SetKeyInputFocus();
        }
        else
        {
          controlImpl.ClearKeyInputFocus();
        }
        break;
      }

      case Toolkit::Control::Property::BACKGROUND:
      {
        std::string url;
        Vector4 color;
        const Property::Map* map = value.GetMap();
        if( map && !map->Empty() )
        {
          controlImpl.SetBackground( *map );
        }
        else if( value.Get( url ) )
        {
          // don't know the size to load
          Toolkit::Visual::Base visual = Toolkit::VisualFactory::Get().CreateVisual( url, ImageDimensions() );
          if( visual )
          {
            controlImpl.mImpl->RegisterVisual( Toolkit::Control::Property::BACKGROUND, visual );
            visual.SetDepthIndex( DepthIndex::BACKGROUND );
          }
        }
        else if( value.Get( color ) )
        {
          controlImpl.SetBackgroundColor(color);
        }
        else
        {
          // The background is an empty property map, so we should clear the background
          controlImpl.ClearBackground();
        }
        break;
      }
    }
  }
}

Property::Value Control::Impl::GetProperty( BaseObject* object, Property::Index index )
{
  Property::Value value;

  Toolkit::Control control = Toolkit::Control::DownCast( BaseHandle( object ) );

  if ( control )
  {
    Control& controlImpl( GetImplementation( control ) );

    switch ( index )
    {
      case Toolkit::Control::Property::STYLE_NAME:
      {
        value = controlImpl.GetStyleName();
        break;
      }
      case Toolkit::Control::Property::BACKGROUND_COLOR:
      {
        DALI_LOG_WARNING( "BACKGROUND_COLOR property is deprecated. Use BACKGROUND property instead\n" );
        value = controlImpl.GetBackgroundColor();
        break;
      }

      case Toolkit::Control::Property::BACKGROUND_IMAGE:
      {
        DALI_LOG_WARNING( "BACKGROUND_IMAGE property is deprecated. Use BACKGROUND property instead\n" );
        Property::Map map;
        Toolkit::Visual::Base visual = controlImpl.mImpl->GetVisual( Toolkit::Control::Property::BACKGROUND );
        if( visual )
        {
          visual.CreatePropertyMap( map );
        }
        value = map;
        break;
      }

      case Toolkit::Control::Property::KEY_INPUT_FOCUS:
      {
        value = controlImpl.HasKeyInputFocus();
        break;
      }

      case Toolkit::Control::Property::BACKGROUND:
      {
        Property::Map map;
        Toolkit::Visual::Base visual = controlImpl.mImpl->GetVisual( Toolkit::Control::Property::BACKGROUND );
        if( visual )
        {
          visual.CreatePropertyMap( map );
        }

        value = map;
        break;
      }

    }
  }

  return value;
}

void Control::Impl::RemoveVisual( RegisteredVisualContainer& visuals, const std::string& visualName )
{
  Actor self( mControlImpl.Self() );

  for ( RegisteredVisualContainer::Iterator visualIter = visuals.Begin();
        visualIter != visuals.End(); ++visualIter )
  {
    Toolkit::Visual::Base visual = (*visualIter)->visual;
    if( visual && visual.GetName() == visualName )
    {
      Toolkit::GetImplementation(visual).SetOffStage( self );
      (*visualIter)->visual.Reset();
      visuals.Erase( visualIter );
      break;
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
