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
#include "button-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/size-negotiation/relayout-container.h>
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/visuals/text/text-visual.h>
#include <dali-toolkit/public-api/align-enumerations.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/public-api/visuals/text-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogButtonFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_BUTTON_CONTROL");
#endif

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
BaseHandle Create()
{
  // empty handle as we cannot create button (but type registered for clicked signal)
  return BaseHandle();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Toolkit::Button, Toolkit::Control, Create)

DALI_PROPERTY_REGISTRATION(Toolkit, Button, "disabled", BOOLEAN, DISABLED)
DALI_PROPERTY_REGISTRATION(Toolkit, Button, "autoRepeating", BOOLEAN, AUTO_REPEATING)
DALI_PROPERTY_REGISTRATION(Toolkit, Button, "initialAutoRepeatingDelay", FLOAT, INITIAL_AUTO_REPEATING_DELAY)
DALI_PROPERTY_REGISTRATION(Toolkit, Button, "nextAutoRepeatingDelay", FLOAT, NEXT_AUTO_REPEATING_DELAY)
DALI_PROPERTY_REGISTRATION(Toolkit, Button, "togglable", BOOLEAN, TOGGLABLE)
DALI_PROPERTY_REGISTRATION(Toolkit, Button, "selected", BOOLEAN, SELECTED)
DALI_PROPERTY_REGISTRATION(Toolkit, Button, "unselectedVisual", MAP, UNSELECTED_VISUAL)
DALI_PROPERTY_REGISTRATION(Toolkit, Button, "selectedVisual", MAP, SELECTED_VISUAL)
DALI_PROPERTY_REGISTRATION(Toolkit, Button, "disabledSelectedVisual", MAP, DISABLED_SELECTED_VISUAL)
DALI_PROPERTY_REGISTRATION(Toolkit, Button, "disabledUnselectedVisual", MAP, DISABLED_UNSELECTED_VISUAL)
DALI_PROPERTY_REGISTRATION(Toolkit, Button, "unselectedBackgroundVisual", MAP, UNSELECTED_BACKGROUND_VISUAL)
DALI_PROPERTY_REGISTRATION(Toolkit, Button, "label", MAP, LABEL)
DALI_PROPERTY_REGISTRATION(Toolkit, Button, "selectedBackgroundVisual", MAP, SELECTED_BACKGROUND_VISUAL)
DALI_PROPERTY_REGISTRATION(Toolkit, Button, "disabledUnselectedBackgroundVisual", MAP, DISABLED_UNSELECTED_BACKGROUND_VISUAL)
DALI_PROPERTY_REGISTRATION(Toolkit, Button, "disabledSelectedBackgroundVisual", MAP, DISABLED_SELECTED_BACKGROUND_VISUAL)
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit, Button, "labelRelativeAlignment", STRING, LABEL_RELATIVE_ALIGNMENT)
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit, Button, "labelPadding", VECTOR4, LABEL_PADDING)
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit, Button, "visualPadding", VECTOR4, VISUAL_PADDING)

// Signals:
DALI_SIGNAL_REGISTRATION(Toolkit, Button, "pressed", SIGNAL_PRESSED)
DALI_SIGNAL_REGISTRATION(Toolkit, Button, "released", SIGNAL_RELEASED)
DALI_SIGNAL_REGISTRATION(Toolkit, Button, "clicked", SIGNAL_CLICKED)
DALI_SIGNAL_REGISTRATION(Toolkit, Button, "stateChanged", SIGNAL_STATE_CHANGED)

// Actions:
DALI_ACTION_REGISTRATION(Toolkit, Button, "buttonClick", ACTION_BUTTON_CLICK)

DALI_TYPE_REGISTRATION_END()

DALI_ENUM_TO_STRING_TABLE_BEGIN(ALIGNMENT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::Button, BEGIN)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::Button, END)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::Button, TOP)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Internal::Button, BOTTOM)
DALI_ENUM_TO_STRING_TABLE_END(ALIGNMENT)

const Scripting::StringEnum ALIGNMENT_STRING_TABLE[] =
  {
    {"BEGIN", Button::BEGIN},
    {"END", Button::END},
    {"TOP", Button::TOP},
    {"BOTTOM", Button::BOTTOM},
};

const unsigned int ALIGNMENT_STRING_TABLE_COUNT = sizeof(ALIGNMENT_STRING_TABLE) / sizeof(ALIGNMENT_STRING_TABLE[0]);

const Property::Index VISUAL_INDEX_FOR_STATE[Button::STATE_COUNT][Button::VISUAL_STATE_COUNT] =
  {
    {Toolkit::Button::Property::UNSELECTED_BACKGROUND_VISUAL, Toolkit::Button::Property::UNSELECTED_VISUAL},
    {Toolkit::Button::Property::SELECTED_BACKGROUND_VISUAL, Toolkit::Button::Property::SELECTED_VISUAL},
    {Toolkit::Button::Property::DISABLED_UNSELECTED_BACKGROUND_VISUAL, Toolkit::Button::Property::DISABLED_UNSELECTED_VISUAL},
    {Toolkit::Button::Property::DISABLED_SELECTED_BACKGROUND_VISUAL, Toolkit::Button::Property::DISABLED_SELECTED_VISUAL}};

/**
 * Checks if given map contains a text string
 */
bool MapContainsTextString(Property::Map& map)
{
  bool             result = false;
  Property::Value* value  = map.Find(Toolkit::TextVisual::Property::TEXT);
  if(value)
  {
    std::string textString;
    value->Get(textString);
    if(!textString.empty())
    {
      result = true;
    }
  }
  return result;
}

} // unnamed namespace

Button::Button()
: Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT)),
  mAutoRepeatingTimer(),
  mTextLabelAlignment(END),
  mAutoRepeating(false),
  mTogglableButton(false),
  mTextStringSetFlag(false),
  mInitialAutoRepeatingDelay(0.0f),
  mNextAutoRepeatingDelay(0.0f),
  mAnimationTime(0.0f),
  mButtonPressedState(UNPRESSED),
  mButtonState(UNSELECTED_STATE),
  mPreviousButtonState(mButtonState),
  mClickActionPerforming(false)
{
}

