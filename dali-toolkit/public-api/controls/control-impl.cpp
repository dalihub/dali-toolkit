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
#include <dali-toolkit/public-api/controls/control-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp
#include <limits>
#include <stack>
#include <dali/public-api/actors/image-actor.h>
#include <dali/devel-api/actors/mesh-actor.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/devel-api/geometry/mesh.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/public-api/size-negotiation/relayout-container.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/focus-manager/keyinput-focus-manager.h>
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/devel-api/styling/style-manager.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace
{

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
const char* ACTION_CONTROL_ACTIVATED = "control-activated";
static bool DoAction( BaseObject* object, const std::string& actionName, const Property::Map& attributes )
{
  bool ret = false;

  if( object && ( 0 == strcmp( actionName.c_str(), ACTION_CONTROL_ACTIVATED ) ) )
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
const char* SIGNAL_KEY_EVENT = "key-event";
const char* SIGNAL_KEY_INPUT_FOCUS_GAINED = "key-input-focus-gained";
const char* SIGNAL_KEY_INPUT_FOCUS_LOST = "key-input-focus-lost";
const char* SIGNAL_TAPPED = "tapped";
const char* SIGNAL_PANNED = "panned";
const char* SIGNAL_PINCHED = "pinched";
const char* SIGNAL_LONG_PRESSED = "long-pressed";
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

TypeAction registerAction( typeRegistration, ACTION_CONTROL_ACTIVATED, &DoAction );

DALI_TYPE_REGISTRATION_END()

/**
 * Structure which holds information about the background of a control
 */
struct Background
{
  Actor actor;   ///< Either a MeshActor or an ImageActor
  Vector4 color; ///< The color of the actor.

  /**
   * Constructor
   */
  Background()
  : actor(),
    color( Color::WHITE )
  {
  }
};

/**
 * Creates a white coloured Mesh.
 */
Mesh CreateMesh()
{
  Vector3 white( Color::WHITE );

  MeshData meshData;

  // Create vertices with a white color (actual color is set by actor color)
  MeshData::VertexContainer vertices(4);
  vertices[ 0 ] = MeshData::Vertex( Vector3( -0.5f, -0.5f, 0.0f ), Vector2::ZERO, white );
  vertices[ 1 ] = MeshData::Vertex( Vector3(  0.5f, -0.5f, 0.0f ), Vector2::ZERO, white );
  vertices[ 2 ] = MeshData::Vertex( Vector3( -0.5f,  0.5f, 0.0f ), Vector2::ZERO, white );
  vertices[ 3 ] = MeshData::Vertex( Vector3(  0.5f,  0.5f, 0.0f ), Vector2::ZERO, white );

  // Specify all the faces
  MeshData::FaceIndices faces;
  faces.reserve( 6 ); // 2 triangles in Quad
  faces.push_back( 0 ); faces.push_back( 3 ); faces.push_back( 1 );
  faces.push_back( 0 ); faces.push_back( 2 ); faces.push_back( 3 );

  // Create the mesh data from the vertices and faces
  meshData.SetMaterial( Material::New( "ControlMaterial" ) );
  meshData.SetVertices( vertices );
  meshData.SetFaceIndices( faces );
  meshData.SetHasColor( true );

  return Mesh::New( meshData );
}

/**
 * Sets all the required properties for the background actor.
 *
 * @param[in]  actor              The actor to set the properties on.
 * @param[in]  color              The required color of the actor.
 */
void SetupBackgroundActor( Actor actor, const Vector4& color )
{
  actor.SetColor( color );
  actor.SetPositionInheritanceMode( USE_PARENT_POSITION_PLUS_LOCAL_POSITION );
  actor.SetColorMode( USE_OWN_MULTIPLY_PARENT_COLOR );
  actor.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
}

/**
 * Sets all the required properties for the background actor.
 *
 * @param[in]  actor              The actor to set the properties on.
 * @param[in]  constrainingIndex  The property index to constrain the parent's size on.
 * @param[in]  color              The required color of the actor.
 */
void SetupBackgroundActorConstrained( Actor actor, Property::Index constrainingIndex, const Vector4& color )
{
  actor.SetColor( color );
  actor.SetPositionInheritanceMode( USE_PARENT_POSITION_PLUS_LOCAL_POSITION );
  actor.SetColorMode( USE_OWN_MULTIPLY_PARENT_COLOR );

  Constraint constraint = Constraint::New<Vector3>( actor,
                                                    constrainingIndex,
                                                    EqualToConstraint() );
  constraint.AddSource( ParentSource( Actor::Property::SIZE ) );
  constraint.Apply();
}

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
    mBackground( NULL ),
    mStartingPinchScale( NULL ),
    mKeyEventSignal(),
    mPinchGestureDetector(),
    mPanGestureDetector(),
    mTapGestureDetector(),
    mLongPressGestureDetector(),
    mFlags( Control::ControlBehaviour( ACTOR_BEHAVIOUR_NONE ) ),
    mIsKeyboardNavigationSupported( false ),
    mIsKeyboardFocusGroup( false ),
    mAddRemoveBackgroundChild( false )
  {
  }

  ~Impl()
  {
    // All gesture detectors will be destroyed so no need to disconnect.
    delete mBackground;
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

  // Background Methods

  /**
   * Only creates an instance of the background if we actually use it.
   * @return A reference to the Background structure.
   */
  Background& GetBackground()
  {
    if ( !mBackground )
    {
      mBackground = new Background;
    }
    return *mBackground;
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
          controlImpl.SetBackgroundColor( value.Get< Vector4 >() );
          break;
        }

        case Toolkit::Control::Property::BACKGROUND_IMAGE:
        {
          if ( value.HasKey( "image" ) )
          {
            Property::Map imageMap = value.GetValue( "image" ).Get< Property::Map >();
            Image image = Scripting::NewImage( imageMap );

            if ( image )
            {
              controlImpl.SetBackgroundImage( image );
            }
          }
          else if ( value.Get< Property::Map >().Empty() )
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
          value = controlImpl.GetBackgroundColor();
          break;
        }

        case Toolkit::Control::Property::BACKGROUND_IMAGE:
        {
          Property::Map map;

          Background* back = controlImpl.mImpl->mBackground;
          if( back )
          {
            ImageActor imageActor = ImageActor::DownCast( back->actor );
            if ( imageActor )
            {
              Image image = imageActor.GetImage();
              Property::Map imageMap;
              Scripting::CreatePropertyMap( image, imageMap );
              map[ "image" ] = imageMap;
            }
          }

          value = map;
          break;
        }

        case Toolkit::Control::Property::KEY_INPUT_FOCUS:
        {
          value = controlImpl.HasKeyInputFocus();
          break;
        }
      }
    }

    return value;
  }

  // Data

  Control& mControlImpl;
  std::string mStyleName;
  Background* mBackground;           ///< Only create the background if we use it
  Vector3* mStartingPinchScale;      ///< The scale when a pinch gesture starts, TODO: consider removing this
  Toolkit::Control::KeyEventSignalType mKeyEventSignal;
  Toolkit::Control::KeyInputFocusSignalType mKeyInputFocusGainedSignal;
  Toolkit::Control::KeyInputFocusSignalType mKeyInputFocusLostSignal;

  // Gesture Detection
  PinchGestureDetector mPinchGestureDetector;
  PanGestureDetector mPanGestureDetector;
  TapGestureDetector mTapGestureDetector;
  LongPressGestureDetector mLongPressGestureDetector;

  ControlBehaviour mFlags :CONTROL_BEHAVIOUR_FLAG_COUNT;    ///< Flags passed in from constructor.
  bool mIsKeyboardNavigationSupported :1;  ///< Stores whether keyboard navigation is supported by the control.
  bool mIsKeyboardFocusGroup :1;           ///< Stores whether the control is a focus group.
  bool mAddRemoveBackgroundChild:1;        ///< Flag to know when we are adding or removing our own actor to avoid call to OnControlChildAdd

  // Properties - these need to be members of Internal::Control::Impl as they need to function within this class.
  static PropertyRegistration PROPERTY_1;
  static PropertyRegistration PROPERTY_2;
  static PropertyRegistration PROPERTY_3;
  static PropertyRegistration PROPERTY_4;
};

