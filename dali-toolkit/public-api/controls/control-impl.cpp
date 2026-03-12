/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/string-utils.h>
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
#include <dali-toolkit/internal/render-effects/render-effect-impl.h>
#include <dali-toolkit/internal/styling/default-theme.h>
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
void CreateClippingRenderer(ControlImpl& controlImpl)
{
  // We want to add a transparent background if we do not have one for clipping.
  Actor self(controlImpl.Self());
  int   clippingMode = ClippingMode::DISABLED;
  if(self.GetProperty(Actor::Property::CLIPPING_MODE).Get(clippingMode))
  {
    Toolkit::ControlImpl::Impl& controlDataImpl = Toolkit::ControlImpl::Impl::Get(controlImpl);

    if(clippingMode == ClippingMode::CLIP_CHILDREN && (DALI_UNLIKELY(!controlDataImpl.mVisualData) || controlDataImpl.mVisualData->mVisuals.Empty()) && self.GetRendererCount() == 0u)
    {
      controlImpl.SetBackgroundColor(Color::TRANSPARENT);
    }
  }
}

/**
 * @brief Register external accessible getter function only 1 times per each programs.
 * @note We could call this API only for main thread. So use static bool flag is enough than std::once_flag
 */
void RegisterControlAccessibleGetter()
{
  static bool onceFlag = false;
  if(DALI_UNLIKELY(!onceFlag))
  {
    onceFlag = true;
    Accessibility::Accessible::RegisterExternalAccessibleGetter([](Dali::Actor actor) -> std::pair<std::shared_ptr<Accessibility::Accessible>, bool>
    {
      auto control = Toolkit::Control::DownCast(actor);
      if(!control)
      {
        return {nullptr, true};
      }

      if(Toolkit::DevelControl::IsCreateAccessibleEnabled(control))
      {
        auto& controlImpl = Toolkit::GetImplementation(control);
        return {std::shared_ptr<DevelControl::ControlAccessible>(controlImpl.CreateAccessibleObject()), true};
      }

      return {nullptr, false}; });
  }
}
} // unnamed namespace

Toolkit::Control ControlImpl::New()
{
  return New(ControlBehaviour::CONTROL_BEHAVIOUR_DEFAULT);
}

Toolkit::Control ControlImpl::New(ControlBehaviour additionalBehaviour)
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<ControlImpl> controlImpl = new ControlImpl(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT | additionalBehaviour));

  // Pass ownership to handle
  Toolkit::Control handle(*controlImpl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  controlImpl->Initialize();

  return handle;
}

void ControlImpl::SetStyleName(const Dali::String& styleName)
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

const Dali::String& ControlImpl::GetStyleName() const
{
  return mImpl->mStyleName;
}

void ControlImpl::SetBackgroundColor(const Vector4& color)
{
  mImpl->mBackgroundColor = color;

  Property::Map map;
  map.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  map.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, color);

  Toolkit::Internal::Visual::Base* visualImplPtr = mImpl->GetVisualImplPtr(Toolkit::Control::Property::BACKGROUND);
  if(visualImplPtr && visualImplPtr->GetType() == Toolkit::Visual::COLOR)
  {
    // Update background color only
    visualImplPtr->DoAction(DevelVisual::Action::UPDATE_PROPERTY, map);
    return;
  }

  SetBackground(map);
}

void ControlImpl::SetBackground(const Property::Map& map)
{
  Toolkit::Visual::Base visual = Toolkit::VisualFactory::Get().CreateVisual(map);
  visual.SetName("background");
  if(visual)
  {
    mImpl->RegisterVisual(Toolkit::Control::Property::BACKGROUND, visual, DepthIndex::BACKGROUND);
    mImpl->EnableCornerPropertiesOverridden(visual, true);

    // Trigger a size negotiation request that may be needed by the new visual to relayout its contents.
    RelayoutRequest();
  }
}

void ControlImpl::ClearBackground()
{
  mImpl->UnregisterVisual(Toolkit::Control::Property::BACKGROUND);
  mImpl->mBackgroundColor = Color::TRANSPARENT;

  // Trigger a size negotiation request that may be needed when unregistering a visual.
  RelayoutRequest();
}

