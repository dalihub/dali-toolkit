#ifndef DALI_TOOLKIT_CONTROL_DATA_VISUAL_DATA_H
#define DALI_TOOLKIT_CONTROL_DATA_VISUAL_DATA_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/internal/builder/dictionary.h>
#include <dali-toolkit/internal/builder/style.h>
#include <dali-toolkit/internal/visuals/visual-event-observer.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali/devel-api/common/owner-container.h>

#include <dali-toolkit/internal/controls/control/control-data-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
/**
  * @brief Struct used to store Visual within the control, index is a unique key for each visual.
  */
struct RegisteredVisual
{
  Property::Index       index;
  Toolkit::Visual::Base visual;
  bool                  enabled : 1;
  bool                  pending : 1;
  bool                  overideReadyTransition : 1;

  RegisteredVisual(Property::Index aIndex, Toolkit::Visual::Base& aVisual, bool aEnabled, bool aPendingReplacement)
  : index(aIndex),
    visual(aVisual),
    enabled(aEnabled),
    pending(aPendingReplacement),
    overideReadyTransition(false)
  {
  }
};

typedef Dali::OwnerContainer<RegisteredVisual*> RegisteredVisualContainer;

// private inner class
class Control::Impl::VisualData : public Visual::EventObserver
{
public:
  // Constructor
  VisualData(Control::Impl& outer);

  // Destructor
  ~VisualData();

  /**
   * @brief Called when a resource is ready.
   * @param[in] object The visual whose resources are ready
   * @note Overriding method in Visual::EventObserver.
   */
  void ResourceReady(Visual::Base& object) override;

  /**
   * @brief Called when an event occurs.
   * @param[in] object The visual whose events occur
   * @param[in] signalId The signal to emit. See Visual to find supported signals
   * @note Overriding method in Visual::EventObserver.
   */
  void NotifyVisualEvent(Visual::Base& object, Property::Index signalId) override;

  /**
   * @brief Called when the visual needs relayout request.
   * @param[in] object The visual who requests relayout
   */
  void RelayoutRequest(Visual::Base& object) override;

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::IsResourceReady()
   */
  bool IsResourceReady() const;

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::EnableReadyTransitionOverriden()
   */
  void EnableReadyTransitionOverriden(Toolkit::Visual::Base& visual, bool enable);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::GetVisualResourceStatus()
   */
  Toolkit::Visual::ResourceStatus GetVisualResourceStatus(Property::Index index) const;

