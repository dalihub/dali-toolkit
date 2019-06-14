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
#include "keyboard-focus-manager-impl.h"

// EXTERNAL INCLUDES
#include <cstring> // for strcmp
#include <dali/public-api/actors/layer.h>
#include <dali/devel-api/adaptor-framework/accessibility-adaptor.h>
#include <dali/devel-api/adaptor-framework/singleton-service.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/events/touch-data.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/accessibility-manager/accessibility-manager.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/public-api/styling/style-manager.h>
#include <dali-toolkit/devel-api/styling/style-manager-devel.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace // Unnamed namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_KEYBOARD_FOCUS_MANAGER");
#endif

const char* const IS_FOCUS_GROUP_PROPERTY_NAME = "isKeyboardFocusGroup"; // This property will be replaced by a flag in Control.

const char* const FOCUS_BORDER_IMAGE_PATH = DALI_IMAGE_DIR "keyboard_focus.9.png";

BaseHandle Create()
{
  BaseHandle handle = KeyboardFocusManager::Get();

  if ( !handle )
  {
    SingletonService singletonService( SingletonService::Get() );
    if ( singletonService )
    {
      Toolkit::KeyboardFocusManager manager = Toolkit::KeyboardFocusManager( new Internal::KeyboardFocusManager() );
      singletonService.Register( typeid( manager ), manager );
      handle = manager;
    }
  }

  return handle;
}

DALI_TYPE_REGISTRATION_BEGIN_CREATE( Toolkit::KeyboardFocusManager, Dali::BaseHandle, Create, true )

DALI_SIGNAL_REGISTRATION( Toolkit, KeyboardFocusManager, "keyboardPreFocusChange",           SIGNAL_PRE_FOCUS_CHANGE )
DALI_SIGNAL_REGISTRATION( Toolkit, KeyboardFocusManager, "keyboardFocusChanged",             SIGNAL_FOCUS_CHANGED )
DALI_SIGNAL_REGISTRATION( Toolkit, KeyboardFocusManager, "keyboardFocusGroupChanged",        SIGNAL_FOCUS_GROUP_CHANGED )
DALI_SIGNAL_REGISTRATION( Toolkit, KeyboardFocusManager, "keyboardFocusedActorEnterKey",     SIGNAL_FOCUSED_ACTOR_ENTER_KEY )

DALI_TYPE_REGISTRATION_END()

const unsigned int MAX_HISTORY_AMOUNT = 30; ///< Max length of focus history stack

} // unnamed namespace

Toolkit::KeyboardFocusManager KeyboardFocusManager::Get()
{
  Toolkit::KeyboardFocusManager manager;

  SingletonService singletonService( SingletonService::Get() );
  if ( singletonService )
  {
    // Check whether the keyboard focus manager is already created
    Dali::BaseHandle handle = singletonService.GetSingleton( typeid( Toolkit::KeyboardFocusManager ) );
    if(handle)
    {
      // If so, downcast the handle of singleton to keyboard focus manager
      manager = Toolkit::KeyboardFocusManager( dynamic_cast< KeyboardFocusManager* >( handle.GetObjectPtr() ) );
    }
  }

  return manager;
}

KeyboardFocusManager::KeyboardFocusManager()
: mPreFocusChangeSignal(),
  mFocusChangedSignal(),
  mFocusGroupChangedSignal(),
  mFocusedActorEnterKeySignal(),
  mCurrentFocusActor(),
  mFocusIndicatorActor(),
  mIsFocusIndicatorShown( -1 ),
  mFocusGroupLoopEnabled( false ),
  mIsWaitingKeyboardFocusChangeCommit( false ),
  mClearFocusOnTouch( true ),
  mEnableFocusIndicator( true ),
  mAlwaysShowIndicator( true ),
  mFocusHistory(),
  mSlotDelegate( this ),
  mCustomAlgorithmInterface(NULL)
{
  // TODO: Get FocusIndicatorEnable constant from stylesheet to set mIsFocusIndicatorShown.
  Stage::GetCurrent().KeyEventSignal().Connect( mSlotDelegate, &KeyboardFocusManager::OnKeyEvent);
  Stage::GetCurrent().TouchSignal().Connect( mSlotDelegate, &KeyboardFocusManager::OnTouch );
}

