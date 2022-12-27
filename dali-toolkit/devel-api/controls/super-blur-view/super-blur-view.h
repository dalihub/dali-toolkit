#ifndef DALI_TOOLKIT_SUPER_BLUR_VIEW_H
#define DALI_TOOLKIT_SUPER_BLUR_VIEW_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal DALI_INTERNAL
{
class SuperBlurView;
}

/**
 * @brief SuperBlurView accepts an image as input, and displays/animates it with various blur strength.
 * Usage example:-
 *
 *  // initialise\n
 *  SuperBlurView blurView = SuperBlurView::New( blurLevels );\n
 *  blurView.SetProperty( Actor::Property::SIZE, size );  // it is important to set the display size before set the input image!!
 *  Stage::GetCurrent().Add(blurView);\n
 *
 *  // Set the input image
 *  blurView.SetProperty( SuperBlurView::Property::IMAGE_URL, url );\n
 *
 *  // animate the strength of the blur - this can fade between no blur and full blur. .\n
 *  Animation blurAnimation = Animation::New( ... );\n
 *  blurAnimation.AnimateTo( Property( blurView, blurView.GetBlurStrengthPropertyIndex() ), ... );\n
 *  blurAnimation.Play();\n
 */
class DALI_TOOLKIT_API SuperBlurView : public Control
{
public:
  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000 ///< Reserve property indices
  };

  /**
   * @brief An enumeration of properties belonging to the SuperBlurView class.
   */
  struct Property
  {
    enum
    {
      IMAGE_URL = PROPERTY_START_INDEX, ///< name "imageUrl",    @see SetTexture,    type String
    };
  };

  /**
   * @brief Signal type for notifications.
   */
  typedef Signal<void(SuperBlurView source)> SuperBlurViewSignal;

  /**
   * @brief Creates an empty SuperBlurView handle.
   */
  SuperBlurView();

  /**
   * @brief Create an initialized SuperBlurView.
   *
   * @param[in] blurLevels The final blur strength level. It decides how many filtering passes are used to create the group of blurred textures.
   * @return A handle to a newly allocated Dali resource
   */
  static SuperBlurView New(unsigned int blurLevels);

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @param[in] handle the handle to copy from
   */
  SuperBlurView(const SuperBlurView& handle);

  /**
   * @brief Assignment operator.
   *
   * Changes this handle to point to another real object.
   * @param[in] rhs the handle to copy from
   * @return a reference to this
   */
  SuperBlurView& operator=(const SuperBlurView& rhs);

  /**
   * @brief Move constructor.
   *
   * Creates another handle that points to the same real object.
   * @param[in] handle the handle to move from
   */
  SuperBlurView(SuperBlurView&& handle);

  /**
   * @brief Move assignment operator.
   *
   * Changes this handle to point to another real object.
   * @param[in] rhs the handle to move from
   * @return a reference to this
   */
  SuperBlurView& operator=(SuperBlurView&& rhs);

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~SuperBlurView();

  /**
   * @brief Downcast an Object handle to SuperBlurView.
   *
   * If handle points to a SuperBlurView, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a SuperBlurView or an uninitialized handle
   */
  static SuperBlurView DownCast(BaseHandle handle);

  /**
   * @brief Sets a custom texture to be blurred.
   *
   * @param[in] texture The texture that the user wishes to blur
   */
  void SetTexture(Texture texture);

  /**
   * @brief Get the index of the property that can be used to fade the blur in / out.
   *
   * This is the overall strength of the blur.
   * User can use this to animate the blur. A value of 0.0 is zero blur and 1.0 is full blur. Default is 0.0.
   * @return Index of the property that can be used to fade the blur in / out
   */
  Dali::Property::Index GetBlurStrengthPropertyIndex() const;

  /**
   * @brief Set the blur strength to display the texture.
   *
   * @param[in] blurStrength The blur strength used to display the texture.
   */
  void SetBlurStrength(float blurStrength);

  /**
   * @brief Get the current blur strength.
   *
   * @return The current blur strength
   */
  float GetCurrentBlurStrength() const;

  /**
   * @brief Connect to this signal to be notified when the all the blurs have completed.
   *
   * @return The BlurFinished signal
   */
  SuperBlurViewSignal& BlurFinishedSignal();

  /**
   * @brief Get the blurred texture.
   *
   * Should wait for the BlurFinishedSignal before calling this method.
   * @param[in] level Indicate which blurred texture to get, must be a value between 1 and  blurLevels
   * @return The level-th blurred texture
   */
  Texture GetBlurredTexture(unsigned int level);

public: // Not intended for application developers
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL SuperBlurView(Internal::SuperBlurView& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL SuperBlurView(Dali::Internal::CustomActor* internal);
};

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_SUPER_BLUR_VIEW_H */
