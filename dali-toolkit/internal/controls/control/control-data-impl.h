#ifndef DALI_TOOLKIT_CONTROL_DATA_IMPL_H
#define DALI_TOOLKIT_CONTROL_DATA_IMPL_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/type-registry.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-event-observer.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/devel-api/common/owner-container.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/internal/controls/tooltip/tooltip.h>
#include <dali-toolkit/internal/builder/style.h>
#include <dali-toolkit/internal/builder/dictionary.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

 /**
  * Struct used to store Visual within the control, index is a unique key for each visual.
  */
struct RegisteredVisual
{
  Property::Index index;
  Toolkit::Visual::Base visual;
  bool enabled : 1;
  bool pending : 1;

  RegisteredVisual( Property::Index aIndex, Toolkit::Visual::Base &aVisual, bool aEnabled, bool aPendingReplacement )
  : index(aIndex), visual(aVisual), enabled(aEnabled), pending( aPendingReplacement )
  {
  }
};

typedef Dali::OwnerContainer< RegisteredVisual* > RegisteredVisualContainer;


/**
 * @brief Holds the Implementation for the internal control class
 */
class Control::Impl : public ConnectionTracker, public Visual::EventObserver
{

public:

  /**
   * @brief Retrieves the implementation of the internal control class.
   * @param[in] internalControl A ref to the control whose internal implementation is required
   * @return The internal implementation
   */
  static Control::Impl& Get( Internal::Control& internalControl );

  /**
   * @copydoc Get( Internal::Control& )
   */
  static const Control::Impl& Get( const Internal::Control& internalControl );

  /**
   * @brief Constructor.
   * @param[in] controlImpl The control which owns this implementation
   */
  Impl( Control& controlImpl );

  /**
   * @brief Destructor.
   */
  ~Impl();

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
   * @brief Called when a resource is ready.
   * @param[in] object The visual whose resources are ready
   * @note Overriding method in Visual::EventObserver.
   */
  void ResourceReady( Visual::Base& object ) override;

  /**
   * @brief Called when an event occurs.
   * @param[in] object The visual whose events occur
   * @param[in] signalId The signal to emit. See Visual to find supported signals
   * @note Overriding method in Visual::EventObserver.
   */
  void NotifyVisualEvent( Visual::Base& object, Property::Index signalId ) override;

  /**
   * @copydoc Dali::Toolkit::DevelControl::RegisterVisual()
   */
  void RegisterVisual( Property::Index index, Toolkit::Visual::Base& visual );

  /**
   * @copydoc Dali::Toolkit::DevelControl::RegisterVisual()
   */
  void RegisterVisual( Property::Index index, Toolkit::Visual::Base& visual, int depthIndex );

  /**
   * @copydoc Dali::Toolkit::DevelControl::RegisterVisual()
   */
  void RegisterVisual( Property::Index index, Toolkit::Visual::Base& visual, bool enabled );

  /**
   * @copydoc Dali::Toolkit::DevelControl::RegisterVisual()
   */
  void RegisterVisual( Property::Index index, Toolkit::Visual::Base& visual, bool enabled, int depthIndex );

  /**
   * @copydoc Dali::Toolkit::DevelControl::UnregisterVisual()
   */
  void UnregisterVisual( Property::Index index );

  /**
   * @copydoc Dali::Toolkit::DevelControl::GetVisual()
   */
  Toolkit::Visual::Base GetVisual( Property::Index index ) const;

  /**
   * @copydoc Dali::Toolkit::DevelControl::EnableVisual()
   */
  void EnableVisual( Property::Index index, bool enable );

  /**
   * @copydoc Dali::Toolkit::DevelControl::IsVisualEnabled()
   */
  bool IsVisualEnabled( Property::Index index ) const;

  /**
   * @brief Stops observing the given visual.
   * @param[in] visual The visual to stop observing
   */
  void StopObservingVisual( Toolkit::Visual::Base& visual );

  /**
   * @brief Starts observing the given visual.
   * @param[in] visual The visual to start observing
   */
  void StartObservingVisual( Toolkit::Visual::Base& visual);

  /**
   * @copydoc Dali::Toolkit::DevelControl::GetVisualResourceStatus()
   */
  Toolkit::Visual::ResourceStatus GetVisualResourceStatus( Property::Index index ) const;

