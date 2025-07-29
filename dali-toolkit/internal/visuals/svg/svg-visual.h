#ifndef DALI_TOOLKIT_INTERNAL_SVG_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_SVG_VISUAL_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/svg/svg-loader-observer.h>
#include <dali-toolkit/internal/visuals/svg/svg-loader.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-url.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class ImageVisualShaderFactory;
class SvgVisual;
typedef IntrusivePtr<SvgVisual> SvgVisualPtr;

/**
 * The visual which renders a svg image
 *
 * The following property is essential
 *
 * | %Property Name           | Type             |
 * |--------------------------|------------------|
 * | url                      | STRING           |
 *
 */
class SvgVisual : public Visual::Base,
                  public SvgLoaderObserver
{
public:
  /**
   * @brief Create the SVG Visual using the image URL.
   *
   * The visual will parse the SVG image once it is set.
   * And rasterize it into BufferImage synchronously when the associated actor is put on stage, and destroy the BufferImage when it is off stage
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] shaderFactory The ImageVisualShaderFactory object
   * @param[in] imageUrl The URL to svg resource to use
   * @param[in] properties A Property::Map containing settings for this visual
   * @return A smart-pointer to the newly allocated visual.
   */
  static SvgVisualPtr New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, const Property::Map& properties);

  /**
   * @brief Create the SVG Visual using the image URL.
   *
   * The visual will parse the SVG image once it is set.
   * And rasterize it into BufferImage synchronously when the associated actor is put on stage, and destroy the BufferImage when it is off stage
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] shaderFactory The ImageVisualShaderFactory object
   * @param[in] imageUrl The URL to svg resource to use
   * @param[in] size The width and height of the rasterized buffer. The visual size will be used if these are 0.
   * @return A smart-pointer to the newly allocated visual.
   */
  static SvgVisualPtr New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, ImageDimensions size = ImageDimensions());

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
   * @copydoc Visual::Base::EnablePreMultipliedAlpha
   */
  void EnablePreMultipliedAlpha(bool preMultiplied) override;

protected:
  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] shaderFactory The ImageVisualShaderFactory object
   * @param[in] imageUrl The URL to svg resource to use
   * @param[in] size The width and height of the rasterized buffer. The visual size will be used if these are 0.
   */
  SvgVisual(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, ImageDimensions size);

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~SvgVisual();

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

protected: // Implementation of  SvgLoaderObserver
  /**
   * @copydoc Dali::Toolkit::Internal::SvgLoaderObserver::LoadComplete
   */
  void LoadComplete(int32_t loadId, Dali::VectorImageRenderer vectorImageRenderer) override;

  /**
   * @copydoc Dali::Toolkit::Internal::SvgLoaderObserver::RasterizeComplete
   */
  void RasterizeComplete(int32_t rasterizeId, Dali::TextureSet textureSet, Vector4 atlasRect) override;

private:
  /**
   * @bried Rasterize the svg with the given size, and add it to the visual.
   *
   * @param[in] size The target size of the SVG rasterization.
   */
  void AddRasterizationTask(const Vector2& size);

  /**
   * Helper method to set individual values by index key.
   * @param[in] index The index key of the value
   * @param[in] value The value
   */
  void DoSetProperty(Property::Index index, const Property::Value& value);

  /**
   * @brief Checks if atlasing should be attempted
   * @return bool returns true if atlasing can be attempted.
   */
  bool AttemptAtlasing() const;

  /**
   * @brief Emit ResourceReady signal.
   * @note MUST call this API end of function.
   */
  void EmitResourceReady(Toolkit::Visual::ResourceStatus resourceStatus);

  // Undefined
  SvgVisual(const SvgVisual& svgRenderer);

  // Undefined
  SvgVisual& operator=(const SvgVisual& svgRenderer);

private:
  ImageVisualShaderFactory& mImageVisualShaderFactory;
  SvgLoader&                mSvgLoader; ///< reference to Svg loader for fast access

  SvgLoader::SvgLoadId      mSvgLoadId;
  SvgLoader::SvgRasterizeId mSvgRasterizeId;

  Vector4               mAtlasRect;
  Property::Index       mAtlasRectIndex;
  VisualUrl             mImageUrl;
  uint32_t              mDefaultWidth;
  uint32_t              mDefaultHeight;
  WeakHandle<Actor>     mPlacementActor;
  Vector2               mRasterizedSize;
  Dali::ImageDimensions mDesiredSize{};

  bool mLoadCompleted : 1;
  bool mRasterizeCompleted : 1;
  bool mLoadFailed : 1;
  bool mAttemptAtlasing : 1; ///< If true will attempt atlasing, otherwise create unique texture
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_SVG_VISUAL_H */
