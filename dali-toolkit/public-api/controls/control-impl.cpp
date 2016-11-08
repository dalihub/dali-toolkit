/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/control-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp
#include <limits>
#include <stack>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/size-negotiation/relayout-container.h>
#include <dali/devel-api/common/owner-container.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/styling/style-manager.h>
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/visual-factory/devel-visual-properties.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>
#include <dali-toolkit/internal/visuals/color/color-visual.h>
#include <dali-toolkit/internal/visuals/transition-data-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New( Debug::General, false, "LOG_CONTROL_VISUALS");
#endif

/**
 * Struct used to store Visual within the control, index is a unique key for each visual.
 */
struct RegisteredVisual
{
  Property::Index index;
  Toolkit::Visual::Base visual;
  bool enabled;

  RegisteredVisual( Property::Index aIndex, Toolkit::Visual::Base &aVisual, bool aEnabled) :
                   index(aIndex), visual(aVisual), enabled(aEnabled) {}
};

struct HandleIndex
{
  Handle handle; ///< a handle to the target object
  Property::Index index; ///< The index of a property provided by the referenced object

  HandleIndex( )
  : handle(),
    index( Property::INVALID_INDEX )
  {
  }

  HandleIndex( Handle& handle, Property::Index index )
  : handle( handle ),
    index( index )
  {
  }
};


typedef Dali::OwnerContainer< RegisteredVisual* > RegisteredVisualContainer;

/**
 *  Finds visual in given array, returning true if found along with the iterator for that visual as a out parameter
 */
bool FindVisual( Property::Index targetIndex, RegisteredVisualContainer& visuals, RegisteredVisualContainer::Iterator& iter )
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

HandleIndex GetVisualProperty(
  Internal::Control& controlImpl,
  RegisteredVisualContainer& visuals,
  const std::string& visualName,
  Property::Key propertyKey )
{
#if defined(DEBUG_ENABLED)
  std::ostringstream oss;
  oss << "Control::GetHandleIndex(" << visualName << ", " << propertyKey << ")" << std::endl;
  DALI_LOG_INFO( gLogFilter, Debug::General, oss.str().c_str() );
#endif

  // Find visualName in the control
  RegisteredVisualContainer::Iterator iter;
  for ( iter = visuals.Begin(); iter != visuals.End(); iter++ )
  {
    if ( (*iter)->visual.GetName() == visualName )
    {
      break;
    }
  }

  // Does it's renderer have an associated property?
  if( iter != visuals.End() )
  {
    Actor self = controlImpl.Self();
    Property::Index index = self.GetPropertyIndex( propertyKey );
    if( index != Property::INVALID_INDEX )
    {
      // It's an actor property:
      return HandleIndex( self, index );
    }
    else
    {
      // Check if it is a renderer property:
      if( self.GetRendererCount() > 0 )
      {
        // @todo Need to use correct renderer index
        Renderer renderer = self.GetRendererAt(0);
        Property::Index index = renderer.GetPropertyIndex( propertyKey );
        if( index != Property::INVALID_INDEX )
        {
          // It's a renderer property:
          return HandleIndex( renderer, index );
        }
      }
      else
      {
        std::ostringstream oss;
        oss << propertyKey;
        DALI_LOG_WARNING( "Control::GetHandleIndex(%s, %s) No renderers\n", visualName.c_str(), oss.str().c_str() );
      }
    }
  }
  Handle handle;
  return HandleIndex( handle, Property::INVALID_INDEX );
}


/**
 * Creates control through type registry
 */
