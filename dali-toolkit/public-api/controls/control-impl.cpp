//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali-toolkit/public-api/controls/control-impl.h>

#include <boost/thread/tss.hpp>
#include <stack>

#include <dali/integration-api/debug.h>

#include "dali-toolkit/internal/controls/style-change-processor.h"
#include "dali-toolkit/internal/controls/relayout-controller.h"
#include "dali-toolkit/internal/controls/relayout-helper.h"
#include "dali-toolkit/public-api/focus-manager/keyinput-focus-manager.h"
#include "dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h"
#include <dali-toolkit/public-api/controls/control.h>

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

BaseHandle Create()
{
  return ControlImpl::New();
}

TypeRegistration CONTROL_TYPE( typeid(Control), typeid(CustomActor), Create );

TypeAction ACTION_TYPE_1(CONTROL_TYPE, Toolkit::Control::ACTION_CONTROL_ACTIVATED, &ControlImpl::DoAction);

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

} // unnamed namespace

class ControlImpl::Impl : public ConnectionTrackerInterface
{
public:
  // Construction & Destruction
  Impl(ControlImpl& controlImpl)
  : mControlImpl(controlImpl),
    mInitialized( false ),
    mPinchGestureDetector(),
    mPanGestureDetector(),
    mTapGestureDetector(),
    mLongPressGestureDetector(),
    mStartingPinchScale(),
    mLockSetSize( false ),
    mWidthPolicy( Control::Fixed ),
    mHeightPolicy( Control::Fixed ),
    mSize(),
    mSetSize(),
    mMinimumSize(),
    mMaximumSize( MAX_FLOAT_VALUE, MAX_FLOAT_VALUE, MAX_FLOAT_VALUE ),
    mIsKeyboardNavigationSupported(false),
    mIsKeyboardFocusGroup(false),
    mKeyEventSignalV2()
  {
  }

  ~Impl()
  {
    // All gesture detectors will be destroyed so no need to disconnect.
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

  // Data

  ControlImpl& mControlImpl;

  bool mInitialized:1;

  ConnectionTracker mConnectionTracker; // signal connection tracker

  // Gesture Detection

  PinchGestureDetector     mPinchGestureDetector;
  PanGestureDetector       mPanGestureDetector;
  TapGestureDetector       mTapGestureDetector;
  LongPressGestureDetector mLongPressGestureDetector;

  Vector3 mStartingPinchScale;       ///< The scale when a pinch gesture starts

  // Relayout and size negotiation

  bool mLockSetSize;                 ///< Used to avoid. Can't be a bitfield as a reference to this member is used in SetSizeLock helper class.

  Control::SizePolicy mWidthPolicy;  ///< Stores the width policy.
  Control::SizePolicy mHeightPolicy; ///< Stores the height policy.

  Vector3 mSize;                     ///< Stores the current control's size.
  Vector3 mSetSize;                  ///< Always stores the size set through the Actor's API. Useful when reset to the initial size is needed.
  Vector3 mMinimumSize;              ///< Stores the control's minimum size.
  Vector3 mMaximumSize;              ///< Stores the control's maximum size.

  bool mIsKeyboardNavigationSupported;  ///< Stores whether keyboard navigation is supported by the control.
  bool mIsKeyboardFocusGroup;        ///< Stores whether the control is a focus group.

  Toolkit::Control::KeyEventSignalV2 mKeyEventSignalV2;
};

Control ControlImpl::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<ControlImpl> controlImpl = new ControlImpl( false );