  /**
   * @brief Copies the visual properties that are specific to the control instance into the instancedProperties container.
   * @param[in] visuals The control's visual container
   * @param[out] instancedProperties The instanced properties are added to this container
   */
  void CopyInstancedProperties(RegisteredVisualContainer& visuals, Dictionary<Property::Map>& instancedProperties);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::RegisterVisual()
   */
  void RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::RegisterVisual()
   */
  void RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, int depthIndex);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::RegisterVisual()
   */
  void RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, bool enabled);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::RegisterVisual()
   */
  void RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, bool enabled, int depthIndex);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::UnregisterVisual()
   */
  void UnregisterVisual(Property::Index index);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::GetVisual()
   */
  Toolkit::Visual::Base GetVisual(Property::Index index) const;

  /**
   * @brief Get visual by its name
   * @param[in] name Name of visual
   */
  Toolkit::Visual::Base GetVisual(const std::string& name) const;

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::GetVisualProperty()
   */
  Dali::Property GetVisualProperty(Dali::Property::Index index, Dali::Property::Key visualPropertyKey);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::EnableVisual()
   */
  void EnableVisual(Property::Index index, bool enable);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::IsVisualEnabled()
   */
  bool IsVisualEnabled(Property::Index index) const;

  /**
   * @brief Removes a visual from the control's container.
   * @param[in] visuals The container of visuals
   * @param[in] visualName The name of the visual to remove
   */
  void RemoveVisual(RegisteredVisualContainer& visuals, const std::string& visualName);

  /**
   * @brief Removes several visuals from the control's container.
   * @param[in] visuals The container of visuals
   * @param[in] removeVisuals The visuals to remove
   */
  void RemoveVisuals(RegisteredVisualContainer& visuals, DictionaryKeys& removeVisuals);

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
  void RecreateChangedVisuals(Dictionary<Property::Map>& stateVisualsToChange, Dictionary<Property::Map>& instancedProperties);

  /**
   * @brief Replaces visuals and properties from the old state to the new state.
   * @param[in] oldState The old state
   * @param[in] newState The new state
   * @param[in] subState The current sub state
   */
  void ReplaceStateVisualsAndProperties(const StylePtr oldState, const StylePtr newState, const std::string& subState);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::DoAction()
   */
  void DoAction(Dali::Property::Index visualIndex, Dali::Property::Index actionId, const Dali::Property::Value attributes);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::DoActionExtension()
   */
  void DoActionExtension(Dali::Property::Index visualIndex, Dali::Property::Index actionId, Dali::Any attributes);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::VisualEventSignal()
   */
  DevelControl::VisualEventSignalType& VisualEventSignal();

  /**
   * @brief Any visuals set for replacement but not yet ready should still be registered.
   * Reason: If a request was made to register a new visual but the control removed from scene before visual was ready
   * then when this control appears back on stage it should use that new visual.
   *
   * After all registered visuals are set off scene,
   * visuals pending replacement can be taken out of the removal list and set off scene.
   * Iterate through all replacement visuals and add to a move queue then set off scene.
   *
   * @param[in] parent Parent actor to remove visuals from
   */
  void ClearScene(Actor parent);

  /**
   * @brief Clear visuals.
   */
  void ClearVisuals();

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::ApplyFittingMode()
   */
  void ApplyFittingMode(const Vector2& size);

  /**
   * @brief Stops observing the given visual.
   * @param[in] visual The visual to stop observing
   */
  void StopObservingVisual(Toolkit::Visual::Base& visual);

  /**
   * @brief Starts observing the given visual.
   * @param[in] visual The visual to start observing
   */
  void StartObservingVisual(Toolkit::Visual::Base& visual);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::UpdateVisualProperties()
   */
  void UpdateVisualProperties(const std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>>& properties);

private:
  /**
   * Used as an alternative to boolean so that it is obvious whether a visual is enabled/disabled.
   */
  struct VisualState
  {
    enum Type
    {
      DISABLED = 0, ///< Visual disabled.
      ENABLED  = 1  ///< Visual enabled.
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
      SET     = 1  ///< Visual depth value set by caller.
    };
  };

  /**
   * @brief Adds the visual to the list of registered visuals.
   * @param[in] index The Property index of the visual, used to reference visual
   * @param[in,out] visual The visual to register, which can be altered in this function
   * @param[in] enabled false if derived class wants to control when visual is set on stage
   * @param[in] depthIndexValueSet Set to true if the depthIndex has actually been set manually
   * @param[in] depthIndex The visual's depth-index is set to this. If the depth-index is set to DepthIndex::Ranges::AUTO_INDEX,
   *                       the actual depth-index of visual will be determind automatically (Use previous visuals depth-index, or placed on top of all other visuals.)
   *                       Otherwise, the visual's depth-index is set to clamped value, between DepthIndex::Ranges::MINIMUM_DEPTH_INDEX and DepthIndex::Ranges::MAXIMUM_DEPTH_INDEX.
   *
   * @note Registering a visual with an index that already has a registered visual will replace it. The replacement will
   *       occur once the replacement visual is ready (loaded).
   */
  void RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, VisualState::Type enabled, DepthIndexValue::Type depthIndexValueSet, int depthIndex = static_cast<int>(Toolkit::DepthIndex::AUTO_INDEX));

public:
  RegisteredVisualContainer           mVisuals; ///< Stores visuals needed by the control, non trivial type so std::vector used.
  DevelControl::VisualEventSignalType mVisualEventSignal;
  RegisteredVisualContainer           mRemoveVisuals; ///< List of visuals that are being replaced by another visual once ready
private:
  Control::Impl& mOuter;
};
} // namespace Internal
} // namespace Toolkit
} // namespace Dali
#endif // DALI_TOOLKIT_CONTROL_DATA_VISUAL_DATA_H