BaseHandle Create()
{
  return Internal::Control::New();
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

namespace Internal
{

class Control::Impl : public ConnectionTracker
{
public:

  // Construction & Destruction
  Impl(Control& controlImpl)
  : mControlImpl( controlImpl ),
    mStyleName(""),
    mBackgroundVisual(),
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

  ~Impl()
  {
    // All gesture detectors will be destroyed so no need to disconnect.
    delete mStartingPinchScale;
  }

  // Gesture Detection Methods

  void PinchDetected(Actor actor, const PinchGesture& pinch)
  {
    mControlImpl.OnPinch(pinch);
  }

  void PanDetected(Actor actor, const PanGesture& pan)
  {
    mControlImpl.OnPan(pan);
  }

  void TapDetected(Actor actor, const TapGesture& tap)
  {
    mControlImpl.OnTap(tap);
  }

  void LongPressDetected(Actor actor, const LongPressGesture& longPress)
  {
    mControlImpl.OnLongPress(longPress);
  }

  // Properties

  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
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
            // An empty map means the background is no longer required
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
          const Property::Map* map = value.GetMap();
          if( map )
          {
            controlImpl.SetBackground( *map );
          }
          else
          {
            // The background is not a property map, so we should clear the background
            controlImpl.ClearBackground();
          }
          break;
        }
      }
    }
  }

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty( BaseObject* object, Property::Index index )
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
          if( controlImpl.mImpl->mBackgroundVisual )
          {
            controlImpl.mImpl->mBackgroundVisual.CreatePropertyMap( map );
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
          if( controlImpl.mImpl->mBackgroundVisual )
          {
            (controlImpl.mImpl->mBackgroundVisual).CreatePropertyMap( map );
          }

          value = map;
          break;
        }

      }
    }

    return value;
  }

  // Data

  Control& mControlImpl;
  RegisteredVisualContainer mVisuals; // Stores visuals needed by the control, non trivial type so std::vector used.
  std::string mStyleName;
  Toolkit::Visual::Base mBackgroundVisual;   ///< The visual to render the background
  Vector4 mBackgroundColor;                       ///< The color of the background visual
  Vector3* mStartingPinchScale;      ///< The scale when a pinch gesture starts, TODO: consider removing this
  Toolkit::Control::KeyEventSignalType mKeyEventSignal;
  Toolkit::Control::KeyInputFocusSignalType mKeyInputFocusGainedSignal;
  Toolkit::Control::KeyInputFocusSignalType mKeyInputFocusLostSignal;

  // Gesture Detection
  PinchGestureDetector mPinchGestureDetector;
  PanGestureDetector mPanGestureDetector;
  TapGestureDetector mTapGestureDetector;
  LongPressGestureDetector mLongPressGestureDetector;

  ControlBehaviour mFlags : CONTROL_BEHAVIOUR_FLAG_COUNT;    ///< Flags passed in from constructor.
  bool mIsKeyboardNavigationSupported :1;  ///< Stores whether keyboard navigation is supported by the control.
  bool mIsKeyboardFocusGroup :1;           ///< Stores whether the control is a focus group.

  // Properties - these need to be members of Internal::Control::Impl as they need to function within this class.
  static const PropertyRegistration PROPERTY_1;
  static const PropertyRegistration PROPERTY_2;
  static const PropertyRegistration PROPERTY_3;
  static const PropertyRegistration PROPERTY_4;
  static const PropertyRegistration PROPERTY_5;
};

// Properties registered without macro to use specific member variables.
const PropertyRegistration Control::Impl::PROPERTY_1( typeRegistration, "styleName",       Toolkit::Control::Property::STYLE_NAME,       Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty );
const PropertyRegistration Control::Impl::PROPERTY_2( typeRegistration, "backgroundColor", Toolkit::Control::Property::BACKGROUND_COLOR, Property::VECTOR4, &Control::Impl::SetProperty, &Control::Impl::GetProperty );
const PropertyRegistration Control::Impl::PROPERTY_3( typeRegistration, "backgroundImage", Toolkit::Control::Property::BACKGROUND_IMAGE, Property::MAP,     &Control::Impl::SetProperty, &Control::Impl::GetProperty );
const PropertyRegistration Control::Impl::PROPERTY_4( typeRegistration, "keyInputFocus",   Toolkit::Control::Property::KEY_INPUT_FOCUS,  Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty );
const PropertyRegistration Control::Impl::PROPERTY_5( typeRegistration, "background",      Toolkit::Control::Property::BACKGROUND,       Property::MAP,     &Control::Impl::SetProperty, &Control::Impl::GetProperty );

Toolkit::Control Control::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<Control> controlImpl = new Control( ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) );

  // Pass ownership to handle
  Toolkit::Control handle( *controlImpl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  controlImpl->Initialize();

  return handle;
}

