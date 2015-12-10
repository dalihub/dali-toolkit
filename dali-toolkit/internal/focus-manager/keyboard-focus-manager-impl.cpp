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
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/accessibility-manager/accessibility-manager.h>
#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>

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

DALI_SIGNAL_REGISTRATION( Toolkit, KeyboardFocusManager, "keyboardPreFocusChange",           SIGNAL_PRE_FOCUS_CHANGE        )
DALI_SIGNAL_REGISTRATION( Toolkit, KeyboardFocusManager, "keyboardFocusChanged",             SIGNAL_FOCUS_CHANGED           )
DALI_SIGNAL_REGISTRATION( Toolkit, KeyboardFocusManager, "keyboardFocusGroupChanged",        SIGNAL_FOCUS_GROUP_CHANGED     )
DALI_SIGNAL_REGISTRATION( Toolkit, KeyboardFocusManager, "keyboardFocusedActorEnterKey",     SIGNAL_FOCUSED_ACTOR_ENTER_KEY )

DALI_TYPE_REGISTRATION_END()

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
: mCurrentFocusActor(0),
  mFocusIndicatorActor(Actor()),
  mFocusGroupLoopEnabled(false),
  mIsKeyboardFocusEnabled(false),
  mIsFocusIndicatorEnabled(false),
  mIsWaitingKeyboardFocusChangeCommit(false),
  mSlotDelegate(this)
{
  CreateDefaultFocusIndicatorActor();

  OnPhysicalKeyboardStatusChanged(PhysicalKeyboard::Get());

  Toolkit::KeyInputFocusManager::Get().UnhandledKeyEventSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnKeyEvent);
  Stage::GetCurrent().TouchedSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnTouched);
  PhysicalKeyboard::Get().StatusChangedSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnPhysicalKeyboardStatusChanged);
}

KeyboardFocusManager::~KeyboardFocusManager()
{
}

bool KeyboardFocusManager::SetCurrentFocusActor(Actor actor)
{
  DALI_ASSERT_DEBUG( !mIsWaitingKeyboardFocusChangeCommit && "Calling this function in the PreFocusChangeSignal callback?" );

  if( actor )
  {
    return DoSetCurrentFocusActor( actor.GetId() );
  }

  return false;
}

bool KeyboardFocusManager::DoSetCurrentFocusActor(const unsigned int actorID)
{
  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  Actor actor = rootActor.FindChildById(actorID);

  // Check whether the actor is in the stage
  if(actor)
  {
    // Set the focus only when the actor is keyboard focusable
    if(actor.IsKeyboardFocusable())
    {
      // Draw the focus indicator upon the focused actor
      if(mIsFocusIndicatorEnabled && mFocusIndicatorActor)
      {
        actor.Add(mFocusIndicatorActor);
      }

      // Send notification for the change of focus actor
      if( !mFocusChangedSignal.Empty() )
      {
        mFocusChangedSignal.Emit(GetCurrentFocusActor(), actor);
      }

      DALI_LOG_INFO( gLogFilter, Debug::General, "[%s:%d] Focus Changed\n", __FUNCTION__, __LINE__);

      // Save the current focused actor
      mCurrentFocusActor = actorID;

      DALI_LOG_INFO( gLogFilter, Debug::General, "[%s:%d] SUCCEED\n", __FUNCTION__, __LINE__);
      return true;
    }
  }

  DALI_LOG_WARNING("[%s:%d] FAILED\n", __FUNCTION__, __LINE__);
  return false;
}

Actor KeyboardFocusManager::GetCurrentFocusActor()
{
  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  return rootActor.FindChildById(mCurrentFocusActor);
}

