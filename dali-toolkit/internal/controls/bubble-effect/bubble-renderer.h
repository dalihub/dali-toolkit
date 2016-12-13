#ifndef DALI_TOOLKIT_INTERNAL_BUBBLE_RENDERER_H
#define DALI_TOOLKIT_INTERNAL_BUBBLE_RENDERER_H

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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/rendering/renderer.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * BubbleRenderer renders a group of bubbles.Each bubble can be moved separately.
 * Its custom shader achieves similar effect of particle system by applying on a specially created mesh
 * Each bubble is rendered on a patch with two triangles; and each mesh can contain multiple such patches, thus a group.
 */
class BubbleRenderer
{
public:

  /**
   * Constructor   *
   * @return A newly allocated object.
   */
  BubbleRenderer();

  /**
   * @brief Destructor
   */
  ~BubbleRenderer(){}

  /**
   * Prepare for the rendering: create the renderer, and register properties
   * @param[in] numberOfBubble How many groups of uniforms are used to control the bubble movement.
   * Note: Limited by the maximum available uniforms, this parameter cannot be bigger than 100.
   * Ideally use one group of uniform to control one bubble.
   * If the num of patches in the MeshActor is more than groups of uniforms,
   * the uniform values will be shared by multiple bubbles. Allow up to 9 times.
   * @param[in] movementArea The size of the bubble moving area, usually the same size as the background.
   * @param[in] geometry The geometry to be used by the renderer
   * @param[in] textureSet The texture set to be used by the renderer
   * @param[in] shader The shader set to be used by the renderer
   */
  void Initialize( unsigned int numberOfBubble, const Vector2& movementArea, Geometry geometry, TextureSet textureSet, Shader shader  );

  /**
   * Return the mesh actor which is used to display the bubbles
   */
  Renderer& GetRenderer();

  /**
   * Sets the geometry to be used by the renderer
   * @param[in] geometry The geometry to be used by the renderer
   */
  void SetGeometry( Geometry geometry );

  /**
   * Set the start and end positions of the index-th bubble's movement.
   * @param[in] index Indicate which bubble these properties are applied on.
   * @param[in] startAndEndPosition The start and the end position of movement.
   */
  void SetStartAndEndPosition( unsigned int index, const Vector4& startAndEndPosition );

  /**
   * Set the movement completed percentage of the index-th bubble.
   * The bubble will appear at start position when percentage equals to zero,
   * and disappear near end position (affected by gravity) when percentage equals to one.
   * This percentage property is used to animate the bubble movement.
   * @param[in] index Indicate which bubble this property is applied on.
   * @param[in] percentage Set the percentage property value ( between zero and one ).
   */
  void SetPercentage( unsigned int index, float percentage );

  /**
   * Set the gravity applied to the y direction, which makes the bubbles no longer moving on a straight line.
   * @param[in] gravity The gravity on the y direction.
   */
  void SetGravity( float gravity );

  /**
   * Set the scale factor applied to the bubbles
   * @param[in] scale The scale factor applied on all bubbles.
   */
  void SetDynamicScale( float scale );

  /**
   * Get the idx-th percentage property.
   * @param[in] idx The percentage property index.
   * @return the idx-th percentage property.
   */
  Property GetPercentageProperty( unsigned int idx );

  /**
   * Reset the uniform values to default.
   */
  void ResetProperties();

private:

  Renderer     mRenderer;

  //properties mapped as uniforms
  std::vector<Property::Index> mIndicesOffset;             ///< Indices of the properties mapping to uniform array 'uOffset'
  std::vector<Property::Index> mIndiceStartEndPos;         ///< Indices of the properties mapping to uniform array 'uStartAndEndPos'
  std::vector<Property::Index> mIndicesPercentage;         ///< Indices of the properties mapping to uniform array 'uPercentage'
  Property::Index              mIndexGravity;              ///< Index of the property mapping to uniform 'uGravity'
  Property::Index              mIndexDynamicScale;         ///< Index of the property mapping to uniform 'uDynamicScale'
  Property::Index              mIndexInvertedMovementArea; ///< Index of the property mapping to uniform 'uInvertedMovementArea'
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_BUBBLE_RENDERER_H