KeyboardFocusManager::~KeyboardFocusManager()
{
}

void KeyboardFocusManager::GetConfigurationFromStyleManger()
{
    Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();
    if( styleManager )
    {
      Property::Map config = Toolkit::DevelStyleManager::GetConfigurations( styleManager );
      mAlwaysShowIndicator = config["alwaysShowFocus"].Get<bool>();
      mIsFocusIndicatorShown = static_cast<int>(mAlwaysShowIndicator);
      mClearFocusOnTouch = mIsFocusIndicatorShown ? false : true;
    }
}

bool KeyboardFocusManager::SetCurrentFocusActor( Actor actor )
{
  DALI_ASSERT_DEBUG( !mIsWaitingKeyboardFocusChangeCommit && "Calling this function in the PreFocusChangeSignal callback?" );

  if( mIsFocusIndicatorShown == -1 )
  {
    GetConfigurationFromStyleManger();
  }

  return DoSetCurrentFocusActor( actor );
}

bool KeyboardFocusManager::DoSetCurrentFocusActor( Actor actor )
{
  bool success = false;

  Actor currentFocusedActor = GetCurrentFocusActor();

  // If developer set focus on same actor, doing nothing
  if( actor == currentFocusedActor )
  {
    if( !actor )
    {
      return false;
    }
    return true;
  }

  // Check whether the actor is in the stage and is keyboard focusable.
  if( actor && actor.IsKeyboardFocusable() && actor.OnStage() )
  {
    if( mIsFocusIndicatorShown && mEnableFocusIndicator )
    {
      actor.Add( GetFocusIndicatorActor() );
    }

    // Send notification for the change of focus actor
    if( !mFocusChangedSignal.Empty() )
    {
      mFocusChangedSignal.Emit(currentFocusedActor, actor);
    }

    Toolkit::Control currentlyFocusedControl = Toolkit::Control::DownCast(currentFocusedActor);
    if( currentlyFocusedControl )
    {
      // Do we need it to remember if it was previously DISABLED?
      currentlyFocusedControl.SetProperty(DevelControl::Property::STATE, DevelControl::NORMAL );
      currentlyFocusedControl.ClearKeyInputFocus();
    }

    DALI_LOG_INFO( gLogFilter, Debug::General, "[%s:%d] Focus Changed\n", __FUNCTION__, __LINE__);

    // Save the current focused actor
    mCurrentFocusActor = actor;

    Toolkit::Control newlyFocusedControl = Toolkit::Control::DownCast(actor);
    if( newlyFocusedControl )
    {
      newlyFocusedControl.SetProperty(DevelControl::Property::STATE, DevelControl::FOCUSED );
      newlyFocusedControl.SetKeyInputFocus();
    }

    // Push Current Focused Actor to FocusHistory
    mFocusHistory.push_back( actor );

    // Delete first element before add new element when Stack is full.
    if( mFocusHistory.size() > MAX_HISTORY_AMOUNT )
    {
       FocusStackIterator beginPos = mFocusHistory.begin();
       mFocusHistory.erase( beginPos );
    }

    DALI_LOG_INFO( gLogFilter, Debug::General, "[%s:%d] SUCCEED\n", __FUNCTION__, __LINE__);
    success = true;
  }
  else
  {
    DALI_LOG_WARNING("[%s:%d] FAILED\n", __FUNCTION__, __LINE__);
  }

  return success;
}

Actor KeyboardFocusManager::GetCurrentFocusActor()
{
  Actor actor = mCurrentFocusActor.GetHandle();
  if( actor && ! actor.OnStage() )
  {
    // If the actor has been removed from the stage, then it should not be focused

    actor.Reset();
    mCurrentFocusActor.Reset();
  }
  return actor;
}

Actor KeyboardFocusManager::GetCurrentFocusGroup()
{
  return GetFocusGroup(GetCurrentFocusActor());
}

