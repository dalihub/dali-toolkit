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
 * @brief Visual provides a renderer for rendering the controls. A control may have multiple visuals.
 *
 * Visuals reuses geometry, shader etc. across controls and manages the renderer and texture sets to exist only when control is on-stage.
 * It also responds to actor size and color change, and provides the clipping at the renderer level.
 * Note: The visual responds to the the Actor::COLOR by blending it with the 'Multiply' operator.
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
   * @brief Set the size of the painting area.
   *
   * @param[in] size The size of the painting area.
   */
  void SetSize( const Vector2& size );

  /**
   * @brief Get the size of the painting area.
   *
   * @return The size of the visual's painting area.
   */
  const Vector2& GetSize() const;

  /**
   * @brief Returns the height for a given width.
   *
   * @param[in] width Width to use.
   *
   * @return The height based on the width.
   */
  float GetHeightForWidth( float width ) const;

  /**
   * @brief Return the natural size of the visual.
   *
   * Deriving classes stipulate the natural size and by default a visual has a ZERO natural size.
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
   * @brief Visual needs to know when the control is put on to the stage to add the renderer.
   *
   * This function should be called when the control is put on to the stage.
   *
   * @param[in] actor The actor using this visual.
   * @post SetOffStage should be called with the same actor when the control is put off stage otherwise memory will be leaked
   */
  void SetOnStage( Actor& actor );

  /**
   * @brief Visual needs to know when the control is removed from the stage to remove the renderer.
   *
   * This function should be called when the control is removed from the stage
   *
   * @param[in] actor The actor using this visual.
   */
  void SetOffStage( Actor& actor );

  /**
   * @brief Remove the renderer from the actor and reset the visual self.
   *
   * This function can be called with an empty handle. If the visual is empty, this is a no-op.
   *
   * @param[in] actor The actor to be set off stage.
   */
  void RemoveAndReset( Actor& actor );

  /**
   * @brief Create the property map representing this visual.
   *
   * @param[out] map The visual property map.
   */
  void CreatePropertyMap( Dali::Property::Map& map ) const;

  /**
   * @brief Sets the value of an existing property.
   *
   * @param [in] index The index of the property.
   * @param [in] propertyValue The new value of the property.
   */
  void SetProperty( Dali::Property::Index index, const Dali::Property::Value& propertyValue );

  /**
   * @brief Retrieves a property value.
   *
   * @param [in] index The index of the property.
   *
   * @return The property value.
   */
  Dali::Property::Value GetProperty( Dali::Property::Index index );

public: // Not intended for application developers

  explicit DALI_INTERNAL Base(Internal::Visual::Base *impl);

};

} // namespace Visual

} // namespace Toolkit

} // namespace Dali

#endif /*DALI_TOOLKIT_VISUAL_BASE_H*/
