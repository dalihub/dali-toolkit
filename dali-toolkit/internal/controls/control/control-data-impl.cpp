/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include "control-accessibility-data.h"
#include "control-visual-data.h"

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
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>
#include <dali-toolkit/internal/visuals/transition-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

#include <dali-toolkit/devel-api/visuals/visual-actions-devel.h>

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
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_CONTROL_DATA");
#endif

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

constexpr int INNER_SHADOW_DEPTH_INDEX = DepthIndex::DECORATION - 2;
constexpr int BORDERLINE_DEPTH_INDEX   = DepthIndex::DECORATION - 1;

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
    bool success = DevelControl::AccessibilityActionSignal(control).Emit({action, Dali::Actor{}});
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

/**
 * Connects a callback function with the object's signals.
 * @param[in] object The object providing the signal.
 * @param[in] tracker Used to disconnect the signal.
 * @param[in] signalName The signal to connect to.
 * @param[in] functor A newly allocated FunctorDelegate.
 * @return True if the signal was connected.
 * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
 */
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
const PropertyRegistration Control::Impl::PROPERTY_27(typeRegistration, "accessibilityValue",             Toolkit::DevelControl::Property::ACCESSIBILITY_VALUE,              Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_28(typeRegistration, "accessibilityScrollable",        Toolkit::DevelControl::Property::ACCESSIBILITY_SCROLLABLE,         Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_29(typeRegistration, "accessibilityStates",            Toolkit::DevelControl::Property::ACCESSIBILITY_STATES,             Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_30(typeRegistration, "accessibilityIsModal",           Toolkit::DevelControl::Property::ACCESSIBILITY_IS_MODAL,           Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_31(typeRegistration, "offScreenRendering",             Toolkit::DevelControl::Property::OFFSCREEN_RENDERING,              Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_32(typeRegistration, "innerShadow",                    Toolkit::DevelControl::Property::INNER_SHADOW,                     Property::MAP,     &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_33(typeRegistration, "borderline",                     Toolkit::DevelControl::Property::BORDERLINE,                       Property::MAP,     &Control::Impl::SetProperty, &Control::Impl::GetProperty);

const AnimatablePropertyRegistration Control::Impl::ANIMATABLE_PROPERTY_1(typeRegistration, "viewCornerRadius",       Toolkit::DevelControl::Property::CORNER_RADIUS,        Property::VECTOR4, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const AnimatablePropertyRegistration Control::Impl::ANIMATABLE_PROPERTY_2(typeRegistration, "viewCornerRadiusPolicy", Toolkit::DevelControl::Property::CORNER_RADIUS_POLICY, Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty); ///< Make animatable, for constarint-input
const AnimatablePropertyRegistration Control::Impl::ANIMATABLE_PROPERTY_3(typeRegistration, "viewCornerSquareness",   Toolkit::DevelControl::Property::CORNER_SQUARENESS,    Property::VECTOR4, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const AnimatablePropertyRegistration Control::Impl::ANIMATABLE_PROPERTY_4(typeRegistration, "viewBorderlineWidth",    Toolkit::DevelControl::Property::BORDERLINE_WIDTH,     Property::FLOAT,   &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const AnimatablePropertyRegistration Control::Impl::ANIMATABLE_PROPERTY_5(typeRegistration, "viewBorderlineColor",    Toolkit::DevelControl::Property::BORDERLINE_COLOR,     Property::VECTOR4, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const AnimatablePropertyRegistration Control::Impl::ANIMATABLE_PROPERTY_6(typeRegistration, "viewBorderlineOffset",   Toolkit::DevelControl::Property::BORDERLINE_OFFSET,    Property::FLOAT,   &Control::Impl::SetProperty, &Control::Impl::GetProperty);

// clang-format on

Control::Impl::Impl(Control& controlImpl)
: mControlImpl(controlImpl),
  mState(Toolkit::DevelControl::NORMAL),
  mSubStateName(""),
  mAccessibilityData(nullptr),
  mVisualData(nullptr),
  mLeftFocusableActorId(-1),
  mRightFocusableActorId(-1),
  mUpFocusableActorId(-1),
  mDownFocusableActorId(-1),
  mClockwiseFocusableActorId(-1),
  mCounterClockwiseFocusableActorId(-1),
  mStyleName(""),
  mBackgroundColor(Color::TRANSPARENT),
  mRenderEffect(nullptr),
  mStartingPinchScale(nullptr),
  mMargin(0, 0, 0, 0),
  mPadding(0, 0, 0, 0),
  mSize(0, 0),
  mKeyEventSignal(),
  mKeyInputFocusGainedSignal(),
  mKeyInputFocusLostSignal(),
  mResourceReadySignal(),
  mPinchGestureDetector(),
  mPanGestureDetector(),
  mTapGestureDetector(),
  mLongPressGestureDetector(),
  mDecorationData{nullptr},
  mOffScreenRenderingImpl(nullptr),
  mOffScreenRenderingType(DevelControl::OffScreenRenderingType::NONE),
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
  mAccessibilityData = std::make_unique<AccessibilityData>(mControlImpl);
}

Control::Impl::~Impl()
{
  if(mVisualData)
  {
    mVisualData->ClearVisuals();
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

void Control::Impl::InitializeVisualData()
{
  mVisualData = std::make_unique<Control::Impl::VisualData>(*this);
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

void Control::Impl::ResourceReady()
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  // Emit signal if all enabled visuals registered by the control are ready or there are no visuals.
  if(mVisualData->IsResourceReady())
  {
    EmitResourceReadySignal();
  }
}

void Control::Impl::RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->RegisterVisual(index, visual);
  }
}

void Control::Impl::RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, int depthIndex)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->RegisterVisual(index, visual, depthIndex);
  }
}

void Control::Impl::RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, bool enabled)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->RegisterVisual(index, visual, enabled);
  }
}

void Control::Impl::RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, bool enabled, int depthIndex)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->RegisterVisual(index, visual, enabled, depthIndex);
  }
}