void KeyboardFocusManager::MoveFocusBackward()
{
  // Find Pre Focused Actor when the list size is more than 1
  if( mFocusHistory.size() > 1 )
  {
    // Delete current focused actor in history
    mFocusHistory.pop_back();

    // If pre-focused actors are not on stage or deleted, remove them in stack
    while( mFocusHistory.size() > 0 )
    {
      // Get pre focused actor
      Actor target = mFocusHistory[ mFocusHistory.size() -1 ].GetHandle();

      // Impl of Actor is not null
      if( target && target.OnStage() )
      {
        // Delete pre focused actor in history because it will pushed again by SetCurrentFocusActor()
        mFocusHistory.pop_back();
        SetCurrentFocusActor( target );
        break;
      }
      else
      {
        // Target is empty handle or off stage. Erase from queue
        mFocusHistory.pop_back();
      }
    }

    // if there is no actor which can get focus, then push current focus actor in stack again
    if( mFocusHistory.size() == 0 )
    {
      Actor currentFocusedActor = GetCurrentFocusActor();
      mFocusHistory.push_back( currentFocusedActor );
    }
  }
}

bool KeyboardFocusManager::IsLayoutControl(Actor actor) const
{
  Toolkit::Control control = Toolkit::Control::DownCast(actor);
  return control && GetImplementation( control ).IsKeyboardNavigationSupported();
}

Toolkit::Control KeyboardFocusManager::GetParentLayoutControl(Actor actor) const
{
  // Get the actor's parent layout control that supports two dimensional keyboard navigation
  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  Actor parent;
  if(actor)
  {
    parent = actor.GetParent();
  }

  while( parent && !IsLayoutControl(parent) && parent != rootActor )
  {
    parent = parent.GetParent();
  }

  return Toolkit::Control::DownCast(parent);
}

bool KeyboardFocusManager::MoveFocus(Toolkit::Control::KeyboardFocus::Direction direction)
{
  Actor currentFocusActor = GetCurrentFocusActor();

  bool succeed = false;

  // Go through the actor's hierarchy until we find a layout control that knows how to move the focus
  Toolkit::Control parentLayoutControl = GetParentLayoutControl( currentFocusActor );
  while( parentLayoutControl && !succeed )
  {
    succeed = DoMoveFocusWithinLayoutControl( parentLayoutControl, currentFocusActor, direction );
    parentLayoutControl = GetParentLayoutControl( parentLayoutControl );
  }

  if( !succeed )
  {
    Actor nextFocusableActor;

    Toolkit::Control currentFocusControl = Toolkit::Control::DownCast(currentFocusActor);

    // If the current focused actor is a control, then find the next focusable actor via the focusable properties.
    if( currentFocusControl )
    {
      int actorId = -1;
      Property::Index index = Property::INVALID_INDEX;
      Property::Value value;

      // Find property index based upon focus direction
      switch ( direction )
      {
        case Toolkit::Control::KeyboardFocus::LEFT:
        {
          index = Toolkit::DevelControl::Property::LEFT_FOCUSABLE_ACTOR_ID;
          break;
        }
        case Toolkit::Control::KeyboardFocus::RIGHT:
        {
          index = Toolkit::DevelControl::Property::RIGHT_FOCUSABLE_ACTOR_ID;
          break;
        }
        case Toolkit::Control::KeyboardFocus::UP:
        {
          index = Toolkit::DevelControl::Property::UP_FOCUSABLE_ACTOR_ID;
          break;
        }
        case Toolkit::Control::KeyboardFocus::DOWN:
        {
          index = Toolkit::DevelControl::Property::DOWN_FOCUSABLE_ACTOR_ID;
          break;
        }
        default:
          break;
      }

      // If the focusable property is set then determine next focusable actor
      if( index != Property::INVALID_INDEX)
      {
        value = currentFocusActor.GetProperty( index );
        actorId = value.Get<int>();

        // If actor's id is valid then find actor form actor's id. The actor should be on the stage.
        if( actorId != -1 )
        {
          if( currentFocusActor.GetParent() )
          {
            nextFocusableActor = currentFocusActor.GetParent().FindChildById( actorId );
          }

          if( !nextFocusableActor )
          {
            nextFocusableActor = Stage::GetCurrent().GetRootLayer().FindChildById( actorId );
          }
        }
      }
    }

    if( !nextFocusableActor )
    {
      // If the implementation of CustomAlgorithmInterface is provided then the PreFocusChangeSignal is no longer emitted.
      if( mCustomAlgorithmInterface )
      {
        mIsWaitingKeyboardFocusChangeCommit = true;
        nextFocusableActor = mCustomAlgorithmInterface->GetNextFocusableActor( currentFocusActor, Actor(), direction );
        mIsWaitingKeyboardFocusChangeCommit = false;
      }
      else if( !mPreFocusChangeSignal.Empty() )
      {
        // Don't know how to move the focus further. The application needs to tell us which actor to move the focus to
        mIsWaitingKeyboardFocusChangeCommit = true;
        nextFocusableActor = mPreFocusChangeSignal.Emit( currentFocusActor, Actor(), direction );
        mIsWaitingKeyboardFocusChangeCommit = false;
      }
    }

    if( nextFocusableActor && nextFocusableActor.IsKeyboardFocusable() )
    {
      // Whether the next focusable actor is a layout control
      if( IsLayoutControl( nextFocusableActor ) )
      {
        // If so, move the focus inside it.
        Toolkit::Control layoutControl = Toolkit::Control::DownCast( nextFocusableActor) ;
        succeed = DoMoveFocusWithinLayoutControl( layoutControl, currentFocusActor, direction );
      }
      else
      {
        // Otherwise, just set focus to the next focusable actor
        succeed = SetCurrentFocusActor( nextFocusableActor );
      }
    }
  }

  return succeed;
}

