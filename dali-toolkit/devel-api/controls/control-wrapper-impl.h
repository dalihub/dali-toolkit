#ifndef DALI_TOOLKIT_INTERNAL_CONTROL_WRAPPER_H
#define DALI_TOOLKIT_INTERNAL_CONTROL_WRAPPER_H

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
#include <dali-toolkit/devel-api/controls/control-wrapper.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace Dali
{
namespace Toolkit
{
class TransitionData;

namespace Visual
{
class Base;
}

namespace Internal
{
class ControlWrapper;

typedef IntrusivePtr<ControlWrapper> ControlWrapperPtr;

/**
 * @copydoc Toolkit::ControlWrapper
 */
class DALI_TOOLKIT_API ControlWrapper : public Control
{
public:
  // Flags for the constructor
  enum CustomControlBehaviour
  {
    CONTROL_BEHAVIOUR_DEFAULT            = Control::CONTROL_BEHAVIOUR_DEFAULT,
    DISABLE_SIZE_NEGOTIATION             = CustomActorImpl::DISABLE_SIZE_NEGOTIATION,
    REQUIRES_KEYBOARD_NAVIGATION_SUPPORT = Control::REQUIRES_KEYBOARD_NAVIGATION_SUPPORT,
    DISABLE_STYLE_CHANGE_SIGNALS         = Control::DISABLE_STYLE_CHANGE_SIGNALS,
    DISABLE_VISUALS                      = Control::DISABLE_VISUALS,

    LAST_CONTROL_BEHAVIOUR_FLAG
  };

  static const int CONTROL_BEHAVIOUR_FLAG_COUNT = Log<LAST_CONTROL_BEHAVIOUR_FLAG - 1>::value + 1; ///< Total count of flags

  /**
   * @brief Control constructor
   *
   * @param[in] behaviourFlags Behavioural flags from CustomControlBehaviour enum
   */
  ControlWrapper(CustomControlBehaviour behaviourFlags);

  /**
   * Create a new ControlWrapper.
   *
   * @param[in] typeName The name of the type that is registered with this control
   * @param[in] controlWrapper The implementation of this control
   *
   * @return A public handle to the newly allocated ControlWrapper.
   */
  static Dali::Toolkit::ControlWrapper New(const std::string& typeName, ControlWrapper* controlWrapper);

public: // From CustomActorImpl
  // Size negotiation helpers

  /**
   * @copydoc Dali::CustomActorImpl::RelayoutRequest()
   */
  void RelayoutRequest();

  /**
   * @copydoc Dali::CustomActorImpl::GetHeightForWidthBase()
   */
  float GetHeightForWidthBase(float width);

  /**
   * @copydoc Dali::CustomActorImpl::GetWidthForHeightBase()
   */
  float GetWidthForHeightBase(float height);

  /**
   * @copydoc Dali::CustomActorImpl::CalculateChildSizeBase()
   */
  float CalculateChildSizeBase(const Dali::Actor& child, Dimension::Type dimension);

  /**
   * @copydoc Dali::CustomActorImpl::RelayoutDependentOnChildrenBase()
   */
  bool RelayoutDependentOnChildrenBase(Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

public: // From Control
  /**
   * @ref Dali::Toolkit::DevelControl::RegisterVisual()
   */
  void RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual);

  /**
   * @ref Dali::Toolkit::DevelControl::RegisterVisual()
   */
  void RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, int depthIndex);

  /**
   * @ref Dali::Toolkit::DevelControl::RegisterVisual()
   */
  void RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, bool enabled);

  /**
   * @ref Dali::Toolkit::DevelControl::RegisterVisual()
   */
  void RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, bool enabled, int depthIndex);

  /**
   * @ref Dali::Toolkit::DevelControl::UnregisterVisual()
   */
  void UnregisterVisual(Property::Index index);

  /**
   * @ref Dali::Toolkit::DevelControl::GetVisual()
   */
  Toolkit::Visual::Base GetVisual(Property::Index index) const;

  /**
   * @ref Dali::Toolkit::DevelControl::EnableVisual()
   */
  void EnableVisual(Property::Index index, bool enable);

  /**
   * @ref Dali::Toolkit::DevelControl::IsVisualEnabled()
   */
  bool IsVisualEnabled(Property::Index index) const;

  /**
   * @ref Dali::Toolkit::DevelControl::CreateTransition()
   */
  Dali::Animation CreateTransition(const Toolkit::TransitionData& transitionData);

  /**
   * @copydoc Dali::Toolkit::Internal::Control::EmitKeyInputFocusSignal()
   */
  void EmitKeyInputFocusSignal(bool focusGained);

  /**
   * @brief Apply the current style
   *
   * This method is called after the Control has been initialized.
   *
   */
  void ApplyThemeStyle();

public:
  /**
   * Enable access to non-native type info from native side
   * @return The type info that was registered on this type
   */
  Dali::TypeInfo GetTypeInfo();

protected:
  /**
   * Protected Destructor
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ControlWrapper();

private:
  /// @cond internal
  /// Undefined.
  DALI_INTERNAL ControlWrapper(const ControlWrapper&);

  /// Undefined.
  DALI_INTERNAL ControlWrapper& operator=(const ControlWrapper& rhs);
  /// @endcond
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::ControlWrapper& GetControlWrapperImpl(Toolkit::ControlWrapper& publicObject)
{
  DALI_ASSERT_ALWAYS(publicObject);

  Dali::RefObject& handle = publicObject.GetImplementation();

  return static_cast<Toolkit::Internal::ControlWrapper&>(handle);
}

inline const Toolkit::Internal::ControlWrapper& GetControlWrapperImpl(const Toolkit::ControlWrapper& publicObject)
{
  DALI_ASSERT_ALWAYS(publicObject);

  const Dali::RefObject& handle = publicObject.GetImplementation();

  return static_cast<const Toolkit::Internal::ControlWrapper&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_CONTROL_WRAPPER_H
