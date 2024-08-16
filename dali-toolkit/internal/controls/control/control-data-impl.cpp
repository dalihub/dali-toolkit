/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/object/object-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <cstring>
#include <limits>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/asset-manager/asset-manager.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/control-wrapper-impl.h>
#include <dali-toolkit/devel-api/visuals/visual-actions-devel.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/public-api/align-enumerations.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace
{
const char* READING_INFO_TYPE_NAME           = "name";
const char* READING_INFO_TYPE_ROLE           = "role";
const char* READING_INFO_TYPE_DESCRIPTION    = "description";
const char* READING_INFO_TYPE_STATE          = "state";
const char* READING_INFO_TYPE_ATTRIBUTE_NAME = "reading_info_type";
const char* READING_INFO_TYPE_SEPARATOR      = "|";
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
const Vector4      FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

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

/**
 *  Finds visual in given array, returning true if found along with the iterator for that visual as a out parameter
 */
bool FindVisual(const Toolkit::Visual::Base findVisual, const RegisteredVisualContainer& visuals, RegisteredVisualContainer::Iterator& iter)
{
  for(iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    Toolkit::Visual::Base visual = (*iter)->visual;
    if(visual && visual == findVisual)
    {
      return true;
    }
  }
  return false;
}

/**
 *  Finds internal visual in given array, returning true if found along with the iterator for that visual as a out parameter
 */
bool FindVisual(const Visual::Base& findInternalVisual, const RegisteredVisualContainer& visuals, RegisteredVisualContainer::Iterator& iter)
{
  for(iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    Visual::Base& visual = Toolkit::GetImplementation((*iter)->visual);
    if((&visual == &findInternalVisual))
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
 * Discard visual from source to visual factory.
 */
void DiscardVisual(RegisteredVisualContainer::Iterator sourceIter, RegisteredVisualContainer& source)
{
  Toolkit::Visual::Base visual = (*sourceIter)->visual;
  if(visual)
  {
    if(Stage::IsInstalled())
    {
      Toolkit::VisualFactory::Get().DiscardVisual(visual);
    }
  }

  source.Erase(sourceIter);
}

/**
 * Performs actions as requested using the action name.
 * @param[in] object The object on which to perform the action.
 * @param[in] actionName The action to perform.
 * @param[in] attributes The attributes with which to perfrom this action.
 * @return true if action has been accepted by this control
 */
constexpr const char* ACTION_ACCESSIBILITY_ACTIVATE  = "activate";
constexpr const char* ACTION_ACCESSIBILITY_ESCAPE    = "escape";
constexpr const char* ACTION_ACCESSIBILITY_INCREMENT = "increment";
constexpr const char* ACTION_ACCESSIBILITY_DECREMENT = "decrement";

// Legacy actions
constexpr const char* ACTION_ACCESSIBILITY_READING_CANCELLED = "ReadingCancelled";
constexpr const char* ACTION_ACCESSIBILITY_READING_PAUSED    = "ReadingPaused";
constexpr const char* ACTION_ACCESSIBILITY_READING_RESUMED   = "ReadingResumed";
constexpr const char* ACTION_ACCESSIBILITY_READING_SKIPPED   = "ReadingSkipped";
constexpr const char* ACTION_ACCESSIBILITY_READING_STOPPED   = "ReadingStopped";

bool PerformAccessibilityAction(Toolkit::Control control, const std::string& actionName, const Property::Map& attributes)
{
  using Dali::Accessibility::ActionType;
  DALI_ASSERT_DEBUG(control);
  DALI_ASSERT_DEBUG(!DevelControl::AccessibilityActionSignal(control).Empty());

  ActionType action = ActionType::MAX_COUNT;
  if(actionName == ACTION_ACCESSIBILITY_ACTIVATE)
  {
    action = ActionType::ACTIVATE;
  }
  else if(actionName == ACTION_ACCESSIBILITY_ESCAPE)
  {
    action = ActionType::ESCAPE;
  }
  else if(actionName == ACTION_ACCESSIBILITY_INCREMENT)
  {
    action = ActionType::INCREMENT;
  }
  else if(actionName == ACTION_ACCESSIBILITY_DECREMENT)
  {
    action = ActionType::DECREMENT;
  }

  if(action != ActionType::MAX_COUNT)
  {
    bool success = DevelControl::AccessibilityActionSignal(control).Emit(action);
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Performed AccessibilityAction: %s, success : %d\n", actionName.c_str(), success);
    return success;
  }

  return false;
}

bool PerformLegacyAccessibilityAction(Toolkit::Control control, const std::string& actionName)
{
  bool ret = true;
  if(0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_ACTIVATE))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!DevelControl::AccessibilityActivateSignal(control).Empty())
    {
      DevelControl::AccessibilityActivateSignal(control).Emit();
    }
    else
    {
      ret = Internal::GetImplementation(control).OnAccessibilityActivated();
    }
  }
  else if(0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_SKIPPED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!DevelControl::AccessibilityReadingSkippedSignal(control).Empty())
    {
      DevelControl::AccessibilityReadingSkippedSignal(control).Emit();
    }
  }
  else if(0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_PAUSED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!DevelControl::AccessibilityReadingPausedSignal(control).Empty())
    {
      DevelControl::AccessibilityReadingPausedSignal(control).Emit();
    }
  }
  else if(0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_RESUMED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!DevelControl::AccessibilityReadingResumedSignal(control).Empty())
    {
      DevelControl::AccessibilityReadingResumedSignal(control).Emit();
    }
  }
  else if(0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_CANCELLED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!DevelControl::AccessibilityReadingCancelledSignal(control).Empty())
    {
      DevelControl::AccessibilityReadingCancelledSignal(control).Emit();
    }
  }
  else if(0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_STOPPED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!DevelControl::AccessibilityReadingStoppedSignal(control).Empty())
    {
      DevelControl::AccessibilityReadingStoppedSignal(control).Emit();
    }
  }
  else
  {
    ret = false;
  }

  if(ret)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Performed Legacy AccessibilityAction: %s\n", actionName.c_str());
  }
  return ret;
}