bool KeyboardFocusManager::DoMoveFocusWithinLayoutControl(Toolkit::Control control, Actor actor, Toolkit::Control::KeyboardFocus::Direction direction)
{
  // Ask the control for the next actor to focus
  Actor nextFocusableActor = GetImplementation( control ).GetNextKeyboardFocusableActor(actor, direction, mFocusGroupLoopEnabled);
  if(nextFocusableActor)
  {
    if(!nextFocusableActor.IsKeyboardFocusable())
    {
      // If the actor is not focusable, ask the same layout control for the next actor to focus
      return DoMoveFocusWithinLayoutControl(control, nextFocusableActor, direction);
    }
    else
    {
      Actor currentFocusActor = GetCurrentFocusActor();
      Actor committedFocusActor = nextFocusableActor;

      // We will try to move the focus to the actor. Emit a signal to notify the proposed actor to focus
      // Signal handler can check the proposed actor and return a different actor if it wishes.
      if( !mPreFocusChangeSignal.Empty() )
      {
        mIsWaitingKeyboardFocusChangeCommit = true;
        committedFocusActor = mPreFocusChangeSignal.Emit(currentFocusActor, nextFocusableActor, direction);
        mIsWaitingKeyboardFocusChangeCommit = false;
      }

      if (committedFocusActor && committedFocusActor.IsKeyboardFocusable())
      {
        // Whether the commited focusable actor is a layout control
        if(IsLayoutControl(committedFocusActor))
        {
          // If so, move the focus inside it.
          Toolkit::Control layoutControl = Toolkit::Control::DownCast(committedFocusActor);
          return DoMoveFocusWithinLayoutControl(layoutControl, currentFocusActor, direction);
        }
        else
        {
          // Otherwise, just set focus to the next focusable actor
          if(committedFocusActor == nextFocusableActor)
          {
            // If the application hasn't changed our proposed actor, we informs the layout control we will
            // move the focus to what the control returns. The control might wish to perform some actions
            // before the focus is actually moved.
            GetImplementation( control ).OnKeyboardFocusChangeCommitted( committedFocusActor );
          }

          return SetCurrentFocusActor(committedFocusActor);
        }
      }
      else
      {
        return false;
      }
    }
  }
  else
  {
    // No more actor can be focused in the given direction within the same layout control.
    return false;
  }
}

