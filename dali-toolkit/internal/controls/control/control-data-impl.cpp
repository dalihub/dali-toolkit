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
#include "control-data-impl.h"

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/accessibility.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/object/handle-devel.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/object-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <cstring>
#include <limits>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/asset-manager/asset-manager.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/control-wrapper-impl.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace
{
const std::string READING_INFO_TYPE_NAME           = "name";
const std::string READING_INFO_TYPE_ROLE           = "role";
const std::string READING_INFO_TYPE_DESCRIPTION    = "description";
const std::string READING_INFO_TYPE_STATE          = "state";
const std::string READING_INFO_TYPE_ATTRIBUTE_NAME = "reading_info_type";
const std::string READING_INFO_TYPE_SEPARATOR      = "|";
} // namespace

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
extern const Dali::Scripting::StringEnum ControlStateTable[];
extern const unsigned int                ControlStateTableCount;

// Not static or anonymous - shared with other translation units
const Scripting::StringEnum ControlStateTable[] = {
  {"NORMAL", Toolkit::DevelControl::NORMAL},
  {"FOCUSED", Toolkit::DevelControl::FOCUSED},
  {"DISABLED", Toolkit::DevelControl::DISABLED},
};
const unsigned int ControlStateTableCount = sizeof(ControlStateTable) / sizeof(ControlStateTable[0]);

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_CONTROL_VISUALS");
#endif

template<typename T>
void Remove(Dictionary<T>& keyValues, const std::string& name)
{
  keyValues.Remove(name);
}

void Remove(DictionaryKeys& keys, const std::string& name)
{
  DictionaryKeys::iterator iter = std::find(keys.begin(), keys.end(), name);
  if(iter != keys.end())
  {
    keys.erase(iter);
  }
}

/**
 *  Finds visual in given array, returning true if found along with the iterator for that visual as a out parameter
 */
bool FindVisual(Property::Index targetIndex, const RegisteredVisualContainer& visuals, RegisteredVisualContainer::Iterator& iter)
{
  for(iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    if((*iter)->index == targetIndex)
    {
      return true;
    }
  }
  return false;
}

/**
 *  Finds visual in given array, returning true if found along with the iterator for that visual as a out parameter
 */
bool FindVisual(std::string visualName, const RegisteredVisualContainer& visuals, RegisteredVisualContainer::Iterator& iter)
{
  for(iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    Toolkit::Visual::Base visual = (*iter)->visual;
    if(visual && visual.GetName() == visualName)
    {
      return true;
    }
  }
  return false;
}

void FindChangableVisuals(Dictionary<Property::Map>& stateVisualsToAdd,
                          Dictionary<Property::Map>& stateVisualsToChange,
                          DictionaryKeys&            stateVisualsToRemove)
{
  DictionaryKeys copyOfStateVisualsToRemove = stateVisualsToRemove;

  for(DictionaryKeys::iterator iter = copyOfStateVisualsToRemove.begin();
      iter != copyOfStateVisualsToRemove.end();
      ++iter)
  {
    const std::string& visualName = (*iter);
    Property::Map*     toMap      = stateVisualsToAdd.Find(visualName);
    if(toMap)
    {
      stateVisualsToChange.Add(visualName, *toMap);
      stateVisualsToAdd.Remove(visualName);
      Remove(stateVisualsToRemove, visualName);
    }
  }
}

Toolkit::Visual::Base GetVisualByName(
  const RegisteredVisualContainer& visuals,
  const std::string&               visualName)
{
  Toolkit::Visual::Base visualHandle;

  RegisteredVisualContainer::Iterator iter;
  for(iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    Toolkit::Visual::Base visual = (*iter)->visual;
    if(visual && visual.GetName() == visualName)
    {
      visualHandle = visual;
      break;
    }
  }
  return visualHandle;
}

Toolkit::Visual::Base GetVisualByIndex(
  const RegisteredVisualContainer& visuals,
  Property::Index                  index)
{
  Toolkit::Visual::Base visualHandle;

  RegisteredVisualContainer::Iterator iter;
  for(iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    if((*iter)->index == index)
    {
      visualHandle = (*iter)->visual;
      break;
    }
  }
  return visualHandle;
}

/**
 * Move visual from source to destination container
 */
void MoveVisual(RegisteredVisualContainer::Iterator sourceIter, RegisteredVisualContainer& source, RegisteredVisualContainer& destination)
{
  Toolkit::Visual::Base visual = (*sourceIter)->visual;
  if(visual)
  {
    RegisteredVisual* rv = source.Release(sourceIter);
    destination.PushBack(rv);
  }
}

/**
 * Performs actions as requested using the action name.
 * @param[in] object The object on which to perform the action.
 * @param[in] actionName The action to perform.
 * @param[in] attributes The attributes with which to perfrom this action.
 * @return true if action has been accepted by this control
 */
const char* ACTION_ACCESSIBILITY_ACTIVATED         = "accessibilityActivated";
const char* ACTION_ACCESSIBILITY_READING_CANCELLED = "ReadingCancelled";
const char* ACTION_ACCESSIBILITY_READING_PAUSED    = "ReadingPaused";
const char* ACTION_ACCESSIBILITY_READING_RESUMED   = "ReadingResumed";
const char* ACTION_ACCESSIBILITY_READING_SKIPPED   = "ReadingSkipped";
const char* ACTION_ACCESSIBILITY_READING_STOPPED   = "ReadingStopped";

