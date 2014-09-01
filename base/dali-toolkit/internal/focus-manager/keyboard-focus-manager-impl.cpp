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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/focus-manager/focus-manager.h>
#include <dali-toolkit/public-api/focus-manager/keyinput-focus-manager.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace // unnamed namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_KEYBOARD_FOCUS_MANAGER");
#endif

const std::string IS_FOCUS_GROUP_PROPERTY_NAME("is-keyboard-focus-group"); // This property will be replaced by a flag in Control.

const char* FOCUS_BORDER_IMAGE_PATH = DALI_IMAGE_DIR "keyboard_focus.png";
const Vector4 FOCUS_BORDER_IMAGE_BORDER = Vector4(7.0f, 7.0f, 7.0f, 7.0f);

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
TypeRegistration KEYBOARD_FOCUS_MANAGER_TYPE( typeid(Dali::Toolkit::KeyboardFocusManager), typeid(Dali::BaseHandle), Create, true /* Create instance at startup */ );

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

  if(actor)
  {
    return DoSetCurrentFocusActor(actor.GetId());
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
      if( !mFocusChangedSignalV2.Empty() )
      {
        mFocusChangedSignalV2.Emit(GetCurrentFocusActor(), actor);
      }

      DALI_LOG_INFO( gLogFilter, Debug::General, "[%s:%d] Focus Changed\n", __FUNCTION__, __LINE__);

      // Save the current focused actor
      mCurrentFocusActor = actorID;

      // Move the accessibility focus to the same actor
//      Toolkit::FocusManager focusManager = Toolkit::FocusManager::Get();
//      focusManager.SetCurrentFocusActor(actor);

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
  return control && control.GetImplementation().IsKeyboardNavigationSupported();
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

bool KeyboardFocusManager::MoveFocus(Toolkit::Control::KeyboardFocusNavigationDirection direction)
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

  if(!succeed && !mPreFocusChangeSignalV2.Empty())
  {
    // Don't know how to move the focus further. The application needs to tell us which actor to move the focus to
    mIsWaitingKeyboardFocusChangeCommit = true;
    Actor nextFocusableActor = mPreFocusChangeSignalV2.Emit(currentFocusActor, Actor(), direction);
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

bool KeyboardFocusManager::DoMoveFocusWithinLayoutControl(Toolkit::Control control, Actor actor, Toolkit::Control::KeyboardFocusNavigationDirection direction)
{
  // Ask the control for the next actor to focus
  Actor nextFocusableActor = control.GetImplementation().GetNextKeyboardFocusableActor(actor, direction, mFocusGroupLoopEnabled);
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
      if( !mPreFocusChangeSignalV2.Empty() )
      {
        mIsWaitingKeyboardFocusChangeCommit = true;
        committedFocusActor = mPreFocusChangeSignalV2.Emit(currentFocusActor, nextFocusableActor, direction);
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
            control.GetImplementation().OnKeyboardFocusChangeCommitted(committedFocusActor);
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
    Toolkit::Control::KeyboardFocusNavigationDirection direction = forward ? Toolkit::Control::Right : Toolkit::Control::Left;
    succeed = DoMoveFocusWithinLayoutControl(parentLayoutControl, GetCurrentFocusActor(), direction);
    parentLayoutControl = GetParentLayoutControl(parentLayoutControl);
  }

  if(!mFocusGroupChangedSignalV2.Empty())
  {
    // Emit a focus group changed signal. The applicaton can move the focus to a new focus group
    mFocusGroupChangedSignalV2.Emit(GetCurrentFocusActor(), forward);
  }

  return succeed;
}

void KeyboardFocusManager::DoActivate(Actor actor)
{
  if(actor)
  {
    Toolkit::Control control = Toolkit::Control::DownCast(actor);
    if(control)
    {
      // Notify the control that it is activated
      control.GetImplementation().OnActivated();
    }

    // Send notification for the activation of focused actor
    if( !mFocusedActorActivatedSignalV2.Empty() )
    {
      mFocusedActorActivatedSignalV2.Emit(actor);
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
    if( !mFocusChangedSignalV2.Empty() )
    {
      mFocusChangedSignalV2.Emit(actor, Actor());
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
    // Create focus group property if not already created.
    Property::Index propertyIsFocusGroup = actor.GetPropertyIndex(IS_FOCUS_GROUP_PROPERTY_NAME);
    if(propertyIsFocusGroup == Property::INVALID_INDEX)
    {
      propertyIsFocusGroup = actor.RegisterProperty(IS_FOCUS_GROUP_PROPERTY_NAME, isFocusGroup);
    }
    else
    {
      actor.SetProperty(propertyIsFocusGroup, isFocusGroup);
    }
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
  Image borderImage = Image::New(FOCUS_BORDER_IMAGE_PATH);

  ImageActor focusIndicator = ImageActor::New(borderImage);
  focusIndicator.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION_PLUS_LOCAL_POSITION );
  focusIndicator.SetStyle( ImageActor::STYLE_NINE_PATCH );
  focusIndicator.SetNinePatchBorder(FOCUS_BORDER_IMAGE_BORDER);
  focusIndicator.SetPosition(Vector3(0.0f, 0.0f, 1.0f));

  // Apply size constraint to the focus indicator
  Constraint constraint = Constraint::New<Vector3>(Actor::SIZE,
                                                   ParentSource(Actor::SIZE),
                                                   EqualToConstraint());
  focusIndicator.ApplyConstraint(constraint);

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

  AccessibilityManager accessibilityManager = AccessibilityManager::Get();
  bool isAccessibilityEnabled = accessibilityManager.IsEnabled();

  Toolkit::FocusManager accessibilityFocusManager = Toolkit::FocusManager::Get();

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
          MoveFocus(Toolkit::Control::Left);
        }

        isFocusStartableKey = true;
      }
      else
      {
        // Move the accessibility focus backward
        accessibilityFocusManager.MoveFocusBackward();
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
          MoveFocus(Toolkit::Control::Right);
        }

        isFocusStartableKey = true;
      }
      else
      {
        // Move the accessibility focus forward
        accessibilityFocusManager.MoveFocusForward();
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
        MoveFocus(Toolkit::Control::Up);
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
        MoveFocus(Toolkit::Control::Down);
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
        // Activate the focused actor
        Actor actor;
        if(!isAccessibilityEnabled)
        {
          actor = GetCurrentFocusActor();
        }
        else
        {
          actor = accessibilityFocusManager.GetCurrentFocusActor();
        }

        if(actor)
        {
          DoActivate(actor);
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
      MoveFocus(Toolkit::Control::Right);
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
  // Clear the focus when user touch the screen
  ClearFocus();
}

Toolkit::KeyboardFocusManager::PreFocusChangeSignalV2& KeyboardFocusManager::PreFocusChangeSignal()
{
  return mPreFocusChangeSignalV2;
}

Toolkit::KeyboardFocusManager::FocusChangedSignalV2& KeyboardFocusManager::FocusChangedSignal()
{
  return mFocusChangedSignalV2;
}

Toolkit::KeyboardFocusManager::FocusGroupChangedSignalV2& KeyboardFocusManager::FocusGroupChangedSignal()
{
  return mFocusGroupChangedSignalV2;
}

Toolkit::KeyboardFocusManager::FocusedActorActivatedSignalV2& KeyboardFocusManager::FocusedActorActivatedSignal()
{
  return mFocusedActorActivatedSignalV2;
}

bool KeyboardFocusManager::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  KeyboardFocusManager* manager = dynamic_cast<KeyboardFocusManager*>(object);

  if( Dali::Toolkit::KeyboardFocusManager::SIGNAL_PRE_FOCUS_CHANGE == signalName )
  {
    manager->PreFocusChangeSignal().Connect( tracker, functor );
  }
  if( Dali::Toolkit::KeyboardFocusManager::SIGNAL_FOCUS_CHANGED == signalName )
  {
    manager->FocusChangedSignal().Connect( tracker, functor );
  }
  if( Dali::Toolkit::KeyboardFocusManager::SIGNAL_FOCUS_GROUP_CHANGED == signalName )
  {
    manager->FocusGroupChangedSignal().Connect( tracker, functor );
  }
  else if( Dali::Toolkit::KeyboardFocusManager::SIGNAL_FOCUSED_ACTOR_ACTIVATED== signalName )
  {
    manager->FocusedActorActivatedSignal().Connect( tracker, functor );
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