bool KeyboardFocusManager::DoMoveFocusToNextFocusGroup(bool forward)
{
  bool succeed = false;

  // Get the parent layout control of the current focus group
  Toolkit::Control parentLayoutControl = GetParentLayoutControl(GetCurrentFocusGroup());

  while(parentLayoutControl && !succeed)
  {
    // If the current focus group has a parent layout control, we can probably automatically
    // move the focus to the next focus group in the forward or backward direction.
    Toolkit::Control::KeyboardFocus::Direction direction = forward ? Toolkit::Control::KeyboardFocus::RIGHT : Toolkit::Control::KeyboardFocus::LEFT;
    succeed = DoMoveFocusWithinLayoutControl(parentLayoutControl, GetCurrentFocusActor(), direction);
    parentLayoutControl = GetParentLayoutControl(parentLayoutControl);
  }

  if(!mFocusGroupChangedSignal.Empty())
  {
    // Emit a focus group changed signal. The applicaton can move the focus to a new focus group
    mFocusGroupChangedSignal.Emit(GetCurrentFocusActor(), forward);
  }

  return succeed;
}

void KeyboardFocusManager::DoKeyboardEnter(Actor actor)
{
  if( actor )
  {
    Toolkit::Control control = Toolkit::Control::DownCast( actor );
    if( control )
    {
      // Notify the control that enter has been pressed on it.
      GetImplementation( control ).KeyboardEnter();
    }

    // Send a notification for the actor.
    if( !mFocusedActorEnterKeySignal.Empty() )
    {
      mFocusedActorEnterKeySignal.Emit( actor );
    }
  }
}

void KeyboardFocusManager::ClearFocus()
{
  Actor actor = GetCurrentFocusActor();
  if( actor )
  {
    if( mFocusIndicatorActor )
    {
      actor.Remove( mFocusIndicatorActor );
    }

    // Send notification for the change of focus actor
    if( !mFocusChangedSignal.Empty() )
    {
      mFocusChangedSignal.Emit( actor, Actor() );
    }

    Toolkit::Control currentlyFocusedControl = Toolkit::Control::DownCast( actor );
    if( currentlyFocusedControl )
    {
      currentlyFocusedControl.SetProperty( DevelControl::Property::STATE, DevelControl::NORMAL );
      currentlyFocusedControl.ClearKeyInputFocus();
    }
  }

  mCurrentFocusActor.Reset();
  mIsFocusIndicatorShown = static_cast<int>(mAlwaysShowIndicator);
}

void KeyboardFocusManager::SetFocusGroupLoop(bool enabled)
{
  mFocusGroupLoopEnabled = enabled;
}

bool KeyboardFocusManager::GetFocusGroupLoop() const
{
  return mFocusGroupLoopEnabled;
}

void KeyboardFocusManager::SetAsFocusGroup(Actor actor, bool isFocusGroup)
{
  if(actor)
  {
    // Create/Set focus group property.
    actor.RegisterProperty( IS_FOCUS_GROUP_PROPERTY_NAME, isFocusGroup, Property::READ_WRITE );
  }
}

bool KeyboardFocusManager::IsFocusGroup(Actor actor) const
{
  // Check whether the actor is a focus group
  bool isFocusGroup = false;

  if(actor)
  {
    Property::Index propertyIsFocusGroup = actor.GetPropertyIndex(IS_FOCUS_GROUP_PROPERTY_NAME);
    if(propertyIsFocusGroup != Property::INVALID_INDEX)
    {
      isFocusGroup = actor.GetProperty<bool>(propertyIsFocusGroup);
    }
  }

  return isFocusGroup;
}

Actor KeyboardFocusManager::GetFocusGroup(Actor actor)
{
  // Go through the actor's hierarchy to check which focus group the actor belongs to
  while (actor && !IsFocusGroup(actor))
  {
    actor = actor.GetParent();
  }

  return actor;
}

void KeyboardFocusManager::SetFocusIndicatorActor(Actor indicator)
{
  if(mFocusIndicatorActor != indicator)
  {
    Actor currentFocusActor = GetCurrentFocusActor();
    if(currentFocusActor)
    {
      // The new focus indicator should be added to the current focused actor immediately
      if(mFocusIndicatorActor)
      {
        currentFocusActor.Remove(mFocusIndicatorActor);
      }

      if(indicator)
      {
        currentFocusActor.Add(indicator);
      }
    }

    mFocusIndicatorActor = indicator;
  }
}