// Properties registered without macro to use specific member variables.
PropertyRegistration Control::Impl::PROPERTY_1( typeRegistration, "style-name",       Toolkit::Control::Property::STYLE_NAME,       Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty );
PropertyRegistration Control::Impl::PROPERTY_2( typeRegistration, "background-color", Toolkit::Control::Property::BACKGROUND_COLOR, Property::VECTOR4, &Control::Impl::SetProperty, &Control::Impl::GetProperty );
PropertyRegistration Control::Impl::PROPERTY_3( typeRegistration, "background-image", Toolkit::Control::Property::BACKGROUND_IMAGE, Property::MAP,     &Control::Impl::SetProperty, &Control::Impl::GetProperty );
PropertyRegistration Control::Impl::PROPERTY_4( typeRegistration, "key-input-focus",  Toolkit::Control::Property::KEY_INPUT_FOCUS,  Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty );

Toolkit::Control Control::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<Control> controlImpl = new Control( ControlBehaviour( ACTOR_BEHAVIOUR_NONE ) );

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

    // Apply new style
    Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();
    GetImpl( styleManager ).ApplyThemeStyle( Toolkit::Control( GetOwner() ) );
  }
}

const std::string& Control::GetStyleName() const
{
  return mImpl->mStyleName;
}

void Control::SetBackgroundColor( const Vector4& color )
{
  Background& background( mImpl->GetBackground() );

  if ( background.actor )
  {
    // Just set the actor color
    background.actor.SetColor( color );
  }
  else
  {
    // Create Mesh Actor
    MeshActor meshActor = MeshActor::New( CreateMesh() );

    SetupBackgroundActorConstrained( meshActor, Actor::Property::SCALE, color );

    background.actor = meshActor;
    // Set the flag to avoid notifying children
    mImpl->mAddRemoveBackgroundChild = true;
    // use insert to guarantee its the first child (so that OVERLAY mode works)
    Self().Insert( 0, meshActor );
    mImpl->mAddRemoveBackgroundChild = false;
  }

  background.color = color;
}

