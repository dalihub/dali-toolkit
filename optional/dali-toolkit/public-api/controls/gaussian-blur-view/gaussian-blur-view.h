#ifndef __DALI_TOOLKIT_GAUSSIAN_BLUR_EFFECT_H__
#define __DALI_TOOLKIT_GAUSSIAN_BLUR_EFFECT_H__

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
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali DALI_IMPORT_API
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
 *
 * GaussianBlurView is a class for applying a render process that blurs an image.
 *
 * Basic idea:-
 *
 * 1) The GaussianBlurView object will render all its child actors offscreen.\n
 * 2) The GaussianBlurView object then blurs the result of step 1), using a two pass separated Gaussian blur.\n
 * 3) The GaussianBlurView object then composites the blur from step 2) with the child actors image from step 1). See GetBlurStrengthPropertyIndex() for more info.\n
 * 4) The GaussianBlurView object gets rendered automatically, either to the screen via the default render task, or via a RenderTask the user has created for
 * e.g. further offscreen rendering.
 *
 * Fundamentally, the GaussianBlurView is simply an Actor in the normal actor tree that affects all of its children. It should be added to your Actor tree and manipulated in the
 * normal ways. It can be considered a 'portal' in the sense that all child actors are clipped to the GaussianBlurView actor bounds.
 *
 * ************\n
 * NB: It is essential to remove the GaussianBlurView from the stage and also to call Deactivate() on it when you are not using it. This will ensure that resources are freed and
 * rendering stops.\n
 * ************\n
 *
 * Usage example:-
 *
 *  // initialise\n
 *  GaussianBlurView gaussianBlurView = GaussianBlurView::New();\n
 *
 *  // create and add some visible actors to the GaussianBlurView, all these child actors will therefore get blurred.\n
 *  Image image = Image::New(...);\n
 *  ImageActor imageActor = ImageActor::New(image);\n
 *  gaussianBlurView.Add(imageActor);\n
 *  ...\n
 *
 *  // Start rendering the GaussianBlurView\n
 *  Stage::GetCurrent().Add(gaussianBlurView);\n
 *  gaussianBlurView.Activate();\n
 *  ...\n
 *
 *  // animate the strength of the blur - this can fade between no blur and full blur. See GetBlurStrengthPropertyIndex().\n
 *  Animation blurAnimation = Animation::New( ... );\n
 *  blurAnimation.AnimateTo( Property( gaussianBlurView, gaussianBlurView.GetBlurStrengthPropertyIndex() ), ... );\n
 *  blurAnimation.Play();\n
 *
 *  ...\n
 *  // Stop rendering the GaussianBlurView\n
 *  Stage::GetCurrent().Remove(gaussianBlurView);\n
 *  gaussianBlurView.Deactivate();\n
 */
class GaussianBlurView : public Control
{
public:
  /**
   * Signal type for notifications
   */
  typedef SignalV2< void (GaussianBlurView source) > GaussianBlurViewSignal;

  /**
   * Create an uninitialized GaussianBlurView; this can be initialized with GaussianBlurView::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  GaussianBlurView();

  /**
   * Copy constructor. Creates another handle that points to the same real object
   */
  GaussianBlurView(const GaussianBlurView& handle);

  /**
   * Assignment operator. Changes this handle to point to another real object
   */
  GaussianBlurView& operator=(const GaussianBlurView& ZoomView);

  /**
   * Virtual destructor.
   */
  virtual ~GaussianBlurView();

  /**
   * Downcast an Object handle to GaussianBlurView. If handle points to a GaussianBlurView the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a GaussianBlurView or an uninitialized handle
   */
  static GaussianBlurView DownCast( BaseHandle handle );

  /**
  * Create an initialized GaussianBlurView, using default settings. The default settings are:-\n
  *
  * numSamples = 5\n
  * blurBellCurveWidth = 1.5\n
  * renderTargetPixelFormat = RGB888\n
  * downsampleWidthScale = 0.5\n
  * downsampleHeightScale = 0.5\n
  * blurUserImage = false
  * @return A handle to a newly allocated Dali resource
  */
  static GaussianBlurView New();