  // Pass ownership to handle
  Control handle( *controlImpl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  controlImpl->Initialize();

  return handle;
}

ControlImpl::~ControlImpl()
{
  if( mImpl->mInitialized )
  {
    // Unregister only if control has been initialized.
    Internal::StyleChangeProcessor::Unregister( this );
  }
  delete mImpl;
}

void ControlImpl::Initialize()
{
  // Register with the style change processor so we are informed when the default style changes
  Internal::StyleChangeProcessor::Register( this );

  // Calling deriving classes
  OnInitialize();

  mImpl->mInitialized = true;
}

void ControlImpl::EnableGestureDetection(Gesture::Type type)
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

void ControlImpl::DisableGestureDetection(Gesture::Type type)
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

PinchGestureDetector ControlImpl::GetPinchGestureDetector() const
{
  return mImpl->mPinchGestureDetector;
}

PanGestureDetector ControlImpl::GetPanGestureDetector() const
{
  return mImpl->mPanGestureDetector;
}

TapGestureDetector ControlImpl::GetTapGestureDetector() const
{
  return mImpl->mTapGestureDetector;
}

LongPressGestureDetector ControlImpl::GetLongPressGestureDetector() const
{
  return mImpl->mLongPressGestureDetector;
}

void ControlImpl::OnPinch(PinchGesture pinch)
{
  if (pinch.state == Gesture::Started)
  {
    mImpl->mStartingPinchScale = Self().GetCurrentScale();
  }

  Self().SetScale(mImpl->mStartingPinchScale * pinch.scale);
}

void ControlImpl::OnStageConnection()
{
  RelayoutRequest();

  // Notify derived classes.
  OnControlStageConnection();
}

void ControlImpl::OnStageDisconnection()
{
  // Notify derived classes
  OnControlStageDisconnection();
}

void ControlImpl::OnChildAdd(Actor& child)
{
  // Request for relayout.
  RelayoutRequest();

  // Notify derived classes.
  OnControlChildAdd( child );
}

void ControlImpl::OnChildRemove(Actor& child)
{
  // Request for relayout.
  RelayoutRequest();

  // Notify derived classes.
  OnControlChildRemove( child );
}

void ControlImpl::OnSizeSet(const Vector3& targetSize)
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

void ControlImpl::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
  // Do Nothing
}

bool ControlImpl::OnTouchEvent(const TouchEvent& event)
{
  return false; // Do not consume
}

bool ControlImpl::OnKeyEvent(const KeyEvent& event)
{
  return false; // Do not consume
}

bool ControlImpl::OnMouseWheelEvent(const MouseWheelEvent& event)
{
  return false; // Do not consume
}

void ControlImpl::OnKeyInputFocusGained()
{
  // Do Nothing
}

void ControlImpl::OnKeyInputFocusLost()
{
  // Do Nothing
}

Actor ControlImpl::GetChildByAlias(const std::string& actorAlias)
{
  return Actor();
}

bool ControlImpl::OnAccessibilityPan(PanGesture gesture)
{
  return false; // Accessibility pan gesture is not handled by default
}

bool ControlImpl::OnAccessibilityValueChange(bool isIncrease)
{
  return false; // Accessibility value change action is not handled by default
}


void ControlImpl::SetKeyboardNavigationSupport(bool isSupported)
{
  mImpl->mIsKeyboardNavigationSupported = isSupported;
}

bool ControlImpl::IsKeyboardNavigationSupported()
{
  return mImpl->mIsKeyboardNavigationSupported;
}

void ControlImpl::SetAsKeyboardFocusGroup(bool isFocusGroup)
{
  mImpl->mIsKeyboardFocusGroup = isFocusGroup;

  // The following line will be removed when the deprecated API in KeyboardFocusManager is deleted
  KeyboardFocusManager::Get().SetAsFocusGroup(Self(), isFocusGroup);
}

bool ControlImpl::IsKeyboardFocusGroup()
{
  return KeyboardFocusManager::Get().IsFocusGroup(Self());
}

Actor ControlImpl::GetNextKeyboardFocusableActor(Actor currentFocusedActor, Control::KeyboardFocusNavigationDirection direction, bool loopEnabled)
{
  return Actor();
}

bool ControlImpl::DoAction(BaseObject* object, const std::string& actionName, const std::vector<Property::Value>& attributes)
{
  bool ret = false;

  return ret;
}

void ControlImpl::DoActivatedAction(const PropertyValueContainer& attributes)
{
  OnActivated();
}

Toolkit::Control::KeyEventSignalV2& ControlImpl::KeyEventSignal()
{
  return mImpl->mKeyEventSignalV2;
}

void ControlImpl::SetSizePolicy( Control::SizePolicy widthPolicy, Control::SizePolicy heightPolicy )
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

void ControlImpl::GetSizePolicy( Control::SizePolicy& widthPolicy, Control::SizePolicy& heightPolicy ) const
{
  widthPolicy = mImpl->mWidthPolicy;
  heightPolicy = mImpl->mHeightPolicy;
}

void ControlImpl::SetMinimumSize( const Vector3& size )
{
  if ( mImpl->mMinimumSize != size )
  {
    mImpl->mMinimumSize = size;

    // Only relayout if our control is using the minimum or range policy.
    if ( ( mImpl->mHeightPolicy == Control::Minimum ) || ( mImpl->mWidthPolicy  == Control::Minimum ) ||
         ( mImpl->mHeightPolicy == Control::Range   ) || ( mImpl->mWidthPolicy  == Control::Range   ) )
    {
      RelayoutRequest();
    }
  }
}

