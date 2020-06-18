#ifndef DALI_TOOLKIT_INTERNAL_N_PATCH_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_N_PATCH_VISUAL_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/sampler.h>
#include <dali/public-api/rendering/shader.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/texture-upload-observer.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-url.h>

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
 * | borderOnly               | BOOLEAN          |
 * | border                   | RECTANGLE        |
 * | auxiliaryImage           | STRING           |
 * | auxiliaryImageAlpha      | FLOAT            |
 */
class NPatchVisual: public Visual::Base, public TextureUploadObserver
{
public:

  /**
   * @brief Create an N-patch visual using an image URL.
   *
   * The visual will load the image synchronously when the associated actor is put on stage, and destroy the image when it is off stage
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] imageUrl The URL to 9 patch image resource to use
   * @param[in] properties A Property::Map containing settings for this visual
   * @return A smart-pointer to the newly allocated visual.
   */
  static NPatchVisualPtr New( VisualFactoryCache& factoryCache, const VisualUrl& imageUrl, const Property::Map& properties );

  /**
   * @brief Create an N-patch visual using an image URL.
   *
   * The visual will load the image synchronously when the associated actor is put on stage, and destroy the image when it is off stage
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] imageUrl The URL to 9 patch image resource to use
   * @return A smart-pointer to the newly allocated visual.
   */
  static NPatchVisualPtr New( VisualFactoryCache& factoryCache, const VisualUrl& imageUrl );

public:  // from Visual

  /**
   * @copydoc Visual::Base::GetNaturalSize
   */
  void GetNaturalSize( Vector2& naturalSize ) override;

  /**
   * @copydoc Visual::Base::CreatePropertyMap
   */
  void DoCreatePropertyMap( Property::Map& map ) const override;

  /**
   * @copydoc Visual::Base::CreateInstancePropertyMap
   */
  void DoCreateInstancePropertyMap( Property::Map& map ) const override;

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
  void DoSetProperties( const Property::Map& propertyMap ) override;

  /**
   * @copydoc Visual::Base::DoSetOnStage
   */
  void DoSetOnStage( Actor& actor ) override;

  /**
   * @copydoc Visual::Base::DoSetOffStage
   */
  void DoSetOffStage( Actor& actor ) override;

  /**
   * @copydoc Visual::Base::OnSetTransform
   */
  void OnSetTransform() override;

private:

  /**
   * Loads the NPatch image and the Auxiliary image if needed
   */
  void LoadImages();

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
   * @brief Applies texture and related uniforms
   */
  void ApplyTextureAndUniforms();

  /**
   * Helper method to get the default Nine patch geometry from cache or create and store it there
   * @param subType to use
   * @return the geometry
   */
  Geometry GetNinePatchGeometry( VisualFactoryCache::GeometryType subType );

  /**
   * @brief Creates a geometry for the grid size to be used by this visuals' shaders
   *
   * @param[in] gridSize The grid size of the solid geometry to create
   * @return Returns the created geometry for the grid size
   */
  Geometry CreateGridGeometry( Uint16Pair gridSize );

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
  Geometry CreateBorderGeometry( Uint16Pair gridSize );

  /**
   * @brief Creates a renderer by using loaded resource.
   */
  void SetResource();

private:

  /**
   * @copydoc TextureUploadObserver::UploadCompleted
   *
   * To avoid rendering garbage pixels, renderer should be added to actor after the resources are ready.
   * This callback is the place to add the renderer as it would be called once the loading is finished.
   */
  void UploadComplete( bool loadSuccess, int32_t textureId, TextureSet textureSet, bool useAtlasing, const Vector4& atlasRect, bool preMultiplied ) override {}

  /**
   * @copydoc TextureUploadObserver::LoadComplete
   *
   * To avoid rendering garbage pixels, renderer should be added to actor after the resources are ready.
   * This callback is the place to add the renderer as it would be called once the loading is finished.
   */
  void LoadComplete( bool loadSuccess, Devel::PixelBuffer pixelBuffer, const VisualUrl& url, bool preMultiplied ) override;

private:

  WeakHandle<Actor>  mPlacementActor;       ///< Weakhandle to contain Actor during texture loading
  NPatchLoader&      mLoader;               ///< reference to N patch loader for fast access
  VisualUrl          mImageUrl;             ///< The url to the N patch to load
  VisualUrl          mAuxiliaryUrl;         ///< An auxiliary image that can be displayed on top of the N-Patch
  std::size_t        mId;                   ///< id of the N patch (from loader/cache)
  Devel::PixelBuffer mAuxiliaryPixelBuffer; ///< pixel buffer of the auxiliary mask image
  bool               mBorderOnly;           ///< if only border is desired
  Rect<int>          mBorder;               ///< The size of the border
  float              mAuxiliaryImageAlpha;  ///< The alpha value for the auxiliary image only
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_N_PATCH_VISUAL_H
