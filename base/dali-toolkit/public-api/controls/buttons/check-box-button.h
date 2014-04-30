#ifndef __DALI_TOOLKIT_CHECK_BOX_BUTTON_H__
#define __DALI_TOOLKIT_CHECK_BOX_BUTTON_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/buttons/button.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

// Forward declarations

namespace Internal DALI_INTERNAL
{
class CheckBoxButton;
}

/**
 * CheckBoxButton provides a check box button which user can check or uncheck.
 *
 * By default a CheckBoxButton emits a Button::ClickedSignal() signal when the button changes its state to checked or unchecked.
 *
 * The button's appearance could be modified by setting images or actors with CheckBoxButton::SetBackgroundImage,
 * CheckBoxButton::SetCheckedImage, CheckBoxButton::SetDimmedBackgroundImage and CheckBoxButton::SetDimmedCheckedImage.
 *
 * When the button is not dimmed, if it's not checked it only shows the \e background image. The \e checked image is shown over the
 * \e background image when the box is checked (\e background image is not replaced by \e checked image).
 *
 * When the button is dimmed, \e background image and \e checked image are replaced by \e dimmed images.
 *
 * CheckBoxButton doesn't have a text. However, a Dali::Toolkit::TableView with a Dali::TextActor or a Dali::Toolkit::TextView
 * and a CheckBoxButton could be used instead.
 */
class CheckBoxButton : public Button
{
public:
  //Action Names
  static const char* const ACTION_CHECK_BOX_BUTTON_CLICK;

  // Properties
  static const std::string USE_FADE_ANIMATION_PROPERTY_NAME;
  static const std::string USE_CHECK_ANIMATION_PROPERTY_NAME;

public:

  /**
   * Create an uninitialized CheckBoxButton; this can be initialized with CheckBoxButton::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  CheckBoxButton();

  /**
   * Copy constructor.
   */
  CheckBoxButton( const CheckBoxButton& checkBox );

  /**
   * Assignment operator.
   */
  CheckBoxButton& operator=( const CheckBoxButton& checkBox );

  /**
   * Virtual destructor.
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~CheckBoxButton();

  /**
   * Create an initialized CheckBoxButton.
   * @return A handle to a newly allocated Dali resource.
   */
  static CheckBoxButton New();

  /**
   * Downcast an Object handle to CheckBoxButton. If handle points to a CheckBoxButton the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a CheckBoxButton or an uninitialized handle
   */
  static CheckBoxButton DownCast( BaseHandle handle );

  /**
   * Sets the button as checked or unchecked.
   *
   * Emits a Button::ClickedSignal() signal if the checkbox is not dimmed and the new state,
   * given in the \e checked param, is different than the previous one.
   *
   * @param[in] checked state.
   */
  void SetChecked( bool checked );

  /**
   * @return \e true if the button is checked.
   */
  bool IsChecked() const;

  /**
   * Sets the background image.
   *
   * @param[in] image The background image.
   */
  void SetBackgroundImage( Image image );

  /**
   * @copydoc SetBackgroundImage( Image image )
   */
  void SetBackgroundImage( Actor image );

  /**
   * Gets the background image.
   * @return An actor with the background image.
   */
  Actor GetBackgroundImage() const;

  /**
   * Sets the checked image.
   *
   * @param[in] image The checked image.
   */
  void SetCheckedImage( Image image );

  /**
   * @copydoc SetCheckedImage( Image image )
   */
  void SetCheckedImage( Actor image );

  /**
   * Gets the checked image.
   * @return An actor with the checked image.
   */
  Actor GetCheckedImage() const;

  /**
   * Sets the dimmed background image.
   *
   * @param[in] image The dimmed background image.
   */
  void SetDimmedBackgroundImage( Image image );

  /**
   * @copydoc SetDimmedBackgroundImage( Image image )
   */
  void SetDimmedBackgroundImage( Actor image );

  /**
   * Gets the dimmed background image.
   * @return An actor with the dimmed background image.
   */
  Actor GetDimmedBackgroundImage() const;

  /**
   * Sets the dimmed checked image.
   *
   * @param[in] image The dimmed checked image.
   */
  void SetDimmedCheckedImage( Image image );

  /**
   * @copydoc SetDimmedCheckedImage( Image image )
   */
  void SetDimmedCheckedImage( Actor image );

  /**
   * Gets the dimmed checked image.
   * @return An actor with the dimmed checked image.
   */
  Actor GetDimmedCheckedImage() const;

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  CheckBoxButton( Internal::CheckBoxButton& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  CheckBoxButton( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_CHECK_BOX_BUTTON_H__