static bool DoAction(BaseObject* object, const std::string& actionName, const Property::Map& attributes)
{
  bool ret = true;

  Dali::BaseHandle handle(object);

  Toolkit::Control control = Toolkit::Control::DownCast(handle);

  DALI_ASSERT_ALWAYS(control);

  if(0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_ACTIVATED) ||
     actionName == "activate")
  {
    // if cast succeeds there is an implementation so no need to check
    if(!DevelControl::AccessibilityActivateSignal(control).Empty())
      DevelControl::AccessibilityActivateSignal(control).Emit();
    else
      ret = Internal::GetImplementation(control).OnAccessibilityActivated();
  }
  else if(0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_SKIPPED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!DevelControl::AccessibilityReadingSkippedSignal(control).Empty())
      DevelControl::AccessibilityReadingSkippedSignal(control).Emit();
  }
  else if(0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_PAUSED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!DevelControl::AccessibilityReadingPausedSignal(control).Empty())
      DevelControl::AccessibilityReadingPausedSignal(control).Emit();
  }
  else if(0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_RESUMED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!DevelControl::AccessibilityReadingResumedSignal(control).Empty())
      DevelControl::AccessibilityReadingResumedSignal(control).Emit();
  }
  else if(0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_CANCELLED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!DevelControl::AccessibilityReadingCancelledSignal(control).Empty())
      DevelControl::AccessibilityReadingCancelledSignal(control).Emit();
  }
  else if(0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_STOPPED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!DevelControl::AccessibilityReadingStoppedSignal(control).Empty())
      DevelControl::AccessibilityReadingStoppedSignal(control).Emit();
  }
  else
  {
    ret = false;
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
const char* SIGNAL_KEY_EVENT              = "keyEvent";
const char* SIGNAL_KEY_INPUT_FOCUS_GAINED = "keyInputFocusGained";
const char* SIGNAL_KEY_INPUT_FOCUS_LOST   = "keyInputFocusLost";
const char* SIGNAL_TAPPED                 = "tapped";
const char* SIGNAL_PANNED                 = "panned";
const char* SIGNAL_PINCHED                = "pinched";
const char* SIGNAL_LONG_PRESSED           = "longPressed";
const char* SIGNAL_GET_NAME               = "getName";
const char* SIGNAL_GET_DESCRIPTION        = "getDescription";
const char* SIGNAL_DO_GESTURE             = "doGesture";
static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  Dali::BaseHandle handle(object);

  bool             connected(false);
  Toolkit::Control control = Toolkit::Control::DownCast(handle);
  if(control)
  {
    Internal::Control& controlImpl(Internal::GetImplementation(control));
    connected = true;

    if(0 == strcmp(signalName.c_str(), SIGNAL_KEY_EVENT))
    {
      controlImpl.KeyEventSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.c_str(), SIGNAL_KEY_INPUT_FOCUS_GAINED))
    {
      controlImpl.KeyInputFocusGainedSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.c_str(), SIGNAL_KEY_INPUT_FOCUS_LOST))
    {
      controlImpl.KeyInputFocusLostSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.c_str(), SIGNAL_TAPPED))
    {
      controlImpl.EnableGestureDetection(GestureType::TAP);
      controlImpl.GetTapGestureDetector().DetectedSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.c_str(), SIGNAL_PANNED))
    {
      controlImpl.EnableGestureDetection(GestureType::PAN);
      controlImpl.GetPanGestureDetector().DetectedSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.c_str(), SIGNAL_PINCHED))
    {
      controlImpl.EnableGestureDetection(GestureType::PINCH);
      controlImpl.GetPinchGestureDetector().DetectedSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.c_str(), SIGNAL_LONG_PRESSED))
    {
      controlImpl.EnableGestureDetection(GestureType::LONG_PRESS);
      controlImpl.GetLongPressGestureDetector().DetectedSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.c_str(), SIGNAL_GET_NAME))
    {
      DevelControl::AccessibilityGetNameSignal(control).Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.c_str(), SIGNAL_GET_DESCRIPTION))
    {
      DevelControl::AccessibilityGetDescriptionSignal(control).Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.c_str(), SIGNAL_DO_GESTURE))
    {
      DevelControl::AccessibilityDoGestureSignal(control).Connect(tracker, functor);
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
DALI_TYPE_REGISTRATION_BEGIN(Control, CustomActor, Create);

// Note: Properties are registered separately below.

SignalConnectorType registerSignal1(typeRegistration, SIGNAL_KEY_EVENT, &DoConnectSignal);
SignalConnectorType registerSignal2(typeRegistration, SIGNAL_KEY_INPUT_FOCUS_GAINED, &DoConnectSignal);
SignalConnectorType registerSignal3(typeRegistration, SIGNAL_KEY_INPUT_FOCUS_LOST, &DoConnectSignal);
SignalConnectorType registerSignal4(typeRegistration, SIGNAL_TAPPED, &DoConnectSignal);
SignalConnectorType registerSignal5(typeRegistration, SIGNAL_PANNED, &DoConnectSignal);
SignalConnectorType registerSignal6(typeRegistration, SIGNAL_PINCHED, &DoConnectSignal);
SignalConnectorType registerSignal7(typeRegistration, SIGNAL_LONG_PRESSED, &DoConnectSignal);
SignalConnectorType registerSignal8(typeRegistration, SIGNAL_GET_NAME, &DoConnectSignal);
SignalConnectorType registerSignal9(typeRegistration, SIGNAL_GET_DESCRIPTION, &DoConnectSignal);
SignalConnectorType registerSignal10(typeRegistration, SIGNAL_DO_GESTURE, &DoConnectSignal);

TypeAction registerAction1(typeRegistration, "activate", &DoAction);
TypeAction registerAction2(typeRegistration, ACTION_ACCESSIBILITY_ACTIVATED, &DoAction);
TypeAction registerAction3(typeRegistration, ACTION_ACCESSIBILITY_READING_SKIPPED, &DoAction);
TypeAction registerAction4(typeRegistration, ACTION_ACCESSIBILITY_READING_CANCELLED, &DoAction);
TypeAction registerAction5(typeRegistration, ACTION_ACCESSIBILITY_READING_STOPPED, &DoAction);
TypeAction registerAction6(typeRegistration, ACTION_ACCESSIBILITY_READING_PAUSED, &DoAction);
TypeAction registerAction7(typeRegistration, ACTION_ACCESSIBILITY_READING_RESUMED, &DoAction);

DALI_TYPE_REGISTRATION_END()

/**
 * @brief Iterate through given container and setOffScene any visual found
 *
 * @param[in] container Container of visuals
 * @param[in] parent Parent actor to remove visuals from
 */
void SetVisualsOffScene(const RegisteredVisualContainer& container, Actor parent)
{
  for(auto iter = container.Begin(), end = container.End(); iter != end; iter++)
  {
    if((*iter)->visual)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::SetOffScene Setting visual(%d) off stage\n", (*iter)->index);
      Toolkit::GetImplementation((*iter)->visual).SetOffScene(parent);
    }
  }
}

} // unnamed namespace

