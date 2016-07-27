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

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>

namespace Dali
{
class Image;
class Vector4;

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class VisualFactory;
}

/**
 * @brief VisualFactory is a singleton object that provides and shares visuals between controls
 *
 * By setting environment variable 'DALI_DEBUG_RENDERING', a debug visual is used which renders a quad wireframe.
 *
 * The visual type is required in the property map for requesting a visual.
 *
 * | %Property Name           | Type             |
 * |--------------------------|------------------|
 * | rendererType             | STRING           |
 */
class DALI_IMPORT_API VisualFactory : public BaseHandle
{
public:

  /**
   * @brief Create or retrieve VisualFactory singleton.
   *
   * @return A handle to the VisualFactory control.
   */
  static VisualFactory Get();

  /**
   * @brief Create a VisualFactory handle.
   *
   * Calling member functions with an uninitialised handle is not allowed.
   */
  VisualFactory();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~VisualFactory();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param[in] handle A reference to the copied handle.
   */
  VisualFactory( const VisualFactory& handle );

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] handle  A reference to the copied handle.
   * @return A reference to this.
   */
  VisualFactory& operator=( const VisualFactory& handle );

  /**
   * @brief Request the visual
   *
   * @param[in] propertyMap The map contains the properties required by the control renderer
   *            Depends on the content of the map, different kind of renderer would be returned.
   * @return The pointer pointing to visual
   */
  Visual::Base CreateVisual( const Property::Map& propertyMap  );

  /**
   * @brief Request the visual to render the image.
   *
   * @param[in] image The image to be rendered.
   * @return The pointer pointing to the visual
   */
  Visual::Base CreateVisual( const Image& image );

  /**
   * @brief Request the visual to render the given resource at the url.
   *
   * @param[in] url The URL to the resource to be rendered.
   * @param[in] size The width and height to fit the loaded image to.
   * @return The pointer pointing to the visual
   */
  Visual::Base CreateVisual( const std::string& url, ImageDimensions size );

private:

  explicit DALI_INTERNAL VisualFactory(Internal::VisualFactory *impl);

};


/**
 * @brief Template to allow discard old visual, get new one and set it on stage if possible
 *
 * @tparam ParameterType0 The type of first argument passed to the CreateVisual()
 * @tparam ParameterType1 The type of second argument passed to the CreateVisual()
 * @SINCE_1_0.39
 * @param[in] actor Actor for which the renderer will be replaced
 * @param[in,out] visual The visual to be replaced
 * @param[in] param0 First template based argument passed to the renderer factory
 * @param[in] param1 Second template based argument passed to the renderer factory
 */
template< class ParameterType0, class ParameterType1 >
void InitializeVisual( Actor& actor, Visual::Base& visual, ParameterType0& param0, ParameterType1& param1 )
{
  visual.RemoveAndReset( actor );
  visual = Toolkit::VisualFactory::Get().CreateVisual( param0, param1 );
  if( visual && actor && actor.OnStage() )
  {
    visual.SetOnStage( actor );
  }
}

/**
 * @brief Template to allow discard old visual, get new one and set it on stage if possible
 *
 * @tparam ParameterType The type of argument passed to the CreateVisual()
 * @SINCE_1_0.39
 * @param[in] actor Actor for which the renderer will be replaced
 * @param[in,out] visual The visual to be replaced
 * @param[in] param Template based argument passed to the renderer factory
 */
template< class ParameterType >
void InitializeVisual( Actor& actor, Visual::Base& visual, ParameterType& param )
{
  visual.RemoveAndReset( actor );
  visual =  Toolkit::VisualFactory::Get().CreateVisual( param );
  if( visual && actor && actor.OnStage() )
  {
    visual.SetOnStage( actor );
  }
}

} // namespace Toolkit

} // namespace Dali


#endif /* __DALI_TOOLKIT_RENDERER_FACTORY_H__ */
