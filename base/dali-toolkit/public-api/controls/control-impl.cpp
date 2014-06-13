/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/public-api/controls/control-impl.h>

#include <stack>

#include <dali/integration-api/debug.h>

#include "dali-toolkit/internal/controls/relayout-controller.h"
#include "dali-toolkit/internal/controls/relayout-helper.h"
#include "dali-toolkit/public-api/focus-manager/keyinput-focus-manager.h"
#include "dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h"
#include <dali-toolkit/public-api/controls/control.h>

#include <dali-toolkit/public-api/styling/style-manager.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>

namespace Dali
{

namespace Toolkit
{

const Property::Index Control::PROPERTY_BACKGROUND_COLOR      = Internal::Control::CONTROL_PROPERTY_START_INDEX;
const Property::Index Control::PROPERTY_BACKGROUND            = Internal::Control::CONTROL_PROPERTY_START_INDEX + 1;
const Property::Index Control::PROPERTY_WIDTH_POLICY          = Internal::Control::CONTROL_PROPERTY_START_INDEX + 2;
const Property::Index Control::PROPERTY_HEIGHT_POLICY         = Internal::Control::CONTROL_PROPERTY_START_INDEX + 3;
const Property::Index Control::PROPERTY_MINIMUM_SIZE          = Internal::Control::CONTROL_PROPERTY_START_INDEX + 4;
const Property::Index Control::PROPERTY_MAXIMUM_SIZE          = Internal::Control::CONTROL_PROPERTY_START_INDEX + 5;
const Property::Index Control::PROPERTY_KEY_INPUT_FOCUS       = Internal::Control::CONTROL_PROPERTY_START_INDEX + 6;

namespace
{

const Scripting::StringEnum< Control::SizePolicy > SIZE_POLICY_STRING_TABLE[] =
{
  { "FIXED",      Control::Fixed      },
  { "MINIMUM",    Control::Minimum    },
  { "MAXIMUM",    Control::Maximum    },
  { "RANGE",      Control::Range      },
  { "FLEXIBLE",   Control::Flexible   },
};
const unsigned int SIZE_POLICY_STRING_TABLE_COUNT = sizeof( SIZE_POLICY_STRING_TABLE ) / sizeof( SIZE_POLICY_STRING_TABLE[0] );

#if defined(DEBUG_ENABLED)
Integration::Log::Filter* gLogFilter  = Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_CONTROL");
#endif

const float MAX_FLOAT_VALUE( std::numeric_limits<float>::max() );
const float BACKGROUND_ACTOR_Z_POSITION( -0.1f );

BaseHandle Create()
{
  return Internal::Control::New();
}

TypeRegistration CONTROL_TYPE( typeid(Control), typeid(CustomActor), Create );

// Property Registration after Internal::Control::Impl definition below

TypeAction ACTION_TYPE_1(CONTROL_TYPE, Toolkit::Control::ACTION_CONTROL_ACTIVATED, &Internal::Control::DoAction);

/**
 * Helper class used to set the Control's size through the Actor's API or through children added.
 */
class SetSizeLock
{
public:
  SetSizeLock( bool& lock )
  : mLock( lock )
  {
    mLock = true;
  }