void ControlImpl::SetRenderEffect(Toolkit::RenderEffect effect)
{
  ClearRenderEffect();

  if(effect)
  {
    Internal::RenderEffectImpl* object = dynamic_cast<Internal::RenderEffectImpl*>(effect.GetObjectPtr());
    DALI_ASSERT_ALWAYS(object && "Given render effect is not valid.");

    Dali::Toolkit::Control ownerControl(GetOwner());
    object->SetOwnerControl(ownerControl);

    mImpl->mRenderEffect = object;
  }
  else
  {
    mImpl->mRenderEffect.Reset();
  }
}

RenderEffect ControlImpl::GetRenderEffect() const
{
  return RenderEffect(mImpl->mRenderEffect.Get());
}

void ControlImpl::ClearRenderEffect()
{
  if(mImpl->mRenderEffect)
  {
    Internal::RenderEffectImplPtr effectImpl = std::move(mImpl->mRenderEffect);

    // Reset handle first to avoid circular reference
    mImpl->mRenderEffect.Reset();

    effectImpl->ClearOwnerControl();
  }
}

void ControlImpl::SetResourceReady()
{
  Toolkit::ControlImpl::Impl& controlDataImpl = Toolkit::ControlImpl::Impl::Get(*this);
  controlDataImpl.ResourceReady();
}

Dali::Actor ControlImpl::GetOffScreenRenderableSourceActor()
{
  // Need to override this in FORWARD OffScreenRenderable
  return Dali::Actor();
}

bool ControlImpl::IsOffScreenRenderTaskExclusive()
{
  return false;
}

std::shared_ptr<Toolkit::DevelControl::ControlAccessible> ControlImpl::GetAccessibleObject()
{
  return mImpl->GetAccessibleObject();
}

void ControlImpl::EnableGestureDetection(GestureType::Value type)
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

void ControlImpl::DisableGestureDetection(GestureType::Value type)
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

void ControlImpl::SetKeyboardNavigationSupport(bool isSupported)
{
  mImpl->mIsKeyboardNavigationSupported = isSupported;
}

bool ControlImpl::IsKeyboardNavigationSupported()
{
  return mImpl->mIsKeyboardNavigationSupported;
}

void ControlImpl::SetKeyInputFocus()
{
  if(Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    Toolkit::KeyInputFocusManager::Get().SetFocus(Toolkit::Control::DownCast(Self()));
  }
}

bool ControlImpl::HasKeyInputFocus()
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

void ControlImpl::ClearKeyInputFocus()
{
  if(Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    Toolkit::KeyInputFocusManager::Get().RemoveFocus(Toolkit::Control::DownCast(Self()));
  }
}

void ControlImpl::SetAsKeyboardFocusGroup(bool isFocusGroup)
{
  mImpl->mIsKeyboardFocusGroup = isFocusGroup;

  // The following line will be removed when the deprecated API in KeyboardFocusManager is deleted
  Toolkit::KeyboardFocusManager::Get().SetAsFocusGroup(Self(), isFocusGroup);
}

bool ControlImpl::IsKeyboardFocusGroup()
{
  return Toolkit::KeyboardFocusManager::Get().IsFocusGroup(Self());
}

void ControlImpl::KeyboardEnter()
{
  // Inform deriving classes
  OnKeyboardEnter();
}

bool ControlImpl::OnAccessibilityActivated()
{
  if(Toolkit::KeyboardFocusManager::Get().SetCurrentFocusActor(Self()))
  {
    return OnKeyboardEnter();
  }
  return false;
}

bool ControlImpl::OnKeyboardEnter()
{
  return false; // Keyboard enter is not handled by default
}

bool ControlImpl::OnAccessibilityPan(PanGesture gesture)
{
  return false; // Accessibility pan gesture is not handled by default
}

bool ControlImpl::OnAccessibilityValueChange(bool isIncrease)
{
  return false; // Accessibility value change action is not handled by default
}

bool ControlImpl::OnAccessibilityZoom()
{
  return false; // Accessibility zoom action is not handled by default
}

