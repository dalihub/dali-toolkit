/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/lifecycle-controller.h>
#include <dali/devel-api/common/singleton-service.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/asset-manager/asset-manager.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/focus-manager/focus-finder.h>
#include <dali-toolkit/devel-api/styling/style-manager-devel.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/styling/style-manager.h>
#include <dali/devel-api/adaptor-framework/accessibility.h>

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

const char* const FOCUS_BORDER_IMAGE_FILE_NAME = "keyboard_focus.9.png";

BaseHandle Create()
{
  BaseHandle handle = KeyboardFocusManager::Get();

  if(!handle)
  {
    SingletonService singletonService(SingletonService::Get());
    if(singletonService)
    {
      Toolkit::KeyboardFocusManager manager = Toolkit::KeyboardFocusManager(new Internal::KeyboardFocusManager());
      singletonService.Register(typeid(manager), manager);
      handle = manager;
    }
  }

  return handle;
}

DALI_TYPE_REGISTRATION_BEGIN_CREATE(Toolkit::KeyboardFocusManager, Dali::BaseHandle, Create, true)

DALI_SIGNAL_REGISTRATION(Toolkit, KeyboardFocusManager, "keyboardPreFocusChange", SIGNAL_PRE_FOCUS_CHANGE)
DALI_SIGNAL_REGISTRATION(Toolkit, KeyboardFocusManager, "keyboardFocusChanged", SIGNAL_FOCUS_CHANGED)
DALI_SIGNAL_REGISTRATION(Toolkit, KeyboardFocusManager, "keyboardFocusGroupChanged", SIGNAL_FOCUS_GROUP_CHANGED)
DALI_SIGNAL_REGISTRATION(Toolkit, KeyboardFocusManager, "keyboardFocusedActorEnterKey", SIGNAL_FOCUSED_ACTOR_ENTER_KEY)

DALI_TYPE_REGISTRATION_END()

const unsigned int MAX_HISTORY_AMOUNT = 30; ///< Max length of focus history stack

} // unnamed namespace

Toolkit::KeyboardFocusManager KeyboardFocusManager::Get()
{
  Toolkit::KeyboardFocusManager manager;

  SingletonService singletonService(SingletonService::Get());
  if(singletonService)
  {
    // Check whether the keyboard focus manager is already created
    Dali::BaseHandle handle = singletonService.GetSingleton(typeid(Toolkit::KeyboardFocusManager));
    if(handle)
    {
      // If so, downcast the handle of singleton to keyboard focus manager
      manager = Toolkit::KeyboardFocusManager(dynamic_cast<KeyboardFocusManager*>(handle.GetObjectPtr()));
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
  mFocusHistory(),
  mSlotDelegate(this),
  mCustomAlgorithmInterface(NULL),
  mCurrentFocusedWindow(),
  mIsFocusIndicatorShown(UNKNOWN),
  mEnableFocusIndicator(ENABLE),
  mAlwaysShowIndicator(ALWAYS_SHOW),
  mFocusGroupLoopEnabled(false),
  mIsWaitingKeyboardFocusChangeCommit(false),
  mClearFocusOnTouch(true),
  mEnableDefaultAlgorithm(false)
{
  // TODO: Get FocusIndicatorEnable constant from stylesheet to set mIsFocusIndicatorShown.

  LifecycleController::Get().InitSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnAdaptorInit);
}

void KeyboardFocusManager::OnAdaptorInit()
{
  if(Adaptor::IsAvailable())
  {
    // Retrieve all the existing scene holders
    Dali::SceneHolderList sceneHolders = Adaptor::Get().GetSceneHolders();
    for(auto iter = sceneHolders.begin(); iter != sceneHolders.end(); ++iter)
    {
      (*iter).KeyEventSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnKeyEvent);
      (*iter).TouchedSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnTouch);
      (*iter).WheelEventGeneratedSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnWheelEvent);
      Dali::Window window = DevelWindow::DownCast(*iter);
      if(window)
      {
        window.FocusChangeSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnWindowFocusChanged);
      }
    }

    // Get notified when any new scene holder is created afterwards
    Adaptor::Get().WindowCreatedSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnSceneHolderCreated);
  }
}