// clang-format off
// Properties registered without macro to use specific member variables.
const PropertyRegistration Control::Impl::PROPERTY_1(typeRegistration,  "styleName",                      Toolkit::Control::Property::STYLE_NAME,                            Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_4(typeRegistration,  "keyInputFocus",                  Toolkit::Control::Property::KEY_INPUT_FOCUS,                       Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_5(typeRegistration,  "background",                     Toolkit::Control::Property::BACKGROUND,                            Property::MAP,     &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_6(typeRegistration,  "margin",                         Toolkit::Control::Property::MARGIN,                                Property::EXTENTS, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_7(typeRegistration,  "padding",                        Toolkit::Control::Property::PADDING,                               Property::EXTENTS, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_8(typeRegistration,  "tooltip",                        Toolkit::DevelControl::Property::TOOLTIP,                          Property::MAP,     &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_9(typeRegistration,  "state",                          Toolkit::DevelControl::Property::STATE,                            Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_10(typeRegistration, "subState",                       Toolkit::DevelControl::Property::SUB_STATE,                        Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_11(typeRegistration, "leftFocusableActorId",           Toolkit::DevelControl::Property::LEFT_FOCUSABLE_ACTOR_ID,          Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_12(typeRegistration, "rightFocusableActorId",          Toolkit::DevelControl::Property::RIGHT_FOCUSABLE_ACTOR_ID,         Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_13(typeRegistration, "upFocusableActorId",             Toolkit::DevelControl::Property::UP_FOCUSABLE_ACTOR_ID,            Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_14(typeRegistration, "downFocusableActorId",           Toolkit::DevelControl::Property::DOWN_FOCUSABLE_ACTOR_ID,          Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_15(typeRegistration, "shadow",                         Toolkit::DevelControl::Property::SHADOW,                           Property::MAP,     &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_16(typeRegistration, "accessibilityAttributes",        Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES,         Property::MAP,     &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_17(typeRegistration, "accessibilityName",              Toolkit::DevelControl::Property::ACCESSIBILITY_NAME,               Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_18(typeRegistration, "accessibilityDescription",       Toolkit::DevelControl::Property::ACCESSIBILITY_DESCRIPTION,        Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_19(typeRegistration, "accessibilityTranslationDomain", Toolkit::DevelControl::Property::ACCESSIBILITY_TRANSLATION_DOMAIN, Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_20(typeRegistration, "accessibilityRole",              Toolkit::DevelControl::Property::ACCESSIBILITY_ROLE,               Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_21(typeRegistration, "accessibilityHighlightable",     Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE,      Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
// clang-format on

Control::Impl::Impl(Control& controlImpl)
: mControlImpl(controlImpl),
  mState(Toolkit::DevelControl::NORMAL),
  mSubStateName(""),
  mLeftFocusableActorId(-1),
  mRightFocusableActorId(-1),
  mUpFocusableActorId(-1),
  mDownFocusableActorId(-1),
  mStyleName(""),
  mBackgroundColor(Color::TRANSPARENT),
  mStartingPinchScale(nullptr),
  mMargin(0, 0, 0, 0),
  mPadding(0, 0, 0, 0),
  mKeyEventSignal(),
  mKeyInputFocusGainedSignal(),
  mKeyInputFocusLostSignal(),
  mResourceReadySignal(),
  mVisualEventSignal(),
  mAccessibilityGetNameSignal(),
  mAccessibilityGetDescriptionSignal(),
  mAccessibilityDoGestureSignal(),
  mPinchGestureDetector(),
  mPanGestureDetector(),
  mTapGestureDetector(),
  mLongPressGestureDetector(),
  mTooltip(NULL),
  mInputMethodContext(),
  mIdleCallback(nullptr),
  mFlags(Control::ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT)),
  mIsKeyboardNavigationSupported(false),
  mIsKeyboardFocusGroup(false),
  mIsEmittingResourceReadySignal(false),
  mNeedToEmitResourceReady(false)
{
  Dali::Accessibility::Accessible::RegisterControlAccessibilityGetter(
    [](Dali::Actor actor) -> Dali::Accessibility::Accessible* {
      return Control::Impl::GetAccessibilityObject(actor);
    });

  accessibilityConstructor = [](Dali::Actor actor) -> std::unique_ptr<Dali::Accessibility::Accessible> {
    return std::unique_ptr<Dali::Accessibility::Accessible>(new DevelControl::AccessibleImpl(actor,
                                                                                             Dali::Accessibility::Role::UNKNOWN));
  };

  size_t len = static_cast<size_t>(Dali::Accessibility::RelationType::MAX_COUNT);
  mAccessibilityRelations.reserve(len);
  for(auto i = 0u; i < len; ++i)
  {
    mAccessibilityRelations.push_back({});
  }
}

Control::Impl::~Impl()
{
  for(auto&& iter : mVisuals)
  {
    StopObservingVisual(iter->visual);
  }

  for(auto&& iter : mRemoveVisuals)
  {
    StopObservingVisual(iter->visual);
  }

  // All gesture detectors will be destroyed so no need to disconnect.
  delete mStartingPinchScale;

  if(mIdleCallback && Adaptor::IsAvailable())
  {
    // Removes the callback from the callback manager in case the control is destroyed before the callback is executed.
    Adaptor::Get().RemoveIdle(mIdleCallback);
  }
}

Control::Impl& Control::Impl::Get(Internal::Control& internalControl)
{
  return *internalControl.mImpl;
}

const Control::Impl& Control::Impl::Get(const Internal::Control& internalControl)
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

void Control::Impl::RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual)
{
  RegisterVisual(index, visual, VisualState::ENABLED, DepthIndexValue::NOT_SET);
}

void Control::Impl::RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, int depthIndex)
{
  RegisterVisual(index, visual, VisualState::ENABLED, DepthIndexValue::SET, depthIndex);
}

void Control::Impl::RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, bool enabled)
{
  RegisterVisual(index, visual, (enabled ? VisualState::ENABLED : VisualState::DISABLED), DepthIndexValue::NOT_SET);
}

void Control::Impl::RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, bool enabled, int depthIndex)
{
  RegisterVisual(index, visual, (enabled ? VisualState::ENABLED : VisualState::DISABLED), DepthIndexValue::SET, depthIndex);
}

void Control::Impl::RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, VisualState::Type enabled, DepthIndexValue::Type depthIndexValueSet, int depthIndex)
{
  DALI_LOG_INFO(gLogFilter, Debug::Concise, "RegisterVisual:%d \n", index);

  bool  visualReplaced(false);
  Actor self = mControlImpl.Self();

  // Set the depth index, if not set by caller this will be either the current visual depth, max depth of all visuals
  // or zero.
  int requiredDepthIndex = visual.GetDepthIndex();

  if(depthIndexValueSet == DepthIndexValue::SET)
  {
    requiredDepthIndex = depthIndex;
  }

  // Visual replacement, existing visual should only be removed from stage when replacement ready.
  if(!mVisuals.Empty())
  {
    RegisteredVisualContainer::Iterator registeredVisualsiter;
    // Check if visual (index) is already registered, this is the current visual.
    if(FindVisual(index, mVisuals, registeredVisualsiter))
    {
      Toolkit::Visual::Base& currentRegisteredVisual = (*registeredVisualsiter)->visual;
      if(currentRegisteredVisual)
      {
        // Store current visual depth index as may need to set the replacement visual to same depth
        const int currentDepthIndex = (*registeredVisualsiter)->visual.GetDepthIndex();

        // No longer required to know if the replaced visual's resources are ready
        StopObservingVisual(currentRegisteredVisual);

        // If control staged and visual enabled then visuals will be swapped once ready
        if(self.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE) && enabled)
        {
          // Check if visual is currently in the process of being replaced ( is in removal container )
          RegisteredVisualContainer::Iterator visualQueuedForRemoval;
          if(FindVisual(index, mRemoveVisuals, visualQueuedForRemoval))
          {
            // Visual with same index is already in removal container so current visual pending
            // Only the the last requested visual will be displayed so remove current visual which is staged but not ready.
            Toolkit::GetImplementation(currentRegisteredVisual).SetOffScene(self);
            mVisuals.Erase(registeredVisualsiter);
          }
          else
          {
            // current visual not already in removal container so add now.
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "RegisterVisual Move current registered visual to removal Queue: %d \n", index);
            MoveVisual(registeredVisualsiter, mVisuals, mRemoveVisuals);
          }
        }
        else
        {
          // Control not staged or visual disabled so can just erase from registered visuals and new visual will be added later.
          mVisuals.Erase(registeredVisualsiter);
        }

        // If we've not set the depth-index value and the new visual does not have a depth index applied to it, then use the previously set depth-index for this index
        if((depthIndexValueSet == DepthIndexValue::NOT_SET) &&
           (visual.GetDepthIndex() == 0))
        {
          requiredDepthIndex = currentDepthIndex;
        }
      }

      visualReplaced = true;
    }
  }

  // If not set, set the name of the visual to the same name as the control's property.
  // ( If the control has been type registered )
  if(visual.GetName().empty())
  {
    // returns empty string if index is not found as long as index is not -1
    std::string visualName = self.GetPropertyName(index);
    if(!visualName.empty())
    {
      DALI_LOG_INFO(gLogFilter, Debug::Concise, "Setting visual name for property %d to %s\n", index, visualName.c_str());
      visual.SetName(visualName);
    }
  }

  if(!visualReplaced) // New registration entry
  {
    // If we've not set the depth-index value, we have more than one visual and the visual does not have a depth index, then set it to be the highest
    if((depthIndexValueSet == DepthIndexValue::NOT_SET) &&
       (mVisuals.Size() > 0) &&
       (visual.GetDepthIndex() == 0))
    {
      int maxDepthIndex = std::numeric_limits<int>::min();

      RegisteredVisualContainer::ConstIterator       iter;
      const RegisteredVisualContainer::ConstIterator endIter = mVisuals.End();
      for(iter = mVisuals.Begin(); iter != endIter; iter++)
      {
        const int visualDepthIndex = (*iter)->visual.GetDepthIndex();
        if(visualDepthIndex > maxDepthIndex)
        {
          maxDepthIndex = visualDepthIndex;
        }
      }
      ++maxDepthIndex;                                 // Add one to the current maximum depth index so that our added visual appears on top
      requiredDepthIndex = std::max(0, maxDepthIndex); // Start at zero if maxDepth index belongs to a background
    }
  }

  if(visual)
  {
    // Set determined depth index
    visual.SetDepthIndex(requiredDepthIndex);

    // Monitor when the visual resources are ready
    StartObservingVisual(visual);

    DALI_LOG_INFO(gLogFilter, Debug::Concise, "New Visual registration index[%d] depth[%d]\n", index, requiredDepthIndex);
    RegisteredVisual* newRegisteredVisual = new RegisteredVisual(index, visual, (enabled == VisualState::ENABLED ? true : false), (visualReplaced && enabled));
    mVisuals.PushBack(newRegisteredVisual);

    Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(visual);
    // Put on stage if enabled and the control is already on the stage
    if((enabled == VisualState::ENABLED) && self.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
    {
      visualImpl.SetOnScene(self);
    }
    else if(visualImpl.IsResourceReady()) // When not being staged, check if visual already 'ResourceReady' before it was Registered. ( Resource may have been loaded already )
    {
      ResourceReady(visualImpl);
    }
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::RegisterVisual() Registered %s(%d), enabled:%s\n", visual.GetName().c_str(), index, enabled ? "true" : "false");
}

void Control::Impl::UnregisterVisual(Property::Index index)
{
  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(index, mVisuals, iter))
  {
    // stop observing visual
    StopObservingVisual((*iter)->visual);

    Actor self(mControlImpl.Self());
    Toolkit::GetImplementation((*iter)->visual).SetOffScene(self);
    (*iter)->visual.Reset();
    mVisuals.Erase(iter);
  }

  if(FindVisual(index, mRemoveVisuals, iter))
  {
    Actor self(mControlImpl.Self());
    Toolkit::GetImplementation((*iter)->visual).SetOffScene(self);
    (*iter)->pending = false;
    (*iter)->visual.Reset();
    mRemoveVisuals.Erase(iter);
  }
}

Toolkit::Visual::Base Control::Impl::GetVisual(Property::Index index) const
{
  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(index, mVisuals, iter))
  {
    return (*iter)->visual;
  }

  return Toolkit::Visual::Base();
}

void Control::Impl::EnableVisual(Property::Index index, bool enable)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "Control::EnableVisual(%d, %s)\n", index, enable ? "T" : "F");

  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(index, mVisuals, iter))
  {
    if((*iter)->enabled == enable)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::EnableVisual Visual %s(%d) already %s\n", (*iter)->visual.GetName().c_str(), index, enable ? "enabled" : "disabled");
      return;
    }

    (*iter)->enabled  = enable;
    Actor parentActor = mControlImpl.Self();
    if(mControlImpl.Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE)) // If control not on Scene then Visual will be added when SceneConnection is called.
    {
      if(enable)
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::EnableVisual Setting %s(%d) on stage \n", (*iter)->visual.GetName().c_str(), index);
        Toolkit::GetImplementation((*iter)->visual).SetOnScene(parentActor);
      }
      else
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::EnableVisual Setting %s(%d) off stage \n", (*iter)->visual.GetName().c_str(), index);
        Toolkit::GetImplementation((*iter)->visual).SetOffScene(parentActor); // No need to call if control not staged.
      }
    }
  }
  else
  {
    DALI_LOG_WARNING("Control::EnableVisual(%d, %s) FAILED - NO SUCH VISUAL\n", index, enable ? "T" : "F");
  }
}

bool Control::Impl::IsVisualEnabled(Property::Index index) const
{
  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(index, mVisuals, iter))
  {
    return (*iter)->enabled;
  }
  return false;
}

void Control::Impl::StopObservingVisual(Toolkit::Visual::Base& visual)
{
  Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(visual);

  // Stop observing the visual
  visualImpl.RemoveEventObserver(*this);
}

void Control::Impl::StartObservingVisual(Toolkit::Visual::Base& visual)
{
  Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(visual);

  // start observing the visual for events
  visualImpl.AddEventObserver(*this);
}

// Called by a Visual when it's resource is ready
void Control::Impl::ResourceReady(Visual::Base& object)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::Impl::ResourceReady() replacements pending[%d]\n", mRemoveVisuals.Count());

  Actor self = mControlImpl.Self();

  // A resource is ready, find resource in the registered visuals container and get its index
  for(auto registeredIter = mVisuals.Begin(), end = mVisuals.End(); registeredIter != end; ++registeredIter)
  {
    Internal::Visual::Base& registeredVisualImpl = Toolkit::GetImplementation((*registeredIter)->visual);

    if(&object == &registeredVisualImpl)
    {
      RegisteredVisualContainer::Iterator visualToRemoveIter;
      // Find visual with the same index in the removal container
      // Set if off stage as it's replacement is now ready.
      // Remove if from removal list as now removed from stage.
      // Set Pending flag on the ready visual to false as now ready.
      if(FindVisual((*registeredIter)->index, mRemoveVisuals, visualToRemoveIter))
      {
        (*registeredIter)->pending = false;
        Toolkit::GetImplementation((*visualToRemoveIter)->visual).SetOffScene(self);
        mRemoveVisuals.Erase(visualToRemoveIter);
      }
      break;
    }
  }

  // A visual is ready so control may need relayouting if staged
  if(self.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    mControlImpl.RelayoutRequest();
  }

  // Emit signal if all enabled visuals registered by the control are ready.
  if(IsResourceReady())
  {
    // Reset the flag
    mNeedToEmitResourceReady = false;

    EmitResourceReadySignal();
  }
}

void Control::Impl::NotifyVisualEvent(Visual::Base& object, Property::Index signalId)
{
  for(auto registeredIter = mVisuals.Begin(), end = mVisuals.End(); registeredIter != end; ++registeredIter)
  {
    Internal::Visual::Base& registeredVisualImpl = Toolkit::GetImplementation((*registeredIter)->visual);
    if(&object == &registeredVisualImpl)
    {
      Dali::Toolkit::Control handle(mControlImpl.GetOwner());
      mVisualEventSignal.Emit(handle, (*registeredIter)->index, signalId);
      break;
    }
  }
}

bool Control::Impl::IsResourceReady() const
{
  // Iterate through and check all the enabled visuals are ready
  for(auto visualIter = mVisuals.Begin();
      visualIter != mVisuals.End();
      ++visualIter)
  {
    const Toolkit::Visual::Base   visual     = (*visualIter)->visual;
    const Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(visual);

    // one of the enabled visuals is not ready
    if(!visualImpl.IsResourceReady() && (*visualIter)->enabled)
    {
      return false;
    }
  }
  return true;
}

Toolkit::Visual::ResourceStatus Control::Impl::GetVisualResourceStatus(Property::Index index) const
{
  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(index, mVisuals, iter))
  {
    const Toolkit::Visual::Base   visual     = (*iter)->visual;
    const Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(visual);
    return visualImpl.GetResourceStatus();
  }

  return Toolkit::Visual::ResourceStatus::PREPARING;
}

void Control::Impl::AddTransitions(Dali::Animation&               animation,
                                   const Toolkit::TransitionData& handle,
                                   bool                           createAnimation)
{
  // Setup a Transition from TransitionData.
  const Internal::TransitionData& transitionData = Toolkit::GetImplementation(handle);
  TransitionData::Iterator        end            = transitionData.End();
  for(TransitionData::Iterator iter = transitionData.Begin();
      iter != end;
      ++iter)
  {
    TransitionData::Animator* animator = (*iter);

    Toolkit::Visual::Base visual = GetVisualByName(mVisuals, animator->objectName);

    if(visual)
    {
#if defined(DEBUG_ENABLED)
      Dali::TypeInfo  typeInfo;
      ControlWrapper* controlWrapperImpl = dynamic_cast<ControlWrapper*>(&mControlImpl);
      if(controlWrapperImpl)
      {
        typeInfo = controlWrapperImpl->GetTypeInfo();
      }

      DALI_LOG_INFO(gLogFilter, Debug::Concise, "CreateTransition: Found %s visual for %s\n", visual.GetName().c_str(), typeInfo ? typeInfo.GetName().c_str() : "Unknown");
#endif
      Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(visual);
      visualImpl.AnimateProperty(animation, *animator);
    }
    else
    {
      DALI_LOG_INFO(gLogFilter, Debug::Concise, "CreateTransition: Could not find visual. Trying actors");
      // Otherwise, try any actor children of control (Including the control)
      Actor child = mControlImpl.Self().FindChildByName(animator->objectName);
      if(child)
      {
        Property::Index propertyIndex = child.GetPropertyIndex(animator->propertyKey);
        if(propertyIndex != Property::INVALID_INDEX)
        {
          if(animator->animate == false)
          {
            if(animator->targetValue.GetType() != Property::NONE)
            {
              child.SetProperty(propertyIndex, animator->targetValue);
            }
          }
          else // animate the property
          {
            if(animator->initialValue.GetType() != Property::NONE)
            {
              child.SetProperty(propertyIndex, animator->initialValue);
            }

            if(createAnimation && !animation)
            {
              animation = Dali::Animation::New(0.1f);
            }

            animation.AnimateTo(Property(child, propertyIndex),
                                animator->targetValue,
                                animator->alphaFunction,
                                TimePeriod(animator->timePeriodDelay,
                                           animator->timePeriodDuration));
          }
        }
      }
    }
  }
}

Dali::Animation Control::Impl::CreateTransition(const Toolkit::TransitionData& transitionData)
{
  Dali::Animation transition;

  if(transitionData.Count() > 0)
  {
    AddTransitions(transition, transitionData, true);
  }
  return transition;
}

void Control::Impl::DoAction(Dali::Property::Index visualIndex, Dali::Property::Index actionId, const Dali::Property::Value attributes)
{
  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(visualIndex, mVisuals, iter))
  {
    Toolkit::GetImplementation((*iter)->visual).DoAction(actionId, attributes);
  }
}

