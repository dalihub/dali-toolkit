#ifndef __DALI_TOOLKIT_INTERNAL_CONTROL_RENDERER_H__
#define __DALI_TOOLKIT_INTERNAL_CONTROL_RENDERER_H__

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
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/devel-api/rendering/shader.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/control-renderer.h>
#include <dali-toolkit/internal/controls/renderers/renderer-factory-cache.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * Base class for all Control rendering logic. A control may have multiple control renderers.
 *
 * Note: The control renderer responds to the the Actor::COLOR by blending it with the 'Multiply' operator.
 *
 * The following properties are optional
 *
 * | %Property Name            | Type             |
 * |---------------------------|------------------|
 * | custom-shader             | MAP              |
 *
 * where custom-shader is a map with the following properties:
 * | %Property Name            | Type             |
 * |---------------------------|------------------|
 * | vertex-shader             | STRING           |
 * | fragment-shader           | STRING           |
 * | subdivide-grid-x          | INT              |
 * | subdivide-grid-y          | INT              |
 * | shader-hints              | INT              |
 */
class ControlRenderer : public BaseObject
{
public:

  /**
   * Initialisation of the renderer, this API should only called by the RendererFactory:
   *  request the geometry and shader from the cache, if not available, create and save to the cache for sharing;
   *  record the property values.
   *
   * @param[in] propertyMap The properties for the requested ControlRenderer object.
   */
  void Initialize( const Property::Map& propertyMap );

  /**
   * @copydoc Toolkit::ControlRenderer::SetSize
   */
  virtual void SetSize( const Vector2& size );

  /**
   * @copydoc Toolkit::ControlRenderer::GetSize
   */
  const Vector2& GetSize() const;

  /**
   * @copydoc Toolkit::ControlRenderer::GetNaturalSize
   */
  virtual void GetNaturalSize( Vector2& naturalSize ) const;

  /**
   * ToDo: Add this function to Toolkit::ControlRenderer when it is fully implemented.
   *
   * Set the clip rectangular of this renderer.
   * The contents of the renderer will not be visible outside this rectangular.
   *
   * @param [in] clipRect The clipping rectangular.
   */
  virtual void SetClipRect( const Rect<int>& clipRect );

  /**
   *ToDo: Add this function to Toolkit::ControlRenderer when it is fully implemented.
   *
   * Reposition this renderer with a 2D offset.
   *
   * @param[in] offset The offset to reposition the renderer.
   */
  virtual void SetOffset( const Vector2& offset );

  /**
   * @copydoc Toolkit::ControlRenderer::SetDepthIndex
   */
  void SetDepthIndex( float index );

  /**
   * @copydoc Toolkit::ControlRenderer::GetDepthIndex
   */
  float GetDepthIndex() const;

  /**
   * @copydoc Toolkit::ControlRenderer::SetOnStage
   * @pre Impl->mGeometry must be created before this method is called
   */
  void SetOnStage( Actor& actor );

  /**
   * @copydoc Toolkit::ControlRenderer::SetOffStage
   */
  void SetOffStage( Actor& actor );

  /**
   * @copydoc Toolkit::ControlRenderer::CreatePropertyMap
   */
  void CreatePropertyMap( Property::Map& map ) const;

protected:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the RendererFactoryCache object
   */
  ControlRenderer( RendererFactoryCache& factoryCache );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~ControlRenderer();

protected:
  /**
   * @brief Called by CreatePropertyMap() allowing sub classes to respond to the CreatePropertyMap event
   *
   * @param[out] map The renderer property map.
   */
  virtual void DoCreatePropertyMap( Property::Map& map ) const = 0;

  /**
   * @brief Called by Initialize() allowing sub classes to respond to the Initialize event
   *
   * @param[in] factoryCache A pointer pointing to the RendererFactoryCache object
   * @param[in] propertyMap The properties for the requested ControlRenderer object.
   */
  virtual void DoInitialize( const Property::Map& propertyMap ) = 0;

  /**
   * @brief Initialises a renderer ready to be put on stage.
   *
   * @param[inout] renderer The Renderer to initialise. If the renderer is not empty then re-initialise the renderer
   */
  virtual void InitializeRenderer( Renderer& renderer ) = 0;

protected:

  /**
   * @brief Called by SetOnStage() allowing sub classes to respond to the SetOnStage event
   *
   * @param[in] actor The actor applying this renderer.
   */
  virtual void DoSetOnStage( Actor& actor );

  /**
   * @brief Called by SetOffStage() allowing sub classes to respond to the SetOffStage event
   *
   * @param[in] actor The actor applying this renderer.
   */
  virtual void DoSetOffStage( Actor& actor );

protected:

  /**
   * @brief Sets the key to use for caching the renderer. If this is empty then no caching will occur
   *
   * @param[in] cachedRendererKey The key to use for caching the renderer.
   */
  void SetCachedRendererKey( const std::string& cachedRendererKey );

private:

  // Undefined
  ControlRenderer( const ControlRenderer& renderer );

  // Undefined
  ControlRenderer& operator=( const ControlRenderer& renderer );

protected:
  struct Impl;
  Impl* mImpl;
  RendererFactoryCache& mFactoryCache;
};

} // namespace Internal

inline const Internal::ControlRenderer& GetImplementation(const Toolkit::ControlRenderer& renderer)
{
  DALI_ASSERT_ALWAYS( renderer && "ControlRenderer handle is empty" );

  const BaseObject& handle = renderer.GetBaseObject();

  return static_cast<const Internal::ControlRenderer&>(handle);
}

inline Internal::ControlRenderer& GetImplementation(Toolkit::ControlRenderer& renderer)
{
  DALI_ASSERT_ALWAYS( renderer && "ControlRenderer handle is empty" );

  BaseObject& handle = renderer.GetBaseObject();

  return static_cast<Internal::ControlRenderer&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_INTERNAL_CONTROL_RENDERER_H___ */
