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

#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/control-renderer.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class RendererFactoryCache;

/**
 * Base class for all Control rendering logic. A control may have multiple control renderers.
 *
 * Note: The control renderer responds to the the Actor::COLOR by blending it with the 'Multiply' operator.
 */
class ControlRenderer : public BaseObject
{
public:

  /**
   * Initialisation of the renderer, this API should only called by the RendererFactory:
   *  request the geometry and shader from the cache, if not available, create and save to the cache for sharing;
   *  record the property values.
   *
   * @param[in] factoryCache A pointer pointing to the RendererFactoryCache object
   * @param[in] propertyMap The properties for the requested ControlRenderer object.
   */
  virtual void Initialize( RendererFactoryCache& factoryCache, const Property::Map& propertyMap ) = 0;

  /**
   * @copydoc Toolkit::ControlRenderer::SetSize
   */
  virtual void SetSize( const Vector2& size );

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
   * @copydoc Toolkit::ControlRenderer::SetOnStage
   */
  virtual void SetOnStage( Actor& actor );

  /**
   * ToDo: Add this function to Toolkit::ControlRenderer when the Renderer can be removed from actor properly.
   *
   * Renderer is destroyed when control is off stage.
   * This function should be called when the control removes from stage
   *
   * @param[in] actor The actor applying this renderer.
   */
  void SetOffStage( Actor& actor );

protected:

  /**
   * @brief Constructor.
   */
  ControlRenderer();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~ControlRenderer();

private:

  // Undefined
  ControlRenderer( const ControlRenderer& renderer );

  // Undefined
  ControlRenderer& operator=( const ControlRenderer& renderer );

protected:

  struct Impl;
  Impl* mImpl;
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