DevelControl::ControlAccessible* ControlImpl::CreateAccessibleObject()
{
  return new DevelControl::ControlAccessible(Self());
}

Actor ControlImpl::GetNextKeyboardFocusableActor(Actor currentFocusedActor, Toolkit::Control::KeyboardFocus::Direction direction, bool loopEnabled)
{
  return Actor();
}

void ControlImpl::OnKeyboardFocusChangeCommitted(Actor commitedFocusableActor)
{
}

Toolkit::Control::KeyEventSignalType& ControlImpl::KeyEventSignal()
{
  return mImpl->mKeyEventSignal;
}

Toolkit::Control::KeyInputFocusSignalType& ControlImpl::KeyInputFocusGainedSignal()
{
  return mImpl->mKeyInputFocusGainedSignal;
}

Toolkit::Control::KeyInputFocusSignalType& ControlImpl::KeyInputFocusLostSignal()
{
  return mImpl->mKeyInputFocusLostSignal;
}

bool ControlImpl::EmitKeyEventSignal(const KeyEvent& event)
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

void ControlImpl::RefreshRenderEffects()
{
  if(mImpl->mRenderEffect)
  {
    mImpl->mRenderEffect->Refresh();
  }
  if(mImpl->mOffScreenRenderingImpl)
  {
    mImpl->mOffScreenRenderingImpl->Refresh();
  }
}

Dali::Texture ControlImpl::GetOffScreenRenderingOutput() const
{
  if(mImpl->mOffScreenRenderingType != DevelControl::OffScreenRenderingType::REFRESH_ONCE)
  {
    DALI_LOG_ERROR("Precondition unsatisfied: Set property OFFSCREEN_RENDERING to OffScreenRenderingType::REFRESH_ONCE\n");
    return Dali::Texture();
  }
  return mImpl->mOffScreenRenderingImpl->GetTexture();
}

ControlImpl::ControlImpl(ControlBehaviour behaviourFlags)
: CustomActorImpl(static_cast<ActorFlags>(behaviourFlags)),
  mImpl(new Impl(*this))
{
  mImpl->mFlags = behaviourFlags;
}

ControlImpl::~ControlImpl()
{
  // Deactivate render effect before destroying the control impl
  ClearRenderEffect();

  delete mImpl;
}

void ControlImpl::Initialize()
{
  if(!(mImpl->mFlags & DISABLE_VISUALS))
  {
    mImpl->InitializeVisualData();
  }

  RegisterControlAccessibleGetter();

  // Call deriving classes so initialised before styling is applied to them.
  OnInitialize();

  if(!(mImpl->mFlags & DISABLE_STYLE_CHANGE_SIGNALS))
  {
    Toolkit::StyleManager styleManager = StyleManager::Get();

    // if stylemanager is available
    if(styleManager)
    {
      Internal::StyleManager& styleManagerImpl = GetImpl(styleManager);

      // Register for style changes
      styleManagerImpl.ControlStyleChangeSignal().Connect(this, &ControlImpl::OnStyleChange);

      // Apply the current style
      styleManagerImpl.ApplyThemeStyleAtInit(Toolkit::Control(GetOwner()));
    }
  }
  else
  {
    // Apply default theme DISABLE_STYLE_CHANGE_SIGNALS case
    OnApplyDefaultStyle();
  }

  if(mImpl->mFlags & REQUIRES_KEYBOARD_NAVIGATION_SUPPORT)
  {
    SetKeyboardNavigationSupport(true);
  }
}

void ControlImpl::OnInitialize()
{
}

bool ControlImpl::IsResourceReady() const
{
  const Toolkit::ControlImpl::Impl& controlDataImpl = Toolkit::ControlImpl::Impl::Get(*this);
  return controlDataImpl.IsResourceReady();
}

void ControlImpl::OnStyleChange(Toolkit::StyleManager styleManager, StyleChange::Type change)
{
  // By default the control is only interested in theme (not font) changes
  if(styleManager && change == StyleChange::THEME_CHANGE)
  {
    GetImpl(styleManager).ApplyThemeStyle(Toolkit::Control(GetOwner()));
    RelayoutRequest();
  }
}

