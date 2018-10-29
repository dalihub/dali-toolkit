#ifndef DALI_TOOLKIT_INTERNAL_GAUSSIAN_BLUR_VIEW_H
#define DALI_TOOLKIT_INTERNAL_GAUSSIAN_BLUR_VIEW_H

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
#include <sstream>
#include <cmath>
#include <string>

#include <dali/public-api/object/property-map.h>
#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/public-api/render-tasks/render-task.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/devel-api/controls/gaussian-blur-view/gaussian-blur-view.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>

namespace Dali
{

namespace Toolkit
{

class GaussianBlurView;

namespace Internal
{

/**
 * GaussianBlurView implementation class
 */
class GaussianBlurView : public Control
{
public:

  /**
   * @copydoc Dali::Toolkit::GaussianBlurView::GaussianBlurView
   */
  GaussianBlurView();

  /**
   * @copydoc Dali::Toolkit::GaussianBlurView::GaussianBlurView
   */
  GaussianBlurView( const unsigned int numSamples, const float blurBellCurveWidth,
                    const float downsampleWidthScale, const float downsampleHeightScale );

  /**
   * @copydoc Dali::Toolkit::GaussianBlurView::~GaussianBlurView
   */
  virtual ~GaussianBlurView();

  /**
   * @copydoc Dali::Toolkit::GaussianBlurView::New
   */
  static Dali::Toolkit::GaussianBlurView New();

  static Dali::Toolkit::GaussianBlurView New( const unsigned int numSamples, const float blurBellCurveWidth,
                                              const float downsampleWidthScale, const float downsampleHeightScale );

  void Activate();
  void Deactivate();

  Toolkit::ImageView GetBlurredImageView() const;

  /// @copydoc Dali::Toolkit::GaussianBlurView::SetBackgroundColor(const Vector4&)
  void SetBackgroundColor( const Vector4& color );

  /// @copydoc Dali::Toolkit::GaussianBlurView::GetBackgroundColor
  Vector4 GetBackgroundColor() const;

  void SetBlurBellCurveWidth(float blurBellCurveWidth);

  float GetBlurBellCurveWidth();

private:

  enum class Direction
  {
    HORIZONTAL,
    VERTICAL
  };

  void AllocateResources();

  /**
   * Frees up all resources except the final image view.
   * Note, this does reset all locally retained handles to the final image view's texture, including that
   * held by the texture manager.
   */
  void FreeResources();

  /**
   * Separate method to just clean up render tasks, which are also associated with a render task list
   */
  void RemoveRenderTasks();

  /**
   * Separate method to just clean up textures, which are also associated with the TextureManager
   */
  void RemoveTextures();


  virtual void OnInitialize();
  virtual void OnSizeSet(const Vector3& targetSize);

  /**
   * @copydoc Control::OnChildAdd()
   */
  virtual void OnChildAdd( Actor& child );

  /**
   * @copydoc Control::OnChildRemove()
   */
  virtual void OnChildRemove( Actor& child );

  CameraActor CreateCamera( float width, float height );
  Texture CreateTexture( Vector2 size );
  FrameBuffer CreateFramebuffer( Vector2 targetSize, Texture texture );
  RenderTask CreateRenderTask( Actor inputActor, CameraActor cameraActor, FrameBuffer framebuffer );
  Toolkit::ImageView CreateImageView( Vector2 targetSize, Property::Map& customShader,
                                      Direction blurDirection, const std::string& url );



  float CalcGaussianWeight( float x );
  void SetShaderConstants( Actor actor, Vector2 targetSize, Direction direction, uint32_t numSamples );

  std::string GetSampleOffsetsPropertyName( unsigned int index ) const;
  std::string GetSampleWeightsPropertyName( unsigned int index ) const;

private:
  unsigned int mNumSamples;       // number of blur samples in each of horiz/vert directions
  float mBlurBellCurveWidth;      // constant used when calculating the gaussian weights

  float mDownsampleWidthScale; ///< the horizontal scale supplied by the user
  float mDownsampleHeightScale; ///< the vertical scale supplied by the user
  float mDownsampledWidth; ///< Downsampled size after applying scale to blur view's width
  float mDownsampledHeight; ///< Downsampled size after applying scale to blur view's height

  Vector4 mBackgroundColor; ///< The clear color for each render task
  Vector2 mTargetSize; ///< The target size of the scene & blurred image view
  Vector2 mLastSize; ///< The previous target size on allocation/resize

  Actor mChildrenRoot; ///< Holds added children (scene actors)
  Actor mInternalRoot; ///< Holds cameras and internal image views

  CameraActor mRenderFullSizeCamera; ///< For rendering scene and final blurred image
  CameraActor mRenderDownsampledCamera; ///< For rendering horizontal and vertical passes

  FrameBuffer mSceneFramebuffer; ///< Framebuffer is used for rendering the scene and final blurred output
  FrameBuffer mHorizontalBlurFramebuffer; ///< Framebuffer is used for horizontal pass
  FrameBuffer mVerticalBlurFramebuffer; ///< Framebuffer is used for vertical pass

  Texture mSceneOutput; ///< Used for both the scene render and the final render
  Texture mHorizontalOutput;
  Texture mVerticalOutput;
  std::string mSceneOutputUrl; ///< Url registered with Texture Manager for mSceneOutput
  std::string mHorizontalOutputUrl; ///< Url registered with Texture Manager for mHorizontalOutput
  std::string mVerticalOutputUrl; ///< Url registered with Texture Manager for mVerticalOutput

  Toolkit::ImageView mImageViewScene; ///< Holds output of scene pass, input to horizontal pass
  Toolkit::ImageView mImageViewHorizontalBlur; ///< Holds output of horizontal pass, input to vertical pass
  Toolkit::ImageView mImageViewVerticalBlur; ///< Holds output of vertical pass, input to upsample pass
  Toolkit::ImageView mImageViewFinal; ///< Holds final output

  Property::Map mCustomShader; ///< Custom shader for gaussian blur - used for both horizontal & vertical passes

  RenderTask mRenderSceneTask; ///< Task to render scene into first framebuffer
  RenderTask mHorizontalBlurTask; ///< Task to render mSceneOutput texture with horizontal blur with downsampling
  RenderTask mVerticalBlurTask; ///< Task to render mHorizontalOutput texture with vertical blur with downsampling
  RenderTask mUpsampleTask; ///< Task to render mVerticalOutput texture to full size

  /////////////////////////////////////////////////////////////
  // if this is set to true, set the render tasks to refresh once
  bool mRenderOnce:1;
  bool mActivated:1;

private:

  // Undefined copy constructor.
  GaussianBlurView( const GaussianBlurView& );

  // Undefined assignment operator.
  GaussianBlurView& operator=( const GaussianBlurView& );
};

} // namespace Internal


// Helpers for public-api forwarding methods
inline Toolkit::Internal::GaussianBlurView& GetImpl( Toolkit::GaussianBlurView& obj )
{
  DALI_ASSERT_ALWAYS(obj);
  Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<Toolkit::Internal::GaussianBlurView&>(handle);
}

inline const Toolkit::Internal::GaussianBlurView& GetImpl( const Toolkit::GaussianBlurView& obj )
{
  DALI_ASSERT_ALWAYS(obj);
  const Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<const Toolkit::Internal::GaussianBlurView&>(handle);
}


} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_GAUSSIAN_BLUR_VIEW_H
