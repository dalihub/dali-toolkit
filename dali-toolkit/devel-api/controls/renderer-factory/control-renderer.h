#ifndef __DALI_TOOLKIT_CONTROL_RENDERER_H__
#define __DALI_TOOLKIT_CONTROL_RENDERER_H__
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
#include <dali/public-api/actors/actor.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class ControlRenderer;
}

/**
 * ControlRenderer provides renderer for rendering the controls. A control may have multiple ControlRenders.
 * ControlRenderers reuses geometry, shader etc. across controls and manages the renderer and material to exist only when control is on-stage.
 * It also responds to actor size and color change, and provides the clipping at the renderer level.
 * Note: The control renderer responds to the the Actor::COLOR by blending it with the 'Multiply' operator.
 */
class DALI_IMPORT_API ControlRenderer : public BaseHandle
{
public:

  ControlRenderer();
  ControlRenderer(Internal::ControlRenderer *impl);
  ~ControlRenderer();

  /**
   * Set the size of the painting area.
   *
   * @param[in] size The size of the painting area.
   */
  void SetSize( const Vector2& size );

  /**
   * Set the clip rectangular of this renderer.
   * The contents of the renderer will not be visible outside this rectangular.
   *
   * @param [in] clipRect The clipping rectangular.
   */
  void SetClipRect( const Rect<int>& clipRect );

  /**
   * Reposition this renderer with a 2D offset.
   *
   * @param[in] offset The offset to reposition the renderer.
   */
  void SetOffset( const Vector2& offset );

  /**
   * Set the depth index of this renderer.
   * Depth-index controls draw-order for overlapping renderers.
   * Renderer with higher depth indices are rendered in front of other renderer with smaller values
   *
   * @param[in] depthIndex The depth index of this renderer.
   */
  void SetDepthIndex( float index );

  /**
   * Renderer only exists when control is on stage.
   * This function should be called when the control put on stage.
   *
   * @param[in] actor The actor applying this renderer.
   */
  void SetOnStage( Actor& actor );

  /**
   * Renderer is destroyed when control is off stage.
   * This function should be called when the control removes from stage
   *
   * @param[in] actor The actor applying this renderer.
   */
  void SetOffStage( Actor& actor );

};

} // namespace Toolkit

} // namespace Dali

#endif /*__DALI_TOOLKIT_CONTROL_RENDERER_H__*/
