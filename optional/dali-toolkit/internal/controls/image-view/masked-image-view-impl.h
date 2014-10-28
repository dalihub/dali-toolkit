#ifndef __DALI_TOOLKIT_INTERNAL_MASKED_IMAGE_VIEW_H__
#define __DALI_TOOLKIT_INTERNAL_MASKED_IMAGE_VIEW_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/images/frame-buffer-image.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/image-view/masked-image-view.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * @copydoc Dali::Toolkit::MaskedImageView
 */
class MaskedImageView : public Control
{
public:

  typedef Dali::Toolkit::MaskedImageView::ImageRotation ImageRotation;

  /**
   * Create a new MaskedImageView.
   * @return A public handle to the newly allocated MaskedImageView.
   */
  static Dali::Toolkit::MaskedImageView New( unsigned int targetWidth,
                                             unsigned int targetHeight,
                                             Image sourceImage,
                                             Image maskImage );

  /**
   * @copydoc Dali::Toolkit::MaskedImageView::SetSourceImage()
   */
  void SetSourceImage( Image sourceImage );

  /**
   * @copydoc Dali::Toolkit::MaskedImageView::GetSourceImage()
   */
  Image GetSourceImage();

  /**
   * @copydoc Dali::Toolkit::MaskedImageView::SetMaskImage()
   */
  void SetMaskImage( Image maskImage );

  /**
   * @copydoc Dali::Toolkit::MaskedImageView::GetMaskImage()
   */
  Image GetMaskImage();

  /**
   * @copydoc Dali::Toolkit::MaskedImageView::GetPropertyIndex()
   */
  Property::Index GetPropertyIndex( Dali::Toolkit::MaskedImageView::CustomProperty customProperty ) const;

  /**
   * @copydoc Dali::Toolkit::MaskedImageView::Pause()
   */
  void Pause();

  /**
   * @copydoc Dali::Toolkit::MaskedImageView::Resume()
   */
  void Resume();

  /**
   * @copydoc Dali::Toolkit::MaskedImageView::IsPaused()
   */
  bool IsPaused() const;

  /**
   * @copydoc Dali::Toolkit::MaskedImageView::SetEditMode()
   */
  void SetEditMode( Dali::Toolkit::MaskedImageView::EditMode editMode );

  /**
   * @copydoc Dali::Toolkit::MaskedImageView::GetEditMode()
   */
  Dali::Toolkit::MaskedImageView::EditMode GetEditMode() const;

  /**
   * @copydoc Dali::Toolkit::MaskedImageView::SetSourceAspectRatio()
   */
  void SetSourceAspectRatio( float widthOverHeight );

  /**
   * @copydoc Dali::Toolkit::MaskedImageView::GetSourceAspectRatio()
   */
  float GetSourceAspectRatio() const;

  /**
   * @copydoc Dali::Toolkit::MaskedImageView::SetMaximumSourceScale()
   */
  void SetMaximumSourceScale( float scale );

  /**
   * @copydoc Dali::Toolkit::MaskedImageView::GetMaximumSourceScale()
   */
  float GetMaximumSourceScale() const;

  /**
   * @copydoc Dali::Toolkit::MaskedImageView::SetSourceRotation()
   */
  void SetSourceRotation( ImageRotation rotation );

  /**
   * @copydoc Dali::Toolkit::MaskedImageView::GetSourceRotation()
   */
  ImageRotation GetSourceRotation() const;

  /**
   * @copydoc Dali::Toolkit::MaskedImageView::MaskFinishedSignal
   */
  Dali::Toolkit::MaskedImageView::MaskedImageViewSignal& MaskFinishedSignal();

protected:

  /**
   * @copydoc Dali::CustomActorImpl::OnPropertySet()
   */
  void OnPropertySet( Property::Index index, Property::Value propertyValue );

  /**
   * Helper for edit mode.
   */
  void OnPan( Actor source, PanGesture gesture );

  /**
   * Helper for edit mode.
   */
  void OnPinch( Actor actor, PinchGesture pinch );

  /**
   * Construct a new MaskedImageView.
   */
  MaskedImageView();

  /**
   * 2nd-phase initialization.
   */
  void Initialize( unsigned int targetWidth,
                   unsigned int targetHeight,
                   Image sourceImage,
                   Image maskImage );

  /**
   * Helper to apply the desired shader-effect for a given rotation.
   * @param[in] rotation The rotation to apply to the source image.
   */
  void ApplyMaskedImageShader( ImageRotation rotation );

  /**
   * Helper to clamp the source image properties (only in edit mode).
   */
  void ClampSourceSizeAndOffset();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~MaskedImageView();

private:

  // Undefined
  MaskedImageView(const MaskedImageView&);

  // Undefined
  MaskedImageView& operator=(const MaskedImageView& rhs);

  /**
   * Emit MaskFinishedSignal when the render task finished rendering
   * @param[in] renderTask the off-screen render task
   */
  void OnRenderTaskFinished( Dali::RenderTask& renderTask );

private:

  Vector2 mTargetSize;

  Property::Index mCustomProperties[ Dali::Toolkit::MaskedImageView::CUSTOM_PROPERTY_COUNT ];

  // Used for off-screen rendering
  RenderTask mRenderTask;
  ImageActor mSourceImageActor;
  FrameBufferImage mDestinationImage;

  // Create actor to display result of off-screen rendering
  ImageActor mDestinationImageActor;

  // Because ShaderEffect doesn't have a GetEffectImage()
  Image mMaskImage;

  // For edit mode
  Dali::Toolkit::MaskedImageView::EditMode mEditMode;
  PanGestureDetector mPanGestureDetector;
  PinchGestureDetector mPinchDetector;
  bool mSelfPropertySetting;

  struct ImagePosition
  {
    Vector2 mPanOffset;
    Vector2 mStartPinchSize;
    Vector2 mCurrentPinchSize;
  };
  ImagePosition mSourcePosition;
  ImagePosition mMaskPosition;

  ImageRotation mSourceRotation;

  // Limits for edit mode
  float mWidthOverHeight;
  float mMaximumSourceScale;

  Dali::Toolkit::MaskedImageView::MaskedImageViewSignal mMaskFinishedSignal;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::MaskedImageView& GetImpl(Toolkit::MaskedImageView& pub)
{
  DALI_ASSERT_ALWAYS(pub);

  Dali::RefObject& handle = pub.GetImplementation();

  return static_cast<Toolkit::Internal::MaskedImageView&>(handle);
}

inline const Toolkit::Internal::MaskedImageView& GetImpl(const Toolkit::MaskedImageView& pub)
{
  DALI_ASSERT_ALWAYS(pub);

  const Dali::RefObject& handle = pub.GetImplementation();

  return static_cast<const Toolkit::Internal::MaskedImageView&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_MASKED_IMAGE_VIEW_H__
