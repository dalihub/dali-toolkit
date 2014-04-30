#ifndef __DALI_TOOLKIT_INTERNAL_CHECK_BOX_BUTTON_H__
#define __DALI_TOOLKIT_INTERNAL_CHECK_BOX_BUTTON_H__

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
#include <dali/public-api/common/dali-vector.h>

#include <dali-toolkit/public-api/controls/buttons/check-box-button.h>

#include "button-impl.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * CheckBoxButton implementation class.
 *
 * \sa Dali::Toolkit::CheckBoxButton
 */
class CheckBoxButton : public Button
{
public:

  /**
   * Create a new CheckBoxButton.
   * @return A smart-pointer to the newly allocated CheckBoxButton.
   */
  static Dali::Toolkit::CheckBoxButton New();

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::SetChecked( bool checked )
   */
  void SetChecked( bool checked );

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::IsChecked()
   */
  bool IsChecked() const;

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::SetBackgroundImage( Image image )
   */
  void SetBackgroundImage( Image image );

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::SetBackgroundImage( Actor image )
   */
  void SetBackgroundImage( Actor image );

  /**
   * Used by the painter only.
   * @return A reference to the background image.
   */
  Actor& GetBackgroundImage();

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::GetBackgroundImage()
   */
  Actor GetBackgroundImage() const;

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::SetCheckedImage( Image image )
   */
  void SetCheckedImage( Image image );

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::SetCheckedImage( Actor image )
   */
  void SetCheckedImage( Actor image );

  /**
   * Used by the painter only.
   * @return A reference to the checked image.
   */
  Actor& GetCheckedImage();

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::GetCheckedImage()
   */
  Actor GetCheckedImage() const;

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::SetDimmedBackgroundImage( Image image )
   */
  void SetDimmedBackgroundImage( Image image );

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::SetDimmedBackgroundImage( Actor image )
   */
  void SetDimmedBackgroundImage( Actor image );

  /**
   * Used by the painter only.
   * @return A reference to the dimmed background image.
   */
  Actor& GetDimmedBackgroundImage();

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::GetDimmedBackgroundImage()
   */
  Actor GetDimmedBackgroundImage() const;

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::SetDimmedCheckedImage( Image image )
   */
  void SetDimmedCheckedImage( Image image );

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::SetDimmedCheckedImage( Actor image )
   */
  void SetDimmedCheckedImage( Actor image );

  /**
   * Used by the painter only.
   * @return A reference to the dimmed checked image.
   */
  Actor& GetDimmedCheckedImage();

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::GetDimmedCheckedImage()
   */
  Actor GetDimmedCheckedImage() const;

  /**
   * Used by the painter only.
   * @return A reference to the background image that is fading out.
   */
  Actor& GetFadeOutBackgroundImage();

  /**
   * Used by the painter only.
   * @return A reference to the checked image that is fading out.
   */
  Actor& GetFadeOutCheckedImage();

protected: // From ControlImpl

  /**
   * Respond the activate notification.
   */
  virtual void OnActivated();

private:

  /**
   * Perform the click action to click the button.
   * @param[in] attributes The attributes to perfrom this action.
   */
  void DoClickAction(const PropertyValueContainer& attributes);

public:
  /**
   * Performs actions as requested using the action name.
   * @param[in] object The object on which to perform the action.
   * @param[in] actionName The action to perform.
   * @param[in] attributes The attributes with which to perfrom this action.
   * @return true if action has been accepted by this control
   */
  static bool DoAction(BaseObject* object, const std::string& actionName, const std::vector<Property::Value>& attributes);

protected: // From Button

  /**
   * Registers properties
   */
  virtual void OnButtonInitialize();

  /**
   * Emits signals and notifies the painter accordingly with the set button
   * properties when the button is released.
   */
  virtual void OnButtonUp();

  /**
   * Sets the push button animation time.
   * @param animationTime The animation time in seconds.
   */
  virtual void OnAnimationTimeSet( float animationTime );

  /**
   * Retrieves the animation time.
   * @return The animation time in seconds.
   */
  virtual float OnAnimationTimeRequested() const;

protected:

  /**
   * Construct a new CheckBoxButton.
   */
  CheckBoxButton();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~CheckBoxButton();

private:

  // Undefined
  CheckBoxButton( const CheckBoxButton& );

  // Undefined
  CheckBoxButton& operator=( const CheckBoxButton& );


private:
  bool  mChecked;                 ///< Stores the check state.

  Actor mBackgroundImage;         ///< Stores the background image.
  Actor mCheckedImage;            ///< Stores the checked image.
  Actor mDimmedCheckedImage;      ///< Stores the dimmed checked image.
  Actor mDimmedBackgroundImage;   ///< Stores the dimmed background image.
  Actor mFadeOutBackgroundImage;  ///< Stores a background image, which is in a fade out animation, to be removed when the animation finishes.
  Actor mFadeOutCheckedImage;     ///< Stores a foreground image, which is in a fade out animation, to be removed when the animation finishes.

  // Actions
  bool mClickActionPerforming;

  // Properties
  Property::Index mUseFadeAnimationProperty;
  Property::Index mUseCheckAnimationProperty;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::CheckBoxButton& GetImplementation( Toolkit::CheckBoxButton& button )
{
  DALI_ASSERT_ALWAYS( button );

  Dali::RefObject& handle = button.GetImplementation();

  return static_cast<Toolkit::Internal::CheckBoxButton&>( handle );
}

inline const Toolkit::Internal::CheckBoxButton& GetImplementation( const Toolkit::CheckBoxButton& button )
{
  DALI_ASSERT_ALWAYS( button );

  const Dali::RefObject& handle = button.GetImplementation();

  return static_cast<const Toolkit::Internal::CheckBoxButton&>( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_CHECK_BOX_BUTTON_H__
