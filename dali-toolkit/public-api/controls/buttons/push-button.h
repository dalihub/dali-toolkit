#ifndef __DALI_TOOLKIT_PUSH_BUTTON_H__
#define __DALI_TOOLKIT_PUSH_BUTTON_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
 * @brief A PushButton changes its appearance when is pressed and returns to its original when is released.
 *
 * By default a PushButton emits a Button::PressedSignal() signal when the button is pressed, a Button::ClickedSignal() signal when it's clicked
 * and a Button::ReleasedSignal() signal when it's released or having pressed it, the touch point leaves the boundary of the button.
 *
 * PushButton provides the following properties which modify signals emitted:
 * <ul>
 *   <li>\e autorepeating
 *
 *       When \e autorepeating is set to \e true, a Button::PressedSignal(), Button::ReleasedSignal() and Button::ClickedSignal() signals are emitted at regular
 *       intervals while the button is touched.
 *
 *       The intervals could be modified with the PushButton::SetInitialAutoRepeatingDelay and PushButton::SetNextAutoRepeatingDelay methods.
 *
 *       A \e togglable button can't be \e autorepeating. If the \e autorepeating property is set to \e true, then the \e togglable property is set to
 *       false but no signal is emitted.
 *
 *   <li>\e togglable
 *
 *       When \e togglable is set to \e true, a Button::StateChangedSignal() signal is emitted, with the selected state, every time the button is touched instead
 *       of emit Button::PressedSignal(), Button::ClickedSignal() and Button::ReleasedSignal() signals.
 *
 *       An \e autorepeating button can't be \e togglable. If the \e togglable property is set to \e true, then the \e autorepeating property is set to false.
 * </ul>
 *
 * The button's appearance could be modified by setting images or actors with PushButton::SetButtonImage, PushButton::SetBackgroundImage,
 * PushButton::SetSelectedImage, PushButton::SetDisabledBackgroundImage and  PushButton::SetDisabledImage or setting a text with
 * PushButton::SetLabel.
 *
 * The \e background is always shown and doesn't change if the button is pressed or released. The \e button image is shown over the \e background image when the
 * button is not pressed and is replaced by the \e selected image when the button is pressed. The text label is placed always on the top of all images.
 *
 * When the button is disabled, \e background, \e button and \e selected images are replaced by their \e disabled images.
 *
 * The methods used to modify the button's appearance could receive Dali::Actor objects as a parameter, so more complex images could be defined.
 *
 * Is not mandatory set all images. A button could be defined only by setting its \e background image or by setting its \e background and \e selected images.
 */
class DALI_IMPORT_API PushButton : public Button
{
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

  /**
   * @brief Sets the button image.
   *
   * @param[in] image The button image.
   */
  void SetButtonImage( Image image );

  /**
   * @brief SetButtonImage
   *
   * @param[in] image The Actor to be used as the button image.
   *
   * The natural size of the button would be the size of this Actor
   * if it's larger than the background and label
   */
  void SetButtonImage( Actor image );

  /**
   * @brief Gets the button image.
   *
   * @return An actor with the button image.
   */
  Actor GetButtonImage() const;

  /**
   * @brief Sets the background image.
   *
   * @param[in] image The background image.
   */
  void SetBackgroundImage( Image image );

  /**
   * @brief SetBackgroundImage
   *
   * @param[in] image The Actor to be used as the background image.
   *
   * The natural size of the button would be the size of this Actor
   * if it's larger than the button and label
   */
  void SetBackgroundImage( Actor image );

  /**
   * @brief Gets the background image.
   *
   * @return An actor with the background image.
   */
  Actor GetBackgroundImage() const;

  /**
   * @brief Sets the selected image.
   *
   * @param[in] image The selected image.
   */
  void SetSelectedImage( Image image );

  /**
   * @copydoc SetSelectedImage( Image image )
   */
  void SetSelectedImage( Actor image );

  /**
   * @brief Gets the selected image.
   *
   * @return An actor with the selected image.
   */
  Actor GetSelectedImage() const;

  /**
   * @brief Sets the selected background image.
   *
   * @param[in] image The selected background image.
   */
  void SetSelectedBackgroundImage( Image image );

  /**
   * @copydoc SetSelectedBackgroundImage( Image image )
   */
  void SetSelectedBackgroundImage( Actor image );

  /**
   * @brief Gets the selected background image.
   *
   * @return An actor with the selected background image.
   */
  Actor GetSelectedBackgroundImage() const;

  /**
   * @brief Sets the disabled background image.
   *
   * @param[in] image The disabled background image.
   */
  void SetDisabledBackgroundImage( Image image );

  /**
   * @copydoc SetDisabledBackgroundImage( Image image )
   */
  void SetDisabledBackgroundImage( Actor image );

  /**
   * @brief Gets the disabled background image.
   *
   * @return An actor with the disabled background image.
   */
  Actor GetDisabledBackgroundImage() const;

  /**
   * @brief Sets the disabled button image.
   *
   * @param[in] image The disabled button image.
   */
  void SetDisabledImage( Image image );

  /**
   * @copydoc SetDisabledImage( Image image )
   */
  void SetDisabledImage( Actor image );

  /**
   * @brief Gets the disabled image.
   *
   * @return An actor with the disabled image.
   */
  Actor GetDisabledImage() const;

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

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_PUSH_BUTTON_H__