Button::~Button()
{
}

void Button::SetAutoRepeating(bool autoRepeating)
{
  mAutoRepeating = autoRepeating;

  // An autorepeating button can't be a toggle button.
  if(autoRepeating)
  {
    if(IsSelected())
    {
      SetSelected(false); // UnSelect before switching off Toggle feature.
    }
    mTogglableButton = false;
  }
}

void Button::SetInitialAutoRepeatingDelay(float initialAutoRepeatingDelay)
{
  DALI_ASSERT_DEBUG(initialAutoRepeatingDelay > 0.f);
  mInitialAutoRepeatingDelay = initialAutoRepeatingDelay;
}

void Button::SetNextAutoRepeatingDelay(float nextAutoRepeatingDelay)
{
  DALI_ASSERT_DEBUG(nextAutoRepeatingDelay > 0.f);
  mNextAutoRepeatingDelay = nextAutoRepeatingDelay;
}

void Button::SetTogglableButton(bool togglable)
{
  mTogglableButton = togglable;

  // A toggle button can't be an autorepeating button.
  if(togglable)
  {
    mAutoRepeating = false;
  }
}

void Button::SetSelected(bool selected)
{
  if(mTogglableButton)
  {
    DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "Button::SetSelected (%s)\n", (selected ? "true" : "false"));

    if(selected && (mButtonState != SELECTED_STATE))
    {
      ChangeState(SELECTED_STATE);
    }
    else if(!selected && (mButtonState != UNSELECTED_STATE))
    {
      ChangeState(UNSELECTED_STATE);
    }
  }
}

void Button::SetDisabled(bool disabled)
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "Button::SetDisabled(%s) state(%d)\n", (disabled) ? "disabled" : "active", mButtonState);

  if(disabled)
  {
    if(mButtonState == SELECTED_STATE)
    {
      ChangeState(DISABLED_SELECTED_STATE);
    }
    else if(mButtonState == UNSELECTED_STATE)
    {
      ChangeState(DISABLED_UNSELECTED_STATE);
    }
  }
  else
  {
    if(mButtonState == DISABLED_SELECTED_STATE)
    {
      ChangeState(SELECTED_STATE);
    }
    else if(mButtonState == DISABLED_UNSELECTED_STATE)
    {
      ChangeState(UNSELECTED_STATE);
    }
  }
}

bool Button::IsDisabled() const
{
  return (mButtonState == DISABLED_SELECTED_STATE || mButtonState == DISABLED_UNSELECTED_STATE);
}

bool Button::ValidateState(State requestedState)
{
  /*  Below tables shows allowed state transitions
   *  Match rows in first column to following columns, if true then transition allowed.
   *  eg UNSELECTED_STATE to DISABLED_UNSELECTED_STATE is true so state transition allowed.
   *
                                                             to| UNSELECTED_STATE | SELECTED_STATE | DISABLED_UNSELECTED_STATE | DISABLED_SELECTED_STATE |*/
  /* from*/
  bool transitionTable[4][4] = {/* UNSELECTED_STATE*/ {false, true, true, false},
                                /* SELECTED_STATE*/ {true, false, false, true},
                                /* DISABLED_UNSELECTED_STATE*/ {true, true, false, false},
                                /* DISABLED_SELECTED_STATE*/ {false, true, false, false}};

  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "Button::ValidateState ReuestedState:%d, CurrentState:%d, result:%s\n", requestedState, mButtonState, (transitionTable[mButtonState][requestedState]) ? "change-accepted" : "change-denied");

  return transitionTable[mButtonState][requestedState];
}

void Button::ChangeState(State requestedState)
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "Button::ChangeState ReuestedState(%d)\n", requestedState);

  // Validate State before changing
  if(!ValidateState(requestedState))
  {
    DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "Button::ChangeState ReuestedState(%d) not validated\n", requestedState);
    return;
  }

  // If not on stage the button could have still been set to selected so update state
  mPreviousButtonState = mButtonState;   // Store previous state for visual removal (used when animations ended)
  mButtonState         = requestedState; // Update current state

  if(Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    OnStateChange(mButtonState); // Notify derived buttons
    SelectRequiredVisual(VISUAL_INDEX_FOR_STATE[mButtonState][BACKGROUND]);
    SelectRequiredVisual(VISUAL_INDEX_FOR_STATE[mButtonState][FOREGROUND]);
    // If animation supported then visual removal should be performed after any transition animation has completed.
    // If Required Visual is not loaded before current visual is removed then a flickering will be evident.
    // Derived button can override OnButtonVisualRemoval
    OnButtonVisualRemoval(VISUAL_INDEX_FOR_STATE[mPreviousButtonState][BACKGROUND]);
    OnButtonVisualRemoval(VISUAL_INDEX_FOR_STATE[mPreviousButtonState][FOREGROUND]);
    RelayoutRequest();
  }

  Toolkit::Button handle(GetOwner());
  // Emit signal.
  mStateChangedSignal.Emit(handle);
}

bool Button::IsSelected() const
{
  bool selected = (mButtonState == SELECTED_STATE) || (mButtonState == DISABLED_SELECTED_STATE);
  return mTogglableButton && selected;
}

void Button::MergeWithExistingLabelProperties(const Property::Map& inMap, Property::Map& outMap)
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "MergeLabelProperties with %d properties\n", inMap.Count());

  /**
   * Properties for the Label visual could be from a style sheet but after being set the "TEXT" property could be set.
   * Hence would need to create the Text Visual with the complete merged set of properties.
   *
   * 1) Find Label Visual
   * 2) Retrieve current properties ( settings )
   * 3) Merge with new properties ( settings )
   * 4) Return new merged map
   */
  Toolkit::Visual::Base visual = DevelControl::GetVisual(*this, Toolkit::Button::Property::LABEL);
  if(visual)
  {
    DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "MergeLabelProperties Visual already exists, retrieving existing map\n");
    visual.CreatePropertyMap(outMap);
    DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "MergeLabelProperties retrieved %d properties\n", outMap.Count());
  }

  outMap.Merge(inMap);

  // Store if a text string has been supplied.

  mTextStringSetFlag = MapContainsTextString(outMap);

  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "MergeLabelProperties now has %d properties\n", outMap.Count());
}