Actor KeyboardFocusManager::GetFocusIndicatorActor()
{
  if( ! mFocusIndicatorActor )
  {
    // Create the default if it hasn't been set and one that's shared by all the keyboard focusable actors
    mFocusIndicatorActor = Toolkit::ImageView::New( FOCUS_BORDER_IMAGE_PATH );

    // Apply size constraint to the focus indicator
    mFocusIndicatorActor.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  }

  mFocusIndicatorActor.SetParentOrigin( ParentOrigin::CENTER );
  mFocusIndicatorActor.SetAnchorPoint( AnchorPoint::CENTER );
  mFocusIndicatorActor.SetPosition(0.0f, 0.0f);

  return mFocusIndicatorActor;
}

void KeyboardFocusManager::OnKeyEvent(const KeyEvent& event)
{
  AccessibilityAdaptor accessibilityAdaptor = AccessibilityAdaptor::Get();
  bool isAccessibilityEnabled = accessibilityAdaptor.IsEnabled();

  Toolkit::AccessibilityManager accessibilityManager = Toolkit::AccessibilityManager::Get();

  std::string keyName = event.keyPressedName;

  if( mIsFocusIndicatorShown == -1 )
  {
    GetConfigurationFromStyleManger();
  }

  bool isFocusStartableKey = false;

  if(event.state == KeyEvent::Down)
  {
    if (keyName == "Left")
    {
      if(!isAccessibilityEnabled)
      {
        if(!mIsFocusIndicatorShown)
        {
          // Show focus indicator
          mIsFocusIndicatorShown = 1;
        }
        else
        {
          // Move the focus towards left
          MoveFocus(Toolkit::Control::KeyboardFocus::LEFT);
        }

        isFocusStartableKey = true;
      }
      else
      {
        // Move the accessibility focus backward
        accessibilityManager.MoveFocusBackward();
      }
    }
    else if (keyName == "Right")
    {
      if(!isAccessibilityEnabled)
      {
        if(!mIsFocusIndicatorShown)
        {
          // Show focus indicator
          mIsFocusIndicatorShown = 1;
        }
        else
        {
          // Move the focus towards right
          MoveFocus(Toolkit::Control::KeyboardFocus::RIGHT);
        }
      }
      else
      {
        // Move the accessibility focus forward
        accessibilityManager.MoveFocusForward();
      }

      isFocusStartableKey = true;
    }
    else if (keyName == "Up" && !isAccessibilityEnabled)
    {
      if(!mIsFocusIndicatorShown)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = 1;
      }
      else
      {
        // Move the focus towards up
        MoveFocus(Toolkit::Control::KeyboardFocus::UP);
      }

      isFocusStartableKey = true;
    }
    else if (keyName == "Down" && !isAccessibilityEnabled)
    {
      if(!mIsFocusIndicatorShown)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = 1;
      }
      else
      {
        // Move the focus towards down
        MoveFocus(Toolkit::Control::KeyboardFocus::DOWN);
      }

      isFocusStartableKey = true;
    }
    else if (keyName == "Prior" && !isAccessibilityEnabled)
    {
      if(!mIsFocusIndicatorShown)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = 1;
      }
      else
      {
        // Move the focus towards the previous page
        MoveFocus(Toolkit::Control::KeyboardFocus::PAGE_UP);
      }

      isFocusStartableKey = true;
    }
    else if (keyName == "Next" && !isAccessibilityEnabled)
    {
      if(!mIsFocusIndicatorShown)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = 1;
      }
      else
      {
        // Move the focus towards the next page
        MoveFocus(Toolkit::Control::KeyboardFocus::PAGE_DOWN);
      }

      isFocusStartableKey = true;
    }
    else if (keyName == "Tab" && !isAccessibilityEnabled)
    {
      if(!mIsFocusIndicatorShown)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = 1;
      }
      else
      {
        // "Tab" key changes the focus group in the forward direction and
        // "Shift-Tab" key changes it in the backward direction.
        DoMoveFocusToNextFocusGroup(!event.IsShiftModifier());
      }

      isFocusStartableKey = true;
    }
    else if (keyName == "space" && !isAccessibilityEnabled)
    {
      if(!mIsFocusIndicatorShown)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = 1;
      }

      isFocusStartableKey = true;
    }
    else if (keyName == "" && !isAccessibilityEnabled)
    {
      // Check the fake key event for evas-plugin case
      if(!mIsFocusIndicatorShown)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = 1;
      }

      isFocusStartableKey = true;
    }
    else if (keyName == "Backspace" && !isAccessibilityEnabled)
    {
      // Emit signal to go back to the previous view???
    }
    else if (keyName == "Escape" && !isAccessibilityEnabled)
    {
    }
  }
  else if(event.state == KeyEvent::Up)
  {
    if (keyName == "Return")
    {
      if(!mIsFocusIndicatorShown && !isAccessibilityEnabled)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = 1;
      }
      else
      {
        // The focused actor has enter pressed on it
        Actor actor;
        if( !isAccessibilityEnabled )
        {
          actor = GetCurrentFocusActor();
        }
        else
        {
          actor = accessibilityManager.GetCurrentFocusActor();
        }

        if( actor )
        {
          DoKeyboardEnter( actor );
        }
      }

      isFocusStartableKey = true;
    }
  }

  if(isFocusStartableKey && mIsFocusIndicatorShown && !isAccessibilityEnabled)
  {
    Actor actor = GetCurrentFocusActor();
    if( actor )
    {
      if( mEnableFocusIndicator )
      {
        // Make sure the focused actor is highlighted
        actor.Add( GetFocusIndicatorActor() );
      }
    }
    else
    {
      // No actor is focused but keyboard focus is activated by the key press
      // Let's try to move the initial focus
      MoveFocus(Toolkit::Control::KeyboardFocus::RIGHT);
    }
  }
}

