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
#include <dali-toolkit/public-api/controls/control-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/size-negotiation/relayout-container.h>
#include <cstring> // for strcmp
#include <limits>
#include <stack>

// INTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>
#include <dali-toolkit/devel-api/visuals/color-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-actions-devel.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/controls/control/control-visual-data.h>
#include <dali-toolkit/internal/controls/render-effects/render-effect-impl.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>
#include <dali-toolkit/internal/visuals/color/color-visual.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/public-api/align-enumerations.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>
#include <dali-toolkit/public-api/styling/style-manager.h>
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_CONTROL_VISUALS");
#endif

/**
 * @brief Creates a clipping renderer if required.
 * (EG. If no renders exist and clipping is enabled).
 * @param[in] controlImpl The control implementation.
 */
void CreateClippingRenderer(Control& controlImpl)
{
  // We want to add a transparent background if we do not have one for clipping.
  Actor self(controlImpl.Self());
  int   clippingMode = ClippingMode::DISABLED;
  if(self.GetProperty(Actor::Property::CLIPPING_MODE).Get(clippingMode))
  {
    Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(controlImpl);

    if(clippingMode == ClippingMode::CLIP_CHILDREN && controlDataImpl.mVisualData->mVisuals.Empty() && self.GetRendererCount() == 0u)
    {
      controlImpl.SetBackgroundColor(Color::TRANSPARENT);
    }
  }
}

} // unnamed namespace

Toolkit::Control Control::New()
{
  return New(ControlBehaviour::CONTROL_BEHAVIOUR_DEFAULT);
}

Toolkit::Control Control::New(ControlBehaviour additionalBehaviour)
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<Control> controlImpl = new Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT | additionalBehaviour));

  // Pass ownership to handle
  Toolkit::Control handle(*controlImpl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  controlImpl->Initialize();

  return handle;
}

void Control::SetStyleName(const std::string& styleName)
{
  if(styleName != mImpl->mStyleName)
  {
    mImpl->mStyleName = styleName;

    // Apply new style, if stylemanager is available
    Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();
    if(styleManager)
    {
      GetImpl(styleManager).ApplyThemeStyle(Toolkit::Control(GetOwner()));
    }
  }
}

const std::string& Control::GetStyleName() const
{
  return mImpl->mStyleName;
}

void Control::SetBackgroundColor(const Vector4& color)
{
  mImpl->mBackgroundColor = color;

  Property::Map map;
  map[Toolkit::Visual::Property::TYPE]           = Toolkit::Visual::COLOR;
  map[Toolkit::ColorVisual::Property::MIX_COLOR] = color;

  Toolkit::Visual::Base visual = mImpl->GetVisual(Toolkit::Control::Property::BACKGROUND);
  if(visual && visual.GetType() == Toolkit::Visual::COLOR)
  {
    // Update background color only
    mImpl->DoAction(Toolkit::Control::Property::BACKGROUND, DevelVisual::Action::UPDATE_PROPERTY, map);
    return;
  }

  SetBackground(map);
}

void Control::SetBackground(const Property::Map& map)
{
  Toolkit::Visual::Base visual = Toolkit::VisualFactory::Get().CreateVisual(map);
  visual.SetName("background");
  if(visual)
  {
    mImpl->RegisterVisual(Toolkit::Control::Property::BACKGROUND, visual, DepthIndex::BACKGROUND);

    // Trigger a size negotiation request that may be needed by the new visual to relayout its contents.
    RelayoutRequest();
  }
}

void Control::ClearBackground()
{
  mImpl->UnregisterVisual(Toolkit::Control::Property::BACKGROUND);
  mImpl->mBackgroundColor = Color::TRANSPARENT;

  // Trigger a size negotiation request that may be needed when unregistering a visual.
  RelayoutRequest();
}

void Control::SetRenderEffect(Toolkit::RenderEffect effect)
{
  if(mImpl->mRenderEffect != effect)
  {
    ClearRenderEffect();
    mImpl->mRenderEffect = effect;

    if(effect)
    {
      Toolkit::Internal::RenderEffectImpl* object = dynamic_cast<Toolkit::Internal::RenderEffectImpl*>(mImpl->mRenderEffect.GetObjectPtr());
      DALI_ASSERT_ALWAYS(object && "Not a valid RenderEffect set.");

      SetOffScreenRenderableType(object->GetOffScreenRenderableType());

      Dali::Toolkit::Control ownerControl(GetOwner());
      object->SetOwnerControl(ownerControl);
    }
  }
}