void ControlImpl::OnApplyDefaultStyle()
{
}

void ControlImpl::OnPinch(const PinchGesture& pinch)
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

void ControlImpl::OnPan(const PanGesture& pan)
{
}

void ControlImpl::OnTap(const TapGesture& tap)
{
}

void ControlImpl::OnLongPress(const LongPressGesture& longPress)
{
}

void ControlImpl::EmitKeyInputFocusSignal(bool focusGained)
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

void ControlImpl::OnSceneConnection(int depth)
{
  mImpl->OnSceneConnection();

  // The clipping renderer is only created if required.
  CreateClippingRenderer(*this);
}

void ControlImpl::OnSceneDisconnection()
{
  mImpl->OnSceneDisconnection();
}

void ControlImpl::OnKeyInputFocusGained()
{
  EmitKeyInputFocusSignal(true);
}

void ControlImpl::OnKeyInputFocusLost()
{
  EmitKeyInputFocusSignal(false);
}

void ControlImpl::OnChildAdd(Actor& child)
{
}

void ControlImpl::OnChildRemove(Actor& child)
{
}

void ControlImpl::OnPropertySet(Property::Index index, const Property::Value& propertyValue)
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

void ControlImpl::OnSizeSet(const Vector3& targetSize)
{
  Vector2 size(targetSize);

  Toolkit::Internal::Visual::Base* visualImplPtr = mImpl->GetVisualImplPtr(Toolkit::Control::Property::BACKGROUND);
  if(visualImplPtr)
  {
    visualImplPtr->SetControlSize(size); // Send an empty map as we do not want to modify the visual's set transform
  }

  // Apply FittingMode here
  mImpl->mSize = Vector2(targetSize);
  mImpl->RegisterProcessorOnce();

  // Refresh render effects
  RefreshRenderEffects();
}

void ControlImpl::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
  // @todo size negotiate background to new size, animate as well?

  // TODO : Could we clear animation constraint when size animation stopped?
  mImpl->CreateAnimationConstraints(animation.GetBaseObject(), Dali::Actor::Property::SIZE);
}

void ControlImpl::OnAnimateAnimatableProperty(Animation& animation, Property::Index index, Dali::Animation::State state)
{
  if(state == Animation::State::PLAYING)
  {
    mImpl->CreateAnimationConstraints(animation.GetBaseObject(), index);
  }
  else if(state == Animation::State::STOPPED)
  {
    mImpl->ClearAnimationConstraints(animation.GetBaseObject(), index);
  }
}

void ControlImpl::OnConstraintAnimatableProperty(Constraint& constraint, Property::Index index, bool applied)
{
  if(applied)
  {
    mImpl->CreateAnimationConstraints(constraint.GetBaseObject(), index);
  }
  else
  {
    mImpl->ClearAnimationConstraints(constraint.GetBaseObject(), index);
  }
}

void ControlImpl::GetOffScreenRenderTasks(Dali::Vector<Dali::RenderTask>& tasks, bool isForward)
{
  if(mImpl->mRenderEffect)
  {
    mImpl->mRenderEffect->GetOffScreenRenderTasks(tasks, isForward);
  }
  if(mImpl->mOffScreenRenderingImpl)
  {
    mImpl->mOffScreenRenderingImpl->GetOffScreenRenderTasks(tasks, isForward);
  }
}

bool ControlImpl::OnKeyEvent(const KeyEvent& event)
{
  return false; // Do not consume
}

