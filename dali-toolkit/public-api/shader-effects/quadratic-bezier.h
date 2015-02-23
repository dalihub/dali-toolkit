#ifndef __DALI_TOOLKIT_QUADRATIC_BEZIER_H__
#define __DALI_TOOLKIT_QUADRATIC_BEZIER_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/shader-effects/shader-effect.h>

namespace Dali
{

namespace Toolkit
{
/**
 * @brief QuadraticBezier is a custom shader to render quadratic bezier curves and bounded regions.
 *
 * Implementation based on the paper "Resolution Independent Curve Rendering using Programmable Graphics Hardware"
 * by Charles Loop and Jim Blinn.
 *
 * The coordinates of the control points is passed to the shader via uniforms.
 * aNormal attribute is used to pass the coefficients of each control point (xy coordinates) as well as the vertex ID (z coordinate).
 * A quadratic curve should have as normal for the first control point (0.0,0.0), (0.5,0.0) for the second and (1.0,1.0) for the third.
 * Triangles that do not contain curves should have coordinates (0.0,1.0) for each control point in order to be filled properly.
 */

class DALI_IMPORT_API QuadraticBezier : public ShaderEffect
{
public:

  /**
   * @brief Create an uninitialized QuadraticBezier; this can be initialized with QuadraticBezier::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  QuadraticBezier();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~QuadraticBezier();

  /**
   * @brief Create an initialized QuadraticBezier.
   *
   * @param[in] pointCount The maximum number of vertices
   * @param[in] filled Specify whether the the bounded region should be filled or not
   * @return A handle to a newly allocated Dali resource.
   */
  static QuadraticBezier New(unsigned int pointCount, bool filled );

  /**
   * @brief Set position coordinates for a point in the curve
   *
   * @param[in] index The index of the vertex
   * @param[in] position The new position
   */
  void SetPoint( unsigned int index, const Vector3& position );

  /**
   * @brief Set the width of the pathThis is only for not filled curves
   *
   * @param[in] width Width of the line in pixels
   */
  void SetLineWidth( float width );

  /**
   * @brief Sets the color of the curve
   *
   * @param[in] color The new color
   */
  void SetColor( const Vector4& color );

  /**
   * @brief Get the name of a the point property given its index
   *
   * @param[in] index Index of the vertex
   * @return A std::string containing the property name
   */
  std::string GetPointPropertyName( unsigned int index ) const;

  /**
   * @brief Get the name of the line width property
   *
   * @return A std::string containing the property name
   */
  std::string GetLineWidthPropertyName( ) const;

  /**
   * @brief Get the name of the color property
   *
   * @return A std::string containing the property name
   */
  std::string GetColorPropertyName( ) const;

private: // Not intended for application developers

  DALI_INTERNAL QuadraticBezier(ShaderEffect handle);

};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_IMAGE_REGION_EFFECT_H__