void Control::ClearRenderEffect()
{
  if(mImpl->mRenderEffect)
  {
    Toolkit::Internal::RenderEffectImpl* object = dynamic_cast<Toolkit::Internal::RenderEffectImpl*>(mImpl->mRenderEffect.GetObjectPtr());

    if(object)
    {
      object->ClearOwnerControl();
    }
    mImpl->mRenderEffect.Reset();
  }
  SetOffScreenRenderableType(OffScreenRenderable::NONE);
}

void Control::SetResourceReady()
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(*this);
  controlDataImpl.ResourceReady();
}

Dali::Actor Control::GetOffScreenRenderableSourceActor()
{
  // Need to override this in FORWARD OffScreenRenderable
  return Dali::Actor();
}

bool Control::IsOffScreenRenderTaskExclusive()
{
  return false;
}

std::shared_ptr<Toolkit::DevelControl::ControlAccessible> Control::GetAccessibleObject()
{
  return mImpl->GetAccessibleObject();
}

void Control::EnableGestureDetection(GestureType::Value type)
{
  if((type & GestureType::PINCH) && !mImpl->mPinchGestureDetector)
  {
    mImpl->mPinchGestureDetector = PinchGestureDetector::New();
    mImpl->mPinchGestureDetector.DetectedSignal().Connect(mImpl, &Impl::PinchDetected);
    mImpl->mPinchGestureDetector.Attach(Self());
  }

  if((type & GestureType::PAN) && !mImpl->mPanGestureDetector)
  {
    mImpl->mPanGestureDetector = PanGestureDetector::New();
    mImpl->mPanGestureDetector.SetMaximumTouchesRequired(2);
    mImpl->mPanGestureDetector.DetectedSignal().Connect(mImpl, &Impl::PanDetected);
    mImpl->mPanGestureDetector.Attach(Self());
  }

  if((type & GestureType::TAP) && !mImpl->mTapGestureDetector)
  {
    mImpl->mTapGestureDetector = TapGestureDetector::New();
    mImpl->mTapGestureDetector.DetectedSignal().Connect(mImpl, &Impl::TapDetected);
    mImpl->mTapGestureDetector.Attach(Self());
  }

  if((type & GestureType::LONG_PRESS) && !mImpl->mLongPressGestureDetector)
  {
    mImpl->mLongPressGestureDetector = LongPressGestureDetector::New();
    mImpl->mLongPressGestureDetector.DetectedSignal().Connect(mImpl, &Impl::LongPressDetected);
    mImpl->mLongPressGestureDetector.Attach(Self());
  }
}

