#ifndef DALI_TOOLKIT_CONTROL_DATA_IMPL_H
#define DALI_TOOLKIT_CONTROL_DATA_IMPL_H

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

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/accessibility.h>
#include <dali/integration-api/processor-interface.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/public-api/object/type-registry.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/internal/controls/control/control-decoration-data.h>
#include <dali-toolkit/internal/controls/render-effects/offscreen-rendering-impl.h>
#include <dali-toolkit/internal/controls/render-effects/render-effect-impl.h>
#include <dali-toolkit/internal/controls/tooltip/tooltip.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/integration-api/debug.h>
#include <map>
#include <memory>
#include <set>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace Visual
{
class Base;
}

enum class TriStateProperty
{
  AUTO = 0,
  TRUE,
  FALSE
};

/**
 * @brief Holds the Implementation for the internal control class
 */
class Control::Impl : public ConnectionTracker, public Integration::Processor
{
private:
  class AccessibilityData;
  class VisualData;

public:
  /**
   * @brief Retrieves the implementation of the internal control class.
   * @param[in] internalControl A ref to the control whose internal implementation is required
   * @return The internal implementation
   */
  static Control::Impl& Get(Internal::Control& internalControl);

  /**
   * @copydoc Get( Internal::Control& )
   */
  static const Control::Impl& Get(const Internal::Control& internalControl);

  /**
   * @brief Constructor.
   * @param[in] controlImpl The control which owns this implementation
   */
  Impl(Control& controlImpl);

  /**
   * @brief Destructor.
   */
  ~Impl();

  /**
   * @brief Initialize private VisualData context for this impl.
   */
  void InitializeVisualData();

  /**
   * @brief Called when a pinch is detected.
   * @param[in] actor The actor the pinch occurred on
   * @param[in] pinch The pinch gesture details
   */
  void PinchDetected(Actor actor, const PinchGesture& pinch);

  /**
   * @brief Called when a pan is detected.
   * @param[in] actor The actor the pan occurred on
   * @param[in] pinch The pan gesture details
   */
  void PanDetected(Actor actor, const PanGesture& pan);

  /**
   * @brief Called when a tap is detected.
   * @param[in] actor The actor the tap occurred on
   * @param[in] pinch The tap gesture details
   */
  void TapDetected(Actor actor, const TapGesture& tap);

  /**
   * @brief Called when a long-press is detected.
   * @param[in] actor The actor the long-press occurred on
   * @param[in] pinch The long-press gesture details
   */
  void LongPressDetected(Actor actor, const LongPressGesture& longPress);

  /**
   * @brief Called when resources of control are ready.
   */
  void ResourceReady();

