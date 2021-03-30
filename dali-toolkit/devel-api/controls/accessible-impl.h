#ifndef DALI_TOOLKIT_ACCESSIBLE_IMPL_H
#define DALI_TOOLKIT_ACCESSIBLE_IMPL_H
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
// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/accessibility-impl.h>
#include <dali/devel-api/adaptor-framework/accessibility.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali::Toolkit::DevelControl {

/**
 * @brief Represents the Accessible object for Dali::Toolkit::Control and derived classes
 *
 * You can create a derived class (and register it using SetAccessibilityConstructor)
 * in order to customize Accessibility for a given control.
 *
 * @see Dali::Toolkit::DevelControl::SetAccessibilityConstructor
 * @see Dali::Accessibility::Accessible
 * @see Dali::Accessibility::Component
 * @see Dali::Accessibility::Collection
 * @see Dali::Accessibility::Action
 * @see Dali::Accessibility::Value
 * @see Dali::Accessibility::Text
 * @see Dali::Accessibility::EditableText
 */
struct DALI_TOOLKIT_API AccessibleImpl : public virtual Dali::Accessibility::Accessible,
                                         public virtual Dali::Accessibility::Component,
                                         public virtual Dali::Accessibility::Collection,
                                         public virtual Dali::Accessibility::Action
{
  Dali::Actor self;
  Dali::WeakHandle<Dali::Actor> currentHighlightActor;
  bool        modal = false, root = false;

  AccessibleImpl(Dali::Actor self, Dali::Accessibility::Role role, bool modal = false);

  /**
   * @copydoc Dali::Accessibility::Accessible::GetName()
   */
  std::string GetName() override;

  /**
   * @brief Returns the actor's name in the absence of ACCESSIBILITY_NAME property
   */
  virtual std::string GetNameRaw();

  /**
   * @copydoc Dali::Accessibility::Accessible::GetDescription()
   */
  std::string GetDescription() override;

  /**
   * @brief Returns the actor's description in the absence of ACCESSIBILITY_DESCRIPTION property
   */
  virtual std::string GetDescriptionRaw();

  /**
   * @copydoc Dali::Accessibility::Accessible::GetParent()
   */
  Dali::Accessibility::Accessible* GetParent() override;

  /**
   * @copydoc Dali::Accessibility::Accessible::GetChildCount()
   */
  size_t GetChildCount() override;

  /**
   * @copydoc Dali::Accessibility::Accessible::GetChildAtIndex()
   */
  Dali::Accessibility::Accessible* GetChildAtIndex(size_t index) override;

  /**
   * @copydoc Dali::Accessibility::Accessible::GetIndexInParent()
   */
  size_t GetIndexInParent() override;

  /**
   * @copydoc Dali::Accessibility::Accessible::GetRole()
   */
  Dali::Accessibility::Role GetRole() override;

  /**
   * @copydoc Dali::Accessibility::Accessible::GetStates()
   */
  Dali::Accessibility::States GetStates() override;

  /**
   * @copydoc Dali::Accessibility::Accessible::GetAttributes()
   */
  Dali::Accessibility::Attributes GetAttributes() override;

  /**
   * @copydoc Dali::Accessibility::Component::GetExtents()
   */
  Dali::Rect<> GetExtents(Dali::Accessibility::CoordType ctype) override;

  /**
   * @copydoc Dali::Accessibility::Component::GetLayer()
   */
  Dali::Accessibility::ComponentLayer GetLayer() override;

  /**
   * @copydoc Dali::Accessibility::Component::GetMdiZOrder()
   */
  int16_t GetMdiZOrder() override;

  /**
   * @copydoc Dali::Accessibility::Component::GrabFocus()
   */
  bool GrabFocus() override;

  /**
   * @copydoc Dali::Accessibility::Component::GetAlpha()
   */
  double GetAlpha() override;

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
  std::string GetActionName(size_t index) override;

  /**
   * @copydoc Dali::Accessibility::Action::GetLocalizedActionName()
   */
  std::string GetLocalizedActionName(size_t index) override;

  /**
   * @copydoc Dali::Accessibility::Action::GetActionDescription()
   */
  std::string GetActionDescription(size_t index) override;

  /**
   * @copydoc Dali::Accessibility::Action::GetActionCount()
   */
  size_t GetActionCount() override;

  /**
   * @copydoc Dali::Accessibility::Action::GetActionKeyBinding()
   */
  std::string GetActionKeyBinding(size_t index) override;

  /**
   * @copydoc Dali::Accessibility::Action::DoAction(size_t)
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
   * @brief Makes sure that a given child of this container (e.g. ItemView) is visible
   */
  virtual void EnsureChildVisible(Actor child);

  /**
   * @brief Makes sure this actor is visible (when moving the highlight frame to an
   * actor that is scrolled out of the viewport)
   */
  virtual void EnsureSelfVisible();

  /**
   * @brief Returns the index of the property that represents this actor's name
   */
  virtual Dali::Property::Index GetNamePropertyIndex();

  /**
   * @brief Returns the index of the property that represents this actor's description
   */
  virtual Dali::Property::Index GetDescriptionPropertyIndex();
};

} // namespace Dali::Toolkit::DevelControl

#endif // DALI_TOOLKIT_ACCESSIBLE_IMPL_H