void Button::SetLabelAlignment(Button::Align labelAlignment)
{
  mTextLabelAlignment = labelAlignment;
  RelayoutRequest();
}

Button::Align Button::GetLabelAlignment()
{
  return mTextLabelAlignment;
}

/**
 * Create Visual for given index from a property map or url.
 * 1) Check if value passed in is a url and create visual
 * 2) Create visual from map if step (1) is false
 * 3) Register visual with control with false for enable flag. Button will later enable visual when needed ( Button::SelectRequiredVisual )
 * 4) Unregister visual if empty map was provided. This is the method to remove a visual
 */
void Button::CreateVisualsForComponent(Property::Index index, const Property::Value& value, const int visualDepth)
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "CreateVisualsForComponent index(%d)\n", index);
  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base  buttonVisual;

  std::string imageUrl;
  if(value.Get(imageUrl))
  {
    DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "CreateVisualsForComponent Using image URL(%d)\n", index);
    if(!imageUrl.empty())
    {
      DALI_ASSERT_DEBUG(index != Toolkit::Button::Property::LABEL && "Creating a Image Visual instead of Text Visual ");
      buttonVisual = visualFactory.CreateVisual(imageUrl, ImageDimensions());
    }
  }
  else
  {
    // if its not a string then get a Property::Map from the property if possible.
    const Property::Map* map = value.GetMap();
    if(map && !map->Empty()) // Empty map results in current visual removal.
    {
      DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "CreateVisualsForComponent Using Map(%d)\n", index);
      buttonVisual = visualFactory.CreateVisual(*map);
    }
  }

  if(buttonVisual)
  {
    DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "CreateVisualsForComponent RegisterVisual index(%d) enabled(%s)\n", index, DevelControl::IsVisualEnabled(*this, index) ? "true" : "false");
    // enable the visual if needed for current state
    const bool enabled = ((index == VISUAL_INDEX_FOR_STATE[mButtonState][BACKGROUND]) ||
                          (index == VISUAL_INDEX_FOR_STATE[mButtonState][FOREGROUND]) ||
                          (index == Toolkit::Button::Property::LABEL));
    DevelControl::RegisterVisual(*this, index, buttonVisual, enabled, visualDepth);
  }
  else
  {
    DevelControl::UnregisterVisual(*this, index);
    DALI_LOG_INFO(gLogButtonFilter, Debug::General, "CreateVisualsForComponent Visual not created or empty map (clearing visual).(%d)\n", index);
  }
  RelayoutRequest();
}

bool Button::GetPropertyMapForVisual(Property::Index visualIndex, Property::Map& retreivedMap) const
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::General, "GetPropertyMapForVisual visual(%d)\n", visualIndex);
  bool                  success = false;
  Toolkit::Visual::Base visual  = DevelControl::GetVisual(*this, visualIndex);
  if(visual)
  {
    visual.CreatePropertyMap(retreivedMap);
    success = true;
  }
  DALI_LOG_INFO(gLogButtonFilter, Debug::General, "GetPropertyMapForVisual %s\n", success ? "Success" : "Failure");
  return success;
}

bool Button::DoAction(BaseObject* object, const std::string& actionName, const Property::Map& attributes)
{
  bool ret = false;

  Dali::BaseHandle handle(object);

  Toolkit::Button button = Toolkit::Button::DownCast(handle);

  DALI_ASSERT_DEBUG(button);

  if(0 == strcmp(actionName.c_str(), ACTION_BUTTON_CLICK))
  {
    ret = GetImplementation(button).DoClickAction(attributes);
  }

  return ret;
}

bool Button::DoClickAction(const Property::Map& attributes)
{
  // Prevents the button signals from doing a recursive loop by sending an action
  // and re-emitting the signals.
  if(!mClickActionPerforming)
  {
    mClickActionPerforming = true;
    ButtonDown();
    if(!mTogglableButton)
    {
      mButtonPressedState = DEPRESSED;
    }
    ButtonUp();
    mClickActionPerforming = false;

    return true;
  }

  return false;
}

void Button::ButtonDown()
{
  if(mTogglableButton)
  {
    if(mButtonState != SELECTED_STATE)
    {
      SetSelected(true);
      mButtonPressedState = TOGGLE_DEPRESSED;
    }
    else
    {
      mButtonPressedState = DEPRESSED;
    }
  }
  else
  {
    Pressed();
    mButtonPressedState = DEPRESSED;
    if(mAutoRepeating)
    {
      SetUpTimer(mInitialAutoRepeatingDelay);
    }
  }

  // The pressed signal should be emitted regardless of toggle mode.
  Toolkit::Button handle(GetOwner());
  mPressedSignal.Emit(handle);
}

void Button::ButtonUp()
{
  bool emitSignalsForPressAndReleaseAction = false;

  if(DEPRESSED == mButtonPressedState)
  {
    if(mTogglableButton) // Button up will change state
    {
      emitSignalsForPressAndReleaseAction = OnToggleReleased(); // Derived toggle buttons can override this to provide custom behaviour
    }
    else
    {
      Released(); // Button up will result in unselected state
      if(mAutoRepeating)
      {
        mAutoRepeatingTimer.Reset();
      }
      emitSignalsForPressAndReleaseAction = true;
    }
  }
  else if(TOGGLE_DEPRESSED == mButtonPressedState)
  {
    emitSignalsForPressAndReleaseAction = true; // toggle released after being pressed, a click
  }

  if(emitSignalsForPressAndReleaseAction)
  {
    // The clicked and released signals should be emitted regardless of toggle mode.
    Toolkit::Button handle(GetOwner());
    mReleasedSignal.Emit(handle);
    mClickedSignal.Emit(handle);
  }
}

