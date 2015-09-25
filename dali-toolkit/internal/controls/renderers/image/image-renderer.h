#ifndef __DALI_TOOLKIT_INTERNAL_IMAGE_RENDERER_H__
#define __DALI_TOOLKIT_INTERNAL_IMAGE_RENDERER_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/controls/renderers/control-renderer-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/image-operations.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class ImageRenderer;
typedef IntrusivePtr< ImageRenderer > ImageRendererPtr;

/**
 * The renderer which renders an image to the control's quad
 *
 * The following properties are optional
 *
 * | %Property Name            | Type             |
 * |---------------------------|------------------|
 * | image-url                 | STRING           |
 * | image-fitting-mode        | STRING           |
 * | image-sampling-mode       | STRING           |
 * | image-desired-width       | INT              |
 * | image-desired-height      | INT              |
 *
 * where image-fitting-mode should be one of the following fitting modes:
 *   "shrink-to-fit"
 *   "scale-to-fill"
 *   "fit-width"
 *   "fit-height"
 *   "default"
 *
 * where image-sampling-mode should be one of the following sampling modes:
 *   "box"
 *   "nearest"
 *   "linear"
 *   "box-then-nearest"
 *   "box-then-linear"
 *   "no-filter"
 *   "dont-care"
 *   "default"
 *
 */
class ImageRenderer: public ControlRenderer
{
public:

  /**
   * @brief Constructor.
   */
  ImageRenderer();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  ~ImageRenderer();

public:  // from ControlRenderer

  /**
   * @copydoc ControlRenderer::Initialize
   */
  virtual void Initialize( RendererFactoryCache& factoryCache, const Property::Map& propertyMap );

  /**
   * @copydoc ControlRenderer::SetSize
   */
  virtual void SetSize( const Vector2& size );

  /**
   * @copydoc ControlRenderer::SetClipRect
   */
  virtual void SetClipRect( const Rect<int>& clipRect );

  /**
   * @copydoc ControlRenderer::SetOffset
   */
  virtual void SetOffset( const Vector2& offset );

protected:
  /**
   * @copydoc ControlRenderer::DoSetOnStage
   */
  virtual void DoSetOnStage( Actor& actor );

  /**
   * @copydoc ControlRenderer::DoSetOffStage
   */
  virtual void DoSetOffStage( Actor& actor );

public:

  /**
   * Request the geometry and shader from the cache, if not available, create and save to the cache for sharing.
   *
   * @param[in] factoryCache A pointer pointing to the RendererFactoryCache object
   */
  void Initialize( RendererFactoryCache& factoryCache );

  /**
   * @brief Sets the image of this renderer to the resource at imageUrl
   * The renderer will load the Image asynchronously when the associated actor is put on stage, and destroy the image when it is off stage
   *
   * @param[in] imageUrl The URL to to image resource to use
   */
  void SetImage( const std::string& imageUrl );

  /**
   * @brief Sets the image of this renderer to the resource at imageUrl
   * The renderer will load the Image asynchronously when the associated actor is put on stage, and destroy the image when it is off stage
   *
   * @param[in] imageUrl The URL to to image resource to use
   * @param[in] desiredWidth The desired width of the resource to load
   * @param[in] desiredHeight The desired height of the resource to load
   * @param[in] fittingMode The FittingMode of the resource to load
   * @param[in] samplingMode The SamplingMode of the resource to load
   */
  void SetImage( const std::string& imageUrl, int desiredWidth, int desiredHeight, Dali::FittingMode::Type fittingMode, Dali::SamplingMode::Type samplingMode );

  /**
   * @brief Sets the image of this renderer to use
   *
   * @param[in] image The image to use
   */
  void SetImage( Image image );

  /**
   * @brief Gets the image this renderer uses
   *
   * @return The image this renderer uses, which may be null if the image is set from a URL string and the renderer is not set as onstage
   */
  Image GetImage() const;

private:

  /**
   * @brief Applies this renderer's image to the sampler to the material used for this renderer
   */
  void ApplyImageToSampler();

private:
  Image mImage;

  std::string mImageUrl;
  Dali::ImageDimensions mDesiredSize;
  Dali::FittingMode::Type mFittingMode;
  Dali::SamplingMode::Type mSamplingMode;

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_INTERNAL_IMAGE_RENDERER_H__ */
