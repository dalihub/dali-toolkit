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
 * @brief ControlRenderer provides renderer for rendering the controls. A control may have multiple ControlRenders.
 *
 * ControlRenderers reuses geometry, shader etc. across controls and manages the renderer and texture sets to exist only when control is on-stage.
 * It also responds to actor size and color change, and provides the clipping at the renderer level.
 * Note: The control renderer responds to the the Actor::COLOR by blending it with the 'Multiply' operator.
 */
class DALI_IMPORT_API ControlRenderer : public BaseHandle
{
public:

  /**
   * @brief Create an empty ControlRenderer Handle
   */
  ControlRenderer();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ControlRenderer();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param[in] handle A reference to the copied handle.
   */
  ControlRenderer( const ControlRenderer& handle );

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] handle  A reference to the copied handle.
   * @return A reference to this.
   */
  ControlRenderer& operator=( const ControlRenderer& handle );

  /**
   * @brief Set the size of the painting area.
   *
   * @param[in] size The size of the painting area.
   */
  void SetSize( const Vector2& size );

  /**
   * @brief Get the size of the painting area.
   *
   * @return The size of the renderer's painting area.
   */
  const Vector2& GetSize() const;

  /**
   * @brief Return the natural size of the renderer.
   *
   * Deriving classes stipulate the natural size and by default a renderer has a ZERO natural size.
   *
   * @param[out] naturalSize The renderer's natural size
   */
  void GetNaturalSize( Vector2& naturalSize ) const;

  /**
   * @brief Set the depth index of this renderer.
   *
   * Depth-index controls draw-order for overlapping renderers.
   * Renderer with higher depth indices are rendered in front of other renderer with smaller values
   *
   * @param[in] index The depth index of this renderer.
   */
  void SetDepthIndex( float index );

  /**
   * @brief Get the depth index of this renderer
   *
   * @return The depth index of this renderer.
   */
  float GetDepthIndex() const;

  /**
   * @brief Renderer only exists when control is on stage.
   *
   * This function should be called when the control put on stage.
   *
   * @param[in] actor The actor applying this renderer.
   * @post SetOffStage should be called with the same actor when the control is put off stage otherwise memory will be leaked
   */
  void SetOnStage( Actor& actor );

  /**
   * @brief Renderer is destroyed when control is off stage.
   *
   * This function should be called when the control removes from stage
   *
   * @param[in] actor The actor applying this renderer.
   */
  void SetOffStage( Actor& actor );

  /**
   * @brief Remove the renderer from actor and reset the control renderer self.
   *
   * This function can be called with an empty handle. If the control renderer is empty, do nothing.
   *
   * @param[in] actor The actor to be set off stage.
   */
  void RemoveAndReset( Actor& actor );

  /**
   * @brief Create the property map representing this renderer.
   *
   * @param[out] map The renderer property map.
   */
  void CreatePropertyMap( Property::Map& map ) const;

public: // Not intended for application developers

  explicit DALI_INTERNAL ControlRenderer(Internal::ControlRenderer *impl);

};

} // namespace Toolkit

} // namespace Dali

#endif /*__DALI_TOOLKIT_CONTROL_RENDERER_H__*/