bool Button::OnToggleReleased()
{
  SetSelected(!IsSelected());
  mButtonPressedState = UNPRESSED;
  return true;
}

void Button::OnTouchPointLeave()
{
  if(DEPRESSED == mButtonPressedState)
  {
    if(!mTogglableButton)
    {
      Released();

      if(mAutoRepeating)
      {
        mAutoRepeatingTimer.Reset();
      }
    }

    mButtonPressedState = UNPRESSED;

    // The released signal should be emitted regardless of toggle mode.
    Toolkit::Button handle(GetOwner());
    mReleasedSignal.Emit(handle);
  }
}

void Button::OnTouchPointInterrupted()
{
  OnTouchPointLeave();
}

Toolkit::Button::ButtonSignalType& Button::PressedSignal()
{
  return mPressedSignal;
}

Toolkit::Button::ButtonSignalType& Button::ReleasedSignal()
{
  return mReleasedSignal;
}

Toolkit::Button::ButtonSignalType& Button::ClickedSignal()
{
  return mClickedSignal;
}

Toolkit::Button::ButtonSignalType& Button::StateChangedSignal()
{
  return mStateChangedSignal;
}

bool Button::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  Dali::BaseHandle handle(object);

  bool            connected(true);
  Toolkit::Button button = Toolkit::Button::DownCast(handle);

  if(0 == strcmp(signalName.c_str(), SIGNAL_PRESSED))
  {
    button.PressedSignal().Connect(tracker, functor);
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_RELEASED))
  {
    button.ReleasedSignal().Connect(tracker, functor);
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_CLICKED))
  {
    button.ClickedSignal().Connect(tracker, functor);
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_STATE_CHANGED))
  {
    button.StateChangedSignal().Connect(tracker, functor);
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void Button::OnInitialize()
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "Button::OnInitialize\n");

  Actor self = Self();

  mTapDetector = TapGestureDetector::New();
  mTapDetector.Attach(self);
  mTapDetector.DetectedSignal().Connect(this, &Button::OnTap);

  self.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  self.SetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, true);

  self.TouchedSignal().Connect(this, &Button::OnTouch);
}

bool Button::OnAccessibilityActivated()
{
  return OnKeyboardEnter();
}

DevelControl::ControlAccessible* Button::CreateAccessibleObject()
{
  return new ButtonAccessible(Self());
}

bool Button::OnTouch(Actor actor, const TouchEvent& touch)
{
  if(!IsDisabled() && (actor == touch.GetHitActor(0)))
  {
    if(1 == touch.GetPointCount())
    {
      switch(touch.GetState(0))
      {
        case PointState::DOWN:
        {
          ButtonDown();
          break;
        }
        case PointState::UP:
        {
          ButtonUp();
          break;
        }
        case PointState::INTERRUPTED:
        {
          OnTouchPointInterrupted();
          break;
        }
        case PointState::LEAVE:
        {
          OnTouchPointLeave();
          break;
        }
        case PointState::MOTION:
        case PointState::STATIONARY: // FALLTHROUGH
        {
          // Nothing to do
          break;
        }
      }
    }
    else if(1 < touch.GetPointCount())
    {
      OnTouchPointLeave(); // Notification for derived classes.

      // Sets the button state to the default
      mButtonPressedState = UNPRESSED;
    }
  }
  return false;
}

bool Button::OnKeyboardEnter()
{
  // When the enter key is pressed, or button is activated, the click action is performed.
  Property::Map attributes;
  bool          ret = DoClickAction(attributes);

  return ret;
}

void Button::OnSceneDisconnection()
{
  if(DEPRESSED == mButtonPressedState)
  {
    if(!mTogglableButton)
    {
      Released();

      if(mAutoRepeating)
      {
        mAutoRepeatingTimer.Reset();
      }
    }
  }

  mButtonPressedState = UNPRESSED;

  Control::OnSceneDisconnection(); // Visuals will be set off stage
}

void Button::OnSceneConnection(int depth)
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "Button::OnSceneConnection ptr(%p) \n", this);
  OnButtonVisualRemoval(VISUAL_INDEX_FOR_STATE[mPreviousButtonState][BACKGROUND]);
  OnButtonVisualRemoval(VISUAL_INDEX_FOR_STATE[mPreviousButtonState][FOREGROUND]);
  SelectRequiredVisual(Toolkit::Button::Property::LABEL);
  SelectRequiredVisual(VISUAL_INDEX_FOR_STATE[mButtonState][BACKGROUND]);
  SelectRequiredVisual(VISUAL_INDEX_FOR_STATE[mButtonState][FOREGROUND]);
  Control::OnSceneConnection(depth); // Enabled visuals will be put on stage
  RelayoutRequest();
}

