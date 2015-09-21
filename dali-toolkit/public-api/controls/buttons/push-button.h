#ifndef __DALI_TOOLKIT_PUSH_BUTTON_H__
#define __DALI_TOOLKIT_PUSH_BUTTON_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include "button.h"

namespace Dali
{

namespace Toolkit
{

// Forward declarations

namespace Internal DALI_INTERNAL
{
// Forward declarations

class PushButton;
}
/**
 * @addtogroup dali-toolkit-controls-buttons
 * @{
 */

/**
 * @brief A PushButton changes its appearance when is pressed and returns to its original when is released.
 *
 * By default a PushButton emits a Button::PressedSignal() signal when the button is pressed, a Button::ClickedSignal() signal when it's clicked
 * and a Button::ReleasedSignal() signal when it's released or having pressed it, the touch point leaves the boundary of the button.
 *
 * See Button for more detail on signals and modifying appearance via properties.
 */
class DALI_IMPORT_API PushButton : public Button
{
public:

  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Button::PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserving 1000 property indices
  };

  /**
   * @brief An enumeration of properties belonging to the PushButton class.
   */
  struct Property
  {
    enum
    {
      UNSELECTED_ICON = PROPERTY_START_INDEX, ///< Property, name "unselected-icon", type std::string
      SELECTED_ICON,                          ///< Property, name "selected-icon",   type std::string
      ICON_ALIGNMENT,                         ///< Property, name "icon-alignment",  type std::string
      LABEL_PADDING,                          ///< Property, name "label-padding",   type Vector4
      ICON_PADDING,                           ///< Property, name "icon-padding",    type Vector4
    };
  };

public:

  /**
   * @brief Create an uninitialized PushButton; this can be initialized with PushButton::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  PushButton();

  /**
   * @brief Copy constructor.
   */
  PushButton( const PushButton& pushButton );

  /**
   * @brief Assignment operator.
   */
  PushButton& operator=( const PushButton& pushButton );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~PushButton();

  /**
   * @brief Create an initialized PushButton.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static PushButton New();

  /**
   * @brief Downcast an Object handle to PushButton.
   *
   * If handle points to a PushButton the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return handle to a PushButton or an uninitialized handle
   */
  static PushButton DownCast( BaseHandle handle );


  // Deprecated API

  using Button::SetButtonImage;

  /**
   * @brief Sets the unselected image with an Actor.
   *
   * @deprecated DALi 1.0.50
   *
   * @param[in] image The Actor to use.
   */
  void SetButtonImage( Actor image );

  using Button::SetBackgroundImage;

  /**
   * @brief Sets the background image with an Actor.
   *
   * @deprecated DALi 1.0.50
   *
   * @param[in] image The Actor to use.
   */
  void SetBackgroundImage( Actor image );

  using Button::SetSelectedImage;

  /**
   * @brief Sets the selected image with an Actor.
   *
   * @deprecated DALi 1.0.50
   *
   * @param[in] image The Actor to use.
   */
  void SetSelectedImage( Actor image );

  using Button::SetSelectedBackgroundImage;

  /**
   * @brief Sets the selected background image with an Actor.
   *
   * @deprecated DALi 1.0.50
   *
   * @param[in] image The Actor to use.
   */
  void SetSelectedBackgroundImage( Actor image );

  using Button::SetDisabledBackgroundImage;

  /**
   * @brief Sets the disabled background image with an Actor.
   *
   * @deprecated DALi 1.0.50
   *
   * @param[in] image The Actor to use.
   */
  void SetDisabledBackgroundImage( Actor image );

  using Button::SetDisabledImage;

  /**
   * @brief Sets the disabled image with an Actor.
   *
   * @deprecated DALi 1.0.50
   *
   * @param[in] image The Actor to use.
   */
  void SetDisabledImage( Actor image );

  using Button::SetDisabledSelectedImage;

  /**
   * @brief Sets the disabled selected image with an Actor.
   *
   * @deprecated DALi 1.0.50
   *
   * @param[in] image The Actor to use.
   */
  void SetDisabledSelectedImage( Actor image );


public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL PushButton( Internal::PushButton& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  DALI_INTERNAL PushButton( Dali::Internal::CustomActor* internal );
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_PUSH_BUTTON_H__