void Control::Impl::UnregisterVisual(Property::Index index)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UnregisterVisual(index);
  }
}

Toolkit::Visual::Base Control::Impl::GetVisual(Property::Index index) const
{
  return Toolkit::Visual::Base(GetVisualImplPtr(index));
}

Toolkit::Internal::Visual::Base* Control::Impl::GetVisualImplPtr(Property::Index index) const
{
  if(DALI_LIKELY(mVisualData))
  {
    return mVisualData->GetVisualImplPtr(index);
  }
  return nullptr;
}

bool Control::Impl::IsResourceReady() const
{
  if(DALI_LIKELY(mVisualData))
  {
    return mVisualData->IsResourceReady();
  }
  return true;
}

void Control::Impl::OnSceneConnection()
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::OnSceneConnection number of registered visuals(%d)\n", mVisualData ? mVisualData->mVisuals.Size() : 0u);

  if(DALI_LIKELY(mVisualData))
  {
    Actor self = mControlImpl.Self();
    mVisualData->ConnectScene(self);
  }

  if(mOffScreenRenderingImpl) // mOffScreenRenderingType != NONE
  {
    mOffScreenRenderingImpl->SetOwnerControl(Toolkit::Control(mControlImpl.GetOwner()));
  }
}

void Control::Impl::OnSceneDisconnection()
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::OnSceneDisconnection number of registered visuals(%d)\n", mVisualData ? mVisualData->mVisuals.Size() : 0u);

  if(DALI_LIKELY(mVisualData))
  {
    Actor self = mControlImpl.Self();
    mVisualData->ClearScene(self);
  }

  if(mOffScreenRenderingImpl)
  {
    mOffScreenRenderingImpl->ClearOwnerControl();
  }
}

void Control::Impl::EnableReadyTransitionOverridden(Toolkit::Visual::Base& visual, bool enable)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->EnableReadyTransitionOverridden(visual, enable);
  }
}

void Control::Impl::EnableCornerPropertiesOverridden(Toolkit::Visual::Base& visual, bool enable)
{
  if(DALI_LIKELY(mVisualData))
  {
    Property::Map map;
    map[Toolkit::DevelVisual::Property::CORNER_RADIUS]        = DecorationData::GetCornerRadius(mDecorationData);
    map[Toolkit::DevelVisual::Property::CORNER_RADIUS_POLICY] = DecorationData::GetCornerRadiusPolicy(mDecorationData);
    map[Toolkit::DevelVisual::Property::CORNER_SQUARENESS]    = DecorationData::GetCornerSquareness(mDecorationData);

    mVisualData->EnableCornerPropertiesOverridden(visual, enable, map);
  }
}

void Control::Impl::EnableVisual(Property::Index index, bool enable)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->EnableVisual(index, enable);
  }
}

