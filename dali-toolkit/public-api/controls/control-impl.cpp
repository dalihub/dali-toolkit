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
#include <dali/public-api/actors/mesh-actor.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/geometry/mesh.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/scripting/scripting.h>
#include <dali/public-api/size-negotiation/relayout-container.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/focus-manager/keyinput-focus-manager.h>
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/styling/style-manager.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace
{

#if defined(DEBUG_ENABLED)
Integration::Log::Filter* gLogFilter  = Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_CONTROL");
#endif

const float MAX_FLOAT_VALUE( std::numeric_limits<float>::max() );
const Vector3 MAX_SIZE( MAX_FLOAT_VALUE, MAX_FLOAT_VALUE, MAX_FLOAT_VALUE );
const float BACKGROUND_ACTOR_Z_POSITION( -0.1f );

BaseHandle Create()
{
  return Internal::Control::New();
}

// Setup signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Control, CustomActor, Create );

// Note: Properties are registered separately below,

DALI_SIGNAL_REGISTRATION( Control, "key-event",                 SIGNAL_KEY_EVENT              )
DALI_SIGNAL_REGISTRATION( Control, "key-input-focus-gained",    SIGNAL_KEY_INPUT_FOCUS_GAINED )
DALI_SIGNAL_REGISTRATION( Control, "key-input-focus-lost",      SIGNAL_KEY_INPUT_FOCUS_LOST   )
DALI_SIGNAL_REGISTRATION( Control, "tapped",                    SIGNAL_TAPPED                 )
DALI_SIGNAL_REGISTRATION( Control, "panned",                    SIGNAL_PANNED                 )
DALI_SIGNAL_REGISTRATION( Control, "pinched",                   SIGNAL_PINCHED                )
DALI_SIGNAL_REGISTRATION( Control, "long-pressed",              SIGNAL_LONG_PRESSED           )

DALI_ACTION_REGISTRATION( Control, "control-activated",         ACTION_CONTROL_ACTIVATED )

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
  actor.SetZ( BACKGROUND_ACTOR_Z_POSITION );
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
  actor.SetZ( BACKGROUND_ACTOR_Z_POSITION );

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

  /**
   * Size indices for mMinMaxSize array
   */
  enum
  {
    MIN_SIZE_INDEX = 0,
    MAX_SIZE_INDEX = 1
  };

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
    mCurrentSize(),
    mNaturalSize(),
    mFlags( Control::ControlBehaviour( ACTOR_BEHAVIOUR_NONE ) ),
    mIsKeyboardNavigationSupported( false ),
    mIsKeyboardFocusGroup( false ),
    mInitialized( false )
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
      Control& controlImpl( control.GetImplementation() );

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
      Control& controlImpl( control.GetImplementation() );

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

          Actor actor = controlImpl.GetBackgroundActor();
          if ( actor )
          {
            ImageActor imageActor = ImageActor::DownCast( actor );
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
  // @todo change all these to Vector2 when we have a chance to sanitize the public API as well
  Vector3 mCurrentSize; ///< Stores the current control's size, this is the negotiated size
  Vector3 mNaturalSize; ///< Stores the size set through the Actor's API. This is size the actor wants to be. Useful when reset to the initial size is needed.

  ControlBehaviour mFlags :CONTROL_BEHAVIOUR_FLAG_COUNT;    ///< Flags passed in from constructor.
  bool mIsKeyboardNavigationSupported :1;  ///< Stores whether keyboard navigation is supported by the control.
  bool mIsKeyboardFocusGroup :1;           ///< Stores whether the control is a focus group.
  bool mInitialized :1;

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

Vector3 Control::GetNaturalSize()
{
  // could be overridden in derived classes.
  return mImpl->mNaturalSize;
}

float Control::CalculateChildSize( const Dali::Actor& child, Dimension::Type dimension )
{
  // Could be overridden in derived classes.
  return CalculateChildSizeBase( child, dimension );
}

bool Control::RelayoutDependentOnChildren( Dimension::Type dimension )
{
  return RelayoutDependentOnChildrenBase( dimension );
}

float Control::GetHeightForWidth( float width )
{
  // could be overridden in derived classes.
  float height( 0.0f );
  if ( mImpl->mNaturalSize.width > 0.0f )
  {
    height = mImpl->mNaturalSize.height * width / mImpl->mNaturalSize.width;
  }
  return height;
}

float Control::GetWidthForHeight( float height )
{
  // could be overridden in derived classes.
  float width( 0.0f );
  if ( mImpl->mNaturalSize.height > 0.0f )
  {
    width = mImpl->mNaturalSize.width * height / mImpl->mNaturalSize.height;
  }
  return width;
}

const Vector3& Control::GetControlSize() const
{
  return mImpl->mCurrentSize;
}

const Vector3& Control::GetSizeSet() const
{
  return mImpl->mNaturalSize;
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

    // Set the background actor before adding so that we do not inform deriving classes
    background.actor = meshActor;
    Self().Add( meshActor );
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
    // Remove Current actor, unset AFTER removal so that we do not inform deriving classes
    Self().Remove( background.actor );
    background.actor.Reset();
  }

  ImageActor imageActor = ImageActor::New( image );
  SetupBackgroundActor( imageActor, background.color );

  // Set the background actor before adding so that we do not inform derived classes
  background.actor = imageActor;
  Self().Add( imageActor );
}

void Control::ClearBackground()
{
  if ( mImpl->mBackground )
  {
    Background& background( mImpl->GetBackground() );
    Self().Remove( background.actor );

    delete mImpl->mBackground;
    mImpl->mBackground = NULL;
  }
}

Actor Control::GetBackgroundActor() const
{
  if ( mImpl->mBackground )
  {
    return mImpl->mBackground->actor;
  }

  return Actor();
}

void Control::SetKeyboardNavigationSupport(bool isSupported)
{
  mImpl->mIsKeyboardNavigationSupported = isSupported;
}

bool Control::IsKeyboardNavigationSupported()
{
  return mImpl->mIsKeyboardNavigationSupported;
}

void Control::Activate()
{
  // Inform deriving classes
  OnActivated();
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

Actor Control::GetNextKeyboardFocusableActor(Actor currentFocusedActor, Toolkit::Control::KeyboardFocusNavigationDirection direction, bool loopEnabled)
{
  return Actor();
}

void Control::OnKeyboardFocusChangeCommitted(Actor commitedFocusableActor)
{
}

bool Control::DoAction(BaseObject* object, const std::string& actionName, const PropertyValueContainer& attributes)
{
  bool ret = false;

  if( object && ( 0 == strcmp( actionName.c_str(), ACTION_CONTROL_ACTIVATED ) ) )
  {
    Toolkit::Control control = Toolkit::Control::DownCast( BaseHandle( object ) );
    if( control )
    {
      // if cast succeeds there is an implementation so no need to check
      control.GetImplementation().OnActivated();
    }
  }

  return ret;
}

bool Control::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( false );
  Toolkit::Control control = Toolkit::Control::DownCast( handle );
  if ( control )
  {
    Control& controlImpl( control.GetImplementation() );
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
    else
    {
      // signalName does not match any signal
      connected = false;
    }
  }
  return connected;
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
  // Calling deriving classes
  OnInitialize();

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

  mImpl->mInitialized = true;
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

void Control::OnInitialize()
{
}

void Control::OnActivated()
{
}

void Control::OnStyleChange( Toolkit::StyleManager styleManager, StyleChange change )
{
  // By default the control is only interested in theme (not font) changes
  if( change.themeChange )
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

void Control::OnControlStageConnection()
{
}

void Control::OnControlStageDisconnection()
{
}

void Control::OnControlChildAdd( Actor& child )
{
}

void Control::OnControlChildRemove( Actor& child )
{
}

void Control::OnControlSizeSet( const Vector3& size )
{
}

void Control::OnCalculateRelayoutSize( Dimension::Type dimension )
{
}

void Control::OnLayoutNegotiated( float size, Dimension::Type dimension )
{
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

void Control::OnKeyInputFocusGained()
{
  EmitKeyInputFocusSignal( true );
}

void Control::OnKeyInputFocusLost()
{
  EmitKeyInputFocusSignal( false );
}

void Control::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
  // @todo consider animating negotiated child sizes to target size
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

bool Control::OnMouseWheelEvent(const MouseWheelEvent& event)
{
  return false; // Do not consume
}

void Control::OnStageConnection()
{
  // Notify derived classes.
  OnControlStageConnection();
}

void Control::OnStageDisconnection()
{
  // Notify derived classes
  OnControlStageDisconnection();
}

void Control::OnChildAdd(Actor& child)
{
  // If this is the background actor, then we do not want to relayout or inform deriving classes
  if ( mImpl->mBackground && ( child == mImpl->mBackground->actor ) )
  {
    return;
  }

  // Notify derived classes.
  OnControlChildAdd( child );
}

void Control::OnChildRemove(Actor& child)
{
  // If this is the background actor, then we do not want to relayout or inform deriving classes
  if ( mImpl->mBackground && ( child == mImpl->mBackground->actor ) )
  {
    return;
  }

  // Notify derived classes.
  OnControlChildRemove( child );
}

void Control::OnSizeSet(const Vector3& targetSize)
{
  if( targetSize != mImpl->mNaturalSize )
  {
    // Only updates size if set through Actor's API
    mImpl->mNaturalSize = targetSize;
  }

  if( targetSize != mImpl->mCurrentSize )
  {
    // Update control size.
    mImpl->mCurrentSize = targetSize;

    // Notify derived classes.
    OnControlSizeSet( targetSize );
  }
}

void Control::SignalConnected( SlotObserver* slotObserver, CallbackBase* callback )
{
  mImpl->SignalConnected( slotObserver, callback );
}

void Control::SignalDisconnected( SlotObserver* slotObserver, CallbackBase* callback )
{
  mImpl->SignalDisconnected( slotObserver, callback );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