  ~SetSizeLock()
  {
    mLock = false;
  }

private:
  bool& mLock;
};

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
 * Helper function to calculate a dimension given the policy of that dimension; the minimum &
 * maximum values that dimension can be; and the allocated value for that dimension.
 *
 * @param[in]  policy     The size policy for that dimension.
 * @param[in]  minimum    The minimum value that dimension can be.
 * @param[in]  maximum    The maximum value that dimension can be.
 * @param[in]  allocated  The value allocated for that dimension.
 *
 * @return The value that the dimension should be.
 *
 * @note This does not handle Control::Fixed policy.
 */
float Calculate( Control::SizePolicy policy, float minimum, float maximum, float allocated )
{
  float size( allocated );

  switch( policy )
  {
    case Control::Fixed:
    {
      // Use allocated value
      break;
    }

    case Control::Minimum:
    {
      // Size is always at least the minimum.
      size = std::max( allocated, minimum );
      break;
    }

    case Control::Maximum:
    {
      // Size can grow but up to a maximum value.
      size = std::min( allocated, maximum );
      break;
    }

    case Control::Range:
    {
      // Size is at least the minimum and can grow up to the maximum
      size = std::max( size, minimum );
      size = std::min( size, maximum );
     break;
    }

    case Control::Flexible:
    {
      // Size grows or shrinks with no limits.
      size = allocated;
      break;
    }

    default:
    {
      DALI_ASSERT_DEBUG( false && "This function was not intended to be used by any other policy." );
      break;
    }
  }

  return size;
}

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
 * @param[in]  constrainingIndex  The property index to constrain the parent's size on.
 * @param[in]  color              The required color of the actor.
 */
void SetupBackgroundActor( Actor actor, Property::Index constrainingIndex, const Vector4& color )
{
  actor.SetColor( color );
  actor.SetPositionInheritanceMode( USE_PARENT_POSITION_PLUS_LOCAL_POSITION );
  actor.SetZ( BACKGROUND_ACTOR_Z_POSITION );

  Constraint constraint = Constraint::New<Vector3>( constrainingIndex,
                                                    ParentSource( Actor::SIZE ),
                                                    EqualToConstraint() );
  actor.ApplyConstraint( constraint );
}

} // unnamed namespace

namespace Internal
{

class Control::Impl : public ConnectionTrackerInterface
{
public:
  // Construction & Destruction
  Impl(Control& controlImpl)
  : mControlImpl(controlImpl),
    mBackground( NULL ),
    mKeyEventSignalV2(),
    mPinchGestureDetector(),
    mPanGestureDetector(),
    mTapGestureDetector(),
    mLongPressGestureDetector(),
    mStartingPinchScale(),
    mSize(),
    mSetSize(),
    mMinimumSize(),
    mMaximumSize( MAX_FLOAT_VALUE, MAX_FLOAT_VALUE, MAX_FLOAT_VALUE ),
    mLockSetSize( false ),
    mWidthPolicy( Toolkit::Control::Fixed ),
    mHeightPolicy( Toolkit::Control::Fixed ),
    mFlags( Control::CONTROL_BEHAVIOUR_NONE ),
    mIsKeyboardNavigationSupported(false),
    mIsKeyboardFocusGroup(false),
    mInitialized( false )
  {
  }

  ~Impl()
  {
    // All gesture detectors will be destroyed so no need to disconnect.
    if ( mBackground )
    {
      delete mBackground;
    }
  }

  // Gesture Detection Methods

  void PinchDetected(Actor actor, PinchGesture pinch)
  {
    mControlImpl.OnPinch(pinch);
  }

  void PanDetected(Actor actor, PanGesture pan)
  {
    mControlImpl.OnPan(pan);
  }

  void TapDetected(Actor actor, TapGesture tap)
  {
    mControlImpl.OnTap(tap);
  }

  void LongPressDetected(Actor actor, LongPressGesture longPress)
  {
    mControlImpl.OnLongPress(longPress);
  }

  /**
   * @copydoc ConnectionTrackerInterface::SignalConnected
   */
  virtual void SignalConnected( SlotObserver* slotObserver, CallbackBase* callback )
  {
    mConnectionTracker.SignalConnected( slotObserver, callback );
  }

  /**
   * @copydoc ConnectionTrackerInterface::SignalDisconnected
   */
  virtual void SignalDisconnected( SlotObserver* slotObserver, CallbackBase* callback )
  {
    mConnectionTracker.SignalDisconnected( slotObserver, callback );
  }