bool Control::Impl::IsVisualEnabled(Property::Index index) const
{
  if(DALI_LIKELY(mVisualData))
  {
    return mVisualData->IsVisualEnabled(index);
  }
  return false;
}

Toolkit::Visual::ResourceStatus Control::Impl::GetVisualResourceStatus(Property::Index index) const
{
  if(DALI_LIKELY(mVisualData))
  {
    return mVisualData->GetVisualResourceStatus(index);
  }
  return Toolkit::Visual::ResourceStatus::READY;
}

void Control::Impl::AddTransitions(Dali::Animation&               animation,
                                   const Toolkit::TransitionData& handle,
                                   bool                           createAnimation)
{
  if(DALI_UNLIKELY(!mVisualData))
  {
    return;
  }

  // Setup a Transition from TransitionData.
  const Internal::TransitionData& transitionData = Toolkit::GetImplementation(handle);
  TransitionData::Iterator        end            = transitionData.End();
  for(TransitionData::Iterator iter = transitionData.Begin();
      iter != end;
      ++iter)
  {
    TransitionData::Animator* animator = (*iter);

    Toolkit::Visual::Base visual = mVisualData->GetVisual(animator->objectName);
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

void Control::Impl::DoAction(Dali::Property::Index visualIndex, Dali::Property::Index actionId, const Dali::Property::Value attributes)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->DoAction(visualIndex, actionId, attributes);
  }
}

