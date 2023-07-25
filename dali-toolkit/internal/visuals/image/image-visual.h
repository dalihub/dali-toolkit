#ifndef DALI_TOOLKIT_INTERNAL_IMAGE_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_IMAGE_VISUAL_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <memory>

#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/image-loader/atlas-upload-observer.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/internal/texture-manager/texture-upload-observer.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-url.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>

namespace Dali
{
class NativeImage;

namespace Toolkit
{
namespace Internal
{
class ImageVisualShaderFactory;
class ImageVisual;
typedef IntrusivePtr<ImageVisual> ImageVisualPtr;

/**
 * The visual which renders an image to a quad geometry
 *
 * The following properties are optional
 *
 * | %Property Name        | Type              |
 * |-----------------------|-------------------|
 * | url                   | STRING            |
 * | alphaMaskUrl          | STRING            |
 * | fittingMode           | INTEGER OR STRING |
 * | samplingMode          | INTEGER OR STRING |
 * | desiredWidth          | INTEGER           |
 * | desiredHeight         | INTEGER           |
 * | synchronousLoading    | BOOLEAN           |
 * | pixelArea             | VECTOR4           |
 * | wrapModeU             | INTEGER OR STRING |
 * | wrapModeV             | INTEGER OR STRING |
 * | loadPolicy            | INTEGER OR STRING |
 * | releasePolicy         | INTEGER OR STRING |
 * | orientationCorrection | BOOLEAN           |
 *
 * where pixelArea is a rectangular area.
 * In its Vector4 value, the first two elements indicate the top-left position of the area,
 * and the last two elements are the area width and height respectively.
 * If not specified, the default value is [0.0, 0.0, 1.0, 1.0], i.e. the entire area of the image.
 *
 * where wrapModeU and wrapModeV separately decide how the texture should be sampled when the u and v coordinate exceeds the range of 0.0 to 1.0.
 * Its value should be one of the following wrap mode:
 *   "DEFAULT"
 *   "CLAMP_TO_EDGE"
 *   "REPEAT"
 *   "MIRRORED_REPEAT"
 *
 * where imageFittingMode should be one of the following fitting modes:
 *   "SHRINK_TO_FIT"
 *   "SCALE_TO_FILL"
 *   "FIT_WIDTH"
 *   "FIT_HEIGHT"
 *   "DEFAULT"
 *
 * where imageSamplingMode should be one of the following sampling modes:
 *   "BOX"
 *   "NEAREST"
 *   "LINEAR"
 *   "BOX_THEN_NEAREST"
 *   "BOX_THEN_LINEAR"
 *   "NO_FILTER"
 *   "DONT_CARE"
 *   "DEFAULT"
 *
 * where loadPolicy should be one of the following image loading modes
 *   "IMMEDIATE"   // Loads image even if visual not attached to stage yet
 *   "ATTACHED"    // Only loads image once visual is attached to stage
 *
 * where releasePolicy should be one of the following policies for when to cache the image
 *   "DETACHED"    //  Release image from cache when visual detached from stage
 *   "DESTROYED"   //  Keep image in cache until the visual is destroyed
 *   "NEVER"       //  Keep image in cache until application ends.
 *
 * If the Visual is in a LayerUI it will pixel align the image, using a Layer3D will disable pixel alignment.
 * Changing layer behaviour between LayerUI to Layer3D whilst the visual is already staged will not have an effect.
 */
class ImageVisual : public Visual::Base, public ConnectionTracker, public AtlasUploadObserver, public TextureUploadObserver
{
public:
  /**
   * @brief Create a new image visual with a URL.
   *
   * The visual will load the Image asynchronously when the associated actor is put on stage, and destroy the image when it is off stage
   *
   * @param[in] factoryCache The VisualFactoryCache object
   * @param[in] shaderFactory The ImageVisualShaderFactory object
   * @param[in] imageUrl The URL of the image resource to use
   * @param[in] properties A Property::Map containing settings for this visual
   * @param[in] size The width and height to fit the loaded image to.
   * @param[in] fittingMode The FittingMode of the resource to load
   * @param[in] samplingMode The SamplingMode of the resource to load
   * @return A smart-pointer to the newly allocated visual.
   */
  static ImageVisualPtr New(VisualFactoryCache&       factoryCache,
                            ImageVisualShaderFactory& shaderFactory,
                            const VisualUrl&          imageUrl,
                            const Property::Map&      properties,
                            ImageDimensions           size         = ImageDimensions(),
                            FittingMode::Type         fittingMode  = FittingMode::DEFAULT,
                            Dali::SamplingMode::Type  samplingMode = SamplingMode::BOX_THEN_LINEAR);

  /**
   * @brief Create a new image visual with a URL.
   *
   * The visual will load the Image asynchronously when the associated actor is put on stage, and destroy the image when it is off stage
   *
   * @param[in] factoryCache The VisualFactoryCache object
   * @param[in] shaderFactory The ImageVisualShaderFactory object
   * @param[in] imageUrl The URL of the image resource to use
   * @param[in] size The width and height to fit the loaded image to.
   * @param[in] fittingMode The FittingMode of the resource to load
   * @param[in] samplingMode The SamplingMode of the resource to load
   * @return A smart-pointer to the newly allocated visual.
   */
  static ImageVisualPtr New(VisualFactoryCache&       factoryCache,
                            ImageVisualShaderFactory& shaderFactory,
                            const VisualUrl&          imageUrl,
                            ImageDimensions           size         = ImageDimensions(),
                            FittingMode::Type         fittingMode  = FittingMode::DEFAULT,
                            Dali::SamplingMode::Type  samplingMode = SamplingMode::BOX_THEN_LINEAR);

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
   * @brief Constructor with a URL.
   *
   * The visual will load the Image asynchronously when the associated actor is put on stage, and destroy the image when it is off stage
   *
   * @param[in] factoryCache The VisualFactoryCache object
   * @param[in] shaderFactory The ImageVisualShaderFactory object
   * @param[in] imageUrl The URL of the image resource to use
   * @param[in] size The width and height to fit the loaded image to.
   * @param[in] fittingMode The FittingMode of the resource to load
   * @param[in] samplingMode The SamplingMode of the resource to load
   */
  ImageVisual(VisualFactoryCache&       factoryCache,
              ImageVisualShaderFactory& shaderFactory,
              const VisualUrl&          imageUrl,
              ImageDimensions           size,
              FittingMode::Type         fittingMode,
              Dali::SamplingMode::Type  samplingMode);

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~ImageVisual();