  /**
   * @copydoc ConnectionTrackerInterface::GetConnectionCount
   */
  virtual std::size_t GetConnectionCount() const
  {
    return mConnectionTracker.GetConnectionCount();
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
        case Toolkit::Control::PROPERTY_BACKGROUND_COLOR:
        {
          controlImpl.SetBackgroundColor( value.Get< Vector4 >() );
          break;
        }

        case Toolkit::Control::PROPERTY_BACKGROUND:
        {
          if ( value.HasKey( "image" ) )
          {
            Property::Map imageMap = value.GetValue( "image" ).Get< Property::Map >();
            Image image = Scripting::NewImage( imageMap );

            if ( image )
            {
              controlImpl.SetBackground( image );
            }
          }
          else if ( value.Get< Property::Map >().empty() )
          {
            // An empty map means the background is no longer required
            controlImpl.ClearBackground();
          }
          break;
        }

        case Toolkit::Control::PROPERTY_WIDTH_POLICY:
        {
          controlImpl.mImpl->mWidthPolicy = Scripting::GetEnumeration< Toolkit::Control::SizePolicy >( value.Get< std::string >(), SIZE_POLICY_STRING_TABLE, SIZE_POLICY_STRING_TABLE_COUNT );
          break;
        }

        case Toolkit::Control::PROPERTY_HEIGHT_POLICY:
        {
          controlImpl.mImpl->mHeightPolicy = Scripting::GetEnumeration< Toolkit::Control::SizePolicy >( value.Get< std::string >(), SIZE_POLICY_STRING_TABLE, SIZE_POLICY_STRING_TABLE_COUNT );
          break;
        }

        case Toolkit::Control::PROPERTY_MINIMUM_SIZE:
        {
          controlImpl.SetMinimumSize( value.Get< Vector3 >() );
          break;
        }

        case Toolkit::Control::PROPERTY_MAXIMUM_SIZE:
        {
          controlImpl.SetMaximumSize( value.Get< Vector3 >() );
          break;
        }

        case Toolkit::Control::PROPERTY_KEY_INPUT_FOCUS:
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
        case Toolkit::Control::PROPERTY_BACKGROUND_COLOR:
        {
          value = controlImpl.GetBackgroundColor();
          break;
        }

        case Toolkit::Control::PROPERTY_BACKGROUND:
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
              map.push_back( Property::StringValuePair( "image", imageMap ) );
            }
          }

          value = map;
          break;
        }

        case Toolkit::Control::PROPERTY_WIDTH_POLICY:
        {
          value = std::string( Scripting::GetEnumerationName< Toolkit::Control::SizePolicy >( controlImpl.mImpl->mWidthPolicy, SIZE_POLICY_STRING_TABLE, SIZE_POLICY_STRING_TABLE_COUNT ) );
          break;
        }

        case Toolkit::Control::PROPERTY_HEIGHT_POLICY:
        {
          value = std::string( Scripting::GetEnumerationName< Toolkit::Control::SizePolicy >( controlImpl.mImpl->mHeightPolicy, SIZE_POLICY_STRING_TABLE, SIZE_POLICY_STRING_TABLE_COUNT ) );
          break;
        }

        case Toolkit::Control::PROPERTY_MINIMUM_SIZE:
        {
          value = controlImpl.mImpl->mMinimumSize;
          break;
        }

        case Toolkit::Control::PROPERTY_MAXIMUM_SIZE:
        {
          value = controlImpl.mImpl->mMaximumSize;
          break;
        }

        case Toolkit::Control::PROPERTY_KEY_INPUT_FOCUS:
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
  Background* mBackground;           ///< Only create the background if we use it
  ConnectionTracker mConnectionTracker; // signal connection tracker
  Toolkit::Control::KeyEventSignalV2 mKeyEventSignalV2;

  // Gesture Detection
  PinchGestureDetector     mPinchGestureDetector;
  PanGestureDetector       mPanGestureDetector;
  TapGestureDetector       mTapGestureDetector;
  LongPressGestureDetector mLongPressGestureDetector;
  Vector3 mStartingPinchScale;       ///< The scale when a pinch gesture starts

  Vector3 mSize;                     ///< Stores the current control's size.
  Vector3 mSetSize;                  ///< Always stores the size set through the Actor's API. Useful when reset to the initial size is needed.
  Vector3 mMinimumSize;              ///< Stores the control's minimum size.
  Vector3 mMaximumSize;              ///< Stores the control's maximum size.