void Control::Impl::DoActionExtension(Dali::Property::Index visualIndex, Dali::Property::Index actionId, Dali::Any attributes)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->DoActionExtension(visualIndex, actionId, attributes);
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
          if(DALI_LIKELY(controlImpl.mImpl->mVisualData))
          {
            // don't know the size to load
            Toolkit::Visual::Base visual = Toolkit::VisualFactory::Get().CreateVisual(url, ImageDimensions());
            if(visual)
            {
              controlImpl.mImpl->mVisualData->RegisterVisual(Toolkit::Control::Property::BACKGROUND, visual, DepthIndex::BACKGROUND);
              controlImpl.mImpl->EnableCornerPropertiesOverridden(visual, true);
            }
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
          controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.name = std::move(name);
        }
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_DESCRIPTION:
      {
        std::string text;
        if(value.Get(text))
        {
          controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.description = std::move(text);
        }
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_ROLE:
      {
        int32_t role;
        if(value.Get(role))
        {
          controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.role = role;
        }
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE:
      {
        bool highlightable;
        if(value.Get(highlightable))
        {
          controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.isHighlightable = highlightable ? TriStateProperty::TRUE : TriStateProperty::FALSE;
        }
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES:
      {
        const Property::Map* map = value.GetMap();
        if(map && !map->Empty())
        {
          controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.extraAttributes = *map;
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
          controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.isHidden = hidden;

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
          controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.automationId = std::move(automationId);
        }
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_VALUE:
      {
        std::string accessibilityValue;
        if(value.Get(accessibilityValue))
        {
          controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.value = std::move(accessibilityValue);
        }
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_SCROLLABLE:
      {
        bool isScrollable;
        if(value.Get(isScrollable))
        {
          controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.isScrollable = isScrollable;
        }
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_STATES:
      {
        int32_t states;
        if(value.Get(states))
        {
          controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.states = Toolkit::DevelControl::AccessibilityStates{static_cast<uint32_t>(states)};
        }
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_IS_MODAL:
      {
        bool isModal;
        if(value.Get(isModal))
        {
          controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.isModal = isModal;
        }
        break;
      }

      case Toolkit::DevelControl::Property::OFFSCREEN_RENDERING:
      {
        int32_t offscreenRenderingType;
        if(value.Get(offscreenRenderingType))
        {
          controlImpl.mImpl->SetOffScreenRendering(offscreenRenderingType);
        }
        break;
      }

      case Toolkit::DevelControl::Property::INNER_SHADOW:
      {
        const Property::Map* map = value.GetMap();
        if(map && !map->Empty())
        {
          controlImpl.mImpl->SetInnerShadow(*map);
        }
        else
        {
          // The shadow is an empty property map, so we should clear the inner shadow
          controlImpl.mImpl->ClearInnerShadow();
        }
        break;
      }

      case Toolkit::DevelControl::Property::BORDERLINE:
      {
        const Property::Map* map = value.GetMap();
        if(map && !map->Empty())
        {
          controlImpl.mImpl->SetBorderline(*map, true);
        }
        else
        {
          // The shadow is an empty property map, so we should clear the inner shadow
          controlImpl.mImpl->ClearBorderline();
        }
        break;
      }

      case Toolkit::DevelControl::Property::CORNER_RADIUS:
      {
        Vector4 radius;
        if(value.Get(radius))
        {
          if(DecorationData::GetCornerRadius(controlImpl.mImpl->mDecorationData) != radius)
          {
            DecorationData::SetCornerRadius(controlImpl.mImpl->mDecorationData, radius);
            controlImpl.mImpl->UpdateCornerRadius();
          }
        }
        break;
      }

      case Toolkit::DevelControl::Property::CORNER_RADIUS_POLICY:
      {
        int policy;
        if(value.Get(policy))
        {
          if(DecorationData::GetCornerRadiusPolicy(controlImpl.mImpl->mDecorationData) != policy)
          {
            DecorationData::SetCornerRadiusPolicy(controlImpl.mImpl->mDecorationData, policy);
            controlImpl.mImpl->UpdateCornerRadius();
          }
        }
        break;
      }

      case Toolkit::DevelControl::Property::CORNER_SQUARENESS:
      {
        Vector4 squareness;
        if(value.Get(squareness))
        {
          if(DecorationData::GetCornerSquareness(controlImpl.mImpl->mDecorationData) != squareness)
          {
            DecorationData::SetCornerSquareness(controlImpl.mImpl->mDecorationData, squareness);
            controlImpl.mImpl->UpdateCornerRadius();
          }
        }
        break;
      }

      case Toolkit::DevelControl::Property::BORDERLINE_WIDTH:
      {
        float width;
        if(value.Get(width))
        {
          if(DecorationData::GetBorderlineWidth(controlImpl.mImpl->mDecorationData) != width)
          {
            DecorationData::SetBorderlineWidth(controlImpl.mImpl->mDecorationData, width);
            controlImpl.mImpl->UpdateBorderline();
          }
        }
        break;
      }

      case Toolkit::DevelControl::Property::BORDERLINE_COLOR:
      {
        Vector4 color;
        if(value.Get(color))
        {
          if(DecorationData::GetBorderlineColor(controlImpl.mImpl->mDecorationData) != color)
          {
            DecorationData::SetBorderlineColor(controlImpl.mImpl->mDecorationData, color);
            controlImpl.mImpl->UpdateBorderline();
          }
        }
        break;
      }

      case Toolkit::DevelControl::Property::BORDERLINE_OFFSET:
      {
        float offset;
        if(value.Get(offset))
        {
          if(DecorationData::GetBorderlineOffset(controlImpl.mImpl->mDecorationData) != offset)
          {
            DecorationData::SetBorderlineOffset(controlImpl.mImpl->mDecorationData, offset);
            controlImpl.mImpl->UpdateBorderline();
          }
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
        Property::Map map;

        if(DALI_LIKELY(controlImpl.mImpl->mVisualData))
        {
          const Toolkit::Internal::Visual::Base* visualImplPtr = controlImpl.mImpl->mVisualData->GetVisualImplPtr(Toolkit::Control::Property::BACKGROUND);
          if(visualImplPtr)
          {
            visualImplPtr->CreatePropertyMap(map);
          }
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
        Property::Map map;

        if(DALI_LIKELY(controlImpl.mImpl->mVisualData))
        {
          Toolkit::Visual::Base visual = controlImpl.mImpl->mVisualData->GetVisual(Toolkit::DevelControl::Property::SHADOW);
          if(visual)
          {
            visual.CreatePropertyMap(map);
          }
        }

        value = map;
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_NAME:
      {
        value = controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.name;
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_DESCRIPTION:
      {
        value = controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.description;
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_ROLE:
      {
        value = controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.role;
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE:
      {
        value = controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.isHighlightable == TriStateProperty::TRUE ? true : false;
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES:
      {
        value = controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.extraAttributes;
        break;
      }

      case Toolkit::DevelControl::Property::DISPATCH_KEY_EVENTS:
      {
        value = controlImpl.mImpl->mDispatchKeyEvents;
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_HIDDEN:
      {
        value = controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.isHidden;
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
        value = controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.automationId;
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_VALUE:
      {
        value = controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.value;
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_SCROLLABLE:
      {
        value = controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.isScrollable;
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_STATES:
      {
        value = static_cast<int32_t>(controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.states.GetRawData32());
        break;
      }

      case Toolkit::DevelControl::Property::ACCESSIBILITY_IS_MODAL:
      {
        value = controlImpl.mImpl->mAccessibilityData->mAccessibilityProps.isModal;
        break;
      }

      case Toolkit::DevelControl::Property::OFFSCREEN_RENDERING:
      {
        value = controlImpl.mImpl->mOffScreenRenderingType;
        break;
      }

      case Toolkit::DevelControl::Property::INNER_SHADOW:
      {
        Property::Map map;

        if(DALI_LIKELY(controlImpl.mImpl->mVisualData))
        {
          Toolkit::Visual::Base visual = controlImpl.mImpl->mVisualData->GetVisual(Toolkit::DevelControl::Property::INNER_SHADOW);
          if(visual)
          {
            visual.CreatePropertyMap(map);
          }
        }

        value = map;
        break;
      }

      case Toolkit::DevelControl::Property::BORDERLINE:
      {
        Property::Map map;

        if(DALI_LIKELY(controlImpl.mImpl->mVisualData))
        {
          Toolkit::Visual::Base visual = controlImpl.mImpl->mVisualData->GetVisual(Toolkit::DevelControl::Property::BORDERLINE);
          if(visual)
          {
            visual.CreatePropertyMap(map);
          }
        }

        value = map;
        break;
      }

      case Toolkit::DevelControl::Property::CORNER_RADIUS:
      {
        value = DecorationData::GetCornerRadius(controlImpl.mImpl->mDecorationData);
        break;
      }

      case Toolkit::DevelControl::Property::CORNER_RADIUS_POLICY:
      {
        value = DecorationData::GetCornerRadiusPolicy(controlImpl.mImpl->mDecorationData);
        break;
      }

      case Toolkit::DevelControl::Property::CORNER_SQUARENESS:
      {
        value = DecorationData::GetCornerSquareness(controlImpl.mImpl->mDecorationData);
        break;
      }

      case Toolkit::DevelControl::Property::BORDERLINE_WIDTH:
      {
        value = DecorationData::GetBorderlineWidth(controlImpl.mImpl->mDecorationData);
        break;
      }

      case Toolkit::DevelControl::Property::BORDERLINE_COLOR:
      {
        value = DecorationData::GetBorderlineColor(controlImpl.mImpl->mDecorationData);
        break;
      }

      case Toolkit::DevelControl::Property::BORDERLINE_OFFSET:
      {
        value = DecorationData::GetBorderlineOffset(controlImpl.mImpl->mDecorationData);
        break;
      }
    }
  }

  return value;
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

    if(DALI_LIKELY(mVisualData))
    {
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
            mVisualData->ReplaceStateVisualsAndProperties(*oldStateStyle, *newStateStyle, mSubStateName);
          }
        }
      }
    }
  }
}

void Control::Impl::SetSubState(const std::string& subStateName, bool withTransitions)
{
  if(mSubStateName != subStateName)
  {
    if(DALI_LIKELY(mVisualData))
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
              mVisualData->ReplaceStateVisualsAndProperties(*oldStateStyle, *newStateStyle, empty);
            }
          }
        }
      }
    }

    mSubStateName = subStateName;
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

DevelControl::VisualEventSignalType& Control::Impl::VisualEventSignal()
{
  DALI_ASSERT_ALWAYS(mVisualData && "Visual Disabled control cannot use VisualEventSignal!!");
  return mVisualData->VisualEventSignal();
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

void Control::Impl::AppendAccessibilityAttribute(const std::string& key, const std::string value)
{
  mAccessibilityData->AppendAccessibilityAttribute(key, value);
}

void Control::Impl::RemoveAccessibilityAttribute(const std::string& key)
{
  mAccessibilityData->RemoveAccessibilityAttribute(key);
}

void Control::Impl::ClearAccessibilityAttributes()
{
  mAccessibilityData->ClearAccessibilityAttributes();
}

void Control::Impl::SetAccessibilityReadingInfoType(const Dali::Accessibility::ReadingInfoTypes types)
{
  mAccessibilityData->SetAccessibilityReadingInfoType(types);
}

Dali::Accessibility::ReadingInfoTypes Control::Impl::GetAccessibilityReadingInfoType() const
{
  return mAccessibilityData->GetAccessibilityReadingInfoType();
}

void Control::Impl::CheckHighlightedObjectGeometry()
{
  mAccessibilityData->CheckHighlightedObjectGeometry();
}

void Control::Impl::RegisterAccessibilityPositionPropertyNotification()
{
  mAccessibilityData->RegisterAccessibilityPositionPropertyNotification();
}

void Control::Impl::UnregisterAccessibilityPositionPropertyNotification()
{
  mAccessibilityData->UnregisterAccessibilityPositionPropertyNotification();
}

void Control::Impl::RegisterAccessibilityPropertySetSignal()
{
  mAccessibilityData->RegisterAccessibilityPropertySetSignal();
}

void Control::Impl::UnregisterAccessibilityPropertySetSignal()
{
  mAccessibilityData->UnregisterAccessibilityPropertySetSignal();
}

void Control::Impl::OnAccessibilityPropertySet(Dali::Handle& handle, Dali::Property::Index index, const Dali::Property::Value& value)
{
  mAccessibilityData->OnAccessibilityPropertySet(handle, index, value);
}

bool Control::Impl::IsAccessibleCreated() const
{
  return mAccessibilityData->IsAccessibleCreated();
}

void Control::Impl::EnableCreateAccessible(bool enable)
{
  mAccessibilityData->EnableCreateAccessible(enable);
}

bool Control::Impl::IsCreateAccessibleEnabled() const
{
  return mAccessibilityData->IsCreateAccessibleEnabled();
}

void Control::Impl::ApplyFittingMode(const Vector2& size)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->ApplyFittingMode(size);
  }
}

void Control::Impl::SetShadow(const Property::Map& map)
{
  if(DALI_LIKELY(mVisualData))
  {
    Toolkit::Visual::Base visual = Toolkit::VisualFactory::Get().CreateVisual(map);
    visual.SetName("shadow");

    if(visual)
    {
      mVisualData->RegisterVisual(Toolkit::DevelControl::Property::SHADOW, visual, DepthIndex::BACKGROUND_EFFECT);
      EnableCornerPropertiesOverridden(visual, true);

      mControlImpl.RelayoutRequest();
    }
  }
}

void Control::Impl::ClearShadow()
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UnregisterVisual(Toolkit::DevelControl::Property::SHADOW);

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    mControlImpl.RelayoutRequest();
  }
}

void Control::Impl::SetInnerShadow(const Property::Map& map)
{
  if(DALI_LIKELY(mVisualData))
  {
    Toolkit::Visual::Base visual = Toolkit::VisualFactory::Get().CreateVisual(map);
    visual.SetName("innerShadow");

    if(visual)
    {
      mVisualData->RegisterVisual(Toolkit::DevelControl::Property::INNER_SHADOW, visual, INNER_SHADOW_DEPTH_INDEX);
      EnableCornerPropertiesOverridden(visual, true);

      mControlImpl.RelayoutRequest();
    }
  }
}

void Control::Impl::ClearInnerShadow()
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UnregisterVisual(Toolkit::DevelControl::Property::INNER_SHADOW);

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    mControlImpl.RelayoutRequest();
  }
}

void Control::Impl::SetBorderline(const Property::Map& map, bool forciblyCreate)
{
  if(DALI_LIKELY(mVisualData))
  {
    if(!forciblyCreate)
    {
      Toolkit::Internal::Visual::Base* previousVisualImplPtr = mVisualData->GetVisualImplPtr(Toolkit::DevelControl::Property::BORDERLINE);
      if(previousVisualImplPtr)
      {
        previousVisualImplPtr->DoAction(Toolkit::DevelVisual::Action::UPDATE_PROPERTY, map);
        return;
      }
    }
    Toolkit::Visual::Base visual = Toolkit::VisualFactory::Get().CreateVisual(map);
    visual.SetName("borderline");

    if(visual)
    {
      mVisualData->RegisterVisual(Toolkit::DevelControl::Property::BORDERLINE, visual, BORDERLINE_DEPTH_INDEX);
      EnableCornerPropertiesOverridden(visual, true);

      mControlImpl.RelayoutRequest();
    }
  }
}

void Control::Impl::ClearBorderline()
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UnregisterVisual(Toolkit::DevelControl::Property::BORDERLINE);

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    mControlImpl.RelayoutRequest();
  }
}

Dali::Property Control::Impl::GetVisualProperty(Dali::Property::Index index, Dali::Property::Key visualPropertyKey)
{
  if(DALI_LIKELY(mVisualData))
  {
    return mVisualData->GetVisualProperty(index, visualPropertyKey);
  }
  Dali::Handle handle;
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
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UpdateVisualProperties(properties);
  }
}