Control::~Control()
{
  delete mImpl;
}

void Control::SetStyleName( const std::string& styleName )
{
  if( styleName != mImpl->mStyleName )
  {
    mImpl->mStyleName = styleName;

    // Apply new style, if stylemanager is available
    Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();
    if( styleManager )
    {
      GetImpl( styleManager ).ApplyThemeStyle( Toolkit::Control( GetOwner() ) );
    }
  }
}

const std::string& Control::GetStyleName() const
{
  return mImpl->mStyleName;
}

void Control::SetBackgroundColor( const Vector4& color )
{
  mImpl->mBackgroundColor = color;
  Property::Map map;
  map[ Toolkit::VisualProperty::TYPE ] = Toolkit::Visual::COLOR;
  map[ Toolkit::ColorVisual::Property::MIX_COLOR ] = color;
  mImpl->mBackgroundVisual = Toolkit::VisualFactory::Get().CreateVisual( map );
  RegisterVisual( Toolkit::Control::Property::BACKGROUND, mImpl->mBackgroundVisual );
  if( mImpl->mBackgroundVisual )
  {
    mImpl->mBackgroundVisual.SetDepthIndex( DepthIndex::BACKGROUND );
  }
}

Vector4 Control::GetBackgroundColor() const
{
  return mImpl->mBackgroundColor;
}

void Control::SetBackground( const Property::Map& map )
{
  mImpl->mBackgroundVisual = Toolkit::VisualFactory::Get().CreateVisual( map );
  RegisterVisual( Toolkit::Control::Property::BACKGROUND, mImpl->mBackgroundVisual );
  if( mImpl->mBackgroundVisual )
  {
    mImpl->mBackgroundVisual.SetDepthIndex( DepthIndex::BACKGROUND );
  }
}

void Control::SetBackgroundImage( Image image )
{
  mImpl->mBackgroundVisual = Toolkit::VisualFactory::Get().CreateVisual( image );
  RegisterVisual( Toolkit::Control::Property::BACKGROUND, mImpl->mBackgroundVisual );
  if( mImpl->mBackgroundVisual )
  {
    mImpl->mBackgroundVisual.SetDepthIndex( DepthIndex::BACKGROUND );
  }
}

void Control::ClearBackground()
{
  Actor self( Self() );
  mImpl->mBackgroundVisual.RemoveAndReset( self );
  mImpl->mBackgroundColor = Color::TRANSPARENT;
}

void Control::EnableGestureDetection(Gesture::Type type)
{
  if ( (type & Gesture::Pinch) && !mImpl->mPinchGestureDetector )
  {
    mImpl->mPinchGestureDetector = PinchGestureDetector::New();
    mImpl->mPinchGestureDetector.DetectedSignal().Connect(mImpl, &Impl::PinchDetected);
    mImpl->mPinchGestureDetector.Attach(Self());
  }

  if ( (type & Gesture::Pan) && !mImpl->mPanGestureDetector )
  {
    mImpl->mPanGestureDetector = PanGestureDetector::New();
    mImpl->mPanGestureDetector.DetectedSignal().Connect(mImpl, &Impl::PanDetected);
    mImpl->mPanGestureDetector.Attach(Self());
  }

  if ( (type & Gesture::Tap) && !mImpl->mTapGestureDetector )
  {
    mImpl->mTapGestureDetector = TapGestureDetector::New();
    mImpl->mTapGestureDetector.DetectedSignal().Connect(mImpl, &Impl::TapDetected);
    mImpl->mTapGestureDetector.Attach(Self());
  }

  if ( (type & Gesture::LongPress) && !mImpl->mLongPressGestureDetector )
  {
    mImpl->mLongPressGestureDetector = LongPressGestureDetector::New();
    mImpl->mLongPressGestureDetector.DetectedSignal().Connect(mImpl, &Impl::LongPressDetected);
    mImpl->mLongPressGestureDetector.Attach(Self());
  }
}