void KeyboardFocusManager::OnSceneHolderCreated(Dali::Integration::SceneHolder& sceneHolder)
{
  sceneHolder.KeyEventSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnKeyEvent);
  sceneHolder.TouchedSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnTouch);
  sceneHolder.WheelEventGeneratedSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnWheelEvent);
  Dali::Window window = DevelWindow::DownCast(sceneHolder);
  if(window)
  {
    window.FocusChangeSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnWindowFocusChanged);
  }
}

KeyboardFocusManager::~KeyboardFocusManager()
{
}

void KeyboardFocusManager::GetConfigurationFromStyleManger()
{
  Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();
  if(styleManager)
  {
    Property::Map config   = Toolkit::DevelStyleManager::GetConfigurations(styleManager);
    mAlwaysShowIndicator   = config["alwaysShowFocus"].Get<bool>() ? ALWAYS_SHOW : NONE;
    mIsFocusIndicatorShown = (mAlwaysShowIndicator == ALWAYS_SHOW) ? SHOW : HIDE;
    mClearFocusOnTouch     = (mIsFocusIndicatorShown == SHOW) ? false : true;
  }
}

bool KeyboardFocusManager::SetCurrentFocusActor(Actor actor)
{
  DALI_ASSERT_DEBUG(!mIsWaitingKeyboardFocusChangeCommit && "Calling this function in the PreFocusChangeSignal callback?");

  if(mIsFocusIndicatorShown == UNKNOWN)
  {
    GetConfigurationFromStyleManger();
  }

  return DoSetCurrentFocusActor(actor);
}

bool KeyboardFocusManager::DoSetCurrentFocusActor(Actor actor)
{
  bool success = false;

  // If the parent's KEYBOARD_FOCUSABLE_CHILDREN is false, it cannot have focus.
  if(actor)
  {
    Actor parent = actor.GetParent();
    while(parent)
    {
      if(!parent.GetProperty<bool>(DevelActor::Property::KEYBOARD_FOCUSABLE_CHILDREN))
      {
        DALI_LOG_INFO(gLogFilter, Debug::General, "[%s:%d] Parent Actor has KEYBOARD_FOCUSABLE_CHILDREN false,\n", __FUNCTION__, __LINE__);
        return false;
      }
      parent = parent.GetParent();
    }
  }

  if(actor && actor.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE) && actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    Integration::SceneHolder currentWindow = Integration::SceneHolder::Get(actor);

    if(currentWindow.GetRootLayer() != mCurrentFocusedWindow.GetHandle())
    {
      Layer rootLayer       = currentWindow.GetRootLayer();
      mCurrentFocusedWindow = rootLayer;
    }
  }

  Actor currentFocusedActor = GetCurrentFocusActor();

  // If developer set focus on same actor, doing nothing
  if(actor == currentFocusedActor)
  {
    if(!actor)
    {
      return false;
    }
    return true;
  }

  // Check whether the actor is in the stage and is keyboard focusable.
  if(actor && actor.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE) && actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    if((mIsFocusIndicatorShown == SHOW) && (mEnableFocusIndicator == ENABLE))
    {
      actor.Add(GetFocusIndicatorActor());
    }

    // Send notification for the change of focus actor
    if(!mFocusChangedSignal.Empty())
    {
      mFocusChangedSignal.Emit(currentFocusedActor, actor);
    }

    Toolkit::Control currentlyFocusedControl = Toolkit::Control::DownCast(currentFocusedActor);
    if(currentlyFocusedControl)
    {
      // Do we need it to remember if it was previously DISABLED?
      currentlyFocusedControl.SetProperty(DevelControl::Property::STATE, DevelControl::NORMAL);
      currentlyFocusedControl.ClearKeyInputFocus();
    }

    DALI_LOG_INFO(gLogFilter, Debug::General, "[%s:%d] Focus Changed\n", __FUNCTION__, __LINE__);

    // Save the current focused actor
    mCurrentFocusActor = actor;

    bool focusedWindowFound = false;
    for(unsigned int i = 0; i < mCurrentFocusActors.size(); i++)
    {
      if(mCurrentFocusActors[i].first == mCurrentFocusedWindow)
      {
        mCurrentFocusActors[i].second = actor;
        focusedWindowFound            = true;
        break;
      }
    }
    if(!focusedWindowFound)
    {
      // A new window gains the focus, so store the focused actor in that window.
      mCurrentFocusActors.push_back(std::pair<WeakHandle<Layer>, WeakHandle<Actor> >(mCurrentFocusedWindow, actor));
    }

    Toolkit::Control newlyFocusedControl = Toolkit::Control::DownCast(actor);
    if(newlyFocusedControl)
    {
      newlyFocusedControl.SetProperty(DevelControl::Property::STATE, DevelControl::FOCUSED);
      newlyFocusedControl.SetKeyInputFocus();
    }

    // Push Current Focused Actor to FocusHistory
    mFocusHistory.push_back(actor);

    // Delete first element before add new element when Stack is full.
    if(mFocusHistory.size() > MAX_HISTORY_AMOUNT)
    {
      FocusStackIterator beginPos = mFocusHistory.begin();
      mFocusHistory.erase(beginPos);
    }

    DALI_LOG_INFO(gLogFilter, Debug::General, "[%s:%d] SUCCEED\n", __FUNCTION__, __LINE__);
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

  if(actor && !actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    // If the actor has been removed from the stage, then it should not be focused
    actor.Reset();
    mCurrentFocusActor.Reset();
  }
  return actor;
}