void Control::Impl::AppendAccessibilityAttribute(const std::string& key,
                                                 const std::string  value)
{
  Property::Value* val = mAccessibilityAttributes.Find(key);
  if(val)
  {
    mAccessibilityAttributes[key] = Property::Value(value);
  }
  else
  {
    mAccessibilityAttributes.Insert(key, value);
  }
}

void Control::Impl::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  Toolkit::Control control = Toolkit::Control::DownCast(BaseHandle(object));

  if(control)
  {
    Control& controlImpl(GetImplementation(control));

    switch(index)
    {
      case Toolkit::Control::Property::STYLE_NAME:
      {
        controlImpl.SetStyleName(value.Get<std::string>());
        break;
      }

      case Toolkit::DevelControl::Property::STATE:
      {
        bool                   withTransitions = true;
        const Property::Value* valuePtr        = &value;
        const Property::Map*   map             = value.GetMap();
        if(map)
        {
          Property::Value* value2 = map->Find("withTransitions");
          if(value2)
          {
            withTransitions = value2->Get<bool>();
          }

          valuePtr = map->Find("state");
        }

        if(valuePtr)
        {
          Toolkit::DevelControl::State state(controlImpl.mImpl->mState);
          if(Scripting::GetEnumerationProperty<Toolkit::DevelControl::State>(*valuePtr, ControlStateTable, ControlStateTableCount, state))
          {
            controlImpl.mImpl->SetState(state, withTransitions);
          }
        }
      }
      break;

      case Toolkit::DevelControl::Property::SUB_STATE:
      {
        std::string subState;
        if(value.Get(subState))
        {
          controlImpl.mImpl->SetSubState(subState);
        }
      }
      break;

      case Toolkit::DevelControl::Property::LEFT_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          controlImpl.mImpl->mLeftFocusableActorId = focusId;
        }
      }
      break;

      case Toolkit::DevelControl::Property::RIGHT_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          controlImpl.mImpl->mRightFocusableActorId = focusId;
        }
      }
      break;

      case Toolkit::DevelControl::Property::ACCESSIBILITY_NAME:
      {
        std::string name;
        if(value.Get(name))
        {
          controlImpl.mImpl->mAccessibilityName    = name;
          controlImpl.mImpl->mAccessibilityNameSet = true;
        }
        else
        {
          controlImpl.mImpl->mAccessibilityNameSet = false;
        }
      }
      break;

      case Toolkit::DevelControl::Property::ACCESSIBILITY_DESCRIPTION:
      {
        std::string txt;
        if(value.Get(txt))
        {
          controlImpl.mImpl->mAccessibilityDescription    = txt;
          controlImpl.mImpl->mAccessibilityDescriptionSet = true;
        }
        else
        {
          controlImpl.mImpl->mAccessibilityDescriptionSet = false;
        }
      }
      break;

      case Toolkit::DevelControl::Property::ACCESSIBILITY_TRANSLATION_DOMAIN:
      {
        std::string txt;
        if(value.Get(txt))
        {
          controlImpl.mImpl->mAccessibilityTranslationDomain    = txt;
          controlImpl.mImpl->mAccessibilityTranslationDomainSet = true;
        }
        else
        {
          controlImpl.mImpl->mAccessibilityTranslationDomainSet = false;
        }
      }
      break;

      case Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE:
      {
        bool highlightable;
        if(value.Get(highlightable))
        {
          controlImpl.mImpl->mAccessibilityHighlightable    = highlightable;
          controlImpl.mImpl->mAccessibilityHighlightableSet = true;
        }
        else
        {
          controlImpl.mImpl->mAccessibilityHighlightableSet = false;
        }
      }
      break;

      case Toolkit::DevelControl::Property::ACCESSIBILITY_ROLE:
      {
        Dali::Accessibility::Role val;
        if(value.Get(val))
        {
          controlImpl.mImpl->mAccessibilityRole = val;
        }
      }
      break;

      case Toolkit::DevelControl::Property::UP_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          controlImpl.mImpl->mUpFocusableActorId = focusId;
        }
      }
      break;

      case Toolkit::DevelControl::Property::DOWN_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          controlImpl.mImpl->mDownFocusableActorId = focusId;
        }
      }
      break;

      case Toolkit::Control::Property::KEY_INPUT_FOCUS:
      {
        if(value.Get<bool>())
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
        std::string          url;
        Vector4              color;
        const Property::Map* map = value.GetMap();
        if(map && !map->Empty())
        {
          controlImpl.SetBackground(*map);
        }
        else if(value.Get(url))
        {
          // don't know the size to load
          Toolkit::Visual::Base visual = Toolkit::VisualFactory::Get().CreateVisual(url, ImageDimensions());
          if(visual)
          {
            controlImpl.mImpl->RegisterVisual(Toolkit::Control::Property::BACKGROUND, visual, DepthIndex::BACKGROUND);
          }
        }
        else if(value.Get(color))
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

      case Toolkit::Control::Property::MARGIN:
      {
        Extents margin;
        if(value.Get(margin))
        {
          controlImpl.mImpl->SetMargin(margin);
        }
        break;
      }

      case Toolkit::Control::Property::PADDING:
      {
        Extents padding;
        if(value.Get(padding))
        {
          controlImpl.mImpl->SetPadding(padding);
        }
        break;
      }

      case Toolkit::DevelControl::Property::TOOLTIP:
      {
        TooltipPtr& tooltipPtr = controlImpl.mImpl->mTooltip;
        if(!tooltipPtr)
        {
          tooltipPtr = Tooltip::New(control);
        }
        tooltipPtr->SetProperties(value);
        break;
      }

      case Toolkit::DevelControl::Property::SHADOW:
      {
        const Property::Map* map = value.GetMap();
        if(map && !map->Empty())
        {
          controlImpl.mImpl->SetShadow(*map);
        }
        else
        {
          // The shadow is an empty property map, so we should clear the shadow
          controlImpl.mImpl->ClearShadow();
        }
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES:
      {
        const Property::Map* map = value.GetMap();
        if(map && !map->Empty())
        {
          controlImpl.mImpl->mAccessibilityAttributes = *map;
        }
        break;
      }
    }
  }
}

