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
#include <dali/public-api/images/resource-image.h>

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
 * | %Property Name          | Type             |
 * |-------------------------|------------------|
 * | imageUrl                | STRING           |
 * | imageFittingMode        | STRING           |
 * | imageSamplingMode       | STRING           |
 * | imageDesiredWidth       | INT              |
 * | imageDesiredHeight      | INT              |
 *
 * where imageFittingMode should be one of the following fitting modes:
 *   "shrinkToFit"
 *   "scaleToFill"
 *   "fitWidth"
 *   "fitHeight"
 *   "default"
 *
 * where imageSamplingMode should be one of the following sampling modes:
 *   "box"
 *   "nearest"
 *   "linear"
 *   "boxThenNearest"
 *   "boxThenLinear"
 *   "noFilter"
 *   "dontCare"
 *   "default"
 *
 */
class ImageRenderer: public ControlRenderer, public ConnectionTracker
{
public:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the RendererFactoryCache object
   */
  ImageRenderer( RendererFactoryCache& factoryCache );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  ~ImageRenderer();

public:  // from ControlRenderer

  /**
   * @copydoc ControlRenderer::SetSize
   */
  virtual void SetSize( const Vector2& size );

  /**
   * @copydoc ControlRenderer::GetNaturalSize
   */
  virtual void GetNaturalSize( Vector2& naturalSize ) const;

  /**
   * @copydoc ControlRenderer::SetClipRect
   */
  virtual void SetClipRect( const Rect<int>& clipRect );

  /**
   * @copydoc ControlRenderer::SetOffset
   */
  virtual void SetOffset( const Vector2& offset );

  /**
   * @copydoc ControlRenderer::CreatePropertyMap
   */
  virtual void DoCreatePropertyMap( Property::Map& map ) const;

protected:
  /**
   * @copydoc ControlRenderer::DoInitialize
   */
  virtual void DoInitialize( Actor& actor, const Property::Map& propertyMap );

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
   * @brief Sets the image of this renderer to the resource at imageUrl
   * The renderer will load the Image asynchronously when the associated actor is put on stage, and destroy the image when it is off stage
   *
   * @param[in] actor The Actor the renderer is applied to if, empty if the renderer has not been applied to any Actor
   * @param[in] imageUrl The URL to to image resource to use
   */
  void SetImage( Actor& actor, const std::string& imageUrl );

  /**
   * @brief Sets the image of this renderer to the resource at imageUrl
   * The renderer will load the Image asynchronously when the associated actor is put on stage, and destroy the image when it is off stage
   *
   * @param[in] actor The Actor the renderer is applied to if, empty if the renderer has not been applied to any Actor
   * @param[in] imageUrl The URL to to image resource to use
   * @param[in] desiredWidth The desired width of the resource to load
   * @param[in] desiredHeight The desired height of the resource to load
   * @param[in] fittingMode The FittingMode of the resource to load
   * @param[in] samplingMode The SamplingMode of the resource to load
   */
  void SetImage( Actor& actor, const std::string& imageUrl, int desiredWidth, int desiredHeight, Dali::FittingMode::Type fittingMode, Dali::SamplingMode::Type samplingMode );

  /**
   * @brief Sets the image of this renderer to use
   *
   * @param[in] actor The Actor the renderer is applied to if, empty if the renderer has not been applied to any Actor
   * @param[in] image The image to use
   */
  void SetImage( Actor& actor, const Image& image );

private:

  /**
   * @brief Applies the image to the material used for this renderer
   *
   * @param[in] image The Image to apply to the material used for this renderer
   */
  void ApplyImageToSampler( const Image& image );

  /**
   * @brief Initializes the Dali::Renderer from an image url string
   *
   * @param[in] imageUrl The image url string to intialize this ImageRenderer from
   */
  void InitializeRenderer( const std::string& imageUrl );

  /**
   * @brief Initializes the Dali::Renderer from an image handle
   *
   * @param[in] image The image handle to intialize this ImageRenderer from
   */
  void InitializeRenderer( const Image& image );

  /**
   * @brief Creates the Dali::Renderer (potentially from the renderer cache), initializing it
   *
   * @return Returns the created Dali::Renderer
   */
  Renderer CreateRenderer() const;

  /**
   * Callback function of image resource loading succeed
   * @param[in] image The Image content that we attempted to load from mImageUrl
   */
  void OnImageLoaded( ResourceImage image );

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