  /**
   * @copydoc Dali::Toolkit::DevelControl::RegisterVisual()
   */
  void RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual);

  /**
   * @copydoc Dali::Toolkit::DevelControl::RegisterVisual()
   */
  void RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, int depthIndex);

  /**
   * @copydoc Dali::Toolkit::DevelControl::RegisterVisual()
   */
  void RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, bool enabled);

  /**
   * @copydoc Dali::Toolkit::DevelControl::RegisterVisual()
   */
  void RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, bool enabled, int depthIndex);

  /**
   * @copydoc Dali::Toolkit::DevelControl::UnregisterVisual()
   */
  void UnregisterVisual(Property::Index index);

  /**
   * @copydoc Dali::Toolkit::DevelControl::GetVisual()
   */
  Toolkit::Visual::Base GetVisual(Property::Index index) const;

  /**
   * @brief Get the raw pointer of visual impl.
   * It will be used when we want to get visual infomations without increase reference counts.
   * @note Only for internal usage.
   *
   * @param[in] index Index of parameter
   * @return Raw pointer of visual base implements. nullptr if not exist.
   */
  Toolkit::Internal::Visual::Base* GetVisualImplPtr(Property::Index index) const;

  /**
   * @brief Sets the given visual to be ready transition
   *
   * @param[in] control The control
   * @param[in] visual The visual to ready transition overriden
   * @param[in] enable flag to set enabled or disabled.
   */
  void EnableReadyTransitionOverridden(Toolkit::Visual::Base& visual, bool enable);

  /**
   * @brief Enables or disables overriding the given visual's corner properties to its control's
   * @param[in] visual A registered visual
   * @param[in] enable flat to set enabled or disabled.
   */
  void EnableCornerPropertiesOverridden(Toolkit::Visual::Base& visual, bool enable);

  /**
   * @copydoc Dali::Toolkit::DevelControl::EnableVisual()
   */
  void EnableVisual(Property::Index index, bool enable);

  /**
   * @copydoc Dali::Toolkit::DevelControl::IsVisualEnabled()
   */
  bool IsVisualEnabled(Property::Index index) const;

  /**
   * @copydoc Dali::Toolkit::DevelControl::GetVisualResourceStatus()
   */
  Toolkit::Visual::ResourceStatus GetVisualResourceStatus(Property::Index index) const;

  /**
   * @param[in,out] animation Handle to existing animation, or an empty handle that
   * can be set to a New animation if createAnimation is true
   * @param[in] transitionData The transition data describing the animation
   * @param[in] createAnimation True if the animation should be created
   */
  void AddTransitions(Dali::Animation&               animation,
                      const Toolkit::TransitionData& transitionData,
                      bool                           createAnimation = false);

  /**
   * @copydoc Dali::Toolkit::DevelControl::CreateTransition()
   */
  Dali::Animation CreateTransition(const Toolkit::TransitionData& transitionData);

  /**
   * @copydoc Dali::Toolkit::DevelControl::DoAction()
   */
  void DoAction(Dali::Property::Index visualIndex, Dali::Property::Index actionId, const Dali::Property::Value attributes);

  /**
   * @copydoc Dali::Toolkit::DevelControl::DoActionExtension()
   */
  void DoActionExtension(Dali::Property::Index visualIndex, Dali::Property::Index actionId, Dali::Any attributes);

  /**
   * @brief Function used to set control properties.
   * @param[in] object The object whose property to set
   * @param[in] index The index of the property to set
   * @param[in] value The value of the property to set
   */
  static void SetProperty(BaseObject* object, Property::Index index, const Property::Value& value);

  /**
   * @brief Function used to retrieve the value of control properties.
   * @param[in] object The object whose property to get
   * @param[in] index The index of the property to get
   * @return The value of the property
   */
  static Property::Value GetProperty(BaseObject* object, Property::Index index);

  /**
   * @brief Sets the state of the control.
   * @param[in] newState The state to set
   * @param[in] withTransitions Whether to show a transition when changing to the new state
   */
  void SetState(DevelControl::State newState, bool withTransitions = true);

  /**
   * @brief Sets the sub-state of the control.
   * @param[in] newState The sub-state to set
   * @param[in] withTransitions Whether to show a transition when changing to the new sub-state
   */
  void SetSubState(const std::string& subStateName, bool withTransitions = true);

  /**
   * @brief Whether the resource is ready
   * @return True if the resource is read.
   */
  bool IsResourceReady() const;

  /**
   * @copydoc CustomActorImpl::OnSceneConnection()
   */
  void OnSceneConnection();

  /**
   * @copydoc CustomActorImpl::OnSceneDisconnection()
   */
  void OnSceneDisconnection();

  /**
   * @brief Sets the margin.
   * @param[in] margin Margin is a collections of extent ( start, end, top, bottom )
   */
  void SetMargin(Extents margin);

  /**
   * @brief Returns the value of margin
   * @return The value of margin
   */
  Extents GetMargin() const;

  /**
   * @brief Sets the padding.
   * @param[in] padding Padding is a collections of extent ( start, end, top, bottom ).
   */
  void SetPadding(Extents padding);

  /**
   * @brief Returns the value of padding
   * @return The value of padding
   */
  Extents GetPadding() const;

  /**
   * @brief Set the input method context.
   * @param[in] inputMethodContext The input method context.
   */
  void SetInputMethodContext(InputMethodContext& inputMethodContext);

  /**
   * @brief Filter an key event.
   * @param[in] event The key to be filtered.
   * @return True if the key handled, otherwise false.
   */
  bool FilterKeyEvent(const KeyEvent& event);

  /**
   * @brief Get private AccessibilityData context for this impl. If not created yet, it will create new data.
   * @return The l-value of AccessibilityData context.
   */
  [[nodiscard]] AccessibilityData& GetOrCreateAccessibilityData();

  /**
   * @brief Get private AccessibilityData context for this impl.
   * @return The pointer of AccessibilityData context.
   */
  [[nodiscard]] AccessibilityData* GetAccessibilityData() const;

  /**
   * @brief Adds accessibility attribute
   * @param[in] key Attribute name to set
   * @param[in] value Attribute value to set
   *
   * Attribute is added if not existed previously or updated
   * if existed.
   */
  void AppendAccessibilityAttribute(const std::string& key, const std::string value);

  /**
   * @brief Removes accessibility attribute
   * @param[in] key Attribute name to remove
   *
   * Function does nothing if attribute doesn't exist.
   */
  void RemoveAccessibilityAttribute(const std::string& key);

  /**
   * @brief Removes all accessibility attributes
   */
  void ClearAccessibilityAttributes();

  /**
   * @brief Sets reading info type attributes
   * @param[in] types info type attributes to set
   *
   * This function sets, which part of object will be read out
   * by screen-reader.
   */
  void SetAccessibilityReadingInfoType(const Dali::Accessibility::ReadingInfoTypes types);

  /**
   * @brief Gets currently active reading info type attributes
   */
  Dali::Accessibility::ReadingInfoTypes GetAccessibilityReadingInfoType() const;

  /**
   * @copydoc DevelControl::VisualEventSignal()
   */
  DevelControl::VisualEventSignalType& VisualEventSignal();

  /**
   * @brief Sets the shadow with a property map.
   * @param[in] map The shadow property map
   */
  void SetShadow(const Property::Map& map);

  /**
   * @brief Clear the shadow.
   */
  void ClearShadow();

  /**
   * @brief Sets the inner shadow with a property map.
   * @param[in] map The inner shadow property map
   */
  void SetInnerShadow(const Property::Map& map);

  /**
   * @brief Clear the inner shadow.
   */
  void ClearInnerShadow();

  /**
   * @brief Sets the borderline with a property map.
   * @param[in] map The borderline property map
   * @param[in] forciblyCreate Create new visual forcibly, False if we only need to update properties.
   */
  void SetBorderline(const Property::Map& map, bool forciblyCreate);

  /**
   * @brief Clear the borderline.
   */
  void ClearBorderline();

  /**
   * @copydoc DevelControl::GetVisualProperty()
   */
  Dali::Property GetVisualProperty(Dali::Property::Index index, Dali::Property::Key visualPropertyKey);

  /**
   * @brief Retrieves source and destination visual properties for the Transition of this Control.
   * The properties of this Control will be transitioned from the properties of source Control to that of destination Control.
   * If a property value is different between source and destination Control,
   * the property information of each Control will be included in sourceProperties and destinationProperties.
   *
   * @param[out] sourceProperties Source property list to be applied on this Control.
   * @param[out] destinationProperties Destination property list to be applied on this Control.
   * @param[in] source Source control of the animation.
   * @param[in] destination Destination control of the animation.
   *
   * @note This method do not handle Actor properties.
   * And the size and order of the sourceProperties and destinationProperties must be synchronized.
   *
   * This method triggers Control::OnCreateTransition().
   */
  void CreateTransitions(std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>>& sourceProperties,
                         std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>>& destinationProperties,
                         Dali::Toolkit::Control                                              source,
                         Dali::Toolkit::Control                                              destination);

  /**
   * @brief Create constraints to animate animatable properties.
   * @param[in] index The animatable property
   */
  void CreateAnimationConstraints(Property::Index index);

  /**
   * @brief Clear animatable constraints
   * @param[in] index The animatable property
   */
  void ClearAnimationConstraints(Property::Index index);

  /**
   * @brief Update visual properties.
   * @param[in] properties Property list to be used to update visual properties of this Control.
   *
   * @note This method triggers Control::OnUpdateVisualProperties().
   */
  void UpdateVisualProperties(const std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>>& properties);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::GetAccessibleObject()
   */
  std::shared_ptr<Toolkit::DevelControl::ControlAccessible> GetAccessibleObject();

  /**
   * @copydoc Dali::Toolkit::DevelControl::IsAccessibleCreated()
   */
  bool IsAccessibleCreated() const;

  /**
   * @copydoc Dali::Toolkit::DevelControl::EnableCreateAccessible()
   */
  void EnableCreateAccessible(bool enable);

  /**
   * @copydoc Dali::Toolkit::DevelControl::IsCreateAccessibleEnabled()
   */
  bool IsCreateAccessibleEnabled() const;

  /**
   * @brief Apply fittingMode
   *
   * @param[in] size The size of the control
   */
  void ApplyFittingMode(const Vector2& size);

  /**
   * @brief Register processor
   */
  void RegisterProcessorOnce();

