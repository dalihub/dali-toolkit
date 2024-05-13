#ifndef DALI_TOOLKIT_INTERNAL_ANIMATED_IMAGE_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_ANIMATED_IMAGE_VISUAL_H

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
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/devel-api/adaptor-framework/animated-image-loading.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visuals/animated-image-visual-actions-devel.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/internal/visuals/animated-image/image-cache.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-url.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class ImageVisualShaderFactory;
class AnimatedImageVisual;
typedef IntrusivePtr<AnimatedImageVisual> AnimatedImageVisualPtr;

/**
 * The visual which renders an animated image
 *
 * One of the following properties is mandatory
 *
 * | %Property Name     | Type              |
 * |------------------- |-------------------|
 * | url                | STRING            |
 * | urls               | ARRAY of STRING   |
 *
 * The remaining properties are optional
 * | pixelArea          | VECTOR4           |
 * | wrapModeU          | INTEGER OR STRING |
 * | wrapModeV          | INTEGER OR STRING |
 * | cacheSize          | INTEGER           |
 * | batchSize          | INTEGER           |
 * | frameDelay         | INTEGER           |
 *
 * pixelArea is a rectangular area.
 * In its Vector4 value, the first two elements indicate the top-left position of the area,
 * and the last two elements are the area width and height respectively.
 * If not specified, the default value is [0.0, 0.0, 1.0, 1.0], i.e. the entire area of the image.
 *
 * wrapModeU and wrapModeV separately decide how the texture should be sampled when the u and v coordinate exceeds the range of 0.0 to 1.0.
 * Its value should be one of the following wrap mode:
 *   "DEFAULT"
 *   "CLAMP_TO_EDGE"
 *   "REPEAT"
 *   "MIRRORED_REPEAT"
 *
 * cacheSize is used with multiple images - it determines how many images are kept pre-loaded
 * batchSize is used with multiple images - it determines how many images to load on each frame
 * frameDelay is used with multiple images - it is the number of milliseconds between each frame
 */

class AnimatedImageVisual : public Visual::Base,
                            public ConnectionTracker,
                            public ImageCache::FrameReadyObserver
{
public:
  /**
   * @brief Create the animated image Visual using the image URL.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] shaderFactory The ImageVisualShaderFactory object
   * @param[in] imageUrl The URL to animated image resource to use
   * @param[in] properties A Property::Map containing settings for this visual
   * @return A smart-pointer to the newly allocated visual.
   */
  static AnimatedImageVisualPtr New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, const Property::Map& properties);

  /**
   * @brief Create the animated image Visual using image URLs.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] shaderFactory The ImageVisualShaderFactory object
   * @param[in] imageUrls A Property::Array containing the URLs to the image resources
   * @param[in] properties A Property::Map containing settings for this visual
   * @return A smart-pointer to the newly allocated visual.
   */
  static AnimatedImageVisualPtr New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const Property::Array& imageUrls, const Property::Map& properties);

  /**
   * @brief Create the animated image visual using the image URL.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] shaderFactory The ImageVisualShaderFactory object
   * @param[in] imageUrl The URL to animated image resource to use
   * @param[in] size The width and height of the image. The visual size will be used if these are 0.
   */
  static AnimatedImageVisualPtr New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, ImageDimensions size = ImageDimensions());

public: // from Visual
  /**
   * @copydoc Visual::Base::GetNaturalSize
   */
  void GetNaturalSize(Vector2& naturalSize) override;

  /**
   * @copydoc Visual::Base::CreatePropertyMap
   */
  void DoCreatePropertyMap(Property::Map& map) const override;

  /**
   * @copydoc Visual::Base::CreateInstancePropertyMap
   */
  void DoCreateInstancePropertyMap(Property::Map& map) const override;

  /**
   * @copydoc Visual::Base::OnDoAction
   */
  void OnDoAction(const Dali::Property::Index actionId, const Dali::Property::Value& attributes) override;

protected:
  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] shaderFactory The ImageVisualShaderFactory object
   * @param[in] desiredSize The width and height of the image. The visual size will be used if these are 0.
   */
  AnimatedImageVisual(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, ImageDimensions desiredSize);

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  ~AnimatedImageVisual() override;

  /**
   * @copydoc Visual::Base::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Visual::Base::DoSetProperties
   */
  void DoSetProperties(const Property::Map& propertyMap) override;

  /**
   * Helper method to set individual values by index key.
   * @param[in] index The index key of the value
   * @param[in] value The value
   */
  void DoSetProperty(Property::Index index, const Property::Value& value);

  /**
   * @copydoc Visual::Base::DoSetOnScene
   */
  void DoSetOnScene(Actor& actor) override;

  /**
   * @copydoc Visual::Base::DoSetOffScene
   */
  void DoSetOffScene(Actor& actor) override;

  /**
   * @copydoc Visual::Base::OnSetTransform
   */
  void OnSetTransform() override;

  /**
   * @copydoc Visual::Base::UpdateShader
   */
  void UpdateShader() override;

  /**
   * @copydoc Visual::Base::GenerateShader
   */
  Shader GenerateShader() const override;