Property::Value Control::Impl::GetProperty(BaseObject* object, Property::Index index)
{
  Property::Value value;

  Toolkit::Control control = Toolkit::Control::DownCast(BaseHandle(object));

  if(control)
  {
    Control& controlImpl(GetImplementation(control));

    switch(index)
    {
      case Toolkit::Control::Property::STYLE_NAME:
      {
        value = controlImpl.GetStyleName();
        break;
      }

      case Toolkit::DevelControl::Property::STATE:
      {
        value = controlImpl.mImpl->mState;
        break;
      }

      case Toolkit::DevelControl::Property::SUB_STATE:
      {
        value = controlImpl.mImpl->mSubStateName;
        break;
      }

      case Toolkit::DevelControl::Property::LEFT_FOCUSABLE_ACTOR_ID:
      {
        value = controlImpl.mImpl->mLeftFocusableActorId;
        break;
      }

      case Toolkit::DevelControl::Property::RIGHT_FOCUSABLE_ACTOR_ID:
      {
        value = controlImpl.mImpl->mRightFocusableActorId;
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_NAME:
      {
        if(controlImpl.mImpl->mAccessibilityNameSet)
        {
          value = controlImpl.mImpl->mAccessibilityName;
        }
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_DESCRIPTION:
      {
        if(controlImpl.mImpl->mAccessibilityDescriptionSet)
        {
          value = controlImpl.mImpl->mAccessibilityDescription;
        }
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_TRANSLATION_DOMAIN:
      {
        if(controlImpl.mImpl->mAccessibilityTranslationDomainSet)
        {
          value = controlImpl.mImpl->mAccessibilityTranslationDomain;
        }
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE:
      {
        if(controlImpl.mImpl->mAccessibilityHighlightableSet)
        {
          value = controlImpl.mImpl->mAccessibilityHighlightable;
        }
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_ROLE:
      {
        value = Property::Value(controlImpl.mImpl->mAccessibilityRole);
        break;
      }

      case Toolkit::DevelControl::Property::UP_FOCUSABLE_ACTOR_ID:
      {
        value = controlImpl.mImpl->mUpFocusableActorId;
        break;
      }

      case Toolkit::DevelControl::Property::DOWN_FOCUSABLE_ACTOR_ID:
      {
        value = controlImpl.mImpl->mDownFocusableActorId;
        break;
      }

      case Toolkit::Control::Property::KEY_INPUT_FOCUS:
      {
        value = controlImpl.HasKeyInputFocus();
        break;
      }

      case Toolkit::Control::Property::BACKGROUND:
      {
        Property::Map         map;
        Toolkit::Visual::Base visual = controlImpl.mImpl->GetVisual(Toolkit::Control::Property::BACKGROUND);
        if(visual)
        {
          visual.CreatePropertyMap(map);
        }

        value = map;
        break;
      }

      case Toolkit::Control::Property::MARGIN:
      {
        value = controlImpl.mImpl->GetMargin();
        break;
      }

      case Toolkit::Control::Property::PADDING:
      {
        value = controlImpl.mImpl->GetPadding();
        break;
      }

      case Toolkit::DevelControl::Property::TOOLTIP:
      {
        Property::Map map;
        if(controlImpl.mImpl->mTooltip)
        {
          controlImpl.mImpl->mTooltip->CreatePropertyMap(map);
        }
        value = map;
        break;
      }

      case Toolkit::DevelControl::Property::SHADOW:
      {
        Property::Map         map;
        Toolkit::Visual::Base visual = controlImpl.mImpl->GetVisual(Toolkit::DevelControl::Property::SHADOW);
        if(visual)
        {
          visual.CreatePropertyMap(map);
        }

        value = map;
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES:
      {
        value = controlImpl.mImpl->mAccessibilityAttributes;
        break;
      }
    }
  }

  return value;
}

void Control::Impl::RemoveAccessibilityAttribute(const std::string& key)
{
  Property::Value* val = mAccessibilityAttributes.Find(key);
  if(val)
    mAccessibilityAttributes[key] = Property::Value();
}

void Control::Impl::ClearAccessibilityAttributes()
{
  mAccessibilityAttributes.Clear();
}

void Control::Impl::SetAccessibilityReadingInfoType(const Dali::Accessibility::ReadingInfoTypes types)
{
  std::string value;
  if(types[Dali::Accessibility::ReadingInfoType::NAME])
  {
    value += READING_INFO_TYPE_NAME;
  }
  if(types[Dali::Accessibility::ReadingInfoType::ROLE])
  {
    if(!value.empty())
    {
      value += READING_INFO_TYPE_SEPARATOR;
    }
    value += READING_INFO_TYPE_ROLE;
  }
  if(types[Dali::Accessibility::ReadingInfoType::DESCRIPTION])
  {
    if(!value.empty())
    {
      value += READING_INFO_TYPE_SEPARATOR;
    }
    value += READING_INFO_TYPE_DESCRIPTION;
  }
  if(types[Dali::Accessibility::ReadingInfoType::STATE])
  {
    if(!value.empty())
    {
      value += READING_INFO_TYPE_SEPARATOR;
    }
    value += READING_INFO_TYPE_STATE;
  }
  AppendAccessibilityAttribute(READING_INFO_TYPE_ATTRIBUTE_NAME, value);
}

Dali::Accessibility::ReadingInfoTypes Control::Impl::GetAccessibilityReadingInfoType() const
{
  std::string value;
  auto        place = mAccessibilityAttributes.Find(READING_INFO_TYPE_ATTRIBUTE_NAME);
  if(place)
  {
    place->Get(value);
  }
  else
  {
    Dali::Accessibility::ReadingInfoTypes types;
    types[Dali::Accessibility::ReadingInfoType::NAME] = true;
    types[Dali::Accessibility::ReadingInfoType::ROLE] = true;
    types[Dali::Accessibility::ReadingInfoType::DESCRIPTION] = true;
    types[Dali::Accessibility::ReadingInfoType::STATE] = true;
    return types;
  }

  if(value.empty())
  {
    return {};
  }

  Dali::Accessibility::ReadingInfoTypes types;

  if(value.find(READING_INFO_TYPE_NAME) != std::string::npos)
  {
    types[Dali::Accessibility::ReadingInfoType::NAME] = true;
  }
  if(value.find(READING_INFO_TYPE_ROLE) != std::string::npos)
  {
    types[Dali::Accessibility::ReadingInfoType::ROLE] = true;
  }
  if(value.find(READING_INFO_TYPE_DESCRIPTION) != std::string::npos)
  {
    types[Dali::Accessibility::ReadingInfoType::DESCRIPTION] = true;
  }
  if(value.find(READING_INFO_TYPE_STATE) != std::string::npos)
  {
    types[Dali::Accessibility::ReadingInfoType::STATE] = true;
  }

  return types;
}

void Control::Impl::CopyInstancedProperties(RegisteredVisualContainer& visuals, Dictionary<Property::Map>& instancedProperties)
{
  for(RegisteredVisualContainer::Iterator iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    if((*iter)->visual)
    {
      Property::Map instanceMap;
      Toolkit::GetImplementation((*iter)->visual).CreateInstancePropertyMap(instanceMap);
      instancedProperties.Add((*iter)->visual.GetName(), instanceMap);
    }
  }
}

void Control::Impl::RemoveVisual(RegisteredVisualContainer& visuals, const std::string& visualName)
{
  Actor self(mControlImpl.Self());

  for(RegisteredVisualContainer::Iterator visualIter = visuals.Begin();
      visualIter != visuals.End();
      ++visualIter)
  {
    Toolkit::Visual::Base visual = (*visualIter)->visual;
    if(visual && visual.GetName() == visualName)
    {
      Toolkit::GetImplementation(visual).SetOffScene(self);
      (*visualIter)->visual.Reset();
      visuals.Erase(visualIter);
      break;
    }
  }
}

void Control::Impl::RemoveVisuals(RegisteredVisualContainer& visuals, DictionaryKeys& removeVisuals)
{
  Actor self(mControlImpl.Self());
  for(DictionaryKeys::iterator iter = removeVisuals.begin(); iter != removeVisuals.end(); ++iter)
  {
    const std::string visualName = *iter;
    RemoveVisual(visuals, visualName);
  }
}

void Control::Impl::RecreateChangedVisuals(Dictionary<Property::Map>& stateVisualsToChange,
                                           Dictionary<Property::Map>& instancedProperties)
{
  Dali::CustomActor handle(mControlImpl.GetOwner());
  for(Dictionary<Property::Map>::iterator iter = stateVisualsToChange.Begin();
      iter != stateVisualsToChange.End();
      ++iter)
  {
    const std::string&   visualName = (*iter).key;
    const Property::Map& toMap      = (*iter).entry;

    Actor                               self = mControlImpl.Self();
    RegisteredVisualContainer::Iterator registeredVisualsiter;
    // Check if visual (visualName) is already registered, this is the current visual.
    if(FindVisual(visualName, mVisuals, registeredVisualsiter))
    {
      Toolkit::Visual::Base& visual = (*registeredVisualsiter)->visual;
      if(visual)
      {
        // No longer required to know if the replaced visual's resources are ready
        StopObservingVisual(visual);

        // If control staged then visuals will be swapped once ready
        if(self.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
        {
          // Check if visual is currently in the process of being replaced ( is in removal container )
          RegisteredVisualContainer::Iterator visualQueuedForRemoval;
          if(FindVisual(visualName, mRemoveVisuals, visualQueuedForRemoval))
          {
            // Visual with same visual name is already in removal container so current visual pending
            // Only the the last requested visual will be displayed so remove current visual which is staged but not ready.
            Toolkit::GetImplementation(visual).SetOffScene(self);
            (*registeredVisualsiter)->visual.Reset();
            mVisuals.Erase(registeredVisualsiter);
          }
          else
          {
            // current visual not already in removal container so add now.
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "RegisterVisual Move current registered visual to removal Queue: %s \n", visualName.c_str());
            MoveVisual(registeredVisualsiter, mVisuals, mRemoveVisuals);
          }
        }
        else
        {
          // Control not staged or visual disabled so can just erase from registered visuals and new visual will be added later.
          (*registeredVisualsiter)->visual.Reset();
          mVisuals.Erase(registeredVisualsiter);
        }
      }

      const Property::Map* instancedMap = instancedProperties.FindConst(visualName);
      Style::ApplyVisual(handle, visualName, toMap, instancedMap);
    }
  }
}

void Control::Impl::ReplaceStateVisualsAndProperties(const StylePtr oldState, const StylePtr newState, const std::string& subState)
{
  // Collect all old visual names
  DictionaryKeys stateVisualsToRemove;
  if(oldState)
  {
    oldState->visuals.GetKeys(stateVisualsToRemove);
    if(!subState.empty())
    {
      const StylePtr* oldSubState = oldState->subStates.FindConst(subState);
      if(oldSubState)
      {
        DictionaryKeys subStateVisualsToRemove;
        (*oldSubState)->visuals.GetKeys(subStateVisualsToRemove);
        Merge(stateVisualsToRemove, subStateVisualsToRemove);
      }
    }
  }

  // Collect all new visual properties
  Dictionary<Property::Map> stateVisualsToAdd;
  if(newState)
  {
    stateVisualsToAdd = newState->visuals;
    if(!subState.empty())
    {
      const StylePtr* newSubState = newState->subStates.FindConst(subState);
      if(newSubState)
      {
        stateVisualsToAdd.Merge((*newSubState)->visuals);
      }
    }
  }

  // If a name is in both add/remove, move it to change list.
  Dictionary<Property::Map> stateVisualsToChange;
  FindChangableVisuals(stateVisualsToAdd, stateVisualsToChange, stateVisualsToRemove);

  // Copy instanced properties (e.g. text label) of current visuals
  Dictionary<Property::Map> instancedProperties;
  CopyInstancedProperties(mVisuals, instancedProperties);

  // For each visual in remove list, remove from mVisuals
  RemoveVisuals(mVisuals, stateVisualsToRemove);

  // For each visual in add list, create and add to mVisuals
  Dali::CustomActor handle(mControlImpl.GetOwner());
  Style::ApplyVisuals(handle, stateVisualsToAdd, instancedProperties);

  // For each visual in change list, if it requires a new visual,
  // remove old visual, create and add to mVisuals
  RecreateChangedVisuals(stateVisualsToChange, instancedProperties);
}

void Control::Impl::SetState(DevelControl::State newState, bool withTransitions)
{
  DevelControl::State oldState = mState;
  Dali::CustomActor   handle(mControlImpl.GetOwner());
  DALI_LOG_INFO(gLogFilter, Debug::Concise, "Control::Impl::SetState: %s\n", (mState == DevelControl::NORMAL ? "NORMAL" : (mState == DevelControl::FOCUSED ? "FOCUSED" : (mState == DevelControl::DISABLED ? "DISABLED" : "NONE"))));

  if(mState != newState)
  {
    // If mState was Disabled, and new state is Focused, should probably
    // store that fact, e.g. in another property that FocusManager can access.
    mState = newState;

    // Trigger state change and transitions
    // Apply new style, if stylemanager is available
    Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();
    if(styleManager)
    {
      const StylePtr stylePtr = GetImpl(styleManager).GetRecordedStyle(Toolkit::Control(mControlImpl.GetOwner()));

      if(stylePtr)
      {
        std::string oldStateName = Scripting::GetEnumerationName<Toolkit::DevelControl::State>(oldState, ControlStateTable, ControlStateTableCount);
        std::string newStateName = Scripting::GetEnumerationName<Toolkit::DevelControl::State>(newState, ControlStateTable, ControlStateTableCount);

        const StylePtr* newStateStyle = stylePtr->subStates.Find(newStateName);
        const StylePtr* oldStateStyle = stylePtr->subStates.Find(oldStateName);
        if(oldStateStyle && newStateStyle)
        {
          // Only change if both state styles exist
          ReplaceStateVisualsAndProperties(*oldStateStyle, *newStateStyle, mSubStateName);
        }
      }
    }
  }
}

void Control::Impl::SetSubState(const std::string& subStateName, bool withTransitions)
{
  if(mSubStateName != subStateName)
  {
    // Get existing sub-state visuals, and unregister them
    Dali::CustomActor handle(mControlImpl.GetOwner());

    Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();
    if(styleManager)
    {
      const StylePtr stylePtr = GetImpl(styleManager).GetRecordedStyle(Toolkit::Control(mControlImpl.GetOwner()));
      if(stylePtr)
      {
        // Stringify state
        std::string stateName = Scripting::GetEnumerationName<Toolkit::DevelControl::State>(mState, ControlStateTable, ControlStateTableCount);

        const StylePtr* state = stylePtr->subStates.Find(stateName);
        if(state)
        {
          StylePtr stateStyle(*state);

          const StylePtr* newStateStyle = stateStyle->subStates.Find(subStateName);
          const StylePtr* oldStateStyle = stateStyle->subStates.Find(mSubStateName);
          if(oldStateStyle && newStateStyle)
          {
            std::string empty;
            ReplaceStateVisualsAndProperties(*oldStateStyle, *newStateStyle, empty);
          }
        }
      }
    }

    mSubStateName = subStateName;
  }
}

void Control::Impl::OnSceneDisconnection()
{
  Actor self = mControlImpl.Self();

  // Any visuals set for replacement but not yet ready should still be registered.
  // Reason: If a request was made to register a new visual but the control removed from scene before visual was ready
  // then when this control appears back on stage it should use that new visual.

  // Iterate through all registered visuals and set off scene
  SetVisualsOffScene(mVisuals, self);

  // Visuals pending replacement can now be taken out of the removal list and set off scene
  // Iterate through all replacement visuals and add to a move queue then set off scene
  for(auto removalIter = mRemoveVisuals.Begin(), end = mRemoveVisuals.End(); removalIter != end; removalIter++)
  {
    Toolkit::GetImplementation((*removalIter)->visual).SetOffScene(self);
  }

  for(auto replacedIter = mVisuals.Begin(), end = mVisuals.End(); replacedIter != end; replacedIter++)
  {
    (*replacedIter)->pending = false;
  }

  mRemoveVisuals.Clear();
}

void Control::Impl::SetMargin(Extents margin)
{
  mControlImpl.mImpl->mMargin = margin;

  // Trigger a size negotiation request that may be needed when setting a margin.
  mControlImpl.RelayoutRequest();
}

Extents Control::Impl::GetMargin() const
{
  return mControlImpl.mImpl->mMargin;
}

void Control::Impl::SetPadding(Extents padding)
{
  mControlImpl.mImpl->mPadding = padding;

  // Trigger a size negotiation request that may be needed when setting a padding.
  mControlImpl.RelayoutRequest();
}

Extents Control::Impl::GetPadding() const
{
  return mControlImpl.mImpl->mPadding;
}

void Control::Impl::SetInputMethodContext(InputMethodContext& inputMethodContext)
{
  mInputMethodContext = inputMethodContext;
}

bool Control::Impl::FilterKeyEvent(const KeyEvent& event)
{
  bool consumed(false);

  if(mInputMethodContext)
  {
    consumed = mInputMethodContext.FilterEventKey(event);
  }
  return consumed;
}

DevelControl::VisualEventSignalType& Control::Impl::VisualEventSignal()
{
  return mVisualEventSignal;
}

void Control::Impl::SetShadow(const Property::Map& map)
{
  Toolkit::Visual::Base visual = Toolkit::VisualFactory::Get().CreateVisual(map);
  visual.SetName("shadow");

  if(visual)
  {
    mControlImpl.mImpl->RegisterVisual(Toolkit::DevelControl::Property::SHADOW, visual, DepthIndex::BACKGROUND_EFFECT);

    mControlImpl.RelayoutRequest();
  }
}

void Control::Impl::ClearShadow()
{
  mControlImpl.mImpl->UnregisterVisual(Toolkit::DevelControl::Property::SHADOW);

  // Trigger a size negotiation request that may be needed when unregistering a visual.
  mControlImpl.RelayoutRequest();
}

Dali::Property Control::Impl::GetVisualProperty(Dali::Property::Index index, Dali::Property::Key visualPropertyKey)
{
  Toolkit::Visual::Base visual = GetVisualByIndex(mVisuals, index);
  if(visual)
  {
    Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(visual);
    return visualImpl.GetPropertyObject(visualPropertyKey);
  }

  Handle handle;
  return Dali::Property(handle, Property::INVALID_INDEX);
}

void Control::Impl::EmitResourceReadySignal()
{
  if(!mIsEmittingResourceReadySignal)
  {
    // Guard against calls to emit the signal during the callback
    mIsEmittingResourceReadySignal = true;

    // If the signal handler changes visual, it may become ready during this call & therefore this method will
    // get called again recursively. If so, mNeedToEmitResourceReady is set below, and we act on it after that secondary
    // invocation has completed by notifying in an Idle callback to prevent further recursion.
    Dali::Toolkit::Control handle(mControlImpl.GetOwner());
    mResourceReadySignal.Emit(handle);

    if(mNeedToEmitResourceReady)
    {
      // Add idler to emit the signal again
      if(!mIdleCallback)
      {
        // The callback manager takes the ownership of the callback object.
        mIdleCallback = MakeCallback(this, &Control::Impl::OnIdleCallback);
        Adaptor::Get().AddIdle(mIdleCallback, false);
      }
    }

    mIsEmittingResourceReadySignal = false;
  }
  else
  {
    mNeedToEmitResourceReady = true;
  }
}

void Control::Impl::OnIdleCallback()
{
  if(mNeedToEmitResourceReady)
  {
    // Reset the flag
    mNeedToEmitResourceReady = false;

    // A visual is ready so control may need relayouting if staged
    if(mControlImpl.Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
    {
      mControlImpl.RelayoutRequest();
    }

    EmitResourceReadySignal();
  }

  // Set the pointer to null as the callback manager deletes the callback after execute it.
  mIdleCallback = nullptr;
}

Dali::Accessibility::Accessible* Control::Impl::GetAccessibilityObject()
{
  if(!accessibilityObject)
    accessibilityObject = accessibilityConstructor(mControlImpl.Self());
  return accessibilityObject.get();
}

Dali::Accessibility::Accessible* Control::Impl::GetAccessibilityObject(Dali::Actor actor)
{
  if(actor)
  {
    auto q = Dali::Toolkit::Control::DownCast(actor);
    if(q)
    {
      auto q2 = static_cast<Internal::Control*>(&q.GetImplementation());
      return q2->mImpl->GetAccessibilityObject();
    }
  }
  return nullptr;
}

void Control::Impl::CulledChangedCallback(PropertyNotification& p)
{
  if(Dali::Accessibility::IsUp())
  {
    auto self = Dali::Actor::DownCast(p.GetTarget());
    Dali::Accessibility::Accessible::Get(self)->EmitShowing(!self.GetProperty(DevelActor::Property::CULLED).Get<bool>());
  }
}

void Control::Impl::AccessibilityRegister()
{
  if(!accessibilityNotificationSet)
  {
    accessibilityNotificationCulled = mControlImpl.Self().AddPropertyNotification(DevelActor::Property::CULLED, LessThanCondition(0.5f));
    accessibilityNotificationCulled.SetNotifyMode(PropertyNotification::NOTIFY_ON_CHANGED);
    accessibilityNotificationCulled.NotifySignal().Connect(&Control::Impl::CulledChangedCallback);

    accessibilityNotificationSet = true;
  }
}

void Control::Impl::AccessibilityDeregister(bool remove)
{
  if(accessibilityNotificationSet)
  {
    accessibilityNotificationCulled.NotifySignal().Disconnect(&Control::Impl::CulledChangedCallback);
    if(remove)
    {
      mControlImpl.Self().RemovePropertyNotification(accessibilityNotificationCulled);
    }
    accessibilityNotificationCulled.Reset();
    accessibilityNotificationCulled   = {};
    accessibilityNotificationSet      = false;
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
