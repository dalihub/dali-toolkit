#ifndef __DALI_TOOLKIT_RENDERER_FACTORY_IMPL_H__
#define __DALI_TOOLKIT_RENDERER_FACTORY_IMPL_H__

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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/control-renderer.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class RendererFactoryCache;
typedef IntrusivePtr<RendererFactoryCache> RendererFactoryCachePtr;

class ImageAtlasManager;
typedef IntrusivePtr<ImageAtlasManager> ImageAtlasManagerPtr;

/**
 * @copydoc Toolkit::RendererFactory
 */
class RendererFactory : public BaseObject
{
public:

  enum RendererType
  {
    COLOR,
    BORDER,
    GRADIENT,
    IMAGE,
    N_PATCH,
    SVG,
    MESH,
    PRIMITIVE,
    UNDEFINED
  };

  /**
   * @brief Constructor
   *
   * @param[in] debugEnabled If true, use debug renderer to replace all the concrete renderer.
   */
  RendererFactory( bool debugEnabled );

  /**
   * @copydoc Toolkit::RenderFactory::CreateControlRenderer( const Property::Map& )
   */
  Toolkit::ControlRenderer CreateControlRenderer( const Property::Map& propertyMap );

  /**
   * @copydoc Toolkit::RenderFactory::CreateControlRenderer( const Image& )
   */
  Toolkit::ControlRenderer CreateControlRenderer( const Image& image );

  /**
   * @copydoc Toolkit::RenderFactory::CreateControlRenderer( const std::string&, ImageDimensions )
   */
  Toolkit::ControlRenderer CreateControlRenderer( const std::string& image, ImageDimensions size );

public:
  /**
   * @brief Returns an image to be used when a renderer has failed to correctly render
   */
  static Image GetBrokenRendererImage();

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~RendererFactory();

private:

  /**
   * Get the renderer type from the property map.
   *
   * @param[in] propertyMap The map contains the properties of the control renderer
   * @return The rendererType
   */
  RendererType GetRendererType( const Property::Map& propertyMap );

  /**
   * Prepare the atlas manager
   */
  void CreateAtlasManager();

  /**
   * Undefined copy constructor.
   */
  RendererFactory(const RendererFactory&);

  /**
   * Undefined assignment operator.
   */
  RendererFactory& operator=(const RendererFactory& rhs);

private:

  RendererFactoryCachePtr mFactoryCache;
  ImageAtlasManagerPtr    mAtlasManager;
  bool                    mDebugEnabled;
};

} // namespace Internal

inline const Internal::RendererFactory& GetImplementation(const Toolkit::RendererFactory& factory)
{
  DALI_ASSERT_ALWAYS( factory && "RendererFactory handle is empty" );

  const BaseObject& handle = factory.GetBaseObject();

  return static_cast<const Internal::RendererFactory&>(handle);
}

inline Internal::RendererFactory& GetImplementation(Toolkit::RendererFactory& factory)
{
  DALI_ASSERT_ALWAYS( factory && "RendererFactory handle is empty" );

  BaseObject& handle = factory.GetBaseObject();

  return static_cast<Internal::RendererFactory&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_RENDERER_FACTORY_IMPL_H__ */
