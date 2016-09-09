#ifndef DALI_TOOLKIT_INTERNAL_SVG_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_SVG_VISUAL_H

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

//EXTERNAL INCLUDES
#include <dali/devel-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-base-impl.h>

struct NSVGimage;

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

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
class SvgVisual: public Visual::Base
{
public:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   */
  SvgVisual( VisualFactoryCache& factoryCache );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  ~SvgVisual();

public:  // from Visual

  /**
   * @copydoc Visual::Base::GetNaturalSize
   */
  virtual void GetNaturalSize( Vector2& naturalSize ) const;

  /**
   * @copydoc Visual::Base::SetSize
   */
  virtual void SetSize( const Vector2& size );

  /**
   * @copydoc Visual::Base::CreatePropertyMap
   */
  virtual void DoCreatePropertyMap( Property::Map& map ) const;

protected:

  /**
   * @copydoc Visual::Base::DoInitialize
   */
  virtual void DoInitialize( Actor& actor, const Property::Map& propertyMap );

  /**
   * @copydoc Visual::Base::DoSetOnStage
   */
  virtual void DoSetOnStage( Actor& actor );

  /**
   * @copydoc Visual::Base::DoSetOffStage
   */
  virtual void DoSetOffStage( Actor& actor );

public:

  /**
   * @brief Sets the svg image of this visual to the resource at imageUrl
   * The visual will parse the svg image once it is set.
   * And rasterize it into BufferImage synchronously when the associated actor is put on stage, and destroy the BufferImage when it is off stage
   *
   * @param[in] imageUrl The URL to svg resource to use
   */
  void SetImage( const std::string& imageUrl, ImageDimensions size = ImageDimensions() );

  /**
   * @bried Apply the rasterized image to the visual.
   *
   * @param[in] rasterizedPixelData The pixel buffer with the rasterized pixels
   */
  void ApplyRasterizedImage( PixelData rasterizedPixelData );

private:
  /**
   * @bried Rasterize the svg with the given size, and add it to the visual.
   *
   * @param[in] size The target size of the SVG rasterization.
   */
  void AddRasterizationTask( const Vector2& size );


  // Undefined
  SvgVisual( const SvgVisual& svgRenderer );

  // Undefined
  SvgVisual& operator=( const SvgVisual& svgRenderer );

private:
  Vector4              mAtlasRect;
  std::string          mImageUrl;
  NSVGimage*           mParsedImage;
  WeakHandle<Actor>    mPlacementActor;

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_SVG_VISUAL_H */
