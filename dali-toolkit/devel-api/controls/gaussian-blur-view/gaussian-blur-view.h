#ifndef __DALI_TOOLKIT_GAUSSIAN_BLUR_EFFECT_H__
#define __DALI_TOOLKIT_GAUSSIAN_BLUR_EFFECT_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/images/frame-buffer-image.h>
#include <dali/public-api/render-tasks/render-task.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{

/**
 * GaussianBlurView implementation class
 */
class GaussianBlurView;

/**
 * BloomView implementation class - requires access to private methods
 */
class BloomView;

} // namespace Internal
/**
 * @addtogroup dali_toolkit_controls_gaussian_blur_view
 * @{
 */

/**
 * @brief
 * GaussianBlurView is a class for applying a render process that blurs an image.
 *
 * Basic idea:-
 *
 * 1) The GaussianBlurView object will render all its child actors offscreen.\n
 * 2) The GaussianBlurView object then blurs the result of step 1), using a two pass separated Gaussian blur.\n
 * 3) The GaussianBlurView object then composites the blur from step 2) with the child actors image from step 1).
 *    See GetBlurStrengthPropertyIndex() for more info.\n
 * 4) The GaussianBlurView object gets rendered automatically, either to the screen via the default render task,
 *    or via a RenderTask the user has created for e.g. further offscreen rendering.
 *
 * Fundamentally, the GaussianBlurView is simply an Actor in the normal actor tree that affects all of its
 * children. It should be added to your Actor tree and manipulated in the normal ways. It can be considered a
 * 'portal' in the sense that all child actors are clipped to the GaussianBlurView actor bounds.
 *
 * ************\n
 * NB: It is essential to remove the GaussianBlurView from the stage and also to call Deactivate() on it when
 * you are not using it. This will ensure that resources are freed and rendering stops.\n
 * ************\n
 *
 * Usage example:-
 *
 * @code
 *  // Initialise
 *  GaussianBlurView gaussianBlurView = GaussianBlurView::New();
 *
 *  // Create and add some visible actors to the GaussianBlurView, all these child actors will therefore get blurred.
 *  Image image = Image::New(...);
 *  ImageView imageView = ImageView::New(image);
 *  gaussianBlurView.Add(imageView);
 *  ...
 *
 *  // Start rendering the GaussianBlurView
 *  Stage::GetCurrent().Add(gaussianBlurView);
 *  gaussianBlurView.Activate();
 *  ...
 *
 *  // Animate the strength of the blur - this can fade between no blur and full blur. See GetBlurStrengthPropertyIndex().
 *  Animation blurAnimation = Animation::New( ... );
 *  blurAnimation.AnimateTo( Property( gaussianBlurView, gaussianBlurView.GetBlurStrengthPropertyIndex() ), ... );
 *  blurAnimation.Play();
 *
 *  ...
 *  // Stop rendering the GaussianBlurView
 *  Stage::GetCurrent().Remove(gaussianBlurView);
 *  gaussianBlurView.Deactivate();
 * @endcode
 * @remarks This is an experimental feature and might not be supported in the next release.
 * We do recommend not to use this class.
 */
class DALI_TOOLKIT_API GaussianBlurView : public Control
{
public:
  /**
   * @brief Signal type for notifications
   */
  typedef Signal< void (GaussianBlurView source) > GaussianBlurViewSignal;

  /**
   * @brief Create an uninitialized GaussianBlurView; this can be initialized with GaussianBlurView::New().
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  GaussianBlurView();

  /**
   * @brief Copy constructor. Creates another handle that points to the same real object.
   */
  GaussianBlurView(const GaussianBlurView& handle);

  /**
   * @brief Assignment operator. Changes this handle to point to another real object.
   *
   * @param[in] rhs The handle of the object to point to.
   */
  GaussianBlurView& operator=(const GaussianBlurView& rhs);

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~GaussianBlurView();

  /**
   * @brief Downcast a handle to GaussianBlurView handle.
   *
   * If handle points to a GaussianBlurView the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return A handle to a GaussianBlurView or an uninitialized handle
   */
  static GaussianBlurView DownCast( BaseHandle handle );

  /**
   * @brief Create an initialized GaussianBlurView, using default settings. The default settings are:-\n
   *
   * numSamples = 5\n
   * blurBellCurveWidth = 1.5\n
   * downsampleWidthScale = 0.5\n
   * downsampleHeightScale = 0.5\n
   * @return A handle to a newly allocated Dali resource
   */
  static GaussianBlurView New();

  /**
   * @brief Create an initialized GaussianBlurView.
   * @param numSamples The size of the Gaussian blur kernel (number of samples in horizontal / vertical blur directions).
   * @param blurBellCurveWidth The constant controlling the Gaussian function, must be > 0.0. Controls the width
   * of the bell curve, i.e. the look of the blur and also indirectly the amount of blurriness Smaller numbers
   * for a tighter curve. Useful values in the range [0.5..3.0] - near the bottom of that range the curve is
   * weighted heavily towards the centre pixel of the kernel (so there won't be much blur), near the top of that
   * range the pixels have nearly equal weighting (closely approximating a box filter therefore). Values close
   * to zero result in the bell curve lying almost entirely within a single pixel, in other words there will be
   * basically no blur as neighbouring pixels have close to zero weights.
   * @param downsampleWidthScale The width scale factor applied during the blur process, scaling the size of the
   * source image to the size of the final blurred image output.  Useful for downsampling - trades visual
   * quality for processing speed. A value of 1.0f results in no scaling applied.
   * @param downsampleHeightScale The height scale factor applied during the blur process, scaling the size of
   * the source image to the size of the final blurred image output.  Useful for downsampling - trades visual
   * quality for processing speed. A value of 1.0f results in no scaling applied.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static GaussianBlurView New( const unsigned int numSamples,
                               const float blurBellCurveWidth,
                               const float downsampleWidthScale,
                               const float downsampleHeightScale );

  /**
   * @brief Start rendering the GaussianBlurView. Must be called after you Add() it to the stage.
   */
  void Activate();

  /**
   * @brief Render the GaussianBlurView once only. Must be called after all resources are ready
   * (Listen to Control::ResourceReadySignal() for image views).
   */
  void ActivateOnce();

  /**
   * @brief Stop rendering the GaussianBlurView. Must be called after you Remove() it from the stage.
   */
  void Deactivate();

  /**
   * @brief Get the final blurred image.
   *
   * User can call this function to get the blurred result as an ImageView. It is not necessary
   * to call this unless you specifically require it.
   *
   * @return A handle to the blurred image,
   * @pre The user must call Activate() before the render target will be returned.
   */
  ImageView GetBlurredImageView() const;

  /**
   * @brief Set background color for the view. The background will be filled with this color.
   * @param[in] color The background color.
   */
  void SetBackgroundColor( const Vector4& color );

  /**
   * @brief Get the background color.
   * @return The background color.
   */
  Vector4 GetBackgroundColor() const;

  /**
   * Setting the bell curve width will only work if the blur view is also on-stage and Activated.
   * ( Can use ActivateOnce if already running )
   */
  void SetBlurBellCurveWidth(float blurBellCurveWidth);

  /**
   * Get the bell curve width
   */
  float GetBlurBellCurveWidth();

public:

  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The UI Control implementation.
   */
  DALI_INTERNAL GaussianBlurView( Internal::GaussianBlurView& implementation );

  /**
   * @brief Allows the creation of this UI Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  DALI_INTERNAL GaussianBlurView( Dali::Internal::CustomActor* internal );
  /// @endcond

};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_GAUSSIAN_BLUR_EFFECT_H__