void Control::DisableGestureDetection(GestureType::Value type)
{
  if((type & GestureType::PINCH) && mImpl->mPinchGestureDetector)
  {
    mImpl->mPinchGestureDetector.Detach(Self());
    mImpl->mPinchGestureDetector.Reset();
  }

  if((type & GestureType::PAN) && mImpl->mPanGestureDetector)
  {
    mImpl->mPanGestureDetector.Detach(Self());
    mImpl->mPanGestureDetector.Reset();
  }

  if((type & GestureType::TAP) && mImpl->mTapGestureDetector)
  {
    mImpl->mTapGestureDetector.Detach(Self());
    mImpl->mTapGestureDetector.Reset();
  }

  if((type & GestureType::LONG_PRESS) && mImpl->mLongPressGestureDetector)
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

void Control::SetKeyboardNavigationSupport(bool isSupported)
{
  mImpl->mIsKeyboardNavigationSupported = isSupported;
}

bool Control::IsKeyboardNavigationSupported()
{
  return mImpl->mIsKeyboardNavigationSupported;
}

void Control::SetKeyInputFocus()
{
  if(Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    Toolkit::KeyInputFocusManager::Get().SetFocus(Toolkit::Control::DownCast(Self()));
  }
}

bool Control::HasKeyInputFocus()
{
  bool result = false;
  if(Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    Toolkit::Control control = Toolkit::KeyInputFocusManager::Get().GetCurrentFocusControl();
    if(Self() == control)
    {
      result = true;
    }
  }
  return result;
}

void Control::ClearKeyInputFocus()
{
  if(Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    Toolkit::KeyInputFocusManager::Get().RemoveFocus(Toolkit::Control::DownCast(Self()));
  }
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

void Control::KeyboardEnter()
{
  // Inform deriving classes
  OnKeyboardEnter();
}

bool Control::OnAccessibilityActivated()
{
  if(Toolkit::KeyboardFocusManager::Get().SetCurrentFocusActor(Self()))
  {
    return OnKeyboardEnter();
  }
  return false;
}

bool Control::OnKeyboardEnter()
{
  return false; // Keyboard enter is not handled by default
}

bool Control::OnAccessibilityPan(PanGesture gesture)
{
  return false; // Accessibility pan gesture is not handled by default
}

bool Control::OnAccessibilityValueChange(bool isIncrease)
{
  return false; // Accessibility value change action is not handled by default
}

bool Control::OnAccessibilityZoom()
{
  return false; // Accessibility zoom action is not handled by default
}

DevelControl::ControlAccessible* Control::CreateAccessibleObject()
{
  return new DevelControl::ControlAccessible(Self());
}

Actor Control::GetNextKeyboardFocusableActor(Actor currentFocusedActor, Toolkit::Control::KeyboardFocus::Direction direction, bool loopEnabled)
{
  return Actor();
}

void Control::OnKeyboardFocusChangeCommitted(Actor commitedFocusableActor)
{
}

Toolkit::Control::KeyEventSignalType& Control::KeyEventSignal()
{
  return mImpl->mKeyEventSignal;
}

Toolkit::Control::KeyInputFocusSignalType& Control::KeyInputFocusGainedSignal()
{
  return mImpl->mKeyInputFocusGainedSignal;
}

Toolkit::Control::KeyInputFocusSignalType& Control::KeyInputFocusLostSignal()
{
  return mImpl->mKeyInputFocusLostSignal;
}

bool Control::EmitKeyEventSignal(const KeyEvent& event)
{
  // Guard against destruction during signal emission
  Dali::Toolkit::Control handle(GetOwner());

  bool consumed = false;

  consumed = mImpl->FilterKeyEvent(event);

  // signals are allocated dynamically when someone connects
  if(!consumed && !mImpl->mKeyEventSignal.Empty())
  {
    consumed = mImpl->mKeyEventSignal.Emit(handle, event);
  }

  if(!consumed)
  {
    // Notification for derived classes
    consumed = OnKeyEvent(event);
  }

  return consumed;
}

Control::Control(ControlBehaviour behaviourFlags)
: CustomActorImpl(static_cast<ActorFlags>(behaviourFlags)),
  mImpl(new Impl(*this))
{
  mImpl->mFlags = behaviourFlags;
}

Control::~Control()
{
  // Deactivate render effect before destroying the control impl
  ClearRenderEffect();

  delete mImpl;
}

void Control::Initialize()
{
  // Call deriving classes so initialised before styling is applied to them.
  OnInitialize();

  if(!(mImpl->mFlags & DISABLE_STYLE_CHANGE_SIGNALS))
  {
    Toolkit::StyleManager styleManager = StyleManager::Get();

    // if stylemanager is available
    if(styleManager)
    {
      StyleManager& styleManagerImpl = GetImpl(styleManager);

      // Register for style changes
      styleManagerImpl.ControlStyleChangeSignal().Connect(this, &Control::OnStyleChange);

      // Apply the current style
      styleManagerImpl.ApplyThemeStyleAtInit(Toolkit::Control(GetOwner()));
    }
  }

  if(mImpl->mFlags & REQUIRES_KEYBOARD_NAVIGATION_SUPPORT)
  {
    SetKeyboardNavigationSupport(true);
  }
}

void Control::OnInitialize()
{
}

bool Control::IsResourceReady() const
{
  const Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(*this);
  return controlDataImpl.IsResourceReady();
}

void Control::OnStyleChange(Toolkit::StyleManager styleManager, StyleChange::Type change)
{
  // By default the control is only interested in theme (not font) changes
  if(styleManager && change == StyleChange::THEME_CHANGE)
  {
    GetImpl(styleManager).ApplyThemeStyle(Toolkit::Control(GetOwner()));
    RelayoutRequest();
  }
}

void Control::OnPinch(const PinchGesture& pinch)
{
  if(!(mImpl->mStartingPinchScale))
  {
    // lazy allocate
    mImpl->mStartingPinchScale = new Vector3;
  }

  if(pinch.GetState() == GestureState::STARTED)
  {
    *(mImpl->mStartingPinchScale) = Self().GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  }

  Self().SetProperty(Actor::Property::SCALE, *(mImpl->mStartingPinchScale) * pinch.GetScale());
}

void Control::OnPan(const PanGesture& pan)
{
}

void Control::OnTap(const TapGesture& tap)
{
}

void Control::OnLongPress(const LongPressGesture& longPress)
{
}

void Control::EmitKeyInputFocusSignal(bool focusGained)
{
  Dali::Toolkit::Control handle(GetOwner());

  if(Accessibility::IsUp())
  {
    auto accessible = GetAccessibleObject();
    if(DALI_LIKELY(accessible))
    {
      accessible->EmitFocused(focusGained);
      auto parent = dynamic_cast<Dali::Accessibility::ActorAccessible*>(accessible->GetParent());
      if(parent && !accessible->GetStates()[Dali::Accessibility::State::MANAGES_DESCENDANTS])
      {
        parent->EmitActiveDescendantChanged(accessible.get());
      }
    }
  }

  if(focusGained)
  {
    // signals are allocated dynamically when someone connects
    if(!mImpl->mKeyInputFocusGainedSignal.Empty())
    {
      mImpl->mKeyInputFocusGainedSignal.Emit(handle);
    }
  }
  else
  {
    // signals are allocated dynamically when someone connects
    if(!mImpl->mKeyInputFocusLostSignal.Empty())
    {
      mImpl->mKeyInputFocusLostSignal.Emit(handle);
    }
  }
}

void Control::OnSceneConnection(int depth)
{
  mImpl->OnSceneConnection();

  // The clipping renderer is only created if required.
  CreateClippingRenderer(*this);
}

void Control::OnSceneDisconnection()
{
  mImpl->OnSceneDisconnection();
}

void Control::OnKeyInputFocusGained()
{
  EmitKeyInputFocusSignal(true);
}

void Control::OnKeyInputFocusLost()
{
  EmitKeyInputFocusSignal(false);
}

void Control::OnChildAdd(Actor& child)
{
}

void Control::OnChildRemove(Actor& child)
{
}

void Control::OnPropertySet(Property::Index index, const Property::Value& propertyValue)
{
  // If the clipping mode has been set, we may need to create a renderer.
  // Only do this if we are already on-stage as the OnSceneConnection will handle the off-stage clipping controls.
  switch(index)
  {
    case Actor::Property::CLIPPING_MODE:
    {
      if(Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
      {
        // Note: This method will handle whether creation of the renderer is required.
        CreateClippingRenderer(*this);
      }
      break;
    }
    case DevelActor::Property::USER_INTERACTION_ENABLED:
    {
      const bool enabled = propertyValue.Get<bool>();
      if(!enabled && Self() == Dali::Toolkit::KeyboardFocusManager::Get().GetCurrentFocusActor())
      {
        Dali::Toolkit::KeyboardFocusManager::Get().ClearFocus();
      }
      break;
    }
  }
}

void Control::OnSizeSet(const Vector3& targetSize)
{
  Vector2               size(targetSize);
  Toolkit::Visual::Base visual = mImpl->GetVisual(Toolkit::Control::Property::BACKGROUND);
  if(visual)
  {
    visual.SetTransformAndSize(Property::Map(), size); // Send an empty map as we do not want to modify the visual's set transform
  }

  // Apply FittingMode here
  mImpl->mSize = size;
  mImpl->RegisterProcessorOnce();
}

void Control::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
  // @todo size negotiate background to new size, animate as well?
}

void Control::GetOffScreenRenderTasks(std::vector<Dali::RenderTask>& tasks, bool isForward)
{
  if(mImpl->mRenderEffect)
  {
    Toolkit::Internal::RenderEffectImpl* object = dynamic_cast<Toolkit::Internal::RenderEffectImpl*>(mImpl->mRenderEffect.GetObjectPtr());

    if(object)
    {
      object->GetOffScreenRenderTasks(tasks, isForward);
    }
  }
}

bool Control::OnKeyEvent(const KeyEvent& event)
{
  return false; // Do not consume
}

void Control::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  for(unsigned int i = 0, numChildren = Self().GetChildCount(); i < numChildren; ++i)
  {
    Actor   child = Self().GetChildAt(i);
    Vector2 newChildSize(size);

    // When set the padding or margin on the control, child should be resized and repositioned.
    if((mImpl->mPadding.start != 0) || (mImpl->mPadding.end != 0) || (mImpl->mPadding.top != 0) || (mImpl->mPadding.bottom != 0) ||
       (mImpl->mMargin.start != 0) || (mImpl->mMargin.end != 0) || (mImpl->mMargin.top != 0) || (mImpl->mMargin.bottom != 0))
    {
      Extents padding = mImpl->mPadding;

      Dali::CustomActor           ownerActor(GetOwner());
      Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>(ownerActor.GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());

      if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
      {
        std::swap(padding.start, padding.end);
      }

      newChildSize.width  = size.width - (padding.start + padding.end);
      newChildSize.height = size.height - (padding.top + padding.bottom);

      // Cannot use childs Position property as it can already have padding and margin applied on it,
      // so we end up cumulatively applying them over and over again.
      Vector2 childOffset(0.f, 0.f);
      childOffset.x += (mImpl->mMargin.start + padding.start);
      childOffset.y += (mImpl->mMargin.top + padding.top);

      child.SetProperty(Actor::Property::POSITION, Vector2(childOffset.x, childOffset.y));
    }
    container.Add(child, newChildSize);
  }

  // Apply FittingMode here
  mImpl->ApplyFittingMode(size);
}

void Control::OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension)
{
}