protected: // From processor-interface
  /**
   * @copydoc Dali::Integration::Processor::Process()
   */
  void Process(bool postProcessor) override;

  /**
   * @copydoc Dali::Integration::Processor::GetProcessorName()
   */
  std::string_view GetProcessorName() const override
  {
    return "ControlDataImpl";
  }

private:
  /**
   * @brief Emits the resource ready signal.
   */
  void EmitResourceReadySignal();
  /**
   * @brief Callbacks called on idle.
   *
   * @return True if we need to call this idle callback one more time.
   */
  bool OnIdleCallback();

  /**
   * Set off-screen rendering.
   * @param[in] offScreenRenderingType enum OffScreenRenderingType
   */
  void SetOffScreenRendering(int32_t offScreenRenderingType);

  /**
   * Notify to this control's corner radius changed.
   */
  void UpdateCornerRadius();

  /**
   * Notify to this control's borderline changed.
   */
  void UpdateBorderline();

public:
  Control&            mControlImpl;
  DevelControl::State mState;
  std::string         mSubStateName;

  std::unique_ptr<AccessibilityData> mAccessibilityData;
  std::unique_ptr<VisualData>        mVisualData;

  int mLeftFocusableActorId;             ///< Actor ID of Left focusable control.
  int mRightFocusableActorId;            ///< Actor ID of Right focusable control.
  int mUpFocusableActorId;               ///< Actor ID of Up focusable control.
  int mDownFocusableActorId;             ///< Actor ID of Down focusable control.
  int mClockwiseFocusableActorId;        ///< Actor ID of Clockwise focusable control.
  int mCounterClockwiseFocusableActorId; ///< Actor ID of Counter clockwise focusable control.

  std::string                               mStyleName;
  Vector4                                   mBackgroundColor;    ///< The color of the background visual
  RenderEffectImplPtr                       mRenderEffect;       ///< The render effect on this control
  Vector3*                                  mStartingPinchScale; ///< The scale when a pinch gesture starts, TODO: consider removing this
  Extents                                   mMargin;             ///< The margin values
  Extents                                   mPadding;            ///< The padding values
  Vector2                                   mSize;               ///< The size of the control
  Toolkit::Control::KeyEventSignalType      mKeyEventSignal;
  Toolkit::Control::KeyInputFocusSignalType mKeyInputFocusGainedSignal;
  Toolkit::Control::KeyInputFocusSignalType mKeyInputFocusLostSignal;
  Toolkit::Control::ResourceReadySignalType mResourceReadySignal;

  // Gesture Detection
  PinchGestureDetector     mPinchGestureDetector;
  PanGestureDetector       mPanGestureDetector;
  TapGestureDetector       mTapGestureDetector;
  LongPressGestureDetector mLongPressGestureDetector;

  // Decoration data (CornerRadius, Borderline)
  DecorationData* mDecorationData;
  Constraint      mInnerShadowCornerRadiusConstraint; ///< InnerShadow's CornerRadius constriant

  std::unordered_set<Property::Index> mPropertyOnAnimation; ///< Properties that are currently on animation

  // Off screen rendering context
  std::unique_ptr<OffScreenRenderingImpl> mOffScreenRenderingImpl;
  DevelControl::OffScreenRenderingType    mOffScreenRenderingType;

  // Tooltip
  TooltipPtr mTooltip;

  InputMethodContext mInputMethodContext;
  CallbackBase*      mIdleCallback; ///< The idle callback to emit the resource ready signal.

  ControlBehaviour mFlags : CONTROL_BEHAVIOUR_FLAG_COUNT; ///< Flags passed in from constructor.

  // Frequencly touched accessibility relative values.
  // Keep it on Impl to avoid AccessibilityData creation.
  int32_t mAccessibilityRole : Dali::Log<static_cast<uint32_t>(DevelControl::AccessibilityRole::MAX_COUNT)>::value + 2;

  bool mIsKeyboardNavigationSupported : 1; ///< Stores whether keyboard navigation is supported by the control.
  bool mIsKeyboardFocusGroup : 1;          ///< Stores whether the control is a focus group.
  bool mIsEmittingResourceReadySignal : 1; ///< True during ResourceReady().
  bool mIdleCallbackRegistered : 1;        ///< True if need to emit the resource ready signal again.
  bool mDispatchKeyEvents : 1;             ///< Whether the actor emits key event signals
  bool mAccessibleCreatable : 1;           ///< Whether we can create new accessible or not.
  bool mProcessorRegistered : 1;           ///< Whether the processor is registered.

  // Properties - these need to be members of Internal::Control::Impl as they access private methods/data of Internal::Control and Internal::Control::Impl.
  static const PropertyRegistration PROPERTY_1;
  static const PropertyRegistration PROPERTY_2;
  static const PropertyRegistration PROPERTY_3;
  static const PropertyRegistration PROPERTY_4;
  static const PropertyRegistration PROPERTY_5;
  static const PropertyRegistration PROPERTY_6;
  static const PropertyRegistration PROPERTY_7;
  static const PropertyRegistration PROPERTY_8;
  static const PropertyRegistration PROPERTY_9;
  static const PropertyRegistration PROPERTY_10;
  static const PropertyRegistration PROPERTY_11;
  static const PropertyRegistration PROPERTY_12;
  static const PropertyRegistration PROPERTY_13;
  static const PropertyRegistration PROPERTY_14;
  static const PropertyRegistration PROPERTY_15;
  static const PropertyRegistration PROPERTY_16;
  static const PropertyRegistration PROPERTY_17;
  static const PropertyRegistration PROPERTY_18;
  static const PropertyRegistration PROPERTY_19;
  static const PropertyRegistration PROPERTY_20;
  static const PropertyRegistration PROPERTY_21;
  static const PropertyRegistration PROPERTY_22;
  static const PropertyRegistration PROPERTY_23;
  static const PropertyRegistration PROPERTY_24;
  static const PropertyRegistration PROPERTY_25;
  static const PropertyRegistration PROPERTY_26;
  static const PropertyRegistration PROPERTY_27;
  static const PropertyRegistration PROPERTY_28;
  static const PropertyRegistration PROPERTY_29;
  static const PropertyRegistration PROPERTY_30;
  static const PropertyRegistration PROPERTY_31;
  static const PropertyRegistration PROPERTY_32;
  static const PropertyRegistration PROPERTY_33;

  static const AnimatablePropertyRegistration ANIMATABLE_PROPERTY_1;
  static const AnimatablePropertyRegistration ANIMATABLE_PROPERTY_2;
  static const AnimatablePropertyRegistration ANIMATABLE_PROPERTY_3;
  static const AnimatablePropertyRegistration ANIMATABLE_PROPERTY_4;
  static const AnimatablePropertyRegistration ANIMATABLE_PROPERTY_5;
  static const AnimatablePropertyRegistration ANIMATABLE_PROPERTY_6;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_CONTROL_DATA_IMPL_H
