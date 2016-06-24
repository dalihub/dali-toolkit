#ifndef __DALI_TOOLKIT_CHECK_BOX_BUTTON_H__
#define __DALI_TOOLKIT_CHECK_BOX_BUTTON_H__

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
#include <dali-toolkit/public-api/controls/buttons/button.h>

namespace Dali
{

namespace Toolkit
{

// Forward declarations

namespace Internal DALI_INTERNAL
{
class CheckBoxButton;
}
/**
 * @addtogroup dali_toolkit_controls_buttons
 * @{
 */

/**
 * @brief CheckBoxButton provides a check box button which user can check or uncheck.
 *
 * By default a CheckBoxButton emits a Button::ClickedSignal() signal when the button changes its state to selected or unselected.
 *
 * The button's appearance could be modified by Button::SetUnselectedImage, Button::SetBackgroundImage,
 * Button::SetSelectedImage, Button::SetSelectedBackgroundImage, Button::SetDisabledBackgroundImage,
 * Button::SetDisabledImage, and Button::SetDisabledSelectedImage.
 *
 * When the button is not disabled, if it's not selected it only shows the \e background image. The \e selected image is shown over the
 * \e background image when the box is selected (\e background image is not replaced by \e selected image).
 *
 * When the button is disabled, \e background image and \e selected image are replaced by \e disabled images.
 *
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API CheckBoxButton : public Button
{
public:

  /**
   * @brief Create an uninitialized CheckBoxButton; this can be initialized with CheckBoxButton::New().
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   * @SINCE_1_0.0
   */
  CheckBoxButton();

  /**
   * @brief Copy constructor.
   * @SINCE_1_0.0
   */
  CheckBoxButton( const CheckBoxButton& checkBox );

  /**
   * @brief Assignment operator.
   * @SINCE_1_0.0
   */
  CheckBoxButton& operator=( const CheckBoxButton& checkBox );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~CheckBoxButton();

  /**
   * @brief Create an initialized CheckBoxButton.
   * @SINCE_1_0.0
   * @return A handle to a newly allocated Dali resource.
   */
  static CheckBoxButton New();

  /**
   * @brief Downcast a handle to CheckBoxButton handle.
   *
   * If handle points to a CheckBoxButton the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return handle to a CheckBoxButton or an uninitialized handle
   */
  static CheckBoxButton DownCast( BaseHandle handle );

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   * @SINCE_1_0.0
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL CheckBoxButton( Internal::CheckBoxButton& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   * @SINCE_1_0.0
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  DALI_INTERNAL CheckBoxButton( Dali::Internal::CustomActor* internal );
  /// @endcond
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_CHECK_BOX_BUTTON_H__
