#ifndef DALI_TOOLKIT_INTERNAL_N_PATCH_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_N_PATCH_VISUAL_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/devel-api/images/nine-patch-image.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/sampler.h>
#include <dali/public-api/rendering/shader.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-base-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class NPatchVisual;
typedef IntrusivePtr< NPatchVisual > NPatchVisualPtr;

/**
 * The visual which renders an 9 patch image to the control's quad
 *
 * The following properties are optional
 *
 * | %Property Name           | Type             |
 * |--------------------------|------------------|
 * | url                      | STRING           |
 * | borderOnly               | BOOLEAN
 *
 */
class NPatchVisual: public Visual::Base
{
public:

  /**
   * @brief Create an N-patch visual using an image URL.
   *
   * The visual will load the image synchronously when the associated actor is put on stage, and destroy the image when it is off stage
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] imageUrl The URL to 9 patch image resource to use
   */
  static NPatchVisualPtr New( VisualFactoryCache& factoryCache, const std::string& imageUrl );

  /**
   * @brief Create an N-patch visual with a NinePatchImage resource.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] image The NinePatchImage to use
   */
  static NPatchVisualPtr New( VisualFactoryCache& factoryCache, NinePatchImage image );

public:  // from Visual

  /**
   * @copydoc Visual::Base::GetNaturalSize
   */
  virtual void GetNaturalSize( Vector2& naturalSize ) const;

  /**
   * @copydoc Visual::Base::CreatePropertyMap
   */
  virtual void DoCreatePropertyMap( Property::Map& map ) const;

  /**
   * @copydoc Visual::Base::DoSetProperty
   */
  virtual void DoSetProperty( Dali::Property::Index index, const Dali::Property::Value& propertyValue );

  /**
   * @copydoc Visual::Base::DoGetProperty
   */
  virtual Dali::Property::Value DoGetProperty( Dali::Property::Index index );

protected:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache Reference to the VisualFactoryCache object
   */
  NPatchVisual( VisualFactoryCache& factoryCache );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~NPatchVisual();

  /**
   * @copydoc Visual::Base::DoSetProperties
   */
  virtual void DoSetProperties( const Property::Map& propertyMap );

  /**
   * @copydoc Visual::Base::DoSetOnStage
   */
  virtual void DoSetOnStage( Actor& actor );

  /**
   * @copydoc Visual::Base::DoSetOffStage
   */
  virtual void DoSetOffStage( Actor& actor );

  /**
   * @copydoc Visual::Base::OnSetTransform
   */
  virtual void OnSetTransform();

private:

  /**
   * @brief Initialize the renderer with the geometry and shader from the cache, if not available, create and save to the cache for sharing.
   */
  void InitializeRenderer();

  /**
   * @brief Creates a geometry for this renderer's grid size
   *
   * @return Returns the created geometry for this renderer's grid size
   */
  Geometry CreateGeometry();

  /**
   * @brief Creates a shader for this renderer's grid size
   *
   * @return Returns the created shader for this renderer's grid size
   */
  Shader CreateShader();

  /**
   * @brief Creates a geometry for the grid size to be used by this visuals' shaders
   *
   * @param[in] gridSize The grid size of the solid geometry to create
   * @return Returns the created geometry for the grid size
   */
  Geometry CreateGeometry( Uint16Pair gridSize );

  /**
   * @brief Creates a geometry with the border only for the grid size to be used by this visuals' shaders
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
   * @param[in] gridSize The grid size of the solid geometry to create
   * @return Returns the created geometry for the grid size
   */
  Geometry CreateGeometryBorder( Uint16Pair gridSize );

  /**
   * @brief Creates Image from the image url and parses the image for the stretch borders. Will create a error image if the n patch image is invalid
   *
   * @param[in] nPatchImage The NinePatchImage to base our cropped images and stretch borders from
   */
  void InitializeFromImage( NinePatchImage nPatchImage );

  /**
   * @brief Creates an error Image to indicate that there was an error in either the image url or the parsing of the image
   *
   */
  void InitializeFromBrokenImage();

  /**
   * @brief Applies this renderer's image to the sampler to the texture set used for this renderer
   */
  void ApplyImageToSampler();

  /**
   * @brief Changes the current renderer if the n-patch meta data has changed
   *
   * @param[in] oldBorderOnly The old flag indicating if the image should omit the centre of the n-patch and only render the border
   * @param[in] oldGridX The old horizontal grid size of the solid geometry
   * @param[in] oldGridY The old vertical grid size of the solid geometry
   */
  void ChangeRenderer( bool oldBorderOnly, size_t oldGridX, size_t oldGridY );

private:

  NinePatchImage mImage; ///< The image to render if the visual was set from an NinePatchImage, empty otherwise
  Image mCroppedImage;

  std::string mImageUrl; ///< The url to the image resource to render if the visual was set from an image resource url, empty otherwise
  NinePatchImage::StretchRanges mStretchPixelsX;
  NinePatchImage::StretchRanges mStretchPixelsY;
  ImageDimensions mImageSize;
  bool mBorderOnly;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_N_PATCH_VISUAL_H
