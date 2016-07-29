#ifndef DALI_TOOLKIT_PRIMITIVE_VISUAL_PROPERTIES_H
#define DALI_TOOLKIT_PRIMITIVE_VISUAL_PROPERTIES_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{

namespace Toolkit
{

namespace PrimitiveVisual
{

namespace Property
{

enum
{
  /**
   * @brief The specific shape to render.
   * @details Name "shape", type Shape::Type (Property::INTEGER) or Property::STRING.
   * @SINCE_1_1.45
   * @note Optional. If not specified, the default is Shape::SPHERE.
   * @see Shape::Type
   */
  SHAPE = VISUAL_PROPERTY_START_INDEX,

  /**
   * @brief The color of the shape.
   * @details Name "shapeColor", type Property::VECTOR4.
   * @SINCE_1_1.45
   * @note Optional. If not specified, the default is Vector4(0.5, 0.5, 0.5, 1.0).
   * @note Applies to ALL shapes.
   */
  COLOR,

  /**
   * @brief The number of slices as you go around the shape.
   * @details Name "slices", type Property::INTEGER.
   * For spheres and conical frustrums, this determines how many divisions there are as you go around the object.
   * @SINCE_1_1.45
   * @note Optional. If not specified, the default is 128.
   * @note Applies to:
   *      - Shape::SPHERE
   *      - Shape::CONICAL_FRUSTRUM
   *      - Shape::CONE
   *      - Shape::CYLINDER
   * @note The range is from 1 to 255.
   */
  SLICES,

  /**
   * @brief The number of stacks as you go down the shape.
   * @details Name "stacks", type Property::INTEGER.
   * For spheres, 'stacks' determines how many layers there are as you go down the object.
   * @SINCE_1_1.45
   * @note Optional. If not specified, the default is 128.
   * @note Applies to:
   *      - Shape::SPHERE
   * @note The range is from 1 to 255.
   */
  STACKS,

  /**
   * @brief The scale of the radius of the top circle of a conical frustrum.
   * @details Name "scaleTopRadius", type Property::FLOAT.
   * @SINCE_1_1.45
   * @note Optional. If not specified, the default is 1.0f.
   * @note Applies to:
   *      - Shape::CONICAL_FRUSTRUM
   * @note Only values greater than or equal to 0.0f are accepted.
   */
  SCALE_TOP_RADIUS,

  /**
   * @brief The scale of the radius of the bottom circle of a conical frustrum.
   * @details Name "scaleBottomRadius", type Property::FLOAT.
   * @SINCE_1_1.45
   * @note Optional. If not specified, the default is 1.5f.
   * @note Applies to:
   *      - Shape::CONICAL_FRUSTRUM
   *      - Shape::CONE
   * @note Only values greater than or equal to 0.0f are accepted.
   */
  SCALE_BOTTOM_RADIUS,

  /**
   * @brief The scale of the height of a conic.
   * @details Name "scaleHeight", type Property::FLOAT.
   * @SINCE_1_1.45
   * @note Optional. If not specified, the default is 3.0f.
   * @note Applies to:
   *      - Shape::CONICAL_FRUSTRUM
   *      - Shape::CONE
   *      - Shape::CYLINDER
   * @note Only values greater than or equal to 0.0f are accepted.
   */
  SCALE_HEIGHT,

  /**
   * @brief The scale of the radius of a cylinder.
   * @details Name "scaleRadius", type Property::FLOAT.
   * @SINCE_1_1.45
   * @note Optional. If not specified, the default is 1.0f.
   * @note Applies to:
   *      - Shape::CYLINDER
   * @note Only values greater than or equal to 0.0f are accepted.
   */
  SCALE_RADIUS,

  /**
   * @brief The dimensions of a cuboid. Scales in the same fashion as a 9-patch image.
   * @details Name "scaleDimensions", type Property::VECTOR3.
   * @SINCE_1_1.45
   * @note Optional. If not specified, the default is Vector3::ONE.
   * @note Applies to:
   *      - Shape::CUBE
   *      - Shape::OCTAHEDRON
   *      - Shape::BEVELLED_CUBE
   * @note Each vector3 parameter should be greater than or equal to 0.0f.
   */
  SCALE_DIMENSIONS,

  /**
   * @brief Determines how bevelled the cuboid should be, based off the smallest dimension.
   * @details Name "bevelPercentage", type Property::FLOAT.
   * Bevel percentage ranges from 0.0 to 1.0. It affects the ratio of the outer face widths to the width of the overall cube.
   * @SINCE_1_1.45
   * @note Optional. If not specified, the default is 0.0f (no bevel).
   * @note Applies to:
   *      - Shape::BEVELLED_CUBE
   * @note The range is from 0.0f to 1.0f.
   */
  BEVEL_PERCENTAGE,

  /**
   * @brief Defines how smooth the bevelled edges should be.
   * @details Name "bevelSmoothness", type Property::FLOAT.
   * @SINCE_1_1.45
   * @note Optional. If not specified, the default is 0.0f (sharp edges).
   * @note Applies to:
   *      - Shape::BEVELLED_CUBE
   * @note The range is from 0.0f to 1.0f.
   */
  BEVEL_SMOOTHNESS,

  /**
   * @brief The position, in stage space, of the point light that applies lighting to the model.
   * @details Name "lightPosition", type Property::VECTOR3.
   * This is based off the stage's dimensions, so using the width and height of the stage halved will correspond to the center,
   * and using all zeroes will place the light at the top left corner.
   * @SINCE_1_1.45
   * @note Optional. If not specified, the default is an offset outwards from the center of the screen.
   * @note Applies to ALL shapes.
   */
  LIGHT_POSITION,
};

} // namespace Property

namespace Shape
{

/**
 * @brief The primitive shape to render.
 */
enum Type
{
  SPHERE, ///< A perfectly round geometrical object in three-dimensional space. @SINCE_1_1.45
  CONICAL_FRUSTRUM, ///< The area bound between two circles, i.e. a cone with the tip removed. @SINCE_1_1.45
  CONE, ///< Equivalent to a conical frustrum with top radius of zero. @SINCE_1_1.45
  CYLINDER, ///< Equivalent to a conical frustrum with equal radii for the top and bottom circles. @SINCE_1_1.45
  CUBE, ///< Equivalent to a bevelled cube with a bevel percentage of zero. @SINCE_1_1.45
  OCTAHEDRON, ///< Equivalent to a bevelled cube with a bevel percentage of one. @SINCE_1_1.45
  BEVELLED_CUBE, ///< A cube/cuboid with all edges flattened to some degree. @SINCE_1_1.45
};
}

} // namespace PrimitiveVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_PRIMITIVE_VISUAL_PROPERTIES_H
