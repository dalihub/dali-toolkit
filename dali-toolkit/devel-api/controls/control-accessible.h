#ifndef DALI_TOOLKIT_CONTROL_ACCESSIBLE_H
#define DALI_TOOLKIT_CONTROL_ACCESSIBLE_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/accessibility-bridge.h>
#include <dali/devel-api/adaptor-framework/accessibility.h>
#include <dali/devel-api/adaptor-framework/actor-accessible.h>
#include <dali/devel-api/atspi-interfaces/action.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali::Toolkit::DevelControl
{
/**
 * @brief Represents the Accessible object for Dali::Toolkit::Control and derived classes
 *
 * You can create a derived class (and override Control::CreateAccessibleObject)
 * in order to customize Accessibility for a given control.
 *
 * @see Dali::Accessibility::Accessible
 * @see Dali::Accessibility::Component
 * @see Dali::Accessibility::Collection
 * @see Dali::Accessibility::Action
 * @see Dali::Accessibility::Value
 * @see Dali::Accessibility::Text
 * @see Dali::Accessibility::EditableText
 */
struct DALI_TOOLKIT_API ControlAccessible : public Dali::Accessibility::ActorAccessible,
                                            public virtual Dali::Accessibility::Action
{
protected:
  Vector2                       mLastPosition{0.0f, 0.0f};
  Dali::WeakHandle<Dali::Actor> mCurrentHighlightActor;

  void ScrollToSelf();

  /**
   * @brief Register property notification to check highlighted object position
   */
  void RegisterPositionPropertyNotification();

  /**
   * @brief Remove property notification added by RegisterPropertyNotification
   */
  void UnregisterPositionPropertyNotification();

  /**
   * @brief Registers PropertySet signal to notify when ACCESSIBILITY_NAME or ACCESSIBILITY_DESCRIPTION is changed.
   * Note that those two signals only need for highlighted control. So, let us ensure to connect PropertySet signal
   * only if control has been grabbed.
   */
  void RegisterPropertySetSignal();

  /**
   * @brief Unregisters PropertySet signal to notify when ACCESSIBILITY_NAME or ACCESSIBILITY_DESCRIPTION is changed.
   */
  void UnregisterPropertySetSignal();

  /**
   * @brief Check if the actor is showing
   * @return True if the actor is showing
   */
  bool IsShowing();

public:
  ControlAccessible(Dali::Actor self);

  /**
   * @copydoc Dali::Accessibility::Accessible::GetName()
   */
  std::string GetName() const override;

  /**
   * @brief Returns the actor's name in the absence of ACCESSIBILITY_NAME property
   */
  virtual std::string GetNameRaw() const;

  /**
   * @copydoc Dali::Accessibility::Accessible::GetDescription()
   */
  std::string GetDescription() const override;

  /**
   * @brief Returns the actor's description in the absence of ACCESSIBILITY_DESCRIPTION property
   */
  virtual std::string GetDescriptionRaw() const;

  /**
   * @copydoc Dali::Accessibility::Accessible::GetRole()
   */
  Dali::Accessibility::Role GetRole() const override;

  /**
   * @copydoc Dali::Accessibility::Accessible::GetLocalizedRoleName()
   */
  std::string GetLocalizedRoleName() const override;

  /**
   * @copydoc Dali::Accessibility::Accessible::GetStates()
   */
  Dali::Accessibility::States GetStates() override;

  /**
   * @copydoc Dali::Accessibility::Accessible::GetAttributes()
   */
  Dali::Accessibility::Attributes GetAttributes() const override;

  /**
   * @copydoc Dali::Accessibility::Accessible::IsHidden()
   */
  bool IsHidden() const override;

  /**
   * @copydoc Dali::Accessibility::Component::GrabFocus()
   */
  bool GrabFocus() override;

  /**
   * @copydoc Dali::Accessibility::Component::GrabHighlight()
   */
  bool GrabHighlight() override;

  /**
   * @copydoc Dali::Accessibility::Component::ClearHighlight()
   */
  bool ClearHighlight() override;

  /**
   * @copydoc Dali::Accessibility::Action::GetActionName()
   */
  std::string GetActionName(size_t index) const override;

  /**
   * @copydoc Dali::Accessibility::Action::GetLocalizedActionName()
   */
  std::string GetLocalizedActionName(size_t index) const override;

  /**
   * @copydoc Dali::Accessibility::Action::GetActionDescription()
   */
  std::string GetActionDescription(size_t index) const override;

  /**
   * @copydoc Dali::Accessibility::Action::GetActionCount()
   */
  size_t GetActionCount() const override;

  /**
   * @copydoc Dali::Accessibility::Action::GetActionKeyBinding()
   */
  std::string GetActionKeyBinding(size_t index) const override;

  /**
   * @copydoc Dali::Accessibility::Action::DoAction(std::size_t)
   */
  bool DoAction(size_t index) override;

  /**
   * @copydoc Dali::Accessibility::Action::DoAction(const std::string&)
   */
  bool DoAction(const std::string& name) override;

  /**
   * @copydoc Dali::Accessibility::Accessible::DoGesture()
   */
  bool DoGesture(const Dali::Accessibility::GestureInfo& gestureInfo) override;

  /**
   * @copydoc Dali::Accessibility::Accessible::GetRelationSet()
   */
  std::vector<Dali::Accessibility::Relation> GetRelationSet() override;

  /**
   * @copydoc Dali::Accessibility::Accessible::GetStates()
   */
  virtual Dali::Accessibility::States CalculateStates();

  /**
   * @brief Makes sure that a given child (descendant) of this container (e.g. ItemView) is visible
   * @return false if scrolling is not supported or child is already visible
   */
  virtual bool ScrollToChild(Actor child);

  /**
   * @brief Returns the index of the property that represents this actor's name
   */
  virtual Dali::Property::Index GetNamePropertyIndex();

  /**
   * @brief Returns the index of the property that represents this actor's description
   */
  virtual Dali::Property::Index GetDescriptionPropertyIndex();

  /**
   * @brief Sets last object position
   * @param[in] position Last object position
   */
  void SetLastPosition(Vector2 position);

  /**
   * @brief Gets last object position
   * @return The Last object position
   */
  Vector2 GetLastPosition() const;
};

} // namespace Dali::Toolkit::DevelControl

#endif // DALI_TOOLKIT_CONTROL_ACCESSIBLE_H