void KeyboardFocusManager::OnTouch(const TouchData& touch)
{
  // if mIsFocusIndicatorShown is -1, it means Configuration is not loaded.
  // Try to load configuration.
  if( mIsFocusIndicatorShown == -1 )
  {
    GetConfigurationFromStyleManger();
  }

  // Clear the focus when user touch the screen.
  // We only do this on a Down event, otherwise the clear action may override a manually focused actor.
  // If mClearFocusOnTouch is false, do not clear the focus even if user touch the screen.
  if( (( touch.GetPointCount() < 1 ) || ( touch.GetState( 0 ) == PointState::DOWN )) && mClearFocusOnTouch )
  {
    ClearFocus();
  }
}

Toolkit::KeyboardFocusManager::PreFocusChangeSignalType& KeyboardFocusManager::PreFocusChangeSignal()
{
  return mPreFocusChangeSignal;
}

Toolkit::KeyboardFocusManager::FocusChangedSignalType& KeyboardFocusManager::FocusChangedSignal()
{
  return mFocusChangedSignal;
}

Toolkit::KeyboardFocusManager::FocusGroupChangedSignalType& KeyboardFocusManager::FocusGroupChangedSignal()
{
  return mFocusGroupChangedSignal;
}

Toolkit::KeyboardFocusManager::FocusedActorEnterKeySignalType& KeyboardFocusManager::FocusedActorEnterKeySignal()
{
  return mFocusedActorEnterKeySignal;
}

bool KeyboardFocusManager::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  KeyboardFocusManager* manager = static_cast< KeyboardFocusManager* >( object ); // TypeRegistry guarantees that this is the correct type.

  if( 0 == strcmp( signalName.c_str(), SIGNAL_PRE_FOCUS_CHANGE ) )
  {
    manager->PreFocusChangeSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_FOCUS_CHANGED ) )
  {
    manager->FocusChangedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_FOCUS_GROUP_CHANGED ) )
  {
    manager->FocusGroupChangedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_FOCUSED_ACTOR_ENTER_KEY ) )
  {
    manager->FocusedActorEnterKeySignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void KeyboardFocusManager::SetCustomAlgorithm(CustomAlgorithmInterface& interface)
{
  mCustomAlgorithmInterface = &interface;
}

void KeyboardFocusManager::EnableFocusIndicator(bool enable)
{
  if( !enable && mFocusIndicatorActor )
  {
    mFocusIndicatorActor.Unparent();
  }

  mEnableFocusIndicator = enable;
}

bool KeyboardFocusManager::IsFocusIndicatorEnabled() const
{
  return mEnableFocusIndicator;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
