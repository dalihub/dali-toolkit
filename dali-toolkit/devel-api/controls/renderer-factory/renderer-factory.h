#ifndef __DALI_TOOLKIT_RENDERER_FACTORY_H__
#define __DALI_TOOLKIT_RENDERER_FACTORY_H__
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

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/images/image-operations.h>

// INTERNAK INCLUDES
#include <dali-toolkit/devel-api/controls/renderer-factory/control-renderer.h>

namespace Dali
{
class Image;
class Vector4;

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class RendererFactory;
}

/**
 * @brief RendererFactory is a singleton object that provides and shares renderers for controls
 *
 * By setting environment variable 'DALI_DEBUG_RENDERING', all concrete renderer is replaced with the debug renderer which renders a quad wireframe.
 *
 * The renderer type is required in the property map for requesting a control renderer.
 *
 * | %Property Name           | Type             |
 * |--------------------------|------------------|
 * | rendererType             | STRING           |
 */
class DALI_IMPORT_API RendererFactory : public BaseHandle
{
public:

  /**
   * @brief Create or retrieve RendererFactory singleton.
   *
   * @return A handle to the RendererFactory control.
   */
  static RendererFactory Get();

  /**
   * @brief Create a RendererFactory handle.
   *
   * Calling member functions with an uninitialised handle is not allowed.
   */
  RendererFactory();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~RendererFactory();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param[in] handle A reference to the copied handle.
   */
  RendererFactory( const RendererFactory& handle );

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] handle  A reference to the copied handle.
   * @return A reference to this.
   */
  RendererFactory& operator=( const RendererFactory& handle );

  /**
   * @brief Request the control renderer
   *
   * @param[in] propertyMap The map contains the properties required by the control renderer
   *            Depends on the content of the map, different kind of renderer would be returned.
   * @return The pointer pointing to control renderer
   */
  ControlRenderer GetControlRenderer( const Property::Map& propertyMap  );

  /**
   * @brief Request the control renderer to render the given color
   *
   * @param[in] color The color to be rendered
   * @return The pointer pointing to the control renderer
   */
  ControlRenderer GetControlRenderer( const Vector4& color );

  /**
   * @brief Request the current control renderer to render the given color
   *
   * if the current renderer is a handle to an internal color renderer, set this color to it,
   * else the renderer would be a handle to a newly created internal color renderer.
   *
   * @param[in] renderer The ControlRenderer to reset
   * @param[in] actor The Actor the renderer is applied to if, empty if the renderer has not been applied to any Actor
   * @param[in] color The color to be rendered.
   */
  void ResetRenderer( ControlRenderer& renderer, Actor& actor, const Vector4& color );

  /**
   * @brief Request the control renderer to renderer the border with the given size and color.
   *
   * @param[in] borderSize The size of the border. Border size is the same along all edges.
   * @param[in] borderColor The color of the border.
   * @return The pointer pointing to the control renderer
   */
  ControlRenderer GetControlRenderer( float borderSize, const Vector4& borderColor );

  /**
   * @brief Request the control renderer to render the image.
   *
   * @param[in] image The image to be rendered.
   * @return The pointer pointing to the control renderer
   */
  ControlRenderer GetControlRenderer( const Image& image );

  /**
   * @brief Request the current control renderer to render the given image
   *
   * if the current renderer is a handle to an internal image renderer, set this image to it,
   * else the renderer would be a handle to a newly created internal image renderer.
   *
   * @param[in] renderer The ControlRenderer to reset
   * @param[in] actor The Actor the renderer is applied to if, empty if the renderer has not been applied to any Actor
   * @param[in] image The Image to be rendered.
   */
  void ResetRenderer( ControlRenderer& renderer, Actor& actor, const Image& image );

  /**
   * @brief Request the control renderer to render the given resource at the url.
   *
   * @param[in] url The URL to the resource to be rendered.
   * @param[in] size The width and height to fit the loaded image to.
   * @return The pointer pointing to the control renderer
   */
  ControlRenderer GetControlRenderer( const std::string& url,
                                      ImageDimensions size = ImageDimensions() );

  /**
   * @brief Request the current control renderer to render the given resource at the url
   *
   * if the current renderer is a handle to an internal image renderer, set this image to it,
   * else the renderer would be a handle to a newly created internal image renderer.
   *
   * @param[in] renderer The ControlRenderer to reset
   * @param[in] actor The Actor the renderer is applied to if, empty if the renderer has not been applied to any Actor
   * @param[in] url The URL to the resource to be rendered.
   * @param[in] size The width and height to fit the loaded image to.
   */
  void ResetRenderer( ControlRenderer& renderer, Actor& actor, const std::string& url,
                      ImageDimensions size = ImageDimensions() );


  /**
   * @brief Request the current control renderer from the property map, merging the property map with the renderer
   *
   * if the current renderer is capable of merging with the property map the reset the renderer with the merged properties
   * else the renderer would be a handle to a newly created internal renderer.
   *
   * @param[in] renderer The ControlRenderer to reset
   * @param[in] actor The Actor the renderer is applied to if, empty if the renderer has not been applied to any Actor
   * @param[in] propertyMap The map contains the properties required by the control renderer
   *            Depends on the content of the map, different kind of renderer would be returned.
   */
  void ResetRenderer( ControlRenderer& renderer, Actor& actor, const Property::Map& propertyMap );

private:

  explicit DALI_INTERNAL RendererFactory(Internal::RendererFactory *impl);

};

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_RENDERER_FACTORY_H__ */