  /**
  * Create an initialized GaussianBlurView.
  * @param numSamples The size of the Gaussian blur kernel (number of samples in horizontal / vertical blur directions).
  * @param blurBellCurveWidth The constant controlling the Gaussian function, must be > 0.0. Controls the width of the bell curve, i.e. the look of the blur and also indirectly
  * the amount of blurriness Smaller numbers for a tighter curve. Useful values in the range [0.5..3.0] - near the bottom of that range the curve is weighted heavily towards
  * the centre pixel of the kernel (so there won't be much blur), near the top of that range the pixels have nearly equal weighting (closely approximating a box filter
  * therefore). Values close to zero result in the bell curve lying almost entirely within a single pixel, in other words there will be basically no blur as neighbouring pixels
  * have close to zero weights.
  * @param renderTargetPixelFormat The pixel format of the render targets we are using to perform the blur.
  * @param downsampleWidthScale The width scale factor applied during the blur process, scaling the size of the source image to the size of the final blurred image output.
  * Useful for downsampling - trades visual quality for processing speed. A value of 1.0f results in no scaling applied.
  * @param downsampleHeightScale The height scale factor applied during the blur process, scaling the size of the source image to the size of the final blurred image output.
  * Useful for downsampling - trades visual quality for processing speed. A value of 1.0f results in no scaling applied.
  * @param blurUserImage If this is set to true, the GaussianBlurView object will operate in a special mode that allows the user to blur an image of their choice. See
  * SetUserImageAndOutputRenderTarget().
  * @return A handle to a newly allocated Dali resource
  */
  static GaussianBlurView New(const unsigned int numSamples, const float blurBellCurveWidth, const Pixel::Format renderTargetPixelFormat,
                              const float downsampleWidthScale, const float downsampleHeightScale,
                              bool blurUserImage = false);

  /**
   * Adds a child Actor to this Actor.
   * NOTE! if the child already has a parent, it will be removed from old parent
   * and reparented to this actor. This may change childs position, color, shader effect,
   * scale etc as it now inherits them from this actor
   * @pre This Actor (the parent) has been initialized.
   * @pre The child actor has been initialized.
   * @pre The child actor is not the same as the parent actor.
   * @pre The actor is not the Root actor
   * @param [in] child The child.
   * @post The child will be referenced by its parent. This means that the child will be kept alive,
   * even if the handle passed into this method is reset or destroyed.
   * @post This may invalidate ActorContainer iterators.
   */
  void Add(Actor child);

  /**
   * Removes a child Actor from this Actor.
   * If the actor was not a child of this actor, this is a no-op.
   * @pre This Actor (the parent) has been initialized.
   * @pre The child actor is not the same as the parent actor.
   * @param [in] child The child.
   * @post This may invalidate ActorContainer iterators.
   */
  void Remove(Actor child);

  /**
   * Start rendering the GaussianBlurView. Must be called after you Add() it to the stage.
   */
  void Activate();

  /**
   * Render the GaussianBlurView once. Must be called after you Add() it to the stage.
   * Only works with a gaussian blur view created with blurUserImage = true.
   * Listen to the Finished signal to determine when the rendering has completed.
   */
  void ActivateOnce();

  /**
   * Stop rendering the GaussianBlurView. Must be called after you Remove() it from the stage.
   */
  void Deactivate();

  /**
   * Sets a custom image to be blurred and a render target to receive the blurred result. If this is called the children of the GaussianBlurObject will not be rendered blurred,
   * instead the inputImage will get blurred.
   * To retrieve the blurred image the user can either pass a handle on a render target they own as the second parameter to SetUserImageAndOutputRenderTarget( ... ), or they
   * can pass NULL for this parameter and instead call GetBlurredRenderTarget() which will return a handle on a render target created internally to the GaussianBlurView object.
   * @pre This object was created with a New( ... ) call where the blurUserImage argument was set to true. If this was not the case an exception will be thrown.
   * @param inputImage The image that the user wishes to blur.
   * @param outputRenderTarget A render target to receive the blurred result. Passing NULL is allowed. See also GetBlurredRenderTarget().
   */
  void SetUserImageAndOutputRenderTarget(Image inputImage, FrameBufferImage outputRenderTarget);

  /**
   * Get the index of the property that can be used to fade the blur in / out. This is the overall strength of the blur.
   * User can use this to animate the blur. A value of 0.0 is zero blur and 1.0 is full blur. Default is 1.0.
   * Note that if you set the blur to 0.0, the result will be no blur BUT the internal rendering will still be happening. If you wish to turn the blur off, you should remove
   * the GaussianBlurView object from the stage also.
   * @return Index of the property that can be used to fade the blur in / out
   */
  Property::Index GetBlurStrengthPropertyIndex() const;

  /**
   * Get the final blurred image.
   * Use can call this function to get the blurred result as an image, to use as they wish. It is not necessary to call this unless you specifically require it.
   * @pre The user must call Activate() before the render target will be returned.
   * @return A handle on the blurred image, contained in a render target.
   */
  FrameBufferImage GetBlurredRenderTarget() const;

  /**
  * Set background color for the view. The background will be filled with this color.
  * @param[in] color The background color.
  */
  void SetBackgroundColor( const Vector4& color );

  /**
  * Get the background color.
  * @return The background color.
  */
  Vector4 GetBackgroundColor() const;

public: // Signals
  /**
   * If ActivateOnce has been called, then connect to this signal to be notified when the
   * target actor has been rendered.
   * @return The Finished signal
   */
  GaussianBlurViewSignal& FinishedSignal();

public:

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The UI Control implementation.
   */
  GaussianBlurView( Internal::GaussianBlurView& implementation );

  /**
   * Allows the creation of this UI Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  GaussianBlurView( Dali::Internal::CustomActor* internal );

private:

};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_GAUSSIAN_BLUR_EFFECT_H__
