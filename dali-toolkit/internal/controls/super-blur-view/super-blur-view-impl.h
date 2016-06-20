#ifndef __DALI_TOOLKIT_INTERNAL_SUPER_BLUR_VIEW_H__
#define __DALI_TOOLKIT_INTERNAL_SUPER_BLUR_VIEW_H__

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
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/devel-api/controls/super-blur-view/super-blur-view.h>
#include <dali-toolkit/public-api/controls/gaussian-blur-view/gaussian-blur-view.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>

namespace Dali
{

namespace Toolkit
{

class SuperBlurView;

namespace Internal
{

/**
 * SuperBlurView implementation class
 */
class SuperBlurView : public Control
{

public:

  /**
   * @copydoc Dali::Toolkit::SuperBlurView::New
   */
  static Toolkit::SuperBlurView New( unsigned int blurLevels );

  /**
   * @copydoc Dali::Toolkit::SuperBlurView::SetImage
   */
  void SetImage(Image inputImage);

  /**
   * Get the image for blurring.
   * @return The image for blurring.
   */
  Image GetImage();

  /**
   * @copydoc Dali::Toolkit::SuperBlurView::GetBlurStrengthPropertyIndex
   */
  Property::Index GetBlurStrengthPropertyIndex() const;

  /**
   * @copydoc Dali::Toolkit::SuperBlurView::SetBlurStrength
   */
  void SetBlurStrength( float blurStrength );

  /**
   * @copydoc Dali::Toolkit::SuperBlurView::GetCurrentBlurStrength
   */
  float GetCurrentBlurStrength() const;

  /**
   * @copydoc Dali::Toolkit::SuperBlurView::BlurFinishedSignal
   */
  Dali::Toolkit::SuperBlurView::SuperBlurViewSignal& BlurFinishedSignal();

  /**
   * @copydoc Dali::Toolkit::SuperBlurView::GetBlurredImage
   */
  Image GetBlurredImage( unsigned int level );

  // Properties

  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( BaseObject* object, Property::Index propertyIndex, const Property::Value& value );

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty( BaseObject* object, Property::Index propertyIndex );

protected:

  /**
   * Constructor. It initializes the SuperBlurView members
   */
  SuperBlurView( unsigned int blurLevels );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~SuperBlurView();

private: // from Control

  /**
   * @copydoc Toolkit::Control::OnInitialize
   */
  virtual void OnInitialize();

  /**
   * @copydoc CustomActorImpl::OnSizeSet()
   */
  virtual void OnSizeSet(const Vector3& targetSize);

  /**
   * @copydoc CustomActorImpl::OnStageConnection()
   */
  virtual void OnStageConnection( int depth );

  /**
   * @copydoc CustomActorImpl::OnStageDisconnection()
   */
  virtual void OnStageDisconnection();

  /**
   * @copydoc CustomActorImpl::GetNaturalSize()
   */
  virtual Vector3 GetNaturalSize();

private:

  /**
   * Carry out the idx-th pass of blurring
   * @param[in] idx The blur pass index
   * @param[in] image The input image for the current blurring, it is either the original image or the blurred image from the previous pass
   */
  void BlurImage( unsigned int idx, Image image );

  /**
   * Signal handler to tell when the last blur view completes
   * @param[in] blurView The blur view that just completed
   */
  void OnBlurViewFinished( Toolkit::GaussianBlurView blurView );

  /**
   * Clear the resources used to create the blurred image
   */
  void ClearBlurResource();

  /**
   * Sets shader effect on the control renderer
   * @param[in,out] Sets custom shader effect on the given renderer
   */
  void SetShaderEffect( Toolkit::ControlRenderer& renderer );

private:
  std::vector<Toolkit::GaussianBlurView> mGaussianBlurView;
  std::vector<FrameBufferImage>          mBlurredImage;
  std::vector<Toolkit::ControlRenderer>  mRenderers;
  Image                                  mInputImage;
  Vector2                                mTargetSize;

  Toolkit::SuperBlurView::SuperBlurViewSignal mBlurFinishedSignal; ///< Signal emitted when blur has completed.

  Property::Index                        mBlurStrengthPropertyIndex;
  unsigned int                           mBlurLevels;
  bool                                   mResourcesCleared;
};

}

// Helpers for public-api forwarding methods
inline Toolkit::Internal::SuperBlurView& GetImpl( Toolkit::SuperBlurView& obj )
{
  DALI_ASSERT_ALWAYS(obj);
  Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<Toolkit::Internal::SuperBlurView&>(handle);
}

inline const Toolkit::Internal::SuperBlurView& GetImpl( const Toolkit::SuperBlurView& obj )
{
  DALI_ASSERT_ALWAYS(obj);
  const Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<const Toolkit::Internal::SuperBlurView&>(handle);
}

}

}

#endif /* __DALI_TOOLKIT_INTERNALSUPER_BLUR_VIEW_H__ */