void Control::DisableGestureDetection(Gesture::Type type)
{
  if ( (type & Gesture::Pinch) && mImpl->mPinchGestureDetector )
  {
    mImpl->mPinchGestureDetector.Detach(Self());
    mImpl->mPinchGestureDetector.Reset();
  }

  if ( (type & Gesture::Pan) && mImpl->mPanGestureDetector )
  {
    mImpl->mPanGestureDetector.Detach(Self());
    mImpl->mPanGestureDetector.Reset();
  }

  if ( (type & Gesture::Tap) && mImpl->mTapGestureDetector )
  {
    mImpl->mTapGestureDetector.Detach(Self());
    mImpl->mTapGestureDetector.Reset();
  }

  if ( (type & Gesture::LongPress) && mImpl->mLongPressGestureDetector)
  {
    mImpl->mLongPressGestureDetector.Detach(Self());
    mImpl->mLongPressGestureDetector.Reset();
  }
}

PinchGestureDetector Control::GetPinchGestureDetector() const
{
  return mImpl->mPinchGestureDetector;
}

PanGestureDetector Control::GetPanGestureDetector() const
{
  return mImpl->mPanGestureDetector;
}

TapGestureDetector Control::GetTapGestureDetector() const
{
  return mImpl->mTapGestureDetector;
}

LongPressGestureDetector Control::GetLongPressGestureDetector() const
{
  return mImpl->mLongPressGestureDetector;
}

void Control::SetKeyboardNavigationSupport(bool isSupported)
{
  mImpl->mIsKeyboardNavigationSupported = isSupported;
}

bool Control::IsKeyboardNavigationSupported()
{
  return mImpl->mIsKeyboardNavigationSupported;
}

void Control::SetKeyInputFocus()
{
  if( Self().OnStage() )
  {
    Toolkit::KeyInputFocusManager::Get().SetFocus(Toolkit::Control::DownCast(Self()));
  }
}

bool Control::HasKeyInputFocus()
{
  bool result = false;
  if( Self().OnStage() )
  {
    result = Toolkit::KeyInputFocusManager::Get().IsKeyboardListener(Toolkit::Control::DownCast(Self()));
  }
  return result;
}

void Control::ClearKeyInputFocus()
{
  if( Self().OnStage() )
  {
    Toolkit::KeyInputFocusManager::Get().RemoveFocus(Toolkit::Control::DownCast(Self()));
  }
}

void Control::SetAsKeyboardFocusGroup(bool isFocusGroup)
{
  mImpl->mIsKeyboardFocusGroup = isFocusGroup;

  // The following line will be removed when the deprecated API in KeyboardFocusManager is deleted
  Toolkit::KeyboardFocusManager::Get().SetAsFocusGroup(Self(), isFocusGroup);
}

bool Control::IsKeyboardFocusGroup()
{
  return Toolkit::KeyboardFocusManager::Get().IsFocusGroup(Self());
}

void Control::AccessibilityActivate()
{
  // Inform deriving classes
  OnAccessibilityActivated();
}

void Control::KeyboardEnter()
{
  // Inform deriving classes
  OnKeyboardEnter();
}

void Control::RegisterVisual( Property::Index index, Toolkit::Visual::Base& visual )
{
  RegisterVisual( index, visual, true );
}

void Control::RegisterVisual( Property::Index index, Toolkit::Visual::Base& visual, bool enabled )
{
  bool visualReplaced ( false );
  Actor self = Self();

  if ( !mImpl->mVisuals.Empty() )
  {
      RegisteredVisualContainer::Iterator iter;
      // Check if visual (index) is already registered.  Replace if so.
      if ( FindVisual( index, mImpl->mVisuals, iter ) )
      {
        if( (*iter)->visual && self.OnStage() )
        {
          Toolkit::GetImplementation((*iter)->visual).SetOffStage( self );
        }
        (*iter)->visual = visual;
        visualReplaced = true;
      }
  }

  if ( !visualReplaced ) // New registration entry
  {
    mImpl->mVisuals.PushBack( new RegisteredVisual( index, visual, enabled ) );
  }

  if( visual && self.OnStage() && enabled )
  {
    Toolkit::GetImplementation(visual).SetOnStage( self );
  }
}

void Control::UnregisterVisual( Property::Index index )
{
   RegisteredVisualContainer::Iterator iter;
   if ( FindVisual( index, mImpl->mVisuals, iter ) )
   {
     mImpl->mVisuals.Erase( iter );
   }
}