Vector3 Button::GetNaturalSize()
{
  Vector3 size = Vector3::ZERO;

  bool horizontalAlignment = mTextLabelAlignment == BEGIN || mTextLabelAlignment == END; // label and visual side by side

  // Get natural size of foreground ( largest of the possible visuals )
  Size largestProvidedVisual;
  Size labelSize = Size::ZERO;

  bool foreGroundVisualUsed = false;

  for(int state = Button::UNSELECTED_STATE; state < Button::STATE_COUNT; state++)
  {
    Toolkit::Visual::Base visual = DevelControl::GetVisual(*this, VISUAL_INDEX_FOR_STATE[state][FOREGROUND]);
    Size                  visualSize;
    if(visual)
    {
      visual.GetNaturalSize(visualSize);
      largestProvidedVisual.width  = std::max(largestProvidedVisual.width, visualSize.width);
      largestProvidedVisual.height = std::max(largestProvidedVisual.height, visualSize.height);
      foreGroundVisualUsed         = true;
    }
  }

  if(!foreGroundVisualUsed) // If foreground visual not supplied then use the background visual to calculate Natural size
  {
    for(int state = Button::UNSELECTED_STATE; state < Button::STATE_COUNT; state++)
    {
      Toolkit::Visual::Base visual = DevelControl::GetVisual(*this, VISUAL_INDEX_FOR_STATE[state][BACKGROUND]);
      Size                  visualSize;
      if(visual)
      {
        visual.GetNaturalSize(visualSize);
        largestProvidedVisual.width  = std::max(largestProvidedVisual.width, visualSize.width);
        largestProvidedVisual.height = std::max(largestProvidedVisual.height, visualSize.height);
      }
    }
  }

  // Get horizontal padding total
  if(largestProvidedVisual.width > 0) // if visual exists
  {
    size.width += largestProvidedVisual.width + mForegroundPadding.left + mForegroundPadding.right;
  }
  // Get vertical padding total
  if(largestProvidedVisual.height > 0)
  {
    size.height += largestProvidedVisual.height + mForegroundPadding.top + mForegroundPadding.bottom;
  }

  DALI_LOG_INFO(gLogButtonFilter, Debug::General, "GetNaturalSize visual Size(%f,%f)\n", largestProvidedVisual.width, largestProvidedVisual.height);

  // Get natural size of label if text has been set
  if(mTextStringSetFlag)
  {
    Toolkit::Visual::Base visual = DevelControl::GetVisual(*this, Toolkit::Button::Property::LABEL);

    if(visual)
    {
      visual.GetNaturalSize(labelSize);

      DALI_LOG_INFO(gLogButtonFilter, Debug::General, "GetNaturalSize labelSize(%f,%f) padding(%f,%f)\n", labelSize.width, labelSize.height, mLabelPadding.left + mLabelPadding.right, mLabelPadding.top + mLabelPadding.bottom);

      labelSize.width += mLabelPadding.left + mLabelPadding.right;
      labelSize.height += mLabelPadding.top + mLabelPadding.bottom;

      // Add label size to height or width depending on alignment position
      if(horizontalAlignment)
      {
        size.width += labelSize.width;
        size.height = std::max(size.height, labelSize.height);
      }
      else
      {
        size.height += labelSize.height;
        size.width = std::max(size.width, labelSize.width);
      }
    }
  }

  if(size.width < 1 && size.height < 1)
  {
    // if no image or label then use Control's natural size
    DALI_LOG_INFO(gLogButtonFilter, Debug::General, "GetNaturalSize Using control natural size\n");
    size = Control::GetNaturalSize();
  }

  DALI_LOG_INFO(gLogButtonFilter, Debug::General, "Button GetNaturalSize (%f,%f)\n", size.width, size.height);

  return size;
}

void Button::OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension)
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::General, "OnSetResizePolicy\n");
  RelayoutRequest();
}

/**
 * Visuals are sized and positioned in this function.
 * Whilst the control has it's size negotiated it has to size it's visuals explicitly here.
 */

