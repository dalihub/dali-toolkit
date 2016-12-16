#ifndef DALI_TOOLKIT_VISUAL_BASE_H
#define DALI_TOOLKIT_VISUAL_BASE_H
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

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/actors/actor.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
namespace Visual
{
class Base;
}
}

namespace Visual
{
/**
 * @brief A Visual provides a renderer for drawing a control component. A control may have multiple visuals.
 *
 * Visuals reuse geometry, shader etc. across controls. They ensure that the renderer and texture sets exist only when control is on-stage.
 * Each visual also responds to actor size and color change, and provides clipping at the renderer level.
 * Note: The visual responds to the the Actor::COLOR by blending it with the 'Multiply' operator.
 *
 * The following properties are optional, but can be supplied in the property map to Dali::Toolkit::VisualFactory::CreateVisual().
 *
 * | %Property Name          | Type             |
 * |-------------------------|------------------|
 * | customShader            | MAP              |
 * | transform               | MAP              |
 *
 * where \b customShader is a map with at least one of the following properties:
 * | %Property Name          | Type             | Required | Default | Description |
 * |-------------------------|------------------|----------|---------|-------------|
 * | vertexShader            | STRING           | No | "" | Vertex shader code|
 * | fragmentShader          | STRING           | No | "" | Fragment shader code|
 * | subdivideGridX          | INTEGER          | No | 1 | How to subdivide the grid along X |
 * | subdivideGridY          | INTEGER          | No | 1 | How to subdivide the grid along Y |
 * | shaderHints             | INTEGER or ARRAY of STRING | No | NONE | Bitmask of hints @sa Dali::Shader::Hint |
 *
 * and \b transform is a map with the following properties:
 * | %Property Name          | Type             | Required | Default |Description |
 * |-------------------------|------------------|----------|---------|------------|
 * | offset                  | VECTOR2          | No | (0,0) | Offset of visual from origin |
 * | size                    | VECTOR2          | No | (1,1) | size of visual |
 * | origin                  | INTEGER or STRING | No | CENTER | origin of the visual @sa Dali::Toolkit::Align |
 * | anchorPoint             | INTEGER or STRING | No | CENTER | anchor point of the visual @sa Dali::Toolkit::Align |
 * | offsetSizeMode          | VECTOR4          | No | (0,0,0,0) | See below |
 *
 *
 * offsetSizeMode describes whether the offset and the size are
 * relative or absolute by using 0 or 1 respectively in the corresponding
 * components (offsetSizeMode.xy for offset.xy; offsetSizeMode.zw for size.xy).
 *
 * Relative means that the component describes a factor of the parent control size;
 * size.x = 1 means full width; size.y = 0.5 means half height.
 *
 * Absolute means that the component describes world units (equivalent to pixels)
 *
 */
class DALI_IMPORT_API Base : public BaseHandle
{
public:

  /**
   * @brief Create an empty Visual Handle
   */
  Base();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Base();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param[in] handle A reference to the copied handle.
   */
  Base( const Base& handle );

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] handle  A reference to the copied handle.
   * @return A reference to this.
   */
  Base& operator=( const Base& handle );

  /**
   * @brief Set the name of the visual
   *
   * Used by the styling system to animate properties
   * @param[in] name The name to give the visual
   */
  void SetName( const std::string& name );

  /**
   * @brief Get the name of the visual
   *
   * Used by the styling system to animate properties
   * @return The name of the visual
   */
  const std::string& GetName();

  /**
   * @brief Sets the transform and the control size
   *
   * @param[in] transform A property map describing the transform
   * @param[in] controlSize The size of the parent control for visuals that need to scale internally.
   */
  void SetTransformAndSize( const Dali::Property::Map& transform, Size controlSize );

  /**
   * @brief Returns the height for a given width.
   *
   * @param[in] width Width to use.
   *
   * @return The height based on the width.
   */
  float GetHeightForWidth( float width );

  /**
   * @brief Returns the width for a given height.
   *
   * @param[in] height Height to use.
   *
   * @return The width based on the height.
   */
  float GetWidthForHeight( float height );

  /**
   * @brief Return the natural size of the visual.
   *
   * Deriving classes stipulate the natural size and by default a
   * visual has a ZERO natural size.
   *
   * @note A visual may not actually have a natural size until it has
   * been placed on stage and acquired all it's resources.
   *
   * @param[out] naturalSize The visual's natural size
   */
  void GetNaturalSize( Vector2& naturalSize );

  /**
   * @brief Set the depth index of this visual.
   *
   * Depth-index controls draw-order for overlapping visuals.
   * Visuals with higher depth indices are rendered in front of other visual with smaller values
   *
   * @param[in] index The depth index of this visual.
   */
  void SetDepthIndex( float index );

  /**
   * @brief Get the depth index of this visual
   *
   * @return The depth index of this visual.
   */
  float GetDepthIndex() const;

  /**
   * @brief Create the property map representing this visual.
   *
   * @param[out] map The visual property map.
   */
  void CreatePropertyMap( Dali::Property::Map& map ) const;

public: // Not intended for application developers

  explicit DALI_INTERNAL Base(Internal::Visual::Base *impl);

};

} // namespace Visual

} // namespace Toolkit

} // namespace Dali

#endif /*DALI_TOOLKIT_VISUAL_BASE_H*/