Toolkit::Visual::Base Control::GetVisual( Property::Index index ) const
{
  RegisteredVisualContainer::Iterator iter;
  if ( FindVisual( index, mImpl->mVisuals, iter ) )
  {
    return (*iter)->visual;
  }

  return Toolkit::Visual::Base();
}

void Control::EnableVisual( Property::Index index, bool enable )
{
  RegisteredVisualContainer::Iterator iter;
  if ( FindVisual( index, mImpl->mVisuals, iter ) )
  {
    if (  (*iter)->enabled == enable )
    {
      return;
    }

    (*iter)->enabled = enable;
    Actor parentActor = Self();
    if ( Self().OnStage() ) // If control not on Stage then Visual will be added when StageConnection is called.
    {
      if ( enable )
      {

        Toolkit::GetImplementation((*iter)->visual).SetOnStage( parentActor );
      }
      else
      {
        Toolkit::GetImplementation((*iter)->visual).SetOffStage( parentActor );  // No need to call if control not staged.
      }
    }
  }
}

bool Control::IsVisualEnabled( Property::Index index ) const
{
  RegisteredVisualContainer::Iterator iter;
  if ( FindVisual( index, mImpl->mVisuals, iter ) )
  {
    return (*iter)->enabled;
  }
  return false;
}

Dali::Animation Control::CreateTransition( const Toolkit::TransitionData& handle )
{
  Dali::Animation transition;
  const Internal::TransitionData& transitionData = Toolkit::GetImplementation( handle );

  if( transitionData.Count() > 0 )
  {
    // Setup a Transition from TransitionData.
    TransitionData::Iterator end = transitionData.End();
    for( TransitionData::Iterator iter = transitionData.Begin() ;
         iter != end; ++iter )
    {
      TransitionData::Animator* animator = (*iter);
      HandleIndex handleIndex;

      // Attempt to find the object name as a child actor
      Actor child = Self().FindChildByName( animator->objectName );
      if( child )
      {
        Property::Index propertyIndex = child.GetPropertyIndex( animator->propertyKey );
        handleIndex = HandleIndex( child, propertyIndex );
      }
      else
      {
        handleIndex = GetVisualProperty( *this, mImpl->mVisuals,
                                            animator->objectName,
                                            animator->propertyKey );
      }

      if( handleIndex.handle && handleIndex.index != Property::INVALID_INDEX )
      {
        if( animator->animate == false )
        {
          if( animator->targetValue.GetType() != Property::NONE )
          {
            handleIndex.handle.SetProperty( handleIndex.index, animator->targetValue );
          }
        }
        else
        {
          if( animator->initialValue.GetType() != Property::NONE )
          {
            handleIndex.handle.SetProperty( handleIndex.index, animator->initialValue );
          }

          if( ! transition )
          {
            // Create an animation with a default .1 second duration - the animators
            // will automatically force it to the 'right' duration.
            transition = Dali::Animation::New( 0.1f );
          }

          transition.AnimateTo( Property( handleIndex.handle, handleIndex.index ),
                                animator->targetValue,
                                animator->alphaFunction,
                                TimePeriod( animator->timePeriodDelay,
                                            animator->timePeriodDuration ) );
        }
      }
    }
  }

  return transition;
}

bool Control::OnAccessibilityActivated()
{
  return false; // Accessibility activation is not handled by default
}

bool Control::OnKeyboardEnter()
{
  return false; // Keyboard enter is not handled by default
}

bool Control::OnAccessibilityPan(PanGesture gesture)
{
  return false; // Accessibility pan gesture is not handled by default
}

bool Control::OnAccessibilityTouch(const TouchEvent& touchEvent)
{
  return false; // Accessibility touch event is not handled by default
}

bool Control::OnAccessibilityValueChange(bool isIncrease)
{
  return false; // Accessibility value change action is not handled by default
}

bool Control::OnAccessibilityZoom()
{
  return false; // Accessibility zoom action is not handled by default
}