void Control::Impl::EmitResourceReadySignal()
{
  if(DALI_LIKELY(Dali::Adaptor::IsAvailable())) ///< Avoid resource ready callback during shutting down
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
  return mAccessibilityData->GetAccessibleObject();
}

void Control::Impl::RegisterProcessorOnce()
{
  if(DALI_LIKELY(mVisualData))
  {
    if(!mProcessorRegistered)
    {
      Adaptor::Get().RegisterProcessorOnce(*this, true);
      mProcessorRegistered = true;
    }
  }
}

void Control::Impl::SetOffScreenRendering(int32_t offScreenRenderingType)
{
  // Validate input
  {
    constexpr int32_t count = static_cast<int32_t>(DevelControl::OffScreenRenderingTypeCount);
    if(0 > offScreenRenderingType || offScreenRenderingType >= count)
    {
      DALI_LOG_ERROR("Failed to set offscreen rendering. Type index is out of bound.\n");
      return;
    }
  }

  DevelControl::OffScreenRenderingType newType = static_cast<DevelControl::OffScreenRenderingType>(offScreenRenderingType);
  if(newType == DevelControl::OffScreenRenderingType::NONE)
  {
    if(mOffScreenRenderingImpl)
    {
      mOffScreenRenderingImpl->ClearOwnerControl();
      mOffScreenRenderingImpl.reset();
    }
  }
  else if(mOffScreenRenderingType == DevelControl::OffScreenRenderingType::NONE)
  {
    mOffScreenRenderingImpl = std::make_unique<OffScreenRenderingImpl>(newType);

    Dali::Toolkit::Control handle(mControlImpl.GetOwner());
    mOffScreenRenderingImpl->SetOwnerControl(handle);
  }
  else
  {
    mOffScreenRenderingImpl->SetType(newType);
  }
  mOffScreenRenderingType = newType;
}