Vector3 Control::GetNaturalSize()
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::GetNaturalSize for %s\n", Self().GetProperty<std::string>(Dali::Actor::Property::NAME).c_str());
  Toolkit::Visual::Base visual = mImpl->GetVisual(Toolkit::Control::Property::BACKGROUND);
  if(visual)
  {
    Vector2 naturalSize;
    visual.GetNaturalSize(naturalSize);
    naturalSize.width += (mImpl->mPadding.start + mImpl->mPadding.end);
    naturalSize.height += (mImpl->mPadding.top + mImpl->mPadding.bottom);
    return Vector3(naturalSize);
  }
  return Vector3::ZERO;
}

float Control::CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension)
{
  return CalculateChildSizeBase(child, dimension);
}

float Control::GetHeightForWidth(float width)
{
  return GetHeightForWidthBase(width);
}

float Control::GetWidthForHeight(float height)
{
  return GetWidthForHeightBase(height);
}

bool Control::RelayoutDependentOnChildren(Dimension::Type dimension)
{
  return RelayoutDependentOnChildrenBase(dimension);
}

void Control::OnCalculateRelayoutSize(Dimension::Type dimension)
{
}

void Control::OnLayoutNegotiated(float size, Dimension::Type dimension)
{
}

void Control::SignalConnected(SlotObserver* slotObserver, CallbackBase* callback)
{
  mImpl->SignalConnected(slotObserver, callback);
}