Actor Control::GetNextKeyboardFocusableActor(Actor currentFocusedActor, Toolkit::Control::KeyboardFocus::Direction direction, bool loopEnabled)
{
  return Actor();
}

void Control::OnKeyboardFocusChangeCommitted(Actor commitedFocusableActor)
{
}

Toolkit::Control::KeyEventSignalType& Control::KeyEventSignal()
{
  return mImpl->mKeyEventSignal;
}

Toolkit::Control::KeyInputFocusSignalType& Control::KeyInputFocusGainedSignal()
{
  return mImpl->mKeyInputFocusGainedSignal;
}

Toolkit::Control::KeyInputFocusSignalType& Control::KeyInputFocusLostSignal()
{
  return mImpl->mKeyInputFocusLostSignal;
}

bool Control::EmitKeyEventSignal( const KeyEvent& event )
{
  // Guard against destruction during signal emission
  Dali::Toolkit::Control handle( GetOwner() );

  bool consumed = false;

  // signals are allocated dynamically when someone connects
  if ( !mImpl->mKeyEventSignal.Empty() )
  {
    consumed = mImpl->mKeyEventSignal.Emit( handle, event );
  }

  if (!consumed)
  {
    // Notification for derived classes
    consumed = OnKeyEvent(event);
  }

  return consumed;
}

Control::Control( ControlBehaviour behaviourFlags )
: CustomActorImpl( static_cast< ActorFlags >( behaviourFlags ) ),
  mImpl(new Impl(*this))
{
  mImpl->mFlags = behaviourFlags;
}

void Control::Initialize()
{
  // Call deriving classes so initialised before styling is applied to them.
  OnInitialize();

  if( (mImpl->mFlags & REQUIRES_STYLE_CHANGE_SIGNALS) ||
      !(mImpl->mFlags & DISABLE_STYLE_CHANGE_SIGNALS) )
  {
    Toolkit::StyleManager styleManager = StyleManager::Get();

    // if stylemanager is available
    if( styleManager )
    {
      StyleManager& styleManagerImpl = GetImpl( styleManager );

      // Register for style changes
      styleManagerImpl.ControlStyleChangeSignal().Connect( this, &Control::OnStyleChange );

      // Apply the current style
      styleManagerImpl.ApplyThemeStyleAtInit( Toolkit::Control( GetOwner() ) );
    }
  }

  if( mImpl->mFlags & REQUIRES_KEYBOARD_NAVIGATION_SUPPORT )
  {
    SetKeyboardNavigationSupport( true );
  }
}

void Control::OnInitialize()
{
}

void Control::OnControlChildAdd( Actor& child )
{
}

void Control::OnControlChildRemove( Actor& child )
{
}

void Control::OnStyleChange( Toolkit::StyleManager styleManager, StyleChange::Type change )
{
  // By default the control is only interested in theme (not font) changes
  if( styleManager && change == StyleChange::THEME_CHANGE )
  {
    GetImpl( styleManager ).ApplyThemeStyle( Toolkit::Control( GetOwner() ) );
  }
  RelayoutRequest();
}

void Control::OnPinch(const PinchGesture& pinch)
{
  if( !( mImpl->mStartingPinchScale ) )
  {
    // lazy allocate
    mImpl->mStartingPinchScale = new Vector3;
  }

  if( pinch.state == Gesture::Started )
  {
    *( mImpl->mStartingPinchScale ) = Self().GetCurrentScale();
  }

  Self().SetScale( *( mImpl->mStartingPinchScale ) * pinch.scale );
}

void Control::OnPan( const PanGesture& pan )
{
}

void Control::OnTap(const TapGesture& tap)
{
}

void Control::OnLongPress( const LongPressGesture& longPress )
{
}

void Control::EmitKeyInputFocusSignal( bool focusGained )
{
  Dali::Toolkit::Control handle( GetOwner() );

  if ( focusGained )
  {
    // signals are allocated dynamically when someone connects
    if ( !mImpl->mKeyInputFocusGainedSignal.Empty() )
    {
      mImpl->mKeyInputFocusGainedSignal.Emit( handle );
    }
  }
  else
  {
    // signals are allocated dynamically when someone connects
    if ( !mImpl->mKeyInputFocusLostSignal.Empty() )
    {
      mImpl->mKeyInputFocusLostSignal.Emit( handle );
    }
  }
}

