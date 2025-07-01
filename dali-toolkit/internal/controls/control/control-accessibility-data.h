#ifndef DALI_TOOLKIT_CONTROL_ACCESSIBILITY_DATA_H
#define DALI_TOOLKIT_CONTROL_ACCESSIBILITY_DATA_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
// private inner class
class Control::Impl::AccessibilityData : public ConnectionTracker
{
  friend class Toolkit::DevelControl::ControlAccessible;

public:
  // Constructor
  AccessibilityData(Control& controlImpl);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::AppendAccessibilityAttribute()
   */
  void AppendAccessibilityAttribute(const std::string& key, const std::string value);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::RemoveAccessibilityAttribute()
   */
  void RemoveAccessibilityAttribute(const std::string& key);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::ClearAccessibilityAttributes()
   */
  void ClearAccessibilityAttributes();

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::SetAccessibilityReadingInfoType()
   */
  void SetAccessibilityReadingInfoType(const Dali::Accessibility::ReadingInfoTypes types);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::GetAccessibilityReadingInfoType()
   */
  Dali::Accessibility::ReadingInfoTypes GetAccessibilityReadingInfoType() const;

  /**
   * @brief Checks highlighted object geometry if it is showing or not
   */
  void CheckHighlightedObjectGeometry();

  /**
   * @brief Register property notification to check highlighted object position
   */
  void RegisterAccessibilityPositionPropertyNotification();

  /**
   * @brief Remove property notification added by RegisterAccessibilityPositionPropertyNotification
   */
  void UnregisterAccessibilityPositionPropertyNotification();

  /**
   * @brief Register PropertySet signal to check highlighted object name and description
   */
  void RegisterAccessibilityPropertySetSignal();

  /**
   * @brief Remove PropertySet signal added by RegisterAccessibilityPropertySetSignal
   */
  void UnregisterAccessibilityPropertySetSignal();

  /**
   * @brief Signal callback of PropertySet when this object is become highlighted, so RegisterAccessibilityPropertySetSignal called.
   *
   * @param[in] handle Handle of the control.
   * @param[in] index The index of property.
   * @param[in] value The value of property.
   */
  void OnAccessibilityPropertySet(Dali::Handle& handle, Dali::Property::Index index, const Dali::Property::Value& value);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::GetAccessibleObject()
   */
  std::shared_ptr<Toolkit::DevelControl::ControlAccessible> GetAccessibleObject();

public:
  /**
   * @brief Helper function to get default reading info type attributes
   */
  static Dali::Accessibility::ReadingInfoTypes GetDefaultReadingInfoTypes();

  /**
   * @brief Helper function to get control's default state attributes
   */
  static Toolkit::DevelControl::AccessibilityStates GetDefaultControlAccessibilityStates();

public:
  Toolkit::DevelControl::AccessibilityActivateSignalType         mAccessibilityActivateSignal;
  Toolkit::DevelControl::AccessibilityReadingSkippedSignalType   mAccessibilityReadingSkippedSignal;
  Toolkit::DevelControl::AccessibilityReadingPausedSignalType    mAccessibilityReadingPausedSignal;
  Toolkit::DevelControl::AccessibilityReadingResumedSignalType   mAccessibilityReadingResumedSignal;
  Toolkit::DevelControl::AccessibilityReadingCancelledSignalType mAccessibilityReadingCancelledSignal;
  Toolkit::DevelControl::AccessibilityReadingStoppedSignalType   mAccessibilityReadingStoppedSignal;

  Toolkit::DevelControl::AccessibilityGetNameSignalType        mAccessibilityGetNameSignal;
  Toolkit::DevelControl::AccessibilityGetDescriptionSignalType mAccessibilityGetDescriptionSignal;
  Toolkit::DevelControl::AccessibilityDoGestureSignalType      mAccessibilityDoGestureSignal;

  Toolkit::DevelControl::AccessibilityActionSignalType      mAccessibilityActionSignal;
  Toolkit::DevelControl::AccessibilityHighlightedSignalType mAccessibilityHighlightedSignal;

  struct AccessibilityProps
  {
    AccessibilityProps()
    : isHighlightable(TriStateProperty::AUTO),
      isHidden(false),
      isScrollable(false),
      isModal(false)
    {
    }

    AccessibilityProps(const AccessibilityProps&) = default;
    AccessibilityProps(AccessibilityProps&&)      = default;

    std::string name{};
    std::string description{};
    std::string value{};
    std::string automationId{};

    DevelControl::AccessibilityStates                                                 states{};
    std::map<Dali::Accessibility::RelationType, std::set<Accessibility::Accessible*>> relations{};
    Property::Map                                                                     extraAttributes{};

    TriStateProperty isHighlightable : 3;
    bool             isHidden : 1;
    bool             isScrollable : 1;
    bool             isModal : 1;
  } mAccessibilityProps;

private:
  // Accessibility - notification for highlighted object to check if it is showing.
  Dali::PropertyNotification                  mAccessibilityPositionNotification;
  Dali::Accessibility::ScreenRelativeMoveType mAccessibilityLastScreenRelativeMoveType{Accessibility::ScreenRelativeMoveType::OUTSIDE};

  Control& mControlImpl;

  bool mIsAccessibilityPositionPropertyNotificationSet : 1;
  bool mIsAccessibilityPropertySetSignalRegistered : 1;
};
} // namespace Internal
} // namespace Toolkit
} // namespace Dali
#endif // DALI_TOOLKIT_CONTROL_ACCESSIBILITY_DATA_H
