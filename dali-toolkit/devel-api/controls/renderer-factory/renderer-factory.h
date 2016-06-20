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
#include <dali/public-api/object/property-map.h>

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
  ControlRenderer CreateControlRenderer( const Property::Map& propertyMap  );

  /**
   * @brief Request the control renderer to render the image.
   *
   * @param[in] image The image to be rendered.
   * @return The pointer pointing to the control renderer
   */
  ControlRenderer CreateControlRenderer( const Image& image );

  /**
   * @brief Request the control renderer to render the given resource at the url.
   *
   * @param[in] url The URL to the resource to be rendered.
   * @param[in] size The width and height to fit the loaded image to.
   * @return The pointer pointing to the control renderer
   */
  ControlRenderer CreateControlRenderer( const std::string& url, ImageDimensions size );

private:

  explicit DALI_INTERNAL RendererFactory(Internal::RendererFactory *impl);

};


/**
 * @brief Template to allow discard old renderer, get new one and set it on stage if possible
 *
 * @tparam ParameterType0 The type of first argument passed to the CreateControlRenderer()
 * @tparam ParameterType1 The type of second argument passed to the CreateControlRenderer()
 * @SINCE_1_0.39
 * @param[in] actor Actor for which the renderer will be replaced
 * @param[in,out] renderer The renderer object to be replaced
 * @param[in] param0 First template based argument passed to the renderer factory
 * @param[in] param1 Second template based argument passed to the renderer factory
 */
template< class ParameterType0, class ParameterType1 >
void InitializeControlRenderer( Actor& actor, ControlRenderer& renderer, ParameterType0& param0, ParameterType1& param1 )
{
  renderer.RemoveAndReset( actor );
  renderer = Toolkit::RendererFactory::Get().CreateControlRenderer( param0, param1 );
  if( renderer && actor && actor.OnStage() )
  {
    renderer.SetOnStage( actor );
  }
}

/**
 * @brief Template to allow discard old renderer, get new one and set it on stage if possible
 *
 * @tparam ParameterType The type of argument passed to the CreateControlRenderer()
 * @SINCE_1_0.39
 * @param[in] actor Actor for which the renderer will be replaced
 * @param[in,out] renderer The renderer object to be replaced
 * @param[in] param Template based argument passed to the renderer factory
 */
template< class ParameterType >
void InitializeControlRenderer( Actor& actor, ControlRenderer& renderer, ParameterType& param )
{
  renderer.RemoveAndReset( actor );
  renderer =  Toolkit::RendererFactory::Get().CreateControlRenderer( param );
  if( renderer && actor && actor.OnStage() )
  {
    renderer.SetOnStage( actor );
  }
}

} // namespace Toolkit

} // namespace Dali


#endif /* __DALI_TOOLKIT_RENDERER_FACTORY_H__ */
