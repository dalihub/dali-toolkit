#ifndef DALI_TOOLKIT_INTERNAL_BATCH_IMAGE_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_BATCH_IMAGE_VISUAL_H

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

// INTERNAL HEADER
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>

// EXTERNAL INCLUDES
#include <dali/public-api/images/resource-image.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class BatchImageVisual: public Visual::Base, public ConnectionTracker
{
public:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache The VisualFactoryCache object
   * @param[in] atlasManager The atlasManager object
   */
  BatchImageVisual( VisualFactoryCache& factoryCache, ImageAtlasManager& atlasManager );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  ~BatchImageVisual();

public:  // from Visual

  /**
   * @copydoc Visual::Base::SetSize
   */
  virtual void SetSize( const Vector2& size );

  /**
   * @copydoc Visual::Base::GetNaturalSize
   */
  virtual void GetNaturalSize( Vector2& naturalSize ) const;

  /**
   * @copydoc Visual::Base::SetClipRect
   */
  virtual void SetClipRect( const Rect<int>& clipRect );

  /**
   * @copydoc Visual::Base::CreatePropertyMap
   */
  virtual void DoCreatePropertyMap( Property::Map& map ) const;

protected:

  /**
   * @copydoc Visua::Base::DoInitialize
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

private:

  /**
   * Get the batch image rendering shader.
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   */
  static Shader GetBatchShader( VisualFactoryCache& factoryCache );

  /**
   * @brief Initializes the Dali::Renderer from an image url string
   *
   * @param[in] imageUrl The image url string to intialize this ImageVisual from
   */
  void InitializeRenderer( const std::string& imageUrl );

  /**
   * Clean the Visual from cache, and remove the image from atlas if it is not used anymore
   */
  void CleanCache( const std::string& url );

private:

  ImageAtlasManager&      mAtlasManager;
  Vector4                 mAtlasRect;
  std::string             mImageUrl;
  Dali::ImageDimensions   mDesiredSize;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_BATCH_IMAGE_VISUAL_H