const Vector3& ControlImpl::GetMinimumSize() const
{
  return mImpl->mMinimumSize;
}

void ControlImpl::SetMaximumSize( const Vector3& size )
{
  if ( mImpl->mMaximumSize != size )
  {
    mImpl->mMaximumSize = size;

    // Only relayout if our control is using the maximum or range policy.
    if ( ( mImpl->mHeightPolicy == Control::Maximum ) || ( mImpl->mWidthPolicy  == Control::Maximum ) ||
         ( mImpl->mHeightPolicy == Control::Range   ) || ( mImpl->mWidthPolicy  == Control::Range   ) )
    {
      RelayoutRequest();
    }
  }
}

const Vector3& ControlImpl::GetMaximumSize() const
{
  return mImpl->mMaximumSize;
}

Vector3 ControlImpl::GetNaturalSize()
{
  // could be overridden in derived classes.
  return mImpl->mSetSize;
}

float ControlImpl::GetHeightForWidth( float width )
{
  // could be overridden in derived classes.
  float height( 0.0f );
  if ( mImpl->mSetSize.width > 0.0f )
  {
    height = mImpl->mSetSize.height * width / mImpl->mSetSize.width;
  }
  return height;
}

float ControlImpl::GetWidthForHeight( float height )
{
  // could be overridden in derived classes.
  float width( 0.0f );
  if ( mImpl->mSetSize.height > 0.0f )
  {
    width = mImpl->mSetSize.width * height / mImpl->mSetSize.height;
  }
  return width;
}

const Vector3& ControlImpl::GetControlSize() const
{
  return mImpl->mSize;
}

const Vector3& ControlImpl::GetSizeSet() const
{
  return mImpl->mSetSize;
}

void ControlImpl::SetKeyInputFocus()
{
  if( Self().OnStage() )
  {
    KeyInputFocusManager::Get().SetFocus(Control::DownCast(Self()));
  }
}

bool ControlImpl::HasKeyInputFocus()
{
  bool result = false;
  if( Self().OnStage() )
  {
    result = KeyInputFocusManager::Get().IsKeyboardListener(Control::DownCast(Self()));
  }
  return result;
}

void ControlImpl::ClearKeyInputFocus()
{
  if( Self().OnStage() )
  {
    KeyInputFocusManager::Get().RemoveFocus(Control::DownCast(Self()));
  }
}

void ControlImpl::RelayoutRequest()
{
  Internal::RelayoutController::Get().Request();
}

void ControlImpl::Relayout( Vector2 size, ActorSizeContainer& container )
{
  // Avoids relayout again when OnSizeSet callback arrives.
  {
    SetSizeLock lock( mImpl->mLockSetSize );
    Self().SetSize( size );
  }

  // Only relayout controls which requested to be relaid out.
  OnRelaidOut( size, container );
}

void ControlImpl::Relayout( Actor actor, Vector2 size, ActorSizeContainer& container )
{
  if ( actor )
  {
    Control control( Control::DownCast( actor ) );
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

void ControlImpl::OnRelaidOut( Vector2 size, ActorSizeContainer& container )
{
  unsigned int numChildren = Self().GetChildCount();

  for( unsigned int i=0; i<numChildren; ++i )
  {
    container.push_back( ActorSizePair( Self().GetChildAt(i), size ) );
  }
}

void ControlImpl::NegotiateSize( Vector2 allocatedSize, ActorSizeContainer& container )
{
  Vector2 size;

  if ( mImpl->mWidthPolicy == Control::Fixed )
  {
    if ( mImpl->mHeightPolicy == Control::Fixed )
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
    if ( mImpl->mHeightPolicy == Control::Fixed )
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

bool ControlImpl::EmitKeyEventSignal( const KeyEvent& event )
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

void ControlImpl::SignalConnected( SlotObserver* slotObserver, CallbackBase* callback )
{
  mImpl->SignalConnected( slotObserver, callback );
}

void ControlImpl::SignalDisconnected( SlotObserver* slotObserver, CallbackBase* callback )
{
  mImpl->SignalDisconnected( slotObserver, callback );
}

std::size_t ControlImpl::GetConnectionCount() const
{
  return mImpl->GetConnectionCount();
}

ControlImpl::ControlImpl( bool requiresTouchEvents )
: CustomActorImpl( requiresTouchEvents ),
  mImpl(new Impl(*this))
{
}

} // namespace Toolkit

} // namespace Dali