void Button::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::General, "OnRelayout targetSize(%f,%f) ptr(%p) state[%d]\n", size.width, size.height, this, mButtonState);

  Toolkit::Visual::Base currentVisual           = DevelControl::GetVisual(*this, VISUAL_INDEX_FOR_STATE[mButtonState][FOREGROUND]);
  Toolkit::Visual::Base currentBackGroundVisual = DevelControl::GetVisual(*this, VISUAL_INDEX_FOR_STATE[mButtonState][BACKGROUND]);

  // Sizes and padding set to zero, if not present then values will no effect calculations.
  Vector2 visualPosition          = Vector2::ZERO;
  Vector2 labelPosition           = Vector2::ZERO;
  Size    visualSize              = Size::ZERO;
  Padding foregroundVisualPadding = Padding(0.0f, 0.0f, 0.0f, 0.0f);
  Padding labelVisualPadding      = Padding(0.0f, 0.0f, 0.0f, 0.0f);

  if(mTextStringSetFlag)
  {
    DALI_LOG_INFO(gLogButtonFilter, Debug::General, "OnRelayout Label padding setting padding:%f,%f,%f,%f\n", mLabelPadding.y, mLabelPadding.x, mLabelPadding.width, mLabelPadding.height);
    labelVisualPadding = mLabelPadding;
  }

  if(currentVisual)
  {
    DALI_LOG_INFO(gLogButtonFilter, Debug::General, "OnRelayout Foreground Visual setting padding:%f,%f,%f,%f\n", mForegroundPadding.y, mForegroundPadding.x, mForegroundPadding.width, mForegroundPadding.height);
    currentVisual.GetNaturalSize(visualSize);
    foregroundVisualPadding = mForegroundPadding;
  }

  Toolkit::Align::Type visualAnchorPoint = Toolkit::Align::TOP_BEGIN;

  Vector2 visualAndPaddingSize = Vector2((foregroundVisualPadding.x + visualSize.width + foregroundVisualPadding.y),
                                         (foregroundVisualPadding.width + visualSize.height + foregroundVisualPadding.height));

  DALI_LOG_INFO(gLogButtonFilter, Debug::General, "OnRelayout visualAndPaddingSize(%f,%f)\n", visualAndPaddingSize.width, visualAndPaddingSize.height);

  // Text Visual should take all space available after foreground visual size and all padding is considered.
  // Remaining Space priority, Foreground padding, foreground visual, Text padding then Text visual.
  Size remainingSpaceForText = Size::ZERO;

  switch(mTextLabelAlignment)
  {
    case BEGIN:
    {
      visualAnchorPoint = Toolkit::Align::TOP_END;
      visualPosition.x  = foregroundVisualPadding.right;
      visualPosition.y  = foregroundVisualPadding.top;

      labelPosition.x = labelVisualPadding.x;
      labelPosition.y = labelVisualPadding.top;

      remainingSpaceForText.width  = size.width - visualAndPaddingSize.width - labelVisualPadding.x - labelVisualPadding.y;
      remainingSpaceForText.height = size.height - labelVisualPadding.top - labelVisualPadding.bottom;
      break;
    }
    case END:
    {
      visualAnchorPoint = Toolkit::Align::TOP_BEGIN;
      visualPosition.x  = foregroundVisualPadding.left;
      visualPosition.y  = foregroundVisualPadding.top;

      labelPosition.x = visualAndPaddingSize.width + labelVisualPadding.x;
      labelPosition.y = labelVisualPadding.top;

      remainingSpaceForText.width  = size.width - visualAndPaddingSize.width - labelVisualPadding.x - labelVisualPadding.y;
      remainingSpaceForText.height = size.height - labelVisualPadding.top - labelVisualPadding.bottom;
      break;
    }
    case TOP:
    {
      visualAnchorPoint = Toolkit::Align::BOTTOM_END;
      visualPosition.x  = foregroundVisualPadding.left;
      visualPosition.y  = foregroundVisualPadding.bottom;

      labelPosition.x = labelVisualPadding.left;
      labelPosition.y = labelVisualPadding.top;

      remainingSpaceForText.width  = size.width - labelVisualPadding.x - labelVisualPadding.y;
      remainingSpaceForText.height = size.height - visualAndPaddingSize.height - labelVisualPadding.top - labelVisualPadding.bottom;

      break;
    }
    case BOTTOM:
    {
      visualAnchorPoint = Toolkit::Align::TOP_END;
      visualPosition.x  = foregroundVisualPadding.left;
      visualPosition.y  = foregroundVisualPadding.top;

      labelPosition.x = labelVisualPadding.left;
      labelPosition.y = visualAndPaddingSize.height + labelVisualPadding.top;

      remainingSpaceForText.width  = size.width - labelVisualPadding.x - labelVisualPadding.y;
      remainingSpaceForText.height = size.height - visualAndPaddingSize.height - labelVisualPadding.top - labelVisualPadding.bottom;

      break;
    }
  }

  if(currentBackGroundVisual)
  {
    DALI_LOG_INFO(gLogButtonFilter, Debug::General, "OnRelayout Setting visual background size to(%f,%f)\n", size.width, size.height);

    Property::Map visualTransform;

    visualTransform.Add(Toolkit::Visual::Transform::Property::SIZE, size)
      .Add(Toolkit::Visual::Transform::Property::SIZE_POLICY, Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE));

    currentBackGroundVisual.SetTransformAndSize(visualTransform, size);
  }

  if(currentVisual)
  {
    DALI_LOG_INFO(gLogButtonFilter, Debug::General, "OnRelayout Setting visual size to(%f,%f)\n", visualSize.width, visualSize.height);

    Property::Map visualTransform;

    visualTransform.Add(Toolkit::Visual::Transform::Property::SIZE, visualSize)
      .Add(Toolkit::Visual::Transform::Property::OFFSET, visualPosition)
      .Add(Toolkit::Visual::Transform::Property::OFFSET_POLICY, Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE))
      .Add(Toolkit::Visual::Transform::Property::SIZE_POLICY, Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE))
      .Add(Toolkit::Visual::Transform::Property::ORIGIN, Toolkit::Align::TOP_BEGIN)
      .Add(Toolkit::Visual::Transform::Property::ANCHOR_POINT, visualAnchorPoint);

    currentVisual.SetTransformAndSize(visualTransform, size);
  }

  if(mTextStringSetFlag)
  {
    Toolkit::Visual::Base textVisual = DevelControl::GetVisual(*this, Toolkit::Button::Property::LABEL); // No need to search for Label visual if no text set.

    if(textVisual)
    {
      if(!currentVisual)
      {
        DALI_LOG_INFO(gLogButtonFilter, Debug::General, "OnRelayout Only Text\n");
        labelPosition.x = labelVisualPadding.left;
        labelPosition.y = labelVisualPadding.height;
      }

      Vector2 preSize = Vector2(static_cast<int>(remainingSpaceForText.x), static_cast<int>(remainingSpaceForText.y));

      DALI_LOG_INFO(gLogButtonFilter, Debug::General, "OnRelayout text Size(%f,%f) text Position(%f,%f) \n", remainingSpaceForText.width, remainingSpaceForText.height, labelPosition.x, labelPosition.y);

      DALI_LOG_INFO(gLogButtonFilter, Debug::General, "OnRelayout text Size -- (%f,%f) text Position(%f,%f) \n", preSize.width, preSize.height, labelPosition.x, labelPosition.y);

      Property::Map textVisualTransform;
      textVisualTransform.Add(Toolkit::Visual::Transform::Property::SIZE, preSize)
        .Add(Toolkit::Visual::Transform::Property::OFFSET, labelPosition)
        .Add(Toolkit::Visual::Transform::Property::OFFSET_POLICY, Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE))
        .Add(Toolkit::Visual::Transform::Property::SIZE_POLICY, Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE))
        .Add(Toolkit::Visual::Transform::Property::ORIGIN, Toolkit::Align::TOP_BEGIN)
        .Add(Toolkit::Visual::Transform::Property::ANCHOR_POINT, visualAnchorPoint);

      textVisual.SetTransformAndSize(textVisualTransform, size);
    }
  }

  DALI_LOG_INFO(gLogButtonFilter, Debug::General, "OnRelayout selected (%s) \n", IsSelected() ? "yes" : "no");

  DALI_LOG_INFO(gLogButtonFilter, Debug::General, "OnRelayout << \n");
}

void Button::OnTap(Actor actor, const TapGesture& tap)
{
  // Prevents Parent getting a tap event
}

void Button::SetUpTimer(float delay)
{
  mAutoRepeatingTimer = Dali::Timer::New(static_cast<unsigned int>(1000.f * delay));
  mAutoRepeatingTimer.TickSignal().Connect(this, &Button::AutoRepeatingSlot);
  mAutoRepeatingTimer.Start();
}