Actor KeyboardFocusManager::GetFocusActorFromCurrentWindow()
{
  Actor        actor;
  unsigned int index;
  for(index = 0; index < mCurrentFocusActors.size(); index++)
  {
    if(mCurrentFocusActors[index].first == mCurrentFocusedWindow)
    {
      actor = mCurrentFocusActors[index].second.GetHandle();
      break;
    }
  }

  if(actor && !actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    // If the actor has been removed from the window, then the window doesn't have any focused actor
    actor.Reset();
    mCurrentFocusActors.erase(mCurrentFocusActors.begin() + index);
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
  if(mFocusHistory.size() > 1)
  {
    // Delete current focused actor in history
    mFocusHistory.pop_back();

    // If pre-focused actors are not on stage or deleted, remove them in stack
    while(mFocusHistory.size() > 0)
    {
      // Get pre focused actor
      Actor target = mFocusHistory[mFocusHistory.size() - 1].GetHandle();

      // Impl of Actor is not null
      if(target && target.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
      {
        // Delete pre focused actor in history because it will pushed again by SetCurrentFocusActor()
        mFocusHistory.pop_back();
        SetCurrentFocusActor(target);
        break;
      }
      else
      {
        // Target is empty handle or off stage. Erase from queue
        mFocusHistory.pop_back();
      }
    }

    // if there is no actor which can get focus, then push current focus actor in stack again
    if(mFocusHistory.size() == 0)
    {
      Actor currentFocusedActor = GetCurrentFocusActor();
      mFocusHistory.push_back(currentFocusedActor);
    }
  }
}

bool KeyboardFocusManager::IsLayoutControl(Actor actor) const
{
  Toolkit::Control control = Toolkit::Control::DownCast(actor);
  return control && GetImplementation(control).IsKeyboardNavigationSupported();
}

Toolkit::Control KeyboardFocusManager::GetParentLayoutControl(Actor actor) const
{
  // Get the actor's parent layout control that supports two dimensional keyboard navigation
  Actor rootActor;
  Actor parent;
  if(actor)
  {
    Integration::SceneHolder window = Integration::SceneHolder::Get(actor);
    if(window)
    {
      rootActor = window.GetRootLayer();
    }

    parent = actor.GetParent();
  }

  while(parent && !IsLayoutControl(parent) && parent != rootActor)
  {
    parent = parent.GetParent();
  }

  return Toolkit::Control::DownCast(parent);
}

bool KeyboardFocusManager::MoveFocus(Toolkit::Control::KeyboardFocus::Direction direction, const std::string& deviceName)
{
  Actor currentFocusActor = GetCurrentFocusActor();

  bool succeed = false;

  // Go through the actor's hierarchy until we find a layout control that knows how to move the focus
  Toolkit::Control parentLayoutControl = GetParentLayoutControl(currentFocusActor);
  while(parentLayoutControl && !succeed)
  {
    succeed             = DoMoveFocusWithinLayoutControl(parentLayoutControl, currentFocusActor, direction);
    parentLayoutControl = GetParentLayoutControl(parentLayoutControl);
  }

  if(!succeed)
  {
    Actor nextFocusableActor;

    Toolkit::Control currentFocusControl = Toolkit::Control::DownCast(currentFocusActor);

    // If the current focused actor is a control, then find the next focusable actor via the focusable properties.
    if(currentFocusControl)
    {
      int             actorId = -1;
      Property::Index index   = Property::INVALID_INDEX;
      Property::Value value;

      // Find property index based upon focus direction
      switch(direction)
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
      if(index != Property::INVALID_INDEX)
      {
        value   = currentFocusActor.GetProperty(index);
        actorId = value.Get<int>();

        // If actor's id is valid then find actor form actor's id. The actor should be on the stage.
        if(actorId != -1)
        {
          if(currentFocusActor.GetParent())
          {
            nextFocusableActor = currentFocusActor.GetParent().FindChildById(actorId);
          }

          if(!nextFocusableActor)
          {
            Integration::SceneHolder window = Integration::SceneHolder::Get(currentFocusActor);
            if(window)
            {
              nextFocusableActor = window.GetRootLayer().FindChildById(actorId);
            }
          }
        }
      }
    }

    if(!nextFocusableActor)
    {
      // If the implementation of CustomAlgorithmInterface is provided then the PreFocusChangeSignal is no longer emitted.
      if(mCustomAlgorithmInterface)
      {
        mIsWaitingKeyboardFocusChangeCommit = true;
        nextFocusableActor                  = mCustomAlgorithmInterface->GetNextFocusableActor(currentFocusActor, Actor(), direction, deviceName);
        mIsWaitingKeyboardFocusChangeCommit = false;
      }
      else if(!mPreFocusChangeSignal.Empty())
      {
        // Don't know how to move the focus further. The application needs to tell us which actor to move the focus to
        mIsWaitingKeyboardFocusChangeCommit = true;
        nextFocusableActor                  = mPreFocusChangeSignal.Emit(currentFocusActor, Actor(), direction);
        mIsWaitingKeyboardFocusChangeCommit = false;
      }
      else if (mEnableDefaultAlgorithm)
      {
        Layer rootLayer;
        if (currentFocusActor)
        {
          // Find the window of the focused actor.
          Integration::SceneHolder window = Integration::SceneHolder::Get(currentFocusActor);
          if (window)
          {
            rootLayer = window.GetRootLayer();
          }
        }
        else
        {
          // Searches from the currently focused window.
          rootLayer = mCurrentFocusedWindow.GetHandle();
        }
        if (rootLayer)
        {
          // We should find it among the actors nearby.
          nextFocusableActor = Toolkit::FocusFinder::GetNearestFocusableActor(rootLayer, currentFocusActor, direction);
        }
      }
    }

    if(nextFocusableActor && nextFocusableActor.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE))
    {
      // Whether the next focusable actor is a layout control
      if(IsLayoutControl(nextFocusableActor))
      {
        // If so, move the focus inside it.
        Toolkit::Control layoutControl = Toolkit::Control::DownCast(nextFocusableActor);
        succeed                        = DoMoveFocusWithinLayoutControl(layoutControl, currentFocusActor, direction);
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
  Actor nextFocusableActor = GetImplementation(control).GetNextKeyboardFocusableActor(actor, direction, mFocusGroupLoopEnabled);
  if(nextFocusableActor)
  {
    if(!nextFocusableActor.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE))
    {
      // If the actor is not focusable, ask the same layout control for the next actor to focus
      return DoMoveFocusWithinLayoutControl(control, nextFocusableActor, direction);
    }
    else
    {
      Actor currentFocusActor   = GetCurrentFocusActor();
      Actor committedFocusActor = nextFocusableActor;

      // We will try to move the focus to the actor. Emit a signal to notify the proposed actor to focus
      // Signal handler can check the proposed actor and return a different actor if it wishes.
      if(!mPreFocusChangeSignal.Empty())
      {
        mIsWaitingKeyboardFocusChangeCommit = true;
        committedFocusActor                 = mPreFocusChangeSignal.Emit(currentFocusActor, nextFocusableActor, direction);
        mIsWaitingKeyboardFocusChangeCommit = false;
      }

      if(committedFocusActor && committedFocusActor.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE))
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
            GetImplementation(control).OnKeyboardFocusChangeCommitted(committedFocusActor);
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
    succeed                                              = DoMoveFocusWithinLayoutControl(parentLayoutControl, GetCurrentFocusActor(), direction);
    parentLayoutControl                                  = GetParentLayoutControl(parentLayoutControl);
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
  if(actor)
  {
    Toolkit::Control control = Toolkit::Control::DownCast(actor);
    if(control)
    {
      // Notify the control that enter has been pressed on it.
      GetImplementation(control).KeyboardEnter();
    }

    // Send a notification for the actor.
    if(!mFocusedActorEnterKeySignal.Empty())
    {
      mFocusedActorEnterKeySignal.Emit(actor);
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
    if(!mFocusChangedSignal.Empty())
    {
      mFocusChangedSignal.Emit(actor, Actor());
    }

    Toolkit::Control currentlyFocusedControl = Toolkit::Control::DownCast(actor);
    if(currentlyFocusedControl)
    {
      currentlyFocusedControl.SetProperty(DevelControl::Property::STATE, DevelControl::NORMAL);
      currentlyFocusedControl.ClearKeyInputFocus();
    }
  }

  mCurrentFocusActor.Reset();
  mIsFocusIndicatorShown = (mAlwaysShowIndicator == ALWAYS_SHOW) ? SHOW : HIDE;
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
    actor.RegisterProperty(IS_FOCUS_GROUP_PROPERTY_NAME, isFocusGroup, Property::READ_WRITE);
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
  while(actor && !IsFocusGroup(actor))
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
  if(!mFocusIndicatorActor)
  {
    // Create the default if it hasn't been set and one that's shared by all the keyboard focusable actors
    const std::string imageDirPath = AssetManager::GetDaliImagePath();
    mFocusIndicatorActor           = Toolkit::ImageView::New(imageDirPath + FOCUS_BORDER_IMAGE_FILE_NAME);

    // Apply size constraint to the focus indicator
    mFocusIndicatorActor.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  }

  mFocusIndicatorActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  mFocusIndicatorActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  mFocusIndicatorActor.SetProperty(Actor::Property::POSITION, Vector2(0.0f, 0.0f));

  return mFocusIndicatorActor;
}

void KeyboardFocusManager::OnKeyEvent(const KeyEvent& event)
{
  const std::string& keyName = event.GetKeyName();
  const std::string& deviceName = event.GetDeviceName();

  if(mIsFocusIndicatorShown == UNKNOWN)
  {
    GetConfigurationFromStyleManger();
  }

  bool isFocusStartableKey = false;

  if(event.GetState() == KeyEvent::DOWN)
  {
    if(keyName == "Left")
    {
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }
      else
      {
        // Move the focus towards left
        MoveFocus(Toolkit::Control::KeyboardFocus::LEFT, deviceName);
      }

      isFocusStartableKey = true;
    }
    else if(keyName == "Right")
    {
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }
      else
      {
        // Move the focus towards right
        MoveFocus(Toolkit::Control::KeyboardFocus::RIGHT, deviceName);
      }

      isFocusStartableKey = true;
    }
    else if(keyName == "Up")
    {
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }
      else
      {
        // Move the focus towards up
        MoveFocus(Toolkit::Control::KeyboardFocus::UP, deviceName);
      }

      isFocusStartableKey = true;
    }
    else if(keyName == "Down")
    {
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }
      else
      {
        // Move the focus towards down
        MoveFocus(Toolkit::Control::KeyboardFocus::DOWN, deviceName);
      }

      isFocusStartableKey = true;
    }
    else if(keyName == "Prior")
    {
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }
      else
      {
        // Move the focus towards the previous page
        MoveFocus(Toolkit::Control::KeyboardFocus::PAGE_UP, deviceName);
      }

      isFocusStartableKey = true;
    }
    else if(keyName == "Next")
    {
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }
      else
      {
        // Move the focus towards the next page
        MoveFocus(Toolkit::Control::KeyboardFocus::PAGE_DOWN, deviceName);
      }

      isFocusStartableKey = true;
    }
    else if(keyName == "Tab")
    {
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }
      else
      {
        // "Tab" key changes the focus group in the forward direction and
        // "Shift-Tab" key changes it in the backward direction.
        if(!DoMoveFocusToNextFocusGroup(!event.IsShiftModifier()))
        {
          // If the focus group is not changed, Move the focus towards forward, "Shift-Tap" key moves the focus towards backward.
          MoveFocus(event.IsShiftModifier() ? Toolkit::Control::KeyboardFocus::BACKWARD : Toolkit::Control::KeyboardFocus::FORWARD, deviceName);
        }
      }

      isFocusStartableKey = true;
    }
    else if(keyName == "space")
    {
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }

      isFocusStartableKey = true;
    }
    else if(keyName == "")
    {
      // Check the fake key event for evas-plugin case
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }

      isFocusStartableKey = true;
    }
    else if(keyName == "Backspace")
    {
      // Emit signal to go back to the previous view???
    }
    else if(keyName == "Escape")
    {
    }
  }
  else if(event.GetState() == KeyEvent::UP)
  {
    if(keyName == "Return")
    {
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }
      else
      {
        // The focused actor has enter pressed on it
        Actor actor = GetCurrentFocusActor();
        if(actor)
        {
          DoKeyboardEnter(actor);
        }
      }

      isFocusStartableKey = true;
    }
  }

  if(isFocusStartableKey && mIsFocusIndicatorShown == SHOW)
  {
    Actor actor = GetCurrentFocusActor();
    if(actor)
    {
      if(mEnableFocusIndicator == ENABLE)
      {
        // Make sure the focused actor is highlighted
        actor.Add(GetFocusIndicatorActor());
      }
    }
    else
    {
      // No actor is focused but keyboard focus is activated by the key press
      // Let's try to move the initial focus
      MoveFocus(Toolkit::Control::KeyboardFocus::RIGHT, deviceName);
    }
  }
}

