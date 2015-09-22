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
 * RendererFactory is a singleton object that provides and shares renderers for controls
 *
 * The renderer type is required in the property map for requesting a control renderer.
 *
 * | %Property Name            | Type             |
 * |---------------------------|------------------|
 * | renderer-type             | STRING           |
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
   * @param[in] color The color to be rendered.
   * @return Whether a new internal control renderer is created.
   */
  bool ResetRenderer( ControlRenderer& renderer, const Vector4& color );

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
   * @param[in] image The Image to be rendered.
   * @return Whether a new internal control renderer is created.
   */
  bool ResetRenderer( ControlRenderer& renderer, const Image& image );

  /**
   * @brief Request the control renderer to render the given resource at the url.
   *
   * @param[in] url The URL to the resource to be rendered.
   * @return The pointer pointing to the control renderer
   */
  ControlRenderer GetControlRenderer( const std::string& url );

  /**
   * @brief Request the current control renderer to render the given resource at the url
   *
   * if the current renderer is a handle to an internal image renderer, set this image to it,
   * else the renderer would be a handle to a newly created internal image renderer.
   *
   * @param[in] renderer The ControlRenderer to reset
   * @param[in] url The URL to the resource to be rendered.
   * @return Whether a new internal control renderer is created.
   */
  bool ResetRenderer( ControlRenderer& renderer, const std::string& url );

private:

  explicit DALI_INTERNAL RendererFactory(Internal::RendererFactory *impl);

};

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_RENDERER_FACTORY_H__ */