bool Button::AutoRepeatingSlot()
{
  bool consumed = false;
  if(!IsDisabled())
  {
    // Restart the autorepeat timer.
    SetUpTimer(mNextAutoRepeatingDelay);

    Pressed();

    Toolkit::Button handle(GetOwner());

    //Emit signal.
    consumed = mReleasedSignal.Emit(handle);
    consumed = mClickedSignal.Emit(handle);
    consumed |= mPressedSignal.Emit(handle);
  }

  return consumed;
}

void Button::Pressed()
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "Button::Pressed\n");

  if(mButtonState == UNSELECTED_STATE)
  {
    ChangeState(SELECTED_STATE);
    OnPressed(); // Notifies the derived class the button has been pressed.
  }
}

void Button::Released()
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "Button::Released\n");

  if(mButtonState == SELECTED_STATE && !mTogglableButton)
  {
    ChangeState(UNSELECTED_STATE);
    OnReleased(); //    // Notifies the derived class the button has been released.
  }
  mButtonPressedState = UNPRESSED;
}

void Button::SelectRequiredVisual(Property::Index visualIndex)
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "Button::SelectRequiredVisual index(%d) state(%d)\n", visualIndex, mButtonState);
  // only enable visuals that exist
  if(DevelControl::GetVisual(*this, visualIndex))
  {
    DevelControl::EnableVisual(*this, visualIndex, true);
  }
}

void Button::RemoveVisual(Property::Index visualIndex)
{
  // Use OnButtonVisualRemoval if want button developer to have the option to override removal.
  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "Button::RemoveVisual index(%d) state(%d)\n", visualIndex, mButtonState);

  Toolkit::Visual::Base visual = DevelControl::GetVisual(*this, visualIndex);

  if(visual)
  {
    DevelControl::EnableVisual(*this, visualIndex, false);
  }
}

void Button::OnButtonVisualRemoval(Property::Index visualIndex)
{
  // Derived Buttons can over ride this to prevent default removal.
  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "Button::OnButtonVisualRemoval index(%d)\n", visualIndex);
  RemoveVisual(visualIndex);
}

void Button::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  Toolkit::Button button = Toolkit::Button::DownCast(Dali::BaseHandle(object));

  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "Button::SetProperty index[%d]\n", index);

  if(button)
  {
    switch(index)
    {
      case Toolkit::Button::Property::DISABLED:
      {
        GetImplementation(button).SetDisabled(value.Get<bool>());
        break;
      }

      case Toolkit::Button::Property::AUTO_REPEATING:
      {
        GetImplementation(button).SetAutoRepeating(value.Get<bool>());
        break;
      }

      case Toolkit::Button::Property::INITIAL_AUTO_REPEATING_DELAY:
      {
        GetImplementation(button).SetInitialAutoRepeatingDelay(value.Get<float>());
        break;
      }

      case Toolkit::Button::Property::NEXT_AUTO_REPEATING_DELAY:
      {
        GetImplementation(button).SetNextAutoRepeatingDelay(value.Get<float>());
        break;
      }

      case Toolkit::Button::Property::TOGGLABLE:
      {
        GetImplementation(button).SetTogglableButton(value.Get<bool>());
        break;
      }

      case Toolkit::Button::Property::SELECTED:
      {
        GetImplementation(button).SetSelected(value.Get<bool>());
        break;
      }

      case Toolkit::Button::Property::UNSELECTED_VISUAL:
      case Toolkit::Button::Property::SELECTED_VISUAL:
      case Toolkit::Button::Property::DISABLED_SELECTED_VISUAL:
      case Toolkit::Button::Property::DISABLED_UNSELECTED_VISUAL:
      {
        GetImplementation(button).CreateVisualsForComponent(index, value, DepthIndex::CONTENT);
        break;
      }

      case Toolkit::Button::Property::UNSELECTED_BACKGROUND_VISUAL:
      case Toolkit::Button::Property::SELECTED_BACKGROUND_VISUAL:
      case Toolkit::Button::Property::DISABLED_SELECTED_BACKGROUND_VISUAL:
      case Toolkit::Button::Property::DISABLED_UNSELECTED_BACKGROUND_VISUAL:
      {
        GetImplementation(button).CreateVisualsForComponent(index, value, DepthIndex::BACKGROUND);
        break;
      }

      case Toolkit::Button::Property::LABEL:
      {
        Property::Map outTextVisualProperties;
        std::string   textString;

        if(value.Get(textString))
        {
          DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "Button::SetProperty Setting TextVisual with string[%s]\n", textString.c_str());

          Property::Map setPropertyMap;
          setPropertyMap.Add(Toolkit::Visual::Property::TYPE, Toolkit::Visual::TEXT)
            .Add(Toolkit::TextVisual::Property::TEXT, textString);

          GetImplementation(button).MergeWithExistingLabelProperties(setPropertyMap, outTextVisualProperties);
        }
        else
        {
          // Get a Property::Map from the property if possible.
          const Property::Map* setPropertyMap = value.GetMap();
          if(setPropertyMap)
          {
            Property::Map indexKeys = TextVisual::ConvertStringKeysToIndexKeys(*setPropertyMap);
            GetImplementation(button).MergeWithExistingLabelProperties(indexKeys, outTextVisualProperties);
          }
        }

        if(!outTextVisualProperties.Empty())
        {
          GetImplementation(button).CreateVisualsForComponent(index, outTextVisualProperties, DepthIndex::CONTENT);
        }
        break;
      }

      case Toolkit::DevelButton::Property::LABEL_RELATIVE_ALIGNMENT:
      {
        Button::Align labelAlignment(END);
        Scripting::GetEnumeration<Button::Align>(value.Get<std::string>().c_str(),
                                                 ALIGNMENT_TABLE,
                                                 ALIGNMENT_TABLE_COUNT,
                                                 labelAlignment);

        GetImplementation(button).SetLabelAlignment(labelAlignment);
        break;
      }

      case Toolkit::DevelButton::Property::LABEL_PADDING:
      {
        Vector4 padding(value.Get<Vector4>());
        GetImplementation(button).SetLabelPadding(Padding(padding.x, padding.y, padding.z, padding.w));
        break;
      }

      case Toolkit::DevelButton::Property::VISUAL_PADDING:
      {
        Vector4 padding(value.Get<Vector4>());
        GetImplementation(button).SetForegroundPadding(Padding(padding.x, padding.y, padding.z, padding.w));
        break;
      }
    }
  }
}