void Control::SignalDisconnected(SlotObserver* slotObserver, CallbackBase* callback)
{
  mImpl->SignalDisconnected(slotObserver, callback);
}

void Control::MakeVisualTransition(Dali::Property::Map& sourcePropertyMap, Dali::Property::Map& destinationPropertyMap, Dali::Toolkit::Control source, Dali::Toolkit::Control destination, Dali::Property::Index visualIndex)
{
  sourcePropertyMap.Clear();
  destinationPropertyMap.Clear();

  Toolkit::Visual::Base sourceVisual      = DevelControl::GetVisual(GetImplementation(source), visualIndex);
  Toolkit::Visual::Base destinationVisual = DevelControl::GetVisual(GetImplementation(destination), visualIndex);

  // If source or destination doesn't have the visual, do not create transition for the visual.
  if(!sourceVisual || !destinationVisual)
  {
    return;
  }

  Property::Map sourceMap;
  Property::Map destinationMap;
  sourceVisual.CreatePropertyMap(sourceMap);
  destinationVisual.CreatePropertyMap(destinationMap);

  static auto findValueVector4 = [](const Property::Map& map, Property::Index index, const Vector4& defaultValue = Vector4()) -> Vector4 {
    Property::Value* propertyValue = map.Find(index);
    if(propertyValue)
    {
      return propertyValue->Get<Vector4>();
    }
    return defaultValue;
  };

  static auto findValueFloat = [](const Property::Map& map, Property::Index index, const float& defaultValue = 0.0f) -> float {
    Property::Value* propertyValue = map.Find(index);
    if(propertyValue)
    {
      return propertyValue->Get<float>();
    }
    return defaultValue;
  };

  Vector4 defaultMixColor(Color::TRANSPARENT);
  Vector4 defaultCornerRadius(0.0f, 0.0f, 0.0f, 0.0f);
  float   defaultBorderlineWidth(0.0f);
  Vector4 defaultBorderlineColor(0.0f, 0.0f, 0.0f, 1.0f);
  float   defaultBorderlineOffset(0.0f);
  Vector4 defaultCornerSquareness(0.0f, 0.0f, 0.0f, 0.0f);

  Vector4 sourceMixColor         = findValueVector4(sourceMap, Dali::Toolkit::Visual::Property::MIX_COLOR, defaultMixColor);
  Vector4 sourceCornerRadius     = findValueVector4(sourceMap, Toolkit::DevelVisual::Property::CORNER_RADIUS, defaultCornerRadius);
  float   sourceBorderlineWidth  = findValueFloat(sourceMap, Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, defaultBorderlineWidth);
  Vector4 sourceBorderlineColor  = findValueVector4(sourceMap, Toolkit::DevelVisual::Property::BORDERLINE_COLOR, defaultBorderlineColor);
  float   sourceBorderlineOffset = findValueFloat(sourceMap, Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, defaultBorderlineOffset);
  Vector4 sourceCornerSquareness = findValueVector4(sourceMap, Toolkit::DevelVisual::Property::CORNER_SQUARENESS, defaultCornerSquareness);

  Vector4 destinationMixColor         = findValueVector4(destinationMap, Dali::Toolkit::Visual::Property::MIX_COLOR, defaultMixColor);
  Vector4 destinationCornerRadius     = findValueVector4(destinationMap, Toolkit::DevelVisual::Property::CORNER_RADIUS, defaultCornerRadius);
  float   destinationBorderlineWidth  = findValueFloat(destinationMap, Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, defaultBorderlineWidth);
  Vector4 destinationBorderlineColor  = findValueVector4(destinationMap, Toolkit::DevelVisual::Property::BORDERLINE_COLOR, defaultBorderlineColor);
  float   destinationBorderlineOffset = findValueFloat(destinationMap, Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, defaultBorderlineOffset);
  Vector4 destinationCornerSquareness = findValueVector4(destinationMap, Toolkit::DevelVisual::Property::CORNER_SQUARENESS, defaultCornerSquareness);

  // If the value of the source Control and that of destination Control is different, the property should be transitioned.
  if(sourceMixColor != destinationMixColor)
  {
    sourcePropertyMap.Add(Dali::Toolkit::Visual::Property::MIX_COLOR, sourceMixColor);
    destinationPropertyMap.Add(Dali::Toolkit::Visual::Property::MIX_COLOR, destinationMixColor);
  }

  if(sourceCornerRadius != destinationCornerRadius)
  {
    sourcePropertyMap.Add(Dali::Toolkit::DevelVisual::Property::CORNER_RADIUS, sourceCornerRadius);
    destinationPropertyMap.Add(Dali::Toolkit::DevelVisual::Property::CORNER_RADIUS, destinationCornerRadius);
  }

  if(!Dali::Equals(sourceBorderlineWidth, destinationBorderlineWidth))
  {
    sourcePropertyMap.Add(Dali::Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, sourceBorderlineWidth);
    destinationPropertyMap.Add(Dali::Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, destinationBorderlineWidth);
  }

  if(sourceBorderlineColor != destinationBorderlineColor)
  {
    sourcePropertyMap.Add(Dali::Toolkit::DevelVisual::Property::BORDERLINE_COLOR, sourceBorderlineColor);
    destinationPropertyMap.Add(Dali::Toolkit::DevelVisual::Property::BORDERLINE_COLOR, destinationBorderlineColor);
  }

  if(!Dali::Equals(sourceBorderlineOffset, destinationBorderlineOffset))
  {
    sourcePropertyMap.Add(Dali::Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, sourceBorderlineOffset);
    destinationPropertyMap.Add(Dali::Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, destinationBorderlineOffset);
  }

  if(sourceCornerSquareness != destinationCornerSquareness)
  {
    sourcePropertyMap.Add(Dali::Toolkit::DevelVisual::Property::CORNER_SQUARENESS, sourceCornerSquareness);
    destinationPropertyMap.Add(Dali::Toolkit::DevelVisual::Property::CORNER_SQUARENESS, destinationCornerSquareness);
  }
}

Control& GetImplementation(Dali::Toolkit::Control& handle)
{
  CustomActorImpl& customInterface = handle.GetImplementation();
  // downcast to control
  Control& impl = dynamic_cast<Internal::Control&>(customInterface);
  return impl;
}

const Control& GetImplementation(const Dali::Toolkit::Control& handle)
{
  const CustomActorImpl& customInterface = handle.GetImplementation();
  // downcast to control
  const Control& impl = dynamic_cast<const Internal::Control&>(customInterface);
  return impl;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
