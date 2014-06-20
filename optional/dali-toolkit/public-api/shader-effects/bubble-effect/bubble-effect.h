#ifndef __DALI_TOOLKIT_SHADER_BUBBLE_EFFECT_H__
#define __DALI_TOOLKIT_SHADER_BUBBLE_EFFECT_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/dali.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

/**
 * BubbleEffect is a custom shader to achieve similar effect of particle system by applying on a specially created MeshActor
 * Each bubble is rendered on a patch with two triangles; and each mesh can contain multiple such patches.
 */
class BubbleEffect : public ShaderEffect
{
public:

  /**
   * Create an empty BubbleEffect handle
   */
  BubbleEffect();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~BubbleEffect();

  /**
   * Create an initialized BubbleEffect
   * @param[in] numberOfBubble How many groups of uniforms are used to control the bubble movement.
   * Note: Limited by the maximum available uniforms, this parameter cannot be bigger than 100.
   * Ideally use one group of uniform to control one bubble.
   * If the num of patches in the MeshActor is more than groups of uniforms,
   * the uniform values will be shared by multiple bubbles. Allow up to 9 times.
   * @param shapeImagePath File path of the image that will be used as a texture for each bubble.
   * @return A handle to a newly allocated Dali resource.
   */
  static BubbleEffect New( unsigned int numberOfBubble, const std::string& shapeImagePath);

  /**
   * Set the bubble movement area for the BubbleEffect
   * @param[in] movementArea The size of bubble movement area; by default, it is the stage size
   */
  void SetMovementArea( const Vector2& movementArea );

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

  /*
   * Set the width of shape image
   * If one image has multiple shape, bubble effect will parse one shape from the image randomly.
   * @param[in] imageWidth width of shape image
   */
  void SetShapeImageWidth( float imageWidth );

  /**
   * Set the scale factor applied to the bubbles
   * @param[in] scale The scale factor applied on all bubbles.
   */
  void SetDynamicScale( float scale );

  /**
   * Increase both the bubble size and moving speed.
   * Animate this peoperty to create special effect such as all the bubbles blow up on the screen.
   * @param[in] magnification The manified factor applied on the bubble size and moving speed.
   */
  void SetMagnification( float magnification );

  /**
   * Get the name for the idx-th percentage property.
   * @param[in] index The percentage property index.
   * @return std::string containing the property name.
   */
  std::string GetPercentagePropertyName( unsigned int index ) const;

  /**
   * Get the name for the magnification property.
   * @return std::string containinf the property name.
   */
  std::string GetMagnificationPropertyName() const;

  /**
   * Reset the uniform values to default.
   */
  void ResetParameters();

private:// Not intended for application developers

  BubbleEffect( ShaderEffect handle );

private:

  unsigned int mNumberOfBubbles;
  Vector2      mMovementArea;
};

} // namespace Toolkit

} // namespace Dali
#endif /* __DALI_TOOLKIT_SHADER_BUBBLE_EFFECT_H__ */