void KeyboardFocusManager::OnTouch(const TouchEvent& touch)
{
  // if mIsFocusIndicatorShown is UNKNOWN, it means Configuration is not loaded.
  // Try to load configuration.
  if(mIsFocusIndicatorShown == UNKNOWN)
  {
    GetConfigurationFromStyleManger();
  }

  // Clear the focus when user touch the screen.
  // We only do this on a Down event, otherwise the clear action may override a manually focused actor.
  if(((touch.GetPointCount() < 1) || (touch.GetState(0) == PointState::DOWN)))
  {
    // If mClearFocusOnTouch is false, do not clear the focus even if user touch the screen.
    if(mClearFocusOnTouch)
    {
      ClearFocus();
    }

    // If KEYBOARD_FOCUSABLE and TOUCH_FOCUSABLE is true, set focus actor
    Actor hitActor = touch.GetHitActor(0);
    if(hitActor && hitActor.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE) && hitActor.GetProperty<bool>(DevelActor::Property::TOUCH_FOCUSABLE))
    {
      SetCurrentFocusActor(hitActor);
    }
  }
}

bool KeyboardFocusManager::OnWheelEvent(const WheelEvent& event)
{
  bool consumed = false;
  Actor actor = GetCurrentFocusActor();
  if(actor)
  {
    // Notify the actor about the wheel event
    consumed = EmitWheelSignals(actor, event);
  }
  return consumed;
}

