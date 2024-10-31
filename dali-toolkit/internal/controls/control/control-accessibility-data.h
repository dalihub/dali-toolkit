#ifndef DALI_TOOLKIT_CONTROL_ACCESSIBILITY_DATA_H
#define DALI_TOOLKIT_CONTROL_ACCESSIBILITY_DATA_H

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
   * @copydoc Dali::Toolkit::Internal::Control::Impl::CheckHighlightedObjectGeometry()
   */
  void CheckHighlightedObjectGeometry();

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::RegisterAccessibilityPositionPropertyNotification()
   */
  void RegisterAccessibilityPositionPropertyNotification();

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::UnregisterAccessibilityPositionPropertyNotification()
   */
  void UnregisterAccessibilityPositionPropertyNotification();

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::RegisterAccessibilityPropertySetSignal()
   */
  void RegisterAccessibilityPropertySetSignal();

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::UnregisterAccessibilityPropertySetSignal()
   */
  void UnregisterAccessibilityPropertySetSignal();

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::OnAccessibilityPropertySet()
   */
  void OnAccessibilityPropertySet(Dali::Handle& handle, Dali::Property::Index index, const Dali::Property::Value& value);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::GetAccessibleObject()
   */
  std::shared_ptr<Toolkit::DevelControl::ControlAccessible> GetAccessibleObject();

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::IsAccessibleCreated()
   */
  bool IsAccessibleCreated() const;

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::EnableCreateAccessible()
   */
  void EnableCreateAccessible(bool enable);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::Impl::IsCreateAccessibleEnabled()
   */
  bool IsCreateAccessibleEnabled() const;

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

  Toolkit::DevelControl::AccessibilityActionSignalType mAccessibilityActionSignal;

  struct AccessibilityProps
  {
    std::string                                                                       name{};
    std::string                                                                       description{};
    std::string                                                                       value{};
    std::string                                                                       automationId{};
    int32_t                                                                           role{static_cast<int32_t>(DevelControl::AccessibilityRole::NONE)};
    DevelControl::AccessibilityStates                                                 states{};
    std::map<Dali::Accessibility::RelationType, std::set<Accessibility::Accessible*>> relations;
    Property::Map                                                                     extraAttributes{};
    TriStateProperty                                                                  isHighlightable{TriStateProperty::AUTO};
    bool                                                                              isHidden{false};
    bool                                                                              isScrollable{false};
    bool                                                                              isModal{false};
  } mAccessibilityProps;

private:
  // Accessibility - notification for highlighted object to check if it is showing.
  Dali::PropertyNotification                  mAccessibilityPositionNotification;
  Dali::Accessibility::ScreenRelativeMoveType mAccessibilityLastScreenRelativeMoveType{Accessibility::ScreenRelativeMoveType::OUTSIDE};

  Control& mControlImpl;

  bool mIsAccessibilityPositionPropertyNotificationSet : 1;
  bool mIsAccessibilityPropertySetSignalRegistered : 1;
  bool mAccessibleCreatable : 1;
};
} // namespace Internal
} // namespace Toolkit
} // namespace Dali
#endif // DALI_TOOLKIT_CONTROL_ACCESSIBILITY_DATA_H