  /**
   * @copydoc Visual::Base::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Visual::Base::DoSetProperties
   */
  void DoSetProperties(const Property::Map& propertyMap) override;

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

public:
  /**
   * @copydoc AtlasUploadObserver::UploadCompleted
   *
   * To avoid rendering garbage pixels, renderer should be added to actor after the resources are ready.
   * This callback is the place to add the renderer as it would be called once the loading is finished.
   */
  void UploadCompleted() override;

  /**
   * @copydoc TextureUploadObserver::LoadCompleted
   *
   * To avoid rendering garbage pixels, renderer should be added to actor after the resources are ready.
   * This callback is the place to add the renderer as it would be called once the loading is finished.
   */
  void LoadComplete(bool success, TextureInformation textureInformation) override;

private:
  /**
   * Allocate the mask data when a masking property is defined in the property map
   */
  void AllocateMaskData();

  /**
   * @brief Load the texture, will try to atlas unless unable or param set to false.
   * @param[in, out] atlasing flag if the image has been put in a atlas (true), passing false will not atlas even if possible.
   * @param[out] atlasRect if atlasing is used this the texture area of the image in the atlas.
   * @param[out] textures resulting texture set from the image loading.
   * @param[in] orientationCorrection flag determines if orientation correction should be performed
   * @param[in] forceReload flag determines if the texture should be reloaded from its source or use the cached texture.
   */
  void LoadTexture(bool& atlasing, Vector4& atlasRect, TextureSet& textures, bool orientationCorrection, TextureManager::ReloadPolicy forceReload);

  /**
   * @brief Checks if atlasing should be attempted
   * @return bool returns true if atlasing can be attempted.
   */
  bool AttemptAtlasing();

  /**
   * @brief Initializes the Dali::Renderer from the image url
   */
  void InitializeRenderer();

  /**
   * Creates the texture set and adds the texture to it
   * @param[out] textureRect The texture area of the texture in the atlas.
   * @param[in] url The URL of the image resource to use.
   * @param[in] synchronousLoading If true, the resource is loaded synchronously, otherwise asynchronously.
   * @param[in] attemptAtlasing If true will attempt atlasing, otherwise create unique texture
   * @return the texture set to use
   */
  TextureSet CreateTextureSet(Vector4& textureRect, bool synchronousLoading, bool attemptAtlasing);

  /**
   * Set the value to the uTextureRect uniform
   * @param[in] textureRect The texture rectangular area.
   */
  void SetTextureRectUniform(const Vector4& textureRect);

  /**
   * Remove texture with valid TextureId
   */
  void RemoveTexture();

  /**
   * @brief Compute texture size
   */
  void ComputeTextureSize();

  /**
   * @brief Compute mask texture ratio
   * @return The Mask Texture Ratio
   */
  Vector2 ComputeMaskTextureRatio();

  /**
   * Helper method to set individual values by index key.
   * @param[in] index The index key of the value
   * @param[in] value The value
   */
  void DoSetProperty(Property::Index index, const Property::Value& value);

  /**
   * @brief Check whether the mask texture is loaded or not.
   * If MaskingType is MASKING_ON_LOADING and mask texture is failed to load, update shader.
   */
  void CheckMaskTexture();

private:
  Vector4                            mPixelArea;
  WeakHandle<Actor>                  mPlacementActor;
  VisualUrl                          mImageUrl;
  TextureManager::MaskingDataPointer mMaskingData;

  Dali::ImageDimensions     mDesiredSize;
  TextureManager::TextureId mTextureId;
  TextureSet                mTextures;
  Vector2                   mTextureSize;
  Vector2                   mPlacementActorSize;

  ImageVisualShaderFactory& mImageVisualShaderFactory;

  Dali::FittingMode::Type                         mFittingMode : 3;
  Dali::SamplingMode::Type                        mSamplingMode : 4;
  Dali::WrapMode::Type                            mWrapModeU : 3;
  Dali::WrapMode::Type                            mWrapModeV : 3;
  Dali::Toolkit::ImageVisual::LoadPolicy::Type    mLoadPolicy;
  Dali::Toolkit::ImageVisual::ReleasePolicy::Type mReleasePolicy;
  Vector4                                         mAtlasRect;
  Dali::ImageDimensions                           mAtlasRectSize;
  TextureManager::LoadState                       mLoadState;             ///< The texture loading state
  bool                                            mAttemptAtlasing;       ///< If true will attempt atlasing, otherwise create unique texture
  bool                                            mOrientationCorrection; ///< true if the image will have it's orientation corrected.
  bool                                            mNeedYuvToRgb{false};   ///< true if we need to convert yuv to rgb.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_IMAGE_VISUAL_H */