Actor KeyboardFocusManager::GetCurrentFocusGroup()
{
  return GetFocusGroup(GetCurrentFocusActor());
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
  Toolkit::Control parentLayoutControl = GetParentLayoutControl(currentFocusActor);
  while(parentLayoutControl && !succeed)
  {
    succeed = DoMoveFocusWithinLayoutControl(parentLayoutControl, currentFocusActor, direction);
    parentLayoutControl = GetParentLayoutControl(parentLayoutControl);
  }

  if(!succeed && !mPreFocusChangeSignal.Empty())
  {
    // Don't know how to move the focus further. The application needs to tell us which actor to move the focus to
    mIsWaitingKeyboardFocusChangeCommit = true;
    Actor nextFocusableActor = mPreFocusChangeSignal.Emit(currentFocusActor, Actor(), direction);
    mIsWaitingKeyboardFocusChangeCommit = false;

    if ( nextFocusableActor && nextFocusableActor.IsKeyboardFocusable() )
    {
      // Whether the next focusable actor is a layout control
      if(IsLayoutControl(nextFocusableActor))
      {
        // If so, move the focus inside it.
        Toolkit::Control layoutControl = Toolkit::Control::DownCast(nextFocusableActor);
        succeed = DoMoveFocusWithinLayoutControl(layoutControl, currentFocusActor, direction);
      }
      else
      {
        // Otherwise, just set focus to the next focusable actor
        succeed = SetCurrentFocusActor(nextFocusableActor);
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
  if(actor)
  {
    if(mFocusIndicatorActor)
    {
      actor.Remove(mFocusIndicatorActor);
    }

    // Send notification for the change of focus actor
    if( !mFocusChangedSignal.Empty() )
    {
      mFocusChangedSignal.Emit(actor, Actor());
    }
  }

  mCurrentFocusActor = 0;
  mIsFocusIndicatorEnabled = false;
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
  return mFocusIndicatorActor;
}

void KeyboardFocusManager::CreateDefaultFocusIndicatorActor()
{
  // Create a focus indicator actor shared by all the keyboard focusable actors
  Toolkit::ImageView focusIndicator = Toolkit::ImageView::New(FOCUS_BORDER_IMAGE_PATH);
  focusIndicator.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );

  // Apply size constraint to the focus indicator
  focusIndicator.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

  SetFocusIndicatorActor(focusIndicator);
}

void KeyboardFocusManager::OnPhysicalKeyboardStatusChanged(PhysicalKeyboard keyboard)
{
  mIsKeyboardFocusEnabled = keyboard.IsAttached();

  if(mIsKeyboardFocusEnabled)
  {
    // Show indicator when keyboard focus turned on if there is focused actor.
    Actor actor = GetCurrentFocusActor();
    if(actor)
    {
      if(mFocusIndicatorActor)
      {
        actor.Add(mFocusIndicatorActor);
      }
    }
    mIsFocusIndicatorEnabled = true;
  }
  else
  {
    // Hide indicator when keyboard focus turned off
    Actor actor = GetCurrentFocusActor();
    if(actor)
    {
      actor.Remove(mFocusIndicatorActor);
    }
    mIsFocusIndicatorEnabled = false;
  }
}

void KeyboardFocusManager::OnKeyEvent(const KeyEvent& event)
{
  if(!mIsKeyboardFocusEnabled)
  {
    return;
  }

  AccessibilityAdaptor accessibilityAdaptor = AccessibilityAdaptor::Get();
  bool isAccessibilityEnabled = accessibilityAdaptor.IsEnabled();

  Toolkit::AccessibilityManager accessibilityManager = Toolkit::AccessibilityManager::Get();

  std::string keyName = event.keyPressedName;

  bool isFocusStartableKey = false;

  if(event.state == KeyEvent::Down)
  {
    if (keyName == "Left")
    {
      if(!isAccessibilityEnabled)
      {
        if(!mIsFocusIndicatorEnabled)
        {
          // Show focus indicator
          mIsFocusIndicatorEnabled = true;
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
        if(!mIsFocusIndicatorEnabled)
        {
          // Show focus indicator
          mIsFocusIndicatorEnabled = true;
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
      if(!mIsFocusIndicatorEnabled)
      {
        // Show focus indicator
        mIsFocusIndicatorEnabled = true;
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
      if(!mIsFocusIndicatorEnabled)
      {
        // Show focus indicator
        mIsFocusIndicatorEnabled = true;
      }
      else
      {
        // Move the focus towards down
        MoveFocus(Toolkit::Control::KeyboardFocus::DOWN);
      }

      isFocusStartableKey = true;
    }
    else if (keyName == "Tab" && !isAccessibilityEnabled)
    {
      if(!mIsFocusIndicatorEnabled)
      {
        // Show focus indicator
        mIsFocusIndicatorEnabled = true;
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
      if(!mIsFocusIndicatorEnabled)
      {
        // Show focus indicator
        mIsFocusIndicatorEnabled = true;
      }

      isFocusStartableKey = true;
    }
    else if (keyName == "" && !isAccessibilityEnabled)
    {
      // Check the fake key event for evas-plugin case
      if(!mIsFocusIndicatorEnabled)
      {
        // Show focus indicator
        mIsFocusIndicatorEnabled = true;
      }

      isFocusStartableKey = true;
    }
    else if (keyName == "Backspace" && !isAccessibilityEnabled)
    {
      // Emit signal to go back to the previous view???
    }
  }
  else if(event.state == KeyEvent::Up)
  {
    if (keyName == "Return")
    {
      if(!mIsFocusIndicatorEnabled && !isAccessibilityEnabled)
      {
        // Show focus indicator
        mIsFocusIndicatorEnabled = true;
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

  if(isFocusStartableKey && mIsFocusIndicatorEnabled && !isAccessibilityEnabled)
  {
    Actor actor = GetCurrentFocusActor();
    if( !actor )
    {
      // No actor is focused but keyboard focus is activated by the key press
      // Let's try to move the initial focus
      MoveFocus(Toolkit::Control::KeyboardFocus::RIGHT);
    }
    else if(mFocusIndicatorActor)
    {
      // Make sure the focused actor is highlighted
      actor.Add(mFocusIndicatorActor);
    }
  }
}

void KeyboardFocusManager::OnTouched(const TouchEvent& touchEvent)
{
  // Clear the focus when user touch the screen.
  // We only do this on a Down event, otherwise the clear action may override a manually focused actor.
  if( ( touchEvent.GetPointCount() < 1 ) || ( touchEvent.GetPoint( 0 ).state == TouchPoint::Down ) )
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
  KeyboardFocusManager* manager = dynamic_cast<KeyboardFocusManager*>( object );

  if( 0 == strcmp( signalName.c_str(), SIGNAL_PRE_FOCUS_CHANGE ) )
  {
    manager->PreFocusChangeSignal().Connect( tracker, functor );
  }
  if( 0 == strcmp( signalName.c_str(), SIGNAL_FOCUS_CHANGED ) )
  {
    manager->FocusChangedSignal().Connect( tracker, functor );
  }
  if( 0 == strcmp( signalName.c_str(), SIGNAL_FOCUS_GROUP_CHANGED ) )
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

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