  bool mLockSetSize;                 ///< Used to avoid. Can't be a bitfield as a reference to this member is used in SetSizeLock helper class.

  Toolkit::Control::SizePolicy mWidthPolicy:3;  ///< Stores the width policy. 3 bits covers 8 values
  Toolkit::Control::SizePolicy mHeightPolicy:3; ///< Stores the height policy. 3 bits covers 8 values
  ControlBehaviour mFlags:4;           ///< Flags passed in from constructor. Need to increase this size when new enums are added
  bool mIsKeyboardNavigationSupported:1;  ///< Stores whether keyboard navigation is supported by the control.
  bool mIsKeyboardFocusGroup:1;        ///< Stores whether the control is a focus group.
  bool mInitialized:1;

  // Properties - these need to be members of Internal::Control::Impl as they need to functions within this class.
  static PropertyRegistration PROPERTY_1;
  static PropertyRegistration PROPERTY_2;
  static PropertyRegistration PROPERTY_3;
  static PropertyRegistration PROPERTY_4;
  static PropertyRegistration PROPERTY_5;
  static PropertyRegistration PROPERTY_6;
  static PropertyRegistration PROPERTY_7;
};

PropertyRegistration Control::Impl::PROPERTY_1( CONTROL_TYPE, "background-color", Toolkit::Control::PROPERTY_BACKGROUND_COLOR, Property::VECTOR4, &Control::Impl::SetProperty, &Control::Impl::GetProperty );
PropertyRegistration Control::Impl::PROPERTY_2( CONTROL_TYPE, "background",       Toolkit::Control::PROPERTY_BACKGROUND,       Property::MAP,     &Control::Impl::SetProperty, &Control::Impl::GetProperty );
PropertyRegistration Control::Impl::PROPERTY_3( CONTROL_TYPE, "width-policy",     Toolkit::Control::PROPERTY_WIDTH_POLICY,     Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty );
PropertyRegistration Control::Impl::PROPERTY_4( CONTROL_TYPE, "height-policy",    Toolkit::Control::PROPERTY_HEIGHT_POLICY,    Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty );
PropertyRegistration Control::Impl::PROPERTY_5( CONTROL_TYPE, "minimum-size",     Toolkit::Control::PROPERTY_MINIMUM_SIZE,     Property::VECTOR3, &Control::Impl::SetProperty, &Control::Impl::GetProperty );
PropertyRegistration Control::Impl::PROPERTY_6( CONTROL_TYPE, "maximum-size",     Toolkit::Control::PROPERTY_MAXIMUM_SIZE,     Property::VECTOR3, &Control::Impl::SetProperty, &Control::Impl::GetProperty );
PropertyRegistration Control::Impl::PROPERTY_7( CONTROL_TYPE, "key-input-focus",  Toolkit::Control::PROPERTY_KEY_INPUT_FOCUS,  Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty );

Toolkit::Control Control::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<Control> controlImpl = new Control( CONTROL_BEHAVIOUR_NONE );

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

void Control::Initialize()
{

  // Calling deriving classes
  OnInitialize();

  if( mImpl->mFlags & REQUIRES_STYLE_CHANGE_SIGNALS )
  {
    Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();

    // Register for style changes
    styleManager.StyleChangeSignal().Connect( this, &ControlImpl::DoStyleChange );
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

    meshActor.SetAffectedByLighting( false );
    SetupBackgroundActor( meshActor, Actor::SCALE, color );

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

void Control::SetBackground( Image image )
{
  Background& background( mImpl->GetBackground() );

  if ( background.actor )
  {
    // Remove Current actor, unset AFTER removal so that we do not inform deriving classes
    Self().Remove( background.actor );
    background.actor = NULL;
  }

  ImageActor imageActor = ImageActor::New( image );
  SetupBackgroundActor( imageActor, Actor::SIZE, background.color );

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

void Control::OnThemeChange( Toolkit::StyleManager styleManager )
{
  GetImpl( styleManager ).ApplyThemeStyle( GetOwner() );
}

void Control::OnPinch(PinchGesture pinch)
{
  if (pinch.state == Gesture::Started)
  {
    mImpl->mStartingPinchScale = Self().GetCurrentScale();
  }

  Self().SetScale(mImpl->mStartingPinchScale * pinch.scale);
}

void Control::OnStageConnection()
{
  RelayoutRequest();

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

  // Request for relayout as we may need to position the new child and old ones
  RelayoutRequest();

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

  // Request for relayout as we may need to re-position the old child
  RelayoutRequest();

  // Notify derived classes.
  OnControlChildRemove( child );
}

void Control::OnSizeSet(const Vector3& targetSize)
{
  if( ( !mImpl->mLockSetSize ) && ( targetSize != mImpl->mSetSize ) )
  {
    // Only updates size if set through Actor's API
    mImpl->mSetSize = targetSize;
  }

  if( targetSize != mImpl->mSize )
  {
    // Update control size.
    mImpl->mSize = targetSize;

    // Notify derived classes.
    OnControlSizeSet( targetSize );
  }
}

void Control::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
  // Do Nothing
}

bool Control::OnTouchEvent(const TouchEvent& event)
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

void Control::OnKeyInputFocusGained()
{
  // Do Nothing
}

void Control::OnKeyInputFocusLost()
{
  // Do Nothing
}

Actor Control::GetChildByAlias(const std::string& actorAlias)
{
  return Actor();
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


void Control::SetKeyboardNavigationSupport(bool isSupported)
{
  mImpl->mIsKeyboardNavigationSupported = isSupported;
}

bool Control::IsKeyboardNavigationSupported()
{
  return mImpl->mIsKeyboardNavigationSupported;
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

bool Control::DoAction(BaseObject* object, const std::string& actionName, const std::vector<Property::Value>& attributes)
{
  bool ret = false;

  if( object && (actionName == Toolkit::Control::ACTION_CONTROL_ACTIVATED) )
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

void Control::DoStyleChange( Toolkit::StyleManager styleManager, StyleChange change )
{
  if( change.themeChange )
  {
    OnThemeChange( styleManager );
  }
  else if( change.defaultFontChange || change.defaultFontSizeChange )
  {
    // This OnStyleChange(StyleChange change ) is deprecated, use OnFontChange instead
    OnStyleChange( change );

    OnFontChange( change.defaultFontChange, change.defaultFontSizeChange );
  }
}

Toolkit::Control::KeyEventSignalV2& Control::KeyEventSignal()
{
  return mImpl->mKeyEventSignalV2;
}

void Control::SetSizePolicy( Toolkit::Control::SizePolicy widthPolicy, Toolkit::Control::SizePolicy heightPolicy )
{
  bool relayoutRequest( false );

  if ( ( mImpl->mWidthPolicy != widthPolicy ) || ( mImpl->mHeightPolicy != heightPolicy ) )
  {
    relayoutRequest = true;
  }

  mImpl->mWidthPolicy = widthPolicy;
  mImpl->mHeightPolicy = heightPolicy;

  // Ensure RelayoutRequest is called AFTER new policies have been set.
  if ( relayoutRequest )
  {
    RelayoutRequest();
  }
}

void Control::GetSizePolicy( Toolkit::Control::SizePolicy& widthPolicy, Toolkit::Control::SizePolicy& heightPolicy ) const
{
  widthPolicy = mImpl->mWidthPolicy;
  heightPolicy = mImpl->mHeightPolicy;
}

void Control::SetMinimumSize( const Vector3& size )
{
  if ( mImpl->mMinimumSize != size )
  {
    mImpl->mMinimumSize = size;

    // Only relayout if our control is using the minimum or range policy.
    if ( ( mImpl->mHeightPolicy == Toolkit::Control::Minimum ) || ( mImpl->mWidthPolicy  == Toolkit::Control::Minimum ) ||
         ( mImpl->mHeightPolicy == Toolkit::Control::Range   ) || ( mImpl->mWidthPolicy  == Toolkit::Control::Range   ) )
    {
      RelayoutRequest();
    }
  }
}

const Vector3& Control::GetMinimumSize() const
{
  return mImpl->mMinimumSize;
}

void Control::SetMaximumSize( const Vector3& size )
{
  if ( mImpl->mMaximumSize != size )
  {
    mImpl->mMaximumSize = size;

    // Only relayout if our control is using the maximum or range policy.
    if ( ( mImpl->mHeightPolicy == Toolkit::Control::Maximum ) || ( mImpl->mWidthPolicy  == Toolkit::Control::Maximum ) ||
         ( mImpl->mHeightPolicy == Toolkit::Control::Range   ) || ( mImpl->mWidthPolicy  == Toolkit::Control::Range   ) )
    {
      RelayoutRequest();
    }
  }
}

const Vector3& Control::GetMaximumSize() const
{
  return mImpl->mMaximumSize;
}

Vector3 Control::GetNaturalSize()
{
  // could be overridden in derived classes.
  return mImpl->mSetSize;
}

float Control::GetHeightForWidth( float width )
{
  // could be overridden in derived classes.
  float height( 0.0f );
  if ( mImpl->mSetSize.width > 0.0f )
  {
    height = mImpl->mSetSize.height * width / mImpl->mSetSize.width;
  }
  return height;
}

float Control::GetWidthForHeight( float height )
{
  // could be overridden in derived classes.
  float width( 0.0f );
  if ( mImpl->mSetSize.height > 0.0f )
  {
    width = mImpl->mSetSize.width * height / mImpl->mSetSize.height;
  }
  return width;
}

const Vector3& Control::GetControlSize() const
{
  return mImpl->mSize;
}

const Vector3& Control::GetSizeSet() const
{
  return mImpl->mSetSize;
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

void Control::RelayoutRequest()
{
  // unfortunate double negative but thats to guarantee new controls get size negotiation
  // by default and have to "opt-out" if they dont want it
  if( !(mImpl->mFlags & NO_SIZE_NEGOTIATION) )
  {
    Internal::RelayoutController::Request();
  }
}

void Control::Relayout( Vector2 size, ActorSizeContainer& container )
{
  // Avoids relayout again when OnSizeSet callback arrives.
  {
    SetSizeLock lock( mImpl->mLockSetSize );
    Self().SetSize( size );
  }

  // Only relayout controls which requested to be relaid out.
  OnRelaidOut( size, container );
}

void Control::Relayout( Actor actor, Vector2 size, ActorSizeContainer& container )
{
  if ( actor )
  {
    Toolkit::Control control( Toolkit::Control::DownCast( actor ) );
    if( control )
    {
      control.GetImplementation().NegotiateSize( size, container );
    }
    else
    {
      container.push_back( ActorSizePair( actor, size ) );
    }
  }
}

void Control::OnRelaidOut( Vector2 size, ActorSizeContainer& container )
{
  unsigned int numChildren = Self().GetChildCount();

  for( unsigned int i=0; i<numChildren; ++i )
  {
    container.push_back( ActorSizePair( Self().GetChildAt(i), size ) );
  }
}

void Control::NegotiateSize( Vector2 allocatedSize, ActorSizeContainer& container )
{
  Vector2 size;

  if ( mImpl->mWidthPolicy == Toolkit::Control::Fixed )
  {
    if ( mImpl->mHeightPolicy == Toolkit::Control::Fixed )
    {
      // If a control says it has a fixed size, then use the size set by the application / control.
      Vector2 setSize( mImpl->mSetSize );
      if ( setSize != Vector2::ZERO )
      {
        size = setSize;

        // Policy is set to Fixed, so if the application / control has not set one of the dimensions,
        // then we should use the natural size of the control rather than the full allocation.
        if ( EqualsZero( size.width ) )
        {
          size.width = GetWidthForHeight( size.height );
        }
        else if ( EqualsZero( size.height ) )
        {
          size.height = GetHeightForWidth( size.width );
        }
      }
      else
      {
        // If that is not set then set the size to the control's natural size
        size = Vector2( GetNaturalSize() );
      }
    }
    else
    {
      // Width is fixed so if the application / control has set it, then use that.
      if ( !EqualsZero( mImpl->mSetSize.width ) )
      {
        size.width = mImpl->mSetSize.width;
      }
      else
      {
        // Otherwise, set the width to what has been allocated.
        size.width = allocatedSize.width;
      }

      // Height is flexible so ask control what the height should be for our width.
      size.height = GetHeightForWidth( size.width );

      // Ensure height is within our policy rules
      size.height = Calculate( mImpl->mHeightPolicy, mImpl->mMinimumSize.height, mImpl->mMaximumSize.height, size.height );
    }
  }
  else
  {
    if ( mImpl->mHeightPolicy == Toolkit::Control::Fixed )
    {
      // Height is fixed so if the application / control has set it, then use that.
      if ( !EqualsZero( mImpl->mSetSize.height ) )
      {
        size.height = mImpl->mSetSize.height;
      }
      else
      {
        // Otherwise, set the height to what has been allocated.
        size.height = allocatedSize.height;
      }

      // Width is flexible so ask control what the width should be for our height.
      size.width = GetWidthForHeight( size.height );

      // Ensure width is within our policy rules
      size.width = Calculate( mImpl->mWidthPolicy, mImpl->mMinimumSize.width, mImpl->mMaximumSize.width, size.width );
    }
    else
    {
      // Width and height are BOTH flexible.
      // Calculate the width and height using the policy rules.
      size.width = Calculate( mImpl->mWidthPolicy, mImpl->mMinimumSize.width, mImpl->mMaximumSize.width, allocatedSize.width );
      size.height = Calculate( mImpl->mHeightPolicy, mImpl->mMinimumSize.height, mImpl->mMaximumSize.height, allocatedSize.height );
    }
  }

  // If the width has not been set, then set to the allocated width.
  // Also if the width set is greater than the allocated, then set to allocated (no exceed support).
  if ( EqualsZero( size.width ) || ( size.width > allocatedSize.width ) )
  {
    size.width = allocatedSize.width;
  }

  // If the height has not been set, then set to the allocated height.
  // Also if the height set is greater than the allocated, then set to allocated (no exceed support).
  if ( EqualsZero( size.height ) || ( size.height > allocatedSize.height ) )
  {
    size.height = allocatedSize.height;
  }

  DALI_LOG_INFO( gLogFilter, Debug::Verbose,
                 "%p: Natural: [%.2f, %.2f] Allocated: [%.2f, %.2f] Set: [%.2f, %.2f]\n",
                 Self().GetObjectPtr(),
                 GetNaturalSize().x, GetNaturalSize().y,
                 allocatedSize.x, allocatedSize.y,
                 size.x, size.y );

  Relayout( size, container );
}

bool Control::EmitKeyEventSignal( const KeyEvent& event )
{
  // Guard against destruction during signal emission
  Dali::Toolkit::Control handle( GetOwner() );

  bool consumed = false;

  // signals are allocated dynamically when someone connects
  if ( !mImpl->mKeyEventSignalV2.Empty() )
  {
    consumed = mImpl->mKeyEventSignalV2.Emit( handle, event );
  }

  if (!consumed)
  {
    // Notification for derived classes
    consumed = OnKeyEvent(event);
  }

  return consumed;
}

void Control::SignalConnected( SlotObserver* slotObserver, CallbackBase* callback )
{
  mImpl->SignalConnected( slotObserver, callback );
}

void Control::SignalDisconnected( SlotObserver* slotObserver, CallbackBase* callback )
{
  mImpl->SignalDisconnected( slotObserver, callback );
}

std::size_t Control::GetConnectionCount() const
{
  return mImpl->GetConnectionCount();
}

Control::Control( bool requiresTouchEvents )
: CustomActorImpl( requiresTouchEvents ),
  mImpl(new Impl(*this))
{
}

Control::Control( ControlBehaviour behaviourFlags )
: CustomActorImpl( behaviourFlags & REQUIRES_TOUCH_EVENTS ),
  mImpl(new Impl(*this))
{
  mImpl->mFlags = behaviourFlags;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