  /**
   * @param[in,out] animation Handle to existing animation, or an empty handle that
   * can be set to a New animation if createAnimation is true
   * @param[in] transitionData The transition data describing the animation
   * @param[in] createAnimation True if the animation should be created
   */
  void AddTransitions( Dali::Animation& animation,
                       const Toolkit::TransitionData& transitionData,
                       bool createAnimation = false );

  /**
   * @copydoc Dali::Toolkit::DevelControl::CreateTransition()
   */
  Dali::Animation CreateTransition( const Toolkit::TransitionData& transitionData );

  /**
   * @copydoc Dali::Toolkit::DevelControl::DoAction()
   */
  void DoAction( Dali::Property::Index visualIndex, Dali::Property::Index actionId, const Dali::Property::Value attributes );

  /**
   * @brief Function used to set control properties.
   * @param[in] object The object whose property to set
   * @param[in] index The index of the property to set
   * @param[in] value The value of the property to set
   */
  static void SetProperty( BaseObject* object, Property::Index index, const Property::Value& value );

  /**
   * @brief Function used to retrieve the value of control properties.
   * @param[in] object The object whose property to get
   * @param[in] index The index of the property to get
   * @return The value of the property
   */
  static Property::Value GetProperty( BaseObject* object, Property::Index index );

  /**
   * @brief Sets the state of the control.
   * @param[in] newState The state to set
   * @param[in] withTransitions Whether to show a transition when changing to the new state
   */
  void SetState( DevelControl::State newState, bool withTransitions=true );

  /**
   * @brief Sets the sub-state of the control.
   * @param[in] newState The sub-state to set
   * @param[in] withTransitions Whether to show a transition when changing to the new sub-state
   */
  void SetSubState( const std::string& subStateName, bool withTransitions=true );

  /**
   * @brief Replaces visuals and properties from the old state to the new state.
   * @param[in] oldState The old state
   * @param[in] newState The new state
   * @param[in] subState The current sub state
   */
  void ReplaceStateVisualsAndProperties( const StylePtr oldState, const StylePtr newState, const std::string& subState );

  /**
   * @brief Removes a visual from the control's container.
   * @param[in] visuals The container of visuals
   * @param[in] visualName The name of the visual to remove
   */
  void RemoveVisual( RegisteredVisualContainer& visuals, const std::string& visualName );

  /**
   * @brief Removes several visuals from the control's container.
   * @param[in] visuals The container of visuals
   * @param[in] removeVisuals The visuals to remove
   */
  void RemoveVisuals( RegisteredVisualContainer& visuals, DictionaryKeys& removeVisuals );

  /**
   * @brief Copies the visual properties that are specific to the control instance into the instancedProperties container.
   * @param[in] visuals The control's visual container
   * @param[out] instancedProperties The instanced properties are added to this container
   */
  void CopyInstancedProperties( RegisteredVisualContainer& visuals, Dictionary<Property::Map>& instancedProperties );

  /**
   * @brief On state change, ensures visuals are moved or created appropriately.
   *
   * Go through the list of visuals that are common to both states.
   * If they are different types, or are both image types with different
   * URLs, then the existing visual needs moving and the new visual needs creating
   *
   * @param[in] stateVisualsToChange The visuals to change
   * @param[in] instancedProperties The instanced properties @see CopyInstancedProperties
   */
  void RecreateChangedVisuals( Dictionary<Property::Map>& stateVisualsToChange, Dictionary<Property::Map>& instancedProperties );

  /**
   * @brief Whether the resource is ready
   * @return True if the resource is read.
   */
  bool IsResourceReady() const;

  /**
   * @copydoc CustomActorImpl::OnSceneDisconnection()
   */
  void OnSceneDisconnection();

  /**
   * @brief Sets the margin.
   * @param[in] margin Margin is a collections of extent ( start, end, top, bottom )
   */
  void SetMargin( Extents margin );

  /**
   * @brief Returns the value of margin
   * @return The value of margin
   */
  Extents GetMargin() const;

  /**
   * @brief Sets the padding.
   * @param[in] padding Padding is a collections of extent ( start, end, top, bottom ).
   */
  void SetPadding( Extents padding );

  /**
   * @brief Returns the value of padding
   * @return The value of padding
   */
  Extents GetPadding() const;