Property::Value Button::GetProperty(BaseObject* object, Property::Index propertyIndex)
{
  Property::Value value;

  Toolkit::Button button = Toolkit::Button::DownCast(Dali::BaseHandle(object));

  if(button)
  {
    switch(propertyIndex)
    {
      case Toolkit::Button::Property::DISABLED:
      {
        value = GetImplementation(button).IsDisabled();
        break;
      }

      case Toolkit::Button::Property::AUTO_REPEATING:
      {
        value = GetImplementation(button).mAutoRepeating;
        break;
      }

      case Toolkit::Button::Property::INITIAL_AUTO_REPEATING_DELAY:
      {
        value = GetImplementation(button).mInitialAutoRepeatingDelay;
        break;
      }

      case Toolkit::Button::Property::NEXT_AUTO_REPEATING_DELAY:
      {
        value = GetImplementation(button).mNextAutoRepeatingDelay;
        break;
      }

      case Toolkit::Button::Property::TOGGLABLE:
      {
        value = GetImplementation(button).mTogglableButton;
        break;
      }

      case Toolkit::Button::Property::SELECTED:
      {
        value = GetImplementation(button).IsSelected();
        break;
      }

      case Toolkit::Button::Property::UNSELECTED_VISUAL:
      case Toolkit::Button::Property::SELECTED_VISUAL:
      case Toolkit::Button::Property::DISABLED_SELECTED_VISUAL:
      case Toolkit::Button::Property::DISABLED_UNSELECTED_VISUAL:
      case Toolkit::Button::Property::UNSELECTED_BACKGROUND_VISUAL:
      case Toolkit::Button::Property::SELECTED_BACKGROUND_VISUAL:
      case Toolkit::Button::Property::DISABLED_SELECTED_BACKGROUND_VISUAL:
      case Toolkit::Button::Property::DISABLED_UNSELECTED_BACKGROUND_VISUAL:
      case Toolkit::Button::Property::LABEL:
      {
        Property::Map visualProperty;
        if(GetImplementation(button).GetPropertyMapForVisual(propertyIndex, visualProperty))
        {
          value = visualProperty;
        }
        break;
      }

      case Toolkit::DevelButton::Property::LABEL_RELATIVE_ALIGNMENT:
      {
        const char* alignment = Scripting::GetEnumerationName<Button::Align>(GetImplementation(button).GetLabelAlignment(),
                                                                             ALIGNMENT_STRING_TABLE,
                                                                             ALIGNMENT_STRING_TABLE_COUNT);
        if(alignment)
        {
          value = std::string(alignment);
        }

        break;
      }

      case Toolkit::DevelButton::Property::LABEL_PADDING:
      {
        Padding padding = GetImplementation(button).GetLabelPadding();
        value           = Vector4(padding.x, padding.y, padding.top, padding.bottom);
        break;
      }

      case Toolkit::DevelButton::Property::VISUAL_PADDING:
      {
        Padding padding = GetImplementation(button).GetForegroundPadding();
        value           = Vector4(padding.x, padding.y, padding.top, padding.bottom);
      }
    }
  }

  return value;
}

void Button::SetLabelPadding(const Padding& padding)
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "Button::SetLabelPadding padding(%f,%f,%f,%f)\n", padding.left, padding.right, padding.bottom, padding.top);
  mLabelPadding = Padding(padding.left, padding.right, padding.bottom, padding.top);
  RelayoutRequest();
}

Padding Button::GetLabelPadding()
{
  return mLabelPadding;
}

void Button::SetForegroundPadding(const Padding& padding)
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "Button::SetForegroundPadding padding(%f,%f,%f,%f)\n", padding.left, padding.right, padding.bottom, padding.top);
  mForegroundPadding = Padding(padding.left, padding.right, padding.bottom, padding.top);
  RelayoutRequest();
}

Padding Button::GetForegroundPadding()
{
  return mForegroundPadding;
}

std::string Button::ButtonAccessible::GetNameRaw() const
{
  std::string   labelText;
  auto          slf      = Toolkit::Button::DownCast(Self());
  Property::Map labelMap = slf.GetProperty<Property::Map>(Toolkit::Button::Property::LABEL);

  Property::Value* textPropertyPtr = labelMap.Find(Toolkit::TextVisual::Property::TEXT);
  if(textPropertyPtr)
  {
    textPropertyPtr->Get(labelText);
  }

  return labelText;
}

Property::Index Button::ButtonAccessible::GetNamePropertyIndex()
{
  Property::Index label    = Toolkit::Button::Property::LABEL;
  Property::Map   labelMap = Self().GetProperty<Property::Map>(label);

  if(MapContainsTextString(labelMap))
  {
    return label;
  }
  else
  {
    return Property::INVALID_INDEX;
  }
}

Dali::Accessibility::States Button::ButtonAccessible::CalculateStates()
{
  auto tmp                                    = DevelControl::ControlAccessible::CalculateStates();
  tmp[Dali::Accessibility::State::SELECTABLE] = true;
  auto slf                                    = Toolkit::Button::DownCast(Self());
  tmp[Dali::Accessibility::State::ENABLED]    = !slf.GetProperty<bool>(Toolkit::Button::Property::DISABLED);
  tmp[Dali::Accessibility::State::CHECKED]    = slf.GetProperty<bool>(Toolkit::Button::Property::SELECTED);
  return tmp;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