Vector4 Control::GetBackgroundColor() const
{
  if ( mImpl->mBackground )
  {
    return mImpl->mBackground->color;
  }
  return Color::TRANSPARENT;
}

void Control::SetBackgroundImage( Image image )
{
  Background& background( mImpl->GetBackground() );

  if ( background.actor )
  {
    // Remove Current actor, unset AFTER removal
    mImpl->mAddRemoveBackgroundChild = true;
    Self().Remove( background.actor );
    mImpl->mAddRemoveBackgroundChild = false;
    background.actor.Reset();
  }

  ImageActor imageActor = ImageActor::New( image );
  SetupBackgroundActor( imageActor, background.color );

  // Set the background actor before adding so that we do not inform derived classes
  background.actor = imageActor;
  mImpl->mAddRemoveBackgroundChild = true;
  // use insert to guarantee its the first child (so that OVERLAY mode works)
  Self().Insert( 0, imageActor );
  mImpl->mAddRemoveBackgroundChild = false;
}

void Control::ClearBackground()
{
  if ( mImpl->mBackground )
  {
    Background& background( mImpl->GetBackground() );
    mImpl->mAddRemoveBackgroundChild = true;
    Self().Remove( background.actor );
    mImpl->mAddRemoveBackgroundChild = false;

    delete mImpl->mBackground;
    mImpl->mBackground = NULL;
  }
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

bool Control::OnAccessibilityActivated()
{
  return false; // Accessibility activation is not handled by default
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

Toolkit::Control::KeyInputFocusSignalType& Control:: KeyInputFocusGainedSignal()
{
  return mImpl->mKeyInputFocusGainedSignal;
}

Toolkit::Control::KeyInputFocusSignalType& Control:: KeyInputFocusLostSignal()
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
  if( mImpl->mFlags & REQUIRES_STYLE_CHANGE_SIGNALS )
  {
    Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();

    // Register for style changes
    styleManager.StyleChangeSignal().Connect( this, &Control::OnStyleChange );

    // SetTheme
    GetImpl( styleManager ).ApplyThemeStyle( Toolkit::Control( GetOwner() ) );
  }

  if( mImpl->mFlags & REQUIRES_KEYBOARD_NAVIGATION_SUPPORT )
  {
    SetKeyboardNavigationSupport( true );
  }

  // Calling deriving classes
  OnInitialize();
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
  if( change == StyleChange::THEME_CHANGE )
  {
    GetImpl( styleManager ).ApplyThemeStyle( Toolkit::Control( GetOwner() ) );
  }
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

void Control::OnStageConnection()
{
}

void Control::OnStageDisconnection()
{
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
  // If this is the background actor, then we do not want to inform deriving classes
  if ( mImpl->mAddRemoveBackgroundChild )
  {
    return;
  }

  // Notify derived classes.
  OnControlChildAdd( child );
}

void Control::OnChildRemove(Actor& child)
{
  // If this is the background actor, then we do not want to inform deriving classes
  if ( mImpl->mAddRemoveBackgroundChild )
  {
    return;
  }

  // Notify derived classes.
  OnControlChildRemove( child );
}

void Control::OnSizeSet(const Vector3& targetSize)
{
  // Background is resized through size negotiation
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
  if( mImpl->mBackground )
  {
    Actor actor = mImpl->mBackground->actor;
    if( actor )
    {
      return actor.GetNaturalSize();
    }
  }
  return Vector3();
}

float Control::CalculateChildSize( const Dali::Actor& child, Dimension::Type dimension )
{
  return CalculateChildSizeBase( child, dimension );
}

float Control::GetHeightForWidth( float width )
{
  if( mImpl->mBackground )
  {
    Actor actor = mImpl->mBackground->actor;
    if( actor )
    {
      return actor.GetHeightForWidth( width );
    }
  }
  return GetHeightForWidthBase( width );
}

float Control::GetWidthForHeight( float height )
{
  if( mImpl->mBackground )
  {
    Actor actor = mImpl->mBackground->actor;
    if( actor )
    {
      return actor.GetWidthForHeight( height );
    }
  }
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
