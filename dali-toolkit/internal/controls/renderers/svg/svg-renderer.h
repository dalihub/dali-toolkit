#ifndef __DALI_TOOLKIT_INTERNAL_SVG_RENDERER_H__
#define __DALI_TOOLKIT_INTERNAL_SVG_RENDERER_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/controls/renderers/image-atlas-manager.h>

struct NSVGimage;

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * The renderer which renders a svg image
 *
 * The following property is essential
 *
 * | %Property Name           | Type             |
 * |--------------------------|------------------|
 * | imageUrl                 | STRING           |
 *
 */
class SvgRenderer: public ControlRenderer
{
public:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the RendererFactoryCache object
   */
  SvgRenderer( RendererFactoryCache& factoryCache, ImageAtlasManager& atlasManager );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  ~SvgRenderer();

public:  // from ControlRenderer

  /**
   * @copydoc ControlRenderer::GetNaturalSize
   */
  virtual void GetNaturalSize( Vector2& naturalSize ) const;

  /**
   * @copydoc ControlRenderer::SetSize
   */
  virtual void SetSize( const Vector2& size );

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
   * @brief Helper method to determine whether the url indicate that it is a svg image.
   *
   * @param [in] url The URL of the image file.
   * @return true if it is a svg image
   */
  static bool IsSvgUrl( const std::string& url );

  /**
   * @brief Sets the svg image of this renderer to the resource at imageUrl
   * The renderer will parse the svg image once it is set.
   * And rasterize it into BufferImage synchronously when the associated actor is put on stage, and destroy the BufferImage when it is off stage
   *
   * @param[in] imageUrl The URL to svg resource to use
   */
  void SetImage( const std::string& imageUrl, ImageDimensions size = ImageDimensions() );

  /**
   * @bried Apply the rasterized image to the renderer.
   *
   * @param[in] rasterizedPixelData The pixel buffer with the rasterized pixels
   */
  void ApplyRasterizedImage( PixelData rasterizedPixelData );

private:
  /**
   * @bried Rasterize the svg with the given size, and add it to the renderer.
   *
   * @param[in] size The target size of the SVG rasterization.
   */
  void AddRasterizationTask( const Vector2& size );


  // Undefined
  SvgRenderer( const SvgRenderer& svgRenderer );

  // Undefined
  SvgRenderer& operator=( const SvgRenderer& svgRenderer );

private:
  Vector4              mAtlasRect;
  ImageAtlasManager&   mAtlasManager;
  std::string          mImageUrl;
  NSVGimage*           mParsedImage;

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_INTERNAL_SVG_RENDERER_H__ */