void Control::Impl::UpdateCornerRadius()
{
  Property::Map map;
  map[Toolkit::DevelVisual::Property::CORNER_RADIUS]        = DecorationData::GetCornerRadius(mDecorationData);
  map[Toolkit::DevelVisual::Property::CORNER_RADIUS_POLICY] = DecorationData::GetCornerRadiusPolicy(mDecorationData);
  map[Toolkit::DevelVisual::Property::CORNER_SQUARENESS]    = DecorationData::GetCornerSquareness(mDecorationData);

  RegisteredVisualContainer& visuals = mVisualData->mVisuals;
  for(auto it = visuals.begin(); it != visuals.end(); it++)
  {
    if((*it)->overrideCornerProperties)
    {
      (*it)->visual.DoAction(Toolkit::DevelVisual::Action::UPDATE_PROPERTY, map);
    }
  }

  if(mRenderEffect)
  {
    mRenderEffect->SetCornerConstants(map);
  }

  if(mOffScreenRenderingImpl)
  {
    mOffScreenRenderingImpl->SetCornerConstants(map);
  }
}

void Control::Impl::UpdateBorderline()
{
  Property::Map map;
  map[Toolkit::Visual::Property::TYPE]                   = Toolkit::Visual::Type::COLOR;
  map[Toolkit::ColorVisual::Property::MIX_COLOR]         = Color::TRANSPARENT;
  map[Toolkit::DevelVisual::Property::BORDERLINE_WIDTH]  = DecorationData::GetBorderlineWidth(mDecorationData);
  map[Toolkit::DevelVisual::Property::BORDERLINE_COLOR]  = DecorationData::GetBorderlineColor(mDecorationData);
  map[Toolkit::DevelVisual::Property::BORDERLINE_OFFSET] = DecorationData::GetBorderlineOffset(mDecorationData);

  SetBorderline(map, false);
}

void Control::Impl::Process(bool postProcessor)
{
  if(DALI_LIKELY(mVisualData))
  {
    // Call ApplyFittingMode
    mVisualData->ApplyFittingMode(mSize);
  }
  mProcessorRegistered = false;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