bool KeyboardFocusManager::EmitWheelSignals(Actor actor, const WheelEvent& event)
{
  bool consumed = false;

  if(actor)
  {
    Dali::Actor oldParent(actor.GetParent());

    // Only do the conversion and emit the signal if the actor's wheel signal has connections.
    if(!actor.WheelEventSignal().Empty())
    {
      // Emit the signal to the parent
      consumed = actor.WheelEventSignal().Emit(actor, event);
    }
    // if actor doesn't consume WheelEvent, give WheelEvent to its parent.
    if(!consumed)
    {
      // The actor may have been removed/reparented during the signal callbacks.
      Dali::Actor parent = actor.GetParent();

      if(parent &&
         (parent == oldParent))
      {
        consumed = EmitWheelSignals(parent, event);
      }
    }
  }

  return consumed;
}

void KeyboardFocusManager::OnWindowFocusChanged(Window window, bool focusIn)
{
  if(focusIn && mCurrentFocusedWindow.GetHandle() != window.GetRootLayer())
  {
    // Change Current Focused Window
    Layer rootLayer       = window.GetRootLayer();
    mCurrentFocusedWindow = rootLayer;

    // Get Current Focused Actor from window
    Actor currentFocusedActor = GetFocusActorFromCurrentWindow();
    SetCurrentFocusActor(currentFocusedActor);

    if(currentFocusedActor && (mEnableFocusIndicator == ENABLE))
    {
      // Make sure the focused actor is highlighted
      currentFocusedActor.Add(GetFocusIndicatorActor());
      mIsFocusIndicatorShown = SHOW;
    }
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

bool KeyboardFocusManager::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  Dali::BaseHandle handle(object);

  bool                  connected(true);
  KeyboardFocusManager* manager = static_cast<KeyboardFocusManager*>(object); // TypeRegistry guarantees that this is the correct type.

  if(0 == strcmp(signalName.c_str(), SIGNAL_PRE_FOCUS_CHANGE))
  {
    manager->PreFocusChangeSignal().Connect(tracker, functor);
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_FOCUS_CHANGED))
  {
    manager->FocusChangedSignal().Connect(tracker, functor);
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_FOCUS_GROUP_CHANGED))
  {
    manager->FocusGroupChangedSignal().Connect(tracker, functor);
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_FOCUSED_ACTOR_ENTER_KEY))
  {
    manager->FocusedActorEnterKeySignal().Connect(tracker, functor);
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
  if(!enable && mFocusIndicatorActor)
  {
    mFocusIndicatorActor.Unparent();
  }

  mEnableFocusIndicator = enable ? ENABLE : DISABLE;
}

bool KeyboardFocusManager::IsFocusIndicatorEnabled() const
{
  return (mEnableFocusIndicator == ENABLE);
}

void KeyboardFocusManager::EnableDefaultAlgorithm(bool enable)
{
  mEnableDefaultAlgorithm = enable;
}

bool KeyboardFocusManager::IsDefaultAlgorithmEnabled() const
{
  return mEnableDefaultAlgorithm;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