void Control::OnStageConnection( int depth )
{
  for(RegisteredVisualContainer::Iterator iter = mImpl->mVisuals.Begin(); iter!= mImpl->mVisuals.End(); iter++)
  {
    // Check whether the visual is empty and enabled
    if( (*iter)->visual && (*iter)->enabled )
    {
      Actor self( Self() );
      Toolkit::GetImplementation((*iter)->visual).SetOnStage( self );
    }
  }
}

void Control::OnStageDisconnection()
{
  for(RegisteredVisualContainer::Iterator iter = mImpl->mVisuals.Begin(); iter!= mImpl->mVisuals.End(); iter++)
  {
    // Check whether the visual is empty
    if( (*iter)->visual )
    {
      Actor self( Self() );
      Toolkit::GetImplementation((*iter)->visual).SetOffStage( self );
    }
  }
}

void Control::OnKeyInputFocusGained()
{
  EmitKeyInputFocusSignal( true );
}

void Control::OnKeyInputFocusLost()
{
  EmitKeyInputFocusSignal( false );
}

void Control::OnChildAdd(Actor& child)
{
  // Notify derived classes.
  OnControlChildAdd( child );
}

void Control::OnChildRemove(Actor& child)
{
  // Notify derived classes.
  OnControlChildRemove( child );
}

void Control::OnSizeSet(const Vector3& targetSize)
{
  if( mImpl->mBackgroundVisual )
  {
    Vector2 size( targetSize );
    mImpl->mBackgroundVisual.SetSize( size );
  }
}

void Control::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
  // @todo size negotiate background to new size, animate as well?
}

bool Control::OnTouchEvent(const TouchEvent& event)
{
  return false; // Do not consume
}

bool Control::OnHoverEvent(const HoverEvent& event)
{
  return false; // Do not consume
}

bool Control::OnKeyEvent(const KeyEvent& event)
{
  return false; // Do not consume
}

bool Control::OnWheelEvent(const WheelEvent& event)
{
  return false; // Do not consume
}

void Control::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  for( unsigned int i = 0, numChildren = Self().GetChildCount(); i < numChildren; ++i )
  {
    container.Add( Self().GetChildAt( i ), size );
  }
}

void Control::OnSetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension )
{
}

Vector3 Control::GetNaturalSize()
{
  if( mImpl->mBackgroundVisual )
  {
    Vector2 naturalSize;
    mImpl->mBackgroundVisual.GetNaturalSize(naturalSize);
    return Vector3(naturalSize);
  }
  return Vector3::ZERO;
}

float Control::CalculateChildSize( const Dali::Actor& child, Dimension::Type dimension )
{
  return CalculateChildSizeBase( child, dimension );
}

float Control::GetHeightForWidth( float width )
{
  return GetHeightForWidthBase( width );
}

float Control::GetWidthForHeight( float height )
{
  return GetWidthForHeightBase( height );
}

bool Control::RelayoutDependentOnChildren( Dimension::Type dimension )
{
  return RelayoutDependentOnChildrenBase( dimension );
}

void Control::OnCalculateRelayoutSize( Dimension::Type dimension )
{
}

void Control::OnLayoutNegotiated( float size, Dimension::Type dimension )
{
}

void Control::SignalConnected( SlotObserver* slotObserver, CallbackBase* callback )
{
  mImpl->SignalConnected( slotObserver, callback );
}

void Control::SignalDisconnected( SlotObserver* slotObserver, CallbackBase* callback )
{
  mImpl->SignalDisconnected( slotObserver, callback );
}

Control& GetImplementation( Dali::Toolkit::Control& handle )
{
  CustomActorImpl& customInterface = handle.GetImplementation();
  // downcast to control
  Control& impl = dynamic_cast< Internal::Control& >( customInterface );
  return impl;
}

const Control& GetImplementation( const Dali::Toolkit::Control& handle )
{
  const CustomActorImpl& customInterface = handle.GetImplementation();
  // downcast to control
  const Control& impl = dynamic_cast< const Internal::Control& >( customInterface );
  return impl;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