void ControlImpl::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  // When set the padding or margin on the control, child should be resized and repositioned.
  if((mImpl->mPadding.start != 0) || (mImpl->mPadding.end != 0) || (mImpl->mPadding.top != 0) || (mImpl->mPadding.bottom != 0) ||
     (mImpl->mMargin.start != 0) || (mImpl->mMargin.end != 0) || (mImpl->mMargin.top != 0) || (mImpl->mMargin.bottom != 0))
  {
    for(unsigned int i = 0, numChildren = Self().GetChildCount(); i < numChildren; ++i)
    {
      Actor   child = Self().GetChildAt(i);
      Vector2 newChildSize(size);

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

      container.Add(child, newChildSize);
    }
  }

  if(Accessibility::IsUp())
  {
    auto accessible = GetAccessibleObject();
    if(DALI_LIKELY(accessible))
    {
      auto highlightFrame = accessible->GetHighlightActor();
      if(accessible->GetCurrentlyHighlightedActor() == this->Self() &&
         highlightFrame.GetProperty<Vector3>(Dali::Actor::Property::SIZE).GetVectorXY() != size)
      {
        // TODO : Need to consider how we can reduce this cost
        highlightFrame.SetProperty(Actor::Property::SIZE, size);
        container.Add(highlightFrame, size);
      }
    }
  }

  // Apply FittingMode here
  mImpl->ApplyFittingMode(size);
}

void ControlImpl::OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension)
{
}

Vector3 ControlImpl::GetNaturalSize()
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "ControlImpl::GetNaturalSize for %s\n", Dali::Integration::ToStdString(Self().GetProperty(Dali::Actor::Property::NAME)).c_str());
  Toolkit::Internal::Visual::Base* visualImplPtr = mImpl->GetVisualImplPtr(Toolkit::Control::Property::BACKGROUND);
  if(visualImplPtr)
  {
    Vector2 naturalSize;
    visualImplPtr->GetNaturalSize(naturalSize);
    naturalSize.width += (mImpl->mPadding.start + mImpl->mPadding.end);
    naturalSize.height += (mImpl->mPadding.top + mImpl->mPadding.bottom);
    return Vector3(naturalSize);
  }
  return Vector3::ZERO;
}

float ControlImpl::CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension)
{
  return CalculateChildSizeBase(child, dimension);
}

float ControlImpl::GetHeightForWidth(float width)
{
  return GetHeightForWidthBase(width);
}

float ControlImpl::GetWidthForHeight(float height)
{
  return GetWidthForHeightBase(height);
}

bool ControlImpl::RelayoutDependentOnChildren(Dimension::Type dimension)
{
  return RelayoutDependentOnChildrenBase(dimension);
}

void ControlImpl::OnCalculateRelayoutSize(Dimension::Type dimension)
{
}

void ControlImpl::OnLayoutNegotiated(float size, Dimension::Type dimension)
{
}

void ControlImpl::SignalConnected(SlotObserver* slotObserver, CallbackBase* callback)
{
  mImpl->SignalConnected(slotObserver, callback);
}

void ControlImpl::SignalDisconnected(SlotObserver* slotObserver, CallbackBase* callback)
{
  mImpl->SignalDisconnected(slotObserver, callback);
}

void ControlImpl::MakeVisualTransition(Dali::Property::Map& sourcePropertyMap, Dali::Property::Map& destinationPropertyMap, Dali::Toolkit::Control source, Dali::Toolkit::Control destination, Dali::Property::Index visualIndex)
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

  static auto findValueVector4 = [](const Property::Map& map, Property::Index index, const Vector4& defaultValue = Vector4()) -> Vector4
  {
    Property::Value* propertyValue = map.Find(index);
    if(propertyValue)
    {
      return propertyValue->Get<Vector4>();
    }
    return defaultValue;
  };

  static auto findValueFloat = [](const Property::Map& map, Property::Index index, const float& defaultValue = 0.0f) -> float
  {
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

ControlImpl& GetImplementation(Dali::Toolkit::Control& handle)
{
  CustomActorImpl& customInterface = handle.GetImplementation();
  // downcast to control
  ControlImpl& impl = dynamic_cast<Toolkit::ControlImpl&>(customInterface);
  return impl;
}

const ControlImpl& GetImplementation(const Dali::Toolkit::Control& handle)
{
  const CustomActorImpl& customInterface = handle.GetImplementation();
  // downcast to control
  const ControlImpl& impl = dynamic_cast<const Toolkit::ControlImpl&>(customInterface);
  return impl;
}

} // namespace Toolkit

} // namespace Dali