  /**
   * @brief Set the input method context.
   * @param[in] inputMethodContext The input method context.
   */
  void SetInputMethodContext( InputMethodContext& inputMethodContext );

  /**
   * @brief Filter an key event.
   * @param[in] event The key to be filtered.
   * @return True if the key handled, otherwise false.
   */
  bool FilterKeyEvent( const KeyEvent& event );

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

private:

  /**
   * Used as an alternative to boolean so that it is obvious whether a visual is enabled/disabled.
   */
  struct VisualState
  {
    enum Type
    {
      DISABLED = 0, ///< Visual disabled.
      ENABLED = 1   ///< Visual enabled.
    };
  };

  /**
   * Used as an alternative to boolean so that it is obvious whether a visual's depth value has been set or not by the caller.
   */
  struct DepthIndexValue
  {
    enum Type
    {
      NOT_SET = 0, ///< Visual depth value not set by caller.
      SET = 1      ///< Visual depth value set by caller.
    };
  };

  /**
   * @brief Adds the visual to the list of registered visuals.
   * @param[in] index The Property index of the visual, used to reference visual
   * @param[in,out] visual The visual to register, which can be altered in this function
   * @param[in] enabled false if derived class wants to control when visual is set on stage
   * @param[in] depthIndexValueSet Set to true if the depthIndex has actually been set manually
   * @param[in] depthIndex The visual's depth-index is set to this
   *
   * @note Registering a visual with an index that already has a registered visual will replace it. The replacement will
   *       occur once the replacement visual is ready (loaded).
   */
  void RegisterVisual( Property::Index index, Toolkit::Visual::Base& visual, VisualState::Type enabled, DepthIndexValue::Type depthIndexValueSet, int depthIndex = 0 );

  /**
   * @brief Emits the resource ready signal.
   */
  void EmitResourceReadySignal();

  /**
   * @brief Callbacks called on idle.
   */
  void OnIdleCallback();

public:

  Control& mControlImpl;
  DevelControl::State mState;
  std::string mSubStateName;

  int mLeftFocusableActorId;       ///< Actor ID of Left focusable control.
  int mRightFocusableActorId;      ///< Actor ID of Right focusable control.
  int mUpFocusableActorId;         ///< Actor ID of Up focusable control.
  int mDownFocusableActorId;       ///< Actor ID of Down focusable control.

  RegisteredVisualContainer mVisuals;     ///< Stores visuals needed by the control, non trivial type so std::vector used.
  std::string mStyleName;
  Vector4 mBackgroundColor;               ///< The color of the background visual
  Vector3* mStartingPinchScale;           ///< The scale when a pinch gesture starts, TODO: consider removing this
  Extents mMargin;                        ///< The margin values
  Extents mPadding;                       ///< The padding values
  Toolkit::Control::KeyEventSignalType mKeyEventSignal;
  Toolkit::Control::KeyInputFocusSignalType mKeyInputFocusGainedSignal;
  Toolkit::Control::KeyInputFocusSignalType mKeyInputFocusLostSignal;
  Toolkit::Control::ResourceReadySignalType mResourceReadySignal;
  DevelControl::VisualEventSignalType mVisualEventSignal;

  // Gesture Detection
  PinchGestureDetector mPinchGestureDetector;
  PanGestureDetector mPanGestureDetector;
  TapGestureDetector mTapGestureDetector;
  LongPressGestureDetector mLongPressGestureDetector;

  // Tooltip
  TooltipPtr mTooltip;

  InputMethodContext mInputMethodContext;
  CallbackBase* mIdleCallback;             ///< The idle callback to emit the resource ready signal.

  ControlBehaviour mFlags : CONTROL_BEHAVIOUR_FLAG_COUNT;    ///< Flags passed in from constructor.
  bool mIsKeyboardNavigationSupported :1;  ///< Stores whether keyboard navigation is supported by the control.
  bool mIsKeyboardFocusGroup :1;           ///< Stores whether the control is a focus group.
  bool mIsEmittingResourceReadySignal :1;  ///< True during ResourceReady().
  bool mNeedToEmitResourceReady :1;        ///< True if need to emit the resource ready signal again.

  RegisteredVisualContainer mRemoveVisuals;         ///< List of visuals that are being replaced by another visual once ready


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
};


} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_CONTROL_DATA_IMPL_H
