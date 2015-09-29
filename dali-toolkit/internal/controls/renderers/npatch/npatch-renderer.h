#ifndef __DALI_TOOLKIT_INTERNAL_N_PATCH_RENDERER_H__
#define __DALI_TOOLKIT_INTERNAL_N_PATCH_RENDERER_H__

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
#include <dali/public-api/images/nine-patch-image.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/devel-api/rendering/geometry.h>
#include <dali/devel-api/rendering/shader.h>
#include <dali/devel-api/rendering/sampler.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * The renderer which renders an 9 patch image to the control's quad
 *
 * The following properties are optional
 *
 * | %Property Name            | Type             |
 * |---------------------------|------------------|
 * | image-url                 | STRING           |
 * | border-only               | BOOLEAN
 *
 */
class NPatchRenderer: public ControlRenderer
{
public:

  /**
   * @brief Constructor.
   */
  NPatchRenderer();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  ~NPatchRenderer();

public:  // from ControlRenderer

  /**
   * @copydoc ControlRenderer::Initialize
   */
  virtual void Initialize( RendererFactoryCache& factoryCache, const Property::Map& propertyMap );

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
  virtual void CreatePropertyMap( Property::Map& map ) const;

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
   * @brief Sets the 9 patch image of this renderer to the resource at imageUrl
   * The renderer will load the image synchronously when the associated actor is put on stage, and destroy the image when it is off stage
   *
   * @param[in] imageUrl The URL to 9 patch image resource to use
   * @param[in] borderOnly A Flag to indicate if the image should omit the centre of the n-patch and only render the border
   */
  void SetImage( const std::string& imageUrl, bool borderOnly = false );

  /**
   * @brief Sets the 9 patch image of this renderer to the 9 patch image
   *
   * @param[in] image The NinePatchImage to use
   * @param[in] borderOnly A Flag to indicate if the image should omit the centre of the n-patch and only render the border
   */
  void SetImage( NinePatchImage image, bool borderOnly = false );

private:

  /**
   * @brief Creates a geometry for the grid size to be used by this renderers' shaders
   *
   * @param gridSize The grid size of the solid geometry to create
   * @return Returns the created geometry for the grid size
   */
  Geometry CreateGeometry( Uint16Pair gridSize );

  /**
   * @brief Creates a geometry with the border only for the grid size to be used by this renderers' shaders
   * e.g. a 5x4 grid would create a geometry that would look like:
   *
   *   ---------------------
   *   |  /|  /|  /|  /|  /|
   *   |/  |/  |/  |/  |/  |
   *   ---------------------
   *   |  /|           |  /|
   *   |/  |           |/  |
   *   -----           -----
   *   |  /|           |  /|
   *   |/  |           |/  |
   *   ---------------------
   *   |  /|  /|  /|  /|  /|
   *   |/  |/  |/  |/  |/  |
   *   ---------------------
   *
   * @param gridSize The grid size of the solid geometry to create
   * @return Returns the created geometry for the grid size
   */
  Geometry CreateGeometryBorder( Uint16Pair gridSize );

  /**
   * @brief Creates Image from the image url and parses the image for the stretch borders. Will create a error image if the n patch image is invalid
   *
   * @param nPatchImage The NinePatchImage to base our cropped images and stretch borders from
   */
  void InitialiseFromImage( NinePatchImage nPatchImage );

  /**
   * @brief Creates a black Image to indicate that there was an error in either the image url or the parsing of the image
   *
   */
  void CreateErrorImage();

  /**
   * @brief Applies this renderer's image to the sampler to the material used for this renderer
   */
  void ApplyImageToSampler();

private:

  NinePatchImage mImage; ///< The image to render if the renderer was set from an NinePatchImage, empty otherwise
  Image mCroppedImage;
  Geometry mNinePatchGeometry;
  Geometry mNinePatchBorderGeometry;
  Shader mNinePatchShader;

  std::string mImageUrl; ///< The url to the image resource to render if the renderer was set from an image resource url, empty otherwise
  NinePatchImage::StretchRanges mStretchPixelsX;
  NinePatchImage::StretchRanges mStretchPixelsY;
  ImageDimensions mImageSize;
  bool mBorderOnly;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_INTERNAL_N_PATCH_RENDERER_H__ */