private:
  /**
   * @brief Initialize the animated image variables.
   * @param[in] imageUrl The url of the animated image
   */
  void InitializeAnimatedImage(const VisualUrl& imageUrl);

  /**
   * @brief Create image cache for animated image or image array.
   */
  void CreateImageCache();

  /**
   * @brief Adds the texture set to the renderer, and the renderer to the
   * placement actor, and starts the frame timer
   * @param[in] textureSet    The texture set to apply
   * @param[in] firstInterval frame interval(ms) for the first frame.
   */
  void StartFirstFrame(TextureSet& textureSet, uint32_t firstInterval);

  /**
   * @brief Prepares the texture set for displaying
   */
  void PrepareTextureSet();

  /**
   * @brief Set the image size from the texture set
   * @param[in] textureSet The texture set to get the size from
   */
  void SetImageSize(TextureSet& textureSet);

  /**
   * @brief Called when the next frame is ready.
   * @param[in] textureSet the texture set to apply
   * @param[in] interval interval(ms) for the frame
   * @param[in] preMultiplied whether the texture is premultied alpha or not.
   */
  void FrameReady(TextureSet textureSet, uint32_t interval, bool preMultiplied) override;

  /**
   * @brief Display the next frame. It is called when the mFrameDelayTimer ticks.
   * @return true to ensure the timer continues running.
   */
  bool DisplayNextFrame();

  /**
   * @brief Set the state of loading fail of an image or a frame.
   * @return TextureSet of broken image.
   */
  TextureSet SetLoadingFailed();

  /**
   * @brief Allocate mask data.
   * This is allocated only once.
   */
  void AllocateMaskData();

  /**
   * @brief Check whether the mask texture is loaded or not.
   * If MaskingType is MASKING_ON_LOADING and mask texture is failed to load, update shader.
   */
  void CheckMaskTexture();

  /**
   * @brief Callback when the inherited visibility of the actor is changed.
   */
  void OnControlInheritedVisibilityChanged(Actor actor, bool visible);

  /**
   * @brief Callback when the visibility of the window is changed.
   */
  void OnWindowVisibilityChanged(Window window, bool visible);

  // Undefined
  AnimatedImageVisual(const AnimatedImageVisual& animatedImageVisual);

  // Undefined
  AnimatedImageVisual& operator=(const AnimatedImageVisual& animatedImageVisual);

private:
  Timer                     mFrameDelayTimer;
  WeakHandle<Window>        mPlacementWindow;
  WeakHandle<Actor>         mPlacementActor;
  ImageVisualShaderFactory& mImageVisualShaderFactory;

  // Variables for Animated Image player
  Vector4                    mPixelArea;
  VisualUrl                  mImageUrl;
  Dali::AnimatedImageLoading mAnimatedImageLoading; // Only needed for animated image
  uint32_t                   mFrameIndexForJumpTo;  // Frame index into textureRects
  uint32_t                   mCurrentFrameIndex;

  // Variables for Multi-Image player
  ImageCache::UrlList* mImageUrls;
  ImageCache*          mImageCache;
  uint16_t             mCacheSize;
  uint16_t             mBatchSize;
  uint16_t             mFrameDelay;
  int16_t              mLoopCount;
  int16_t              mCurrentLoopIndex;

  // Variables for image visual properties.
  Dali::Toolkit::ImageVisual::LoadPolicy::Type    mLoadPolicy;
  Dali::Toolkit::ImageVisual::ReleasePolicy::Type mReleasePolicy;
  TextureManager::MaskingDataPointer              mMaskingData;
  Dali::ImageDimensions                           mDesiredSize;

  // Shared variables
  uint32_t        mFrameCount; // Number of frames
  ImageDimensions mImageSize;

  DevelAnimatedImageVisual::Action::Type mActionStatus;

  Dali::WrapMode::Type                 mWrapModeU : 3;
  Dali::WrapMode::Type                 mWrapModeV : 3;
  Dali::FittingMode::Type              mFittingMode : 3;
  Dali::SamplingMode::Type             mSamplingMode : 4;
  DevelImageVisual::StopBehavior::Type mStopBehavior : 2;
  bool                                 mStartFirstFrame : 1;
  bool                                 mIsJumpTo : 1;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
#endif /* DALI_TOOLKIT_INTERNAL_ANIMATED_IMAGE_VISUAL_H */