bool DoAccessibilityAction(BaseObject* object, const std::string& actionName, const Property::Map& attributes)
{
  Dali::BaseHandle handle(object);

  Toolkit::Control control = Toolkit::Control::DownCast(handle);

  DALI_ASSERT_ALWAYS(control);

  if(!DevelControl::AccessibilityActionSignal(control).Empty())
  {
    return PerformAccessibilityAction(control, actionName, attributes);
  }

  // Fall back to legacy action is no ActionSignal is connected
  return PerformLegacyAccessibilityAction(control, actionName);
}

bool DoLegacyAccessibilityAction(BaseObject* object, const std::string& actionName, const Property::Map& attributes)
{
  Dali::BaseHandle handle(object);

  Toolkit::Control control = Toolkit::Control::DownCast(handle);

  DALI_ASSERT_ALWAYS(control);

  return PerformLegacyAccessibilityAction(control, actionName);
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

// === Accessibility Actions === START
TypeAction registerAction1(typeRegistration, ACTION_ACCESSIBILITY_ACTIVATE, &DoAccessibilityAction);
TypeAction registerAction2(typeRegistration, ACTION_ACCESSIBILITY_ESCAPE, &DoAccessibilityAction);
TypeAction registerAction3(typeRegistration, ACTION_ACCESSIBILITY_INCREMENT, &DoAccessibilityAction);
TypeAction registerAction4(typeRegistration, ACTION_ACCESSIBILITY_DECREMENT, &DoAccessibilityAction);
// === Accessibility Actions === END

// === Legacy Accessibility Actions === START
TypeAction registerAction5(typeRegistration, ACTION_ACCESSIBILITY_READING_SKIPPED, &DoLegacyAccessibilityAction);
TypeAction registerAction6(typeRegistration, ACTION_ACCESSIBILITY_READING_CANCELLED, &DoLegacyAccessibilityAction);
TypeAction registerAction7(typeRegistration, ACTION_ACCESSIBILITY_READING_STOPPED, &DoLegacyAccessibilityAction);
TypeAction registerAction8(typeRegistration, ACTION_ACCESSIBILITY_READING_PAUSED, &DoLegacyAccessibilityAction);
TypeAction registerAction9(typeRegistration, ACTION_ACCESSIBILITY_READING_RESUMED, &DoLegacyAccessibilityAction);
// === Legacy Accessibility Actions === END

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

Dali::Rect<> GetShowingGeometry(Dali::Rect<> rect, Dali::Toolkit::DevelControl::ControlAccessible* accessible)
{
  Rect<>  parentRect;
  Vector2 currentPosition;
  auto    parent = dynamic_cast<Toolkit::DevelControl::ControlAccessible*>(accessible->GetParent());

  while(parent)
  {
    parentRect = parent->GetExtents(Dali::Accessibility::CoordinateType::WINDOW);

    currentPosition.x = rect.x;
    currentPosition.y = rect.y;

    rect.x      = rect.x > parentRect.x ? rect.x : parentRect.x;
    rect.y      = rect.y > parentRect.y ? rect.y : parentRect.y;
    rect.width  = currentPosition.x + rect.width < parentRect.x + parentRect.width ? currentPosition.x + rect.width - rect.x : parentRect.x + parentRect.width - rect.x;
    rect.height = currentPosition.y + rect.height < parentRect.y + parentRect.height ? currentPosition.y + rect.height - rect.y : parentRect.y + parentRect.height - rect.y;

    if(rect.width < 0 || rect.height < 0)
    {
      return rect;
    }

    parent = dynamic_cast<Toolkit::DevelControl::ControlAccessible*>(parent->GetParent());
  }

  return rect;
}

static bool IsShowingGeometryOnScreen(Dali::Rect<> rect)
{
  return rect.width > 0 && rect.height > 0;
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
const PropertyRegistration Control::Impl::PROPERTY_16(typeRegistration, "accessibilityName",              Toolkit::DevelControl::Property::ACCESSIBILITY_NAME,               Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_17(typeRegistration, "accessibilityDescription",       Toolkit::DevelControl::Property::ACCESSIBILITY_DESCRIPTION,        Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_18(typeRegistration, "accessibilityTranslationDomain", Toolkit::DevelControl::Property::ACCESSIBILITY_TRANSLATION_DOMAIN, Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_19(typeRegistration, "accessibilityRole",              Toolkit::DevelControl::Property::ACCESSIBILITY_ROLE,               Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_20(typeRegistration, "accessibilityHighlightable",     Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE,      Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_21(typeRegistration, "accessibilityAttributes",        Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES,         Property::MAP,     &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_22(typeRegistration, "dispatchKeyEvents",              Toolkit::DevelControl::Property::DISPATCH_KEY_EVENTS,              Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_23(typeRegistration, "accessibilityHidden",            Toolkit::DevelControl::Property::ACCESSIBILITY_HIDDEN,             Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_24(typeRegistration, "clockwiseFocusableActorId",      Toolkit::DevelControl::Property::CLOCKWISE_FOCUSABLE_ACTOR_ID,     Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_25(typeRegistration, "counterClockwiseFocusableActorId", Toolkit::DevelControl::Property::COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID, Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_26(typeRegistration, "automationId",                   Toolkit::DevelControl::Property::AUTOMATION_ID,                    Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_27(typeRegistration, "accessibilityValue",             Toolkit::DevelControl::Property::ACCESSIBILITY_VALUE,                    Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);

// clang-format on

Control::Impl::Impl(Control& controlImpl)
: mControlImpl(controlImpl),
  mState(Toolkit::DevelControl::NORMAL),
  mSubStateName(""),
  mLeftFocusableActorId(-1),
  mRightFocusableActorId(-1),
  mUpFocusableActorId(-1),
  mDownFocusableActorId(-1),
  mClockwiseFocusableActorId(-1),
  mCounterClockwiseFocusableActorId(-1),
  mStyleName(""),
  mBackgroundColor(Color::TRANSPARENT),
  mStartingPinchScale(nullptr),
  mMargin(0, 0, 0, 0),
  mPadding(0, 0, 0, 0),
  mSize(0, 0),
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
  mIdleCallbackRegistered(false),
  mDispatchKeyEvents(true),
  mProcessorRegistered(false)
{
  Accessibility::Accessible::RegisterExternalAccessibleGetter([](Dali::Actor actor) -> std::shared_ptr<Accessibility::Accessible> {
    auto control = Toolkit::Control::DownCast(actor);
    if(!control)
    {
      return nullptr;
    }

    auto& controlImpl = Toolkit::Internal::GetImplementation(control);

    return controlImpl.GetAccessibleObject();
  });
}

Control::Impl::~Impl()
{
  while(!mVisuals.Empty())
  {
    auto iter = mVisuals.End() - 1u;
    StopObservingVisual((*iter)->visual);

    // Discard removed visual. It will be destroyed at next Idle time.
    DiscardVisual(iter, mVisuals);
  }

  while(!mRemoveVisuals.Empty())
  {
    auto removalIter = mRemoveVisuals.End() - 1u;
    StopObservingVisual((*removalIter)->visual);

    // Discard removed visual. It will be destroyed at next Idle time.
    DiscardVisual(removalIter, mRemoveVisuals);
  }

  // All gesture detectors will be destroyed so no need to disconnect.
  delete mStartingPinchScale;

  if(mProcessorRegistered && Adaptor::IsAvailable())
  {
    // Unregister the processor from the adaptor
    Adaptor::Get().UnregisterProcessorOnce(*this, true);
  }

  if(mIdleCallback && Adaptor::IsAvailable())
  {
    // Removes the callback from the callback manager in case the control is destroyed before the callback is executed.
    Adaptor::Get().RemoveIdle(mIdleCallback);
  }
}

Control::Impl& Control::Impl::Get(Internal::Control& internalControl)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  return *internalControl.mImpl;
}

const Control::Impl& Control::Impl::Get(const Internal::Control& internalControl)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  return *internalControl.mImpl;
}

void Control::Impl::CheckHighlightedObjectGeometry()
{
  auto accessible = GetAccessibleObject();
  if(DALI_LIKELY(accessible))
  {
    auto lastPosition   = accessible->GetLastPosition();
    auto accessibleRect = accessible->GetExtents(Dali::Accessibility::CoordinateType::WINDOW);
    auto rect           = GetShowingGeometry(accessibleRect, accessible.get());

    switch(mAccessibilityLastScreenRelativeMoveType)
    {
      case Dali::Accessibility::ScreenRelativeMoveType::OUTSIDE:
      {
        if(IsShowingGeometryOnScreen(rect))
        {
          mAccessibilityLastScreenRelativeMoveType = Dali::Accessibility::ScreenRelativeMoveType::INSIDE;
        }
        break;
      }
      case Dali::Accessibility::ScreenRelativeMoveType::INSIDE:
      {
        if(rect.width < 0 && !Dali::Equals(accessibleRect.x, lastPosition.x))
        {
          mAccessibilityLastScreenRelativeMoveType = (accessibleRect.x < lastPosition.x) ? Dali::Accessibility::ScreenRelativeMoveType::OUTGOING_TOP_LEFT : Dali::Accessibility::ScreenRelativeMoveType::OUTGOING_BOTTOM_RIGHT;
        }
        if(rect.height < 0 && !Dali::Equals(accessibleRect.y, lastPosition.y))
        {
          mAccessibilityLastScreenRelativeMoveType = (accessibleRect.y < lastPosition.y) ? Dali::Accessibility::ScreenRelativeMoveType::OUTGOING_TOP_LEFT : Dali::Accessibility::ScreenRelativeMoveType::OUTGOING_BOTTOM_RIGHT;
        }
        // notify AT-clients on outgoing moves only
        if(mAccessibilityLastScreenRelativeMoveType != Dali::Accessibility::ScreenRelativeMoveType::INSIDE)
        {
          accessible->EmitMovedOutOfScreen(mAccessibilityLastScreenRelativeMoveType);
        }
        break;
      }
      case Dali::Accessibility::ScreenRelativeMoveType::OUTGOING_TOP_LEFT:
      case Dali::Accessibility::ScreenRelativeMoveType::OUTGOING_BOTTOM_RIGHT:
      {
        if(IsShowingGeometryOnScreen(rect))
        {
          mAccessibilityLastScreenRelativeMoveType = Dali::Accessibility::ScreenRelativeMoveType::INSIDE;
        }
        else
        {
          mAccessibilityLastScreenRelativeMoveType = Dali::Accessibility::ScreenRelativeMoveType::OUTSIDE;
        }
        break;
      }
      default:
      {
        break;
      }
    }

    accessible->SetLastPosition(Vector2(accessibleRect.x, accessibleRect.y));
  }
}

void Control::Impl::RegisterAccessibilityPositionPropertyNotification()
{
  if(mIsAccessibilityPositionPropertyNotificationSet)
  {
    return;
  }
  // set default value until first move of object is detected
  mAccessibilityLastScreenRelativeMoveType = Dali::Accessibility::ScreenRelativeMoveType::OUTSIDE;
  // recalculate mAccessibilityLastScreenRelativeMoveType accordingly to the initial position
  CheckHighlightedObjectGeometry();
  mAccessibilityPositionNotification = mControlImpl.Self().AddPropertyNotification(Actor::Property::WORLD_POSITION, StepCondition(1.0f, 1.0f));
  mAccessibilityPositionNotification.SetNotifyMode(PropertyNotification::NOTIFY_ON_CHANGED);
  mAccessibilityPositionNotification.NotifySignal().Connect(this, [this](PropertyNotification&) { CheckHighlightedObjectGeometry(); });
  mIsAccessibilityPositionPropertyNotificationSet = true;
}

void Control::Impl::UnregisterAccessibilityPositionPropertyNotification()
{
  mControlImpl.Self().RemovePropertyNotification(mAccessibilityPositionNotification);
  mIsAccessibilityPositionPropertyNotificationSet = false;
}

void Control::Impl::RegisterAccessibilityPropertySetSignal()
{
  if(mIsAccessibilityPropertySetSignalRegistered)
  {
    return;
  }
  mControlImpl.Self().PropertySetSignal().Connect(this, &Control::Impl::OnAccessibilityPropertySet);
  mIsAccessibilityPropertySetSignalRegistered = true;
}

void Control::Impl::UnregisterAccessibilityPropertySetSignal()
{
  if(!mIsAccessibilityPropertySetSignalRegistered)
  {
    return;
  }
  mControlImpl.Self().PropertySetSignal().Disconnect(this, &Control::Impl::OnAccessibilityPropertySet);
  mIsAccessibilityPropertySetSignalRegistered = false;
}

void Control::Impl::OnAccessibilityPropertySet(Dali::Handle& handle, Dali::Property::Index index, const Dali::Property::Value& value)
{
  auto accessible = GetAccessibleObject();
  if(DALI_LIKELY(accessible))
  {
    if(mAccessibilityGetNameSignal.Empty())
    {
      if(index == DevelControl::Property::ACCESSIBILITY_NAME || (mAccessibilityProps.name.empty() && index == accessible->GetNamePropertyIndex()))
      {
        accessible->Emit(Dali::Accessibility::ObjectPropertyChangeEvent::NAME);
      }
    }

    if(mAccessibilityGetDescriptionSignal.Empty())
    {
      if(index == DevelControl::Property::ACCESSIBILITY_DESCRIPTION || (mAccessibilityProps.description.empty() && index == accessible->GetDescriptionPropertyIndex()))
      {
        accessible->Emit(Dali::Accessibility::ObjectPropertyChangeEvent::DESCRIPTION);
      }
    }
  }
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
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

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

  // Change the depth index value automatically if the visual has DepthIndex to AUTO_INDEX
  // or if RegisterVisual set DepthIndex to AUTO_INDEX.
  const bool requiredDepthIndexChanged = (requiredDepthIndex == DepthIndex::AUTO_INDEX);

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

        // If the visual have a depth index as AUTO_INDEX and the new visual does not have a depth index applied to it, then use the previously set depth-index for this index
        if(requiredDepthIndexChanged)
        {
          requiredDepthIndex = currentDepthIndex;
          DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Use replaced visual index. VisualDepthIndex AUTO_INDEX set as: %d\n", requiredDepthIndex);
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
    // If we have more than one visual and the visual have a depth index as AUTO_INDEX, then set it to be the highest
    if((mVisuals.Size() > 0) && requiredDepthIndexChanged)
    {
      int maxDepthIndex = static_cast<int>(DepthIndex::CONTENT) - 1; // Start at DepthIndex::CONTENT if maxDepth index belongs to a background or no visuals have been added yet.

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
      requiredDepthIndex = ++maxDepthIndex; // Add one to the current maximum depth index so that our added visual appears on top.
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Use top of all visuals. VisualDepthIndex AUTO_INDEX set as: %d\n", requiredDepthIndex);
    }
  }

  if(visual)
  {
    // If required depth index still DepthIndex::AUTO_INDEX, Make it as DepthIndex::CONTENT now
    if(requiredDepthIndex == static_cast<int>(DepthIndex::AUTO_INDEX))
    {
      requiredDepthIndex = static_cast<int>(DepthIndex::CONTENT);
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Some strange cases. VisualDepthIndex AUTO_INDEX set as: %d\n", requiredDepthIndex);
    }

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
    else if(enabled && visualImpl.IsResourceReady()) // When not being staged, check if visual already 'ResourceReady' before it was Registered. ( Resource may have been loaded already )
    {
      ResourceReady(visualImpl);
    }
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::RegisterVisual() Registered %s(%d), enabled:%s\n", visual.GetName().c_str(), index, enabled ? "true" : "false");
}

void Control::Impl::UnregisterVisual(Property::Index index)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

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

    // Discard removed visual. It will be destroyed at next Idle time.
    DiscardVisual(iter, mRemoveVisuals);
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

void Control::Impl::EnableReadyTransitionOverriden(Toolkit::Visual::Base& visual, bool enable)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "Control::EnableReadyTransitionOverriden(%p, %s)\n", visual, enable ? "T" : "F");

  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(visual, mVisuals, iter))
  {
    if((*iter)->overideReadyTransition == enable)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::EnableReadyTransitionOverriden Visual %s(%p) already %s\n", (*iter)->visual.GetName().c_str(), visual, enable ? "enabled" : "disabled");
      return;
    }

    (*iter)->overideReadyTransition = enable;
  }
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

void Control::Impl::ResourceReady()
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  // Emit signal if all enabled visuals registered by the control are ready or there are no visuals.
  if(IsResourceReady())
  {
    EmitResourceReadySignal();
  }
}

// Called by a Visual when it's resource is ready
void Control::Impl::ResourceReady(Visual::Base& object)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::Impl::ResourceReady() replacements pending[%d]\n", mRemoveVisuals.Count());

  Actor self = mControlImpl.Self();

  RegisteredVisualContainer::Iterator registeredIter;

  // A resource is ready, find resource in the registered visuals container and get its index
  if(!FindVisual(object, mVisuals, registeredIter))
  {
    return;
  }

  RegisteredVisualContainer::Iterator visualToRemoveIter;
  // Find visual with the same index in the removal container
  // Set if off stage as it's replacement is now ready.
  // Remove if from removal list as now removed from stage.
  // Set Pending flag on the ready visual to false as now ready.
  if(FindVisual((*registeredIter)->index, mRemoveVisuals, visualToRemoveIter))
  {
    (*registeredIter)->pending = false;
    if(!((*visualToRemoveIter)->overideReadyTransition))
    {
      Toolkit::GetImplementation((*visualToRemoveIter)->visual).SetOffScene(self);
    }

    // Discard removed visual. It will be destroyed at next Idle time.
    DiscardVisual(visualToRemoveIter, mRemoveVisuals);
  }

  // A visual is ready so control may need relayouting if staged
  RelayoutRequest(object);

  // Called by a Visual when it's resource is ready
  if(((*registeredIter)->enabled))
  {
    ResourceReady();
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

void Control::Impl::RelayoutRequest(Visual::Base& object)
{
  if(mControlImpl.Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    mControlImpl.RelayoutRequest();
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

void Control::Impl::DoActionExtension(Dali::Property::Index visualIndex, Dali::Property::Index actionId, Dali::Any attributes)
{
  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(visualIndex, mVisuals, iter))
  {
    Toolkit::GetImplementation((*iter)->visual).DoActionExtension(actionId, attributes);
  }
}

void Control::Impl::AppendAccessibilityAttribute(const std::string& key, const std::string value)
{
  Property::Value* checkedValue = mAccessibilityProps.extraAttributes.Find(key);
  if(checkedValue)
  {
    mAccessibilityProps.extraAttributes[key] = Property::Value(value);
  }
  else
  {
    mAccessibilityProps.extraAttributes.Insert(key, value);
  }
}

void Control::Impl::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

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

      case Toolkit::DevelControl::Property::ACCESSIBILITY_NAME:
      {
        std::string name;
        if(value.Get(name))
        {
          controlImpl.mImpl->mAccessibilityProps.name = std::move(name);
        }
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_DESCRIPTION:
      {
        std::string text;
        if(value.Get(text))
        {
          controlImpl.mImpl->mAccessibilityProps.description = std::move(text);
        }
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_ROLE:
      {
        Dali::Accessibility::Role role;
        if(value.Get(role))
        {
          controlImpl.mImpl->mAccessibilityProps.role = role;
        }
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE:
      {
        bool highlightable;
        if(value.Get(highlightable))
        {
          controlImpl.mImpl->mAccessibilityProps.isHighlightable = highlightable;
        }
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES:
      {
        const Property::Map* map = value.GetMap();
        if(map && !map->Empty())
        {
          controlImpl.mImpl->mAccessibilityProps.extraAttributes = *map;
        }
        break;
      }

      case Toolkit::DevelControl::Property::DISPATCH_KEY_EVENTS:
      {
        bool dispatch;
        if(value.Get(dispatch))
        {
          controlImpl.mImpl->mDispatchKeyEvents = dispatch;
        }
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_HIDDEN:
      {
        bool hidden;
        if(value.Get(hidden))
        {
          controlImpl.mImpl->mAccessibilityProps.isHidden = hidden;

          auto accessible = controlImpl.GetAccessibleObject();
          if(DALI_LIKELY(accessible))
          {
            auto* parent = dynamic_cast<Dali::Accessibility::ActorAccessible*>(accessible->GetParent());
            if(parent)
            {
              parent->OnChildrenChanged();
            }
          }
        }
        break;
      }
      case Toolkit::DevelControl::Property::CLOCKWISE_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          controlImpl.mImpl->mClockwiseFocusableActorId = focusId;
        }
        break;
      }
      case Toolkit::DevelControl::Property::COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          controlImpl.mImpl->mCounterClockwiseFocusableActorId = focusId;
        }
        break;
      }

      case Toolkit::DevelControl::Property::AUTOMATION_ID:
      {
        std::string automationId;
        if(value.Get(automationId))
        {
          controlImpl.mImpl->mAccessibilityProps.automationId = std::move(automationId);
        }
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_VALUE:
      {
        std::string accessibilityValue;
        if(value.Get(accessibilityValue))
        {
          controlImpl.mImpl->mAccessibilityProps.value = std::move(accessibilityValue);
        }
        break;
      }
    }
  }
}

Property::Value Control::Impl::GetProperty(BaseObject* object, Property::Index index)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

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

      case Toolkit::DevelControl::Property::ACCESSIBILITY_NAME:
      {
        value = controlImpl.mImpl->mAccessibilityProps.name;
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_DESCRIPTION:
      {
        value = controlImpl.mImpl->mAccessibilityProps.description;
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_ROLE:
      {
        value = Property::Value(controlImpl.mImpl->mAccessibilityProps.role);
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE:
      {
        value = controlImpl.mImpl->mAccessibilityProps.isHighlightable;
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES:
      {
        value = controlImpl.mImpl->mAccessibilityProps.extraAttributes;
        break;
      }

      case Toolkit::DevelControl::Property::DISPATCH_KEY_EVENTS:
      {
        value = controlImpl.mImpl->mDispatchKeyEvents;
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_HIDDEN:
      {
        value = controlImpl.mImpl->mAccessibilityProps.isHidden;
        break;
      }

      case Toolkit::DevelControl::Property::CLOCKWISE_FOCUSABLE_ACTOR_ID:
      {
        value = controlImpl.mImpl->mClockwiseFocusableActorId;
        break;
      }

      case Toolkit::DevelControl::Property::COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID:
      {
        value = controlImpl.mImpl->mCounterClockwiseFocusableActorId;
        break;
      }

      case Toolkit::DevelControl::Property::AUTOMATION_ID:
      {
        value = controlImpl.mImpl->mAccessibilityProps.automationId;
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_VALUE:
      {
        value = controlImpl.mImpl->mAccessibilityProps.value;
        break;
      }
    }
  }

  return value;
}

void Control::Impl::RemoveAccessibilityAttribute(const std::string& key)
{
  Property::Value* value = mAccessibilityProps.extraAttributes.Find(key);
  if(value)
  {
    mAccessibilityProps.extraAttributes[key] = Property::Value();
  }
}

void Control::Impl::ClearAccessibilityAttributes()
{
  mAccessibilityProps.extraAttributes.Clear();
}

void Control::Impl::SetAccessibilityReadingInfoType(const Dali::Accessibility::ReadingInfoTypes types)
{
  std::string value{};
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
  std::string value{};
  auto        place = mAccessibilityProps.extraAttributes.Find(READING_INFO_TYPE_ATTRIBUTE_NAME);
  if(place)
  {
    place->Get(value);
  }
  else
  {
    Dali::Accessibility::ReadingInfoTypes types;
    types[Dali::Accessibility::ReadingInfoType::NAME]        = true;
    types[Dali::Accessibility::ReadingInfoType::ROLE]        = true;
    types[Dali::Accessibility::ReadingInfoType::DESCRIPTION] = true;
    types[Dali::Accessibility::ReadingInfoType::STATE]       = true;
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
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

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

  if(!mRemoveVisuals.Empty())
  {
    std::reverse(mRemoveVisuals.Begin(), mRemoveVisuals.End());

    while(!mRemoveVisuals.Empty())
    {
      auto removalIter = mRemoveVisuals.End() - 1u;
      Toolkit::GetImplementation((*removalIter)->visual).SetOffScene(self);

      // Discard removed visual. It will be destroyed at next Idle time.
      DiscardVisual(removalIter, mRemoveVisuals);
    }
  }

  for(auto replacedIter = mVisuals.Begin(), end = mVisuals.End(); replacedIter != end; replacedIter++)
  {
    (*replacedIter)->pending = false;
  }
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
    return visualImpl.GetPropertyObject(std::move(visualPropertyKey));
  }

  Handle handle;
  return Dali::Property(handle, Property::INVALID_INDEX);
}

void Control::Impl::CreateTransitions(std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>>& sourceProperties,
                                      std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>>& destinationProperties,
                                      Dali::Toolkit::Control                                              source,
                                      Dali::Toolkit::Control                                              destination)
{
  // Retrieves background properties to be transitioned.
  Dali::Property::Map backgroundSourcePropertyMap, backgroundDestinationPropertyMap;
  mControlImpl.MakeVisualTransition(backgroundSourcePropertyMap, backgroundDestinationPropertyMap, source, destination, Toolkit::Control::Property::BACKGROUND);
  if(backgroundSourcePropertyMap.Count() > 0)
  {
    sourceProperties.push_back(std::pair<Dali::Property::Index, Dali::Property::Map>(Toolkit::Control::Property::BACKGROUND, backgroundSourcePropertyMap));
    destinationProperties.push_back(std::pair<Dali::Property::Index, Dali::Property::Map>(Toolkit::Control::Property::BACKGROUND, backgroundDestinationPropertyMap));
  }

  // Retrieves shadow properties to be transitioned.
  Dali::Property::Map shadowSourcePropertyMap, shadowDestinationPropertyMap;
  mControlImpl.MakeVisualTransition(shadowSourcePropertyMap, shadowDestinationPropertyMap, source, destination, Toolkit::DevelControl::Property::SHADOW);
  if(shadowSourcePropertyMap.Count() > 0)
  {
    sourceProperties.push_back(std::pair<Dali::Property::Index, Dali::Property::Map>(Toolkit::DevelControl::Property::SHADOW, shadowSourcePropertyMap));
    destinationProperties.push_back(std::pair<Dali::Property::Index, Dali::Property::Map>(Toolkit::DevelControl::Property::SHADOW, shadowDestinationPropertyMap));
  }

  // Retrieves transition from inherited class.
  mControlImpl.OnCreateTransitions(sourceProperties, destinationProperties, source, destination);
}

void Control::Impl::UpdateVisualProperties(const std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>>& properties)
{
  for(auto&& data : properties)
  {
    if(data.first == Toolkit::Control::Property::BACKGROUND)
    {
      DoAction(Toolkit::Control::Property::BACKGROUND, DevelVisual::Action::UPDATE_PROPERTY, data.second);
    }
    else if(data.first == Toolkit::DevelControl::Property::SHADOW)
    {
      DoAction(Toolkit::DevelControl::Property::SHADOW, DevelVisual::Action::UPDATE_PROPERTY, data.second);
    }
  }
  mControlImpl.OnUpdateVisualProperties(properties);
}

void Control::Impl::EmitResourceReadySignal()
{
  if(DALI_LIKELY(Stage::IsInstalled())) ///< Avoid resource ready callback during shutting down
  {
    if(!mIsEmittingResourceReadySignal)
    {
      // Guard against calls to emit the signal during the callback
      mIsEmittingResourceReadySignal = true;

      // If the signal handler changes visual, it may become ready during this call & therefore this method will
      // get called again recursively. If so, mIdleCallbackRegistered is set below, and we act on it after that secondary
      // invocation has completed by notifying in an Idle callback to prevent further recursion.
      Dali::Toolkit::Control handle(mControlImpl.GetOwner());
      mResourceReadySignal.Emit(handle);

      mIsEmittingResourceReadySignal = false;
    }
    else
    {
      if(!mIdleCallbackRegistered)
      {
        mIdleCallbackRegistered = true;

        // Add idler to emit the signal again
        if(!mIdleCallback)
        {
          // The callback manager takes the ownership of the callback object.
          mIdleCallback = MakeCallback(this, &Control::Impl::OnIdleCallback);
          if(DALI_UNLIKELY(!Adaptor::Get().AddIdle(mIdleCallback, true)))
          {
            DALI_LOG_ERROR("Fail to add idle callback for control resource ready. Skip this callback.\n");
            mIdleCallback           = nullptr;
            mIdleCallbackRegistered = false;
          }
        }
      }
    }
  }
}

bool Control::Impl::OnIdleCallback()
{
  // Reset the flag
  mIdleCallbackRegistered = false;

  // A visual is ready so control may need relayouting if staged
  if(mControlImpl.Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    mControlImpl.RelayoutRequest();
  }

  EmitResourceReadySignal();

  if(!mIdleCallbackRegistered)
  {
    // Set the pointer to null as the callback manager deletes the callback after execute it.
    mIdleCallback = nullptr;
  }

  // Repeat idle if mIdleCallbackRegistered become true one more time.
  return mIdleCallbackRegistered;
}

std::shared_ptr<Toolkit::DevelControl::ControlAccessible> Control::Impl::GetAccessibleObject()
{
  if(mAccessibleCreatable && !mAccessibleObject)
  {
    mAccessibleObject.reset(mControlImpl.CreateAccessibleObject());
  }

  return mAccessibleObject;
}

bool Control::Impl::IsAccessibleCreated() const
{
  return !!mAccessibleObject;
}

void Control::Impl::EnableCreateAccessible(bool enable)
{
  mAccessibleCreatable = enable;
}

bool Control::Impl::IsCreateAccessibleEnabled() const
{
  return mAccessibleCreatable;
}

void Control::Impl::ApplyFittingMode(const Vector2& size)
{
  Actor self = mControlImpl.Self();
  for(RegisteredVisualContainer::Iterator iter = mVisuals.Begin(); iter != mVisuals.End(); iter++)
  {
    // Check whether the visual is empty and enabled
    if((*iter)->visual && (*iter)->enabled)
    {
      Internal::Visual::Base& visualImpl = Toolkit::GetImplementation((*iter)->visual);

      // If the current visual is using the transform property map, fittingMode will not be applied.
      if(visualImpl.IsIgnoreFittingMode())
      {
        continue;
      }

      Visual::FittingMode fittingMode  = visualImpl.GetFittingMode();
      Property::Map       transformMap = Property::Map();

      // If the fittingMode is DONT_CARE, we don't need to apply fittingMode, just Set empty transformMap
      if(fittingMode == Visual::FittingMode::DONT_CARE)
      {
        if(visualImpl.GetType() != Toolkit::Visual::Type::TEXT)
        {
          ((*iter)->visual).SetTransformAndSize(transformMap, size);
        }
        continue;
      }

      Extents padding = self.GetProperty<Extents>(Toolkit::Control::Property::PADDING);

      bool zeroPadding = (padding == Extents());

      Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>(
        self.GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());
      if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
      {
        std::swap(padding.start, padding.end);
      }

      // remove padding from the size to know how much is left for the visual
      Vector2 finalSize   = size - Vector2(padding.start + padding.end, padding.top + padding.bottom);
      Vector2 finalOffset = Vector2(padding.start, padding.top);

      // Reset PIXEL_AREA after using OVER_FIT_KEEP_ASPECT_RATIO
      if(visualImpl.IsPixelAreaSetForFittingMode())
      {
        visualImpl.SetPixelAreaForFittingMode(FULL_TEXTURE_RECT);
      }

      if((!zeroPadding) || // If padding is not zero
         (fittingMode != Visual::FittingMode::FILL))
      {
        visualImpl.SetTransformMapUsageForFittingMode(true);

        Vector2 naturalSize;
        // NaturalSize will not be used for FILL fitting mode, which is default.
        // Skip GetNaturalSize
        if(fittingMode != Visual::FittingMode::FILL)
        {
          ((*iter)->visual).GetNaturalSize(naturalSize);
        }

        // If FittingMode use FIT_WIDTH or FIT_HEIGTH, it need to change proper fittingMode
        if(fittingMode == Visual::FittingMode::FIT_WIDTH || fittingMode == Visual::FittingMode::FIT_HEIGHT)
        {
          const float widthRatio  = !Dali::EqualsZero(naturalSize.width) ? (finalSize.width / naturalSize.width) : 0.0f;
          const float heightRatio = !Dali::EqualsZero(naturalSize.height) ? (finalSize.height / naturalSize.height) : 0.0f;
          if(widthRatio < heightRatio)
          {
            // Final size has taller form than natural size.
            fittingMode = (fittingMode == Visual::FittingMode::FIT_WIDTH) ? Visual::FittingMode::FIT_KEEP_ASPECT_RATIO : Visual::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO;
          }
          else
          {
            // Final size has wider form than natural size.
            fittingMode = (fittingMode == Visual::FittingMode::FIT_WIDTH) ? Visual::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO : Visual::FittingMode::FIT_KEEP_ASPECT_RATIO;
          }
        }

        // Calculate size for fittingMode
        switch(fittingMode)
        {
          case Visual::FittingMode::FIT_KEEP_ASPECT_RATIO:
          {
            auto availableVisualSize = finalSize;

            // scale to fit the padded area
            finalSize = naturalSize * std::min((!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0),
                                               (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0));

            // calculate final offset within the padded area
            finalOffset += (availableVisualSize - finalSize) * .5f;

            // populate the transform map
            transformMap.Add(Toolkit::Visual::Transform::Property::OFFSET, finalOffset)
              .Add(Toolkit::Visual::Transform::Property::SIZE, finalSize);
            break;
          }
          case Visual::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO:
          {
            auto availableVisualSize = finalSize;
            finalSize                = naturalSize * std::max((!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0.0f),
                                               (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0.0f));

            auto originalOffset = finalOffset;

            if(!visualImpl.IsPixelAreaSetForFittingMode() && !Dali::EqualsZero(finalSize.width) && !Dali::EqualsZero(finalSize.height))
            {
              float   x           = abs((availableVisualSize.width - finalSize.width) / finalSize.width) * .5f;
              float   y           = abs((availableVisualSize.height - finalSize.height) / finalSize.height) * .5f;
              float   widthRatio  = 1.f - abs((availableVisualSize.width - finalSize.width) / finalSize.width);
              float   heightRatio = 1.f - abs((availableVisualSize.height - finalSize.height) / finalSize.height);
              Vector4 pixelArea   = Vector4(x, y, widthRatio, heightRatio);
              visualImpl.SetPixelAreaForFittingMode(pixelArea);
            }

            // populate the transform map
            transformMap.Add(Toolkit::Visual::Transform::Property::OFFSET, originalOffset)
              .Add(Toolkit::Visual::Transform::Property::SIZE, availableVisualSize);
            break;
          }
          case Visual::FittingMode::CENTER:
          {
            auto availableVisualSize = finalSize;
            if(availableVisualSize.width > naturalSize.width && availableVisualSize.height > naturalSize.height)
            {
              finalSize = naturalSize;
            }
            else
            {
              finalSize = naturalSize * std::min((!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0.0f),
                                                 (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0.0f));
            }

            finalOffset += (availableVisualSize - finalSize) * .5f;

            // populate the transform map
            transformMap.Add(Toolkit::Visual::Transform::Property::OFFSET, finalOffset)
              .Add(Toolkit::Visual::Transform::Property::SIZE, finalSize);
            break;
          }
          case Visual::FittingMode::FILL:
          {
            transformMap.Add(Toolkit::Visual::Transform::Property::OFFSET, finalOffset)
              .Add(Toolkit::Visual::Transform::Property::SIZE, finalSize);
            break;
          }
          case Visual::FittingMode::FIT_WIDTH:
          case Visual::FittingMode::FIT_HEIGHT:
          case Visual::FittingMode::DONT_CARE:
          {
            // This FittingMode already converted
            break;
          }
        }

        // Set extra value for applying transformMap
        transformMap.Add(Toolkit::Visual::Transform::Property::OFFSET_POLICY,
                         Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE))
          .Add(Toolkit::Visual::Transform::Property::ORIGIN, Toolkit::Align::TOP_BEGIN)
          .Add(Toolkit::Visual::Transform::Property::ANCHOR_POINT, Toolkit::Align::TOP_BEGIN)
          .Add(Toolkit::Visual::Transform::Property::SIZE_POLICY,
               Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE));
      }
      else if(visualImpl.IsTransformMapSetForFittingMode() && zeroPadding) // Reset offset to zero only if padding applied previously
      {
        visualImpl.SetTransformMapUsageForFittingMode(false);

        // Reset the transform map
        transformMap.Add(Toolkit::Visual::Transform::Property::OFFSET, Vector2::ZERO)
          .Add(Toolkit::Visual::Transform::Property::OFFSET_POLICY,
               Vector2(Toolkit::Visual::Transform::Policy::RELATIVE, Toolkit::Visual::Transform::Policy::RELATIVE))
          .Add(Toolkit::Visual::Transform::Property::SIZE, Vector2::ONE)
          .Add(Toolkit::Visual::Transform::Property::SIZE_POLICY,
               Vector2(Toolkit::Visual::Transform::Policy::RELATIVE, Toolkit::Visual::Transform::Policy::RELATIVE));
      }

      ((*iter)->visual).SetTransformAndSize(transformMap, size);
    }
  }
}

void Control::Impl::RegisterProcessorOnce()
{
  if(!mProcessorRegistered)
  {
    Adaptor::Get().RegisterProcessorOnce(*this, true);
    mProcessorRegistered = true;
  }
}

void Control::Impl::Process(bool postProcessor)
{
  // Call ApplyFittingMode
  ApplyFittingMode(mSize);
  mProcessorRegistered = false;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
