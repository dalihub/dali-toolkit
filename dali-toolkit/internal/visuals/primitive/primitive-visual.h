#ifndef DALI_TOOLKIT_INTERNAL_PRIMITIVE_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_PRIMITIVE_VISUAL_H

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

/*
 * The geometry creation logic was based off similar methods provided by freeGLUT.
 * Original copyright and licence information:
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Fri Dec 3 1999
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAWEL W. OLSZTA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

// EXTERNAL INCLUDES
#include <dali/public-api/common/intrusive-ptr.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/primitive-visual-properties.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class PrimitiveVisual;
typedef IntrusivePtr< PrimitiveVisual > PrimitiveVisualPtr;

/**
 * The visual which renders a simple 3D shape to the control's quad
 *
 * Primitives are created with clockwise winding and back-face culling by default.
 *
 * The following properties are required to create a PrimitiveRender
 *
 * | %Property Name  | Type        |
 * |-----------------|-------------|
 * | shape           | STRING      |
 *
 * In addition, the following properties can be (optionally) supplied to modify the shape's parameters
 *
 * | %Property Name    | Type        | Shapes Affected                          |
 * |-------------------|-------------|------------------------------------------|
 * | shapeColor        | VECTOR4     | all                                      |
 * | slices            | INTEGER     | sphere, cone, conical frustrum, cylinder |
 * | stacks            | INTEGER     | sphere                                   |
 * | scaleTopRadius    | FLOAT       | conical frustrum                         |
 * | scaleBottomRadius | FLOAT       | cone, conical frustrum                   |
 * | scaleHeight       | FLOAT       | cone, conical frustrum, cylinder         |
 * | scaleRadius       | FLOAT       | cylinder                                 |
 * | scaleDimensions   | VECTOR3     | cube, octahedron, bevelled cube          |
 * | bevelPercentage   | FLOAT       | bevelled cube                            |
 * | bevelSmoothness   | FLOAT       | bevelled cube                            |
 *
 * Note: slices and stacks both have an upper limit of 255.
 *
 * Finally, the following can be used to affect the visual's shader
 *
 * | %Property Name  | Type        | Representing                            |
 * |-----------------|-------------|-----------------------------------------|
 * | lightPosition   | VECTOR3     | The position (on stage) of the light    |
 */
class PrimitiveVisual: public Visual::Base
{
public:

  /**
   * @brief Create a new primitive visual.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @return A smart-pointer to the newly allocated visual.
   */
  static PrimitiveVisualPtr New( VisualFactoryCache& factoryCache );

public:  // from Visual

  /**
   * @copydoc Visual::Base::SetSize
   */
  virtual void SetSize( const Vector2& size );

  /**
   * @copydoc Visual::Base::GetNaturalSize
   */
  virtual void GetNaturalSize( Vector2& naturalSize ) const;

  /**
   * @copydoc Visual::Base::CreatePropertyMap
   */
  virtual void DoCreatePropertyMap( Property::Map& map ) const;

  /**
   * @copydoc Visual::Base::DoSetProperty
   */
  virtual void DoSetProperty( Dali::Property::Index index, const Dali::Property::Value& propertyValue );

  /**
   * @copydoc Visual::Base::DoGetProperty
   */
  virtual Dali::Property::Value DoGetProperty( Dali::Property::Index index );

protected:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   */
  PrimitiveVisual( VisualFactoryCache& factoryCache );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~PrimitiveVisual();

  /**
   * @copydoc Visual::Base::DoInitialize
   */
  virtual void DoInitialize( Actor& actor, const Property::Map& propertyMap );

  /**
   * @copydoc Visual::Base::DoSetOnStage
   */
  virtual void DoSetOnStage( Actor& actor );

private:

  //Simple struct to store the position and normal of a single vertex.
  struct Vertex
  {
    Vertex()
    {}

    Vertex( const Vector3& position, const Vector3& normal, const Vector2& textureCoord )
    : position( position ), normal( normal )
    {}

    Vector3 position;
    Vector3 normal;
  };

  /**
   * @brief Initialize the renderer with the geometry and shader from the cache, if not available, create and save to the cache for sharing.
   */
  void InitializeRenderer();

  /**
     * @brief Create a shader for the object to use.
     */
  void CreateShader();

  /**
   * @brief Update shader related info, uniforms, etc. for the new shader.
   */
  void UpdateShaderUniforms();

  /**
   * @brief Create the geometry of the given primitive type.
   */
  void CreateGeometry();

  /**
   * @brief Compute the vertices and the triangles for a sphere.
   * @param[in, out] vertices The vector of vertices.
   * @param[in, out] indices The vector of triangles, consisting of groups of three vertex indices.
   * @param[in] slices The number of slices as you go around the sphere. Affects the smoothness of the surface.
   * @param[in] stacks The number of stacks as you go down the sphere. Affects the smoothness of the surface.
   */
  void CreateSphere( Vector<Vertex>& vertices, Vector<unsigned short>& indices, int slices, int stacks );

  /**
   * @brief Compute the vertices and the triangles for a conic shape.
   * @param[in, out] vertices The vector of vertices.
   * @param[in, out] indices The vector of triangles, consisting of groups of three vertex indices.
   * @param[in] scaleTopRadius The scale of the radius of the top circle, compared to the other dimensions.
   * @param[in] scaleBottomRadius The scale of the radius of the bottom circle, compared to the other dimensions.
   * @param[in] scaleHeight The scale of the height of the object, compared to the other dimensions.
   * @param[in] slices The number of slices as you go around the conic shape. Affects the smoothness of the surface.
   */
  void CreateConic( Vector<Vertex>& vertices, Vector<unsigned short>& indices, float scaleTopRadius,
                           float scaleBottomRadius, float scaleHeight, int slices );

  /**
   * @brief Compute the vertices and the triangles for a bevelled cube.
   * @param[in, out] vertices The vector of vertices.
   * @param[in, out] indices The vector of triangles, consisting of groups of three vertex indices.
   * @Param[in] dimensions The dimensions of the object. Scales in the same fashion as a 9-patch image.
   * @param[in] bevelPercentage The ratio of the outer face widths to the cube's width. Between 0.0 and 1.0.
   * @param[in] bevelSmoothness The smoothness of the bevelled edges. Between 0.0 and 1.0.
   */
  void CreateBevelledCube( Vector<Vertex>& vertices, Vector<unsigned short>& indices, Vector3 dimensions,
                           float bevelPercentage, float bevelSmoothness );

  /**
   * @brief Computes look-up tables for sin and cos, over angle divisions of (2 * Pi) / divisions
   * @param[in, out] sinTable The table of sin values.
   * @param[in, out] cosTable The table of cos values.
   * @param[in] divisions Determines the angle coverage of the table. E.g divisions of '4' will have the sin values 0 = sin(0), 1 = sin(Pi/2), 2 = sin(Pi), 3 = sin(3Pi/2)
   * @Param[in] halfCircle If true, go from 0 to Pi instead of 0 to 2Pi.
   */
  void ComputeCircleTables( Vector<float>& sinTable, Vector<float>& cosTable, int divisions, bool halfCircle );

  /**
   * @brief Compute the vertices for a sphere.
   * @param[in, out] vertices The vector of vertices.
   * @param[in] slices The number of slices as you go around the sphere. Affects the smoothness of the surface.
   * @param[in] stacks The number of stacks as you go down the sphere. Affects the smoothness of the surface.
   */
  void ComputeSphereVertices( Vector<Vertex>& vertices, int slices, int stacks );

  /**
   * @brief Compute the triangles for a sphere.
   * @param[in, out] indices The vector of triangles, consisting of groups of three vertex indices.
   * @param[in] slices The number of slices as you go around the sphere. Affects the smoothness of the surface.
   * @param[in] stacks The number of stacks as you go down the sphere. Affects the smoothness of the surface.
   */
  void FormSphereTriangles( Vector<unsigned short>& indices, int slices, int stacks );

  /**
   * @brief Compute the vertices for a conical.
   * @param[in, out] vertices The vector of vertices.
   * @param[in] scaleTopRadius The scale of the radius of the top circle, compared to the other dimensions.
   * @param[in] scaleBottomRadius The scale of the radius of the bottom circle, compared to the other dimensions.
   * @param[in] scaleHeight The scale of the height of the object, compared to the other dimensions.
   * @param[in] slices The number of slices as you go around the conical. Affects the smoothness of the surface.
   */
  void ComputeConicVertices( Vector<Vertex>& vertices, float scaleTopRadius, float scaleBottomRadius,
                                    float scaleHeight, int slices );

  /**
   * @brief Compute the triangles for a conic.
   * @param[in, out] indices The vector of triangles, consisting of groups of three vertex indices.
   * @param[in] coneTop True if the top circle has a radius of zero, i.e. the object is a complete cone.
   * @param[in] coneBottom True if the bottom circle has a radius of zero, i.e. the object is an inverted complete cone.
   * @param[in] slices The number of slices as you go around the conic. Affects the smoothness of the surface.
   */
  void FormConicTriangles( Vector<unsigned short>& indices, float scaleTopRadius, float scaleBottomRadius,
                                  int slices );

  /**
   * @brief Compute the vertices for a cube.
   * @param[in, out] vertices The vector of vertices.
   * @Param[in] dimensions The dimensions of the object.
   */
  void ComputeCubeVertices( Vector<Vertex>& vertices, Vector3 dimensions );

  /**
   * @brief Compute the triangles for a cube.
   * @param[in, out] indices The vector of triangles, consisting of groups of three vertex indices.
   */
  void FormCubeTriangles( Vector<unsigned short>& indices );

  /**
   * @brief Compute the vertices for an octahedron (maximumly bevelled cube).
   * @param[in, out] vertices The vector of vertices.
   * @Param[in] dimensions The dimensions of the object.
   * @Param[in] smoothness Defines how rounded the edges appear under lighting. Between 0.0 and 1.0.
   */
  void ComputeOctahedronVertices( Vector<Vertex>& vertices, Vector3 dimensions, float smoothness );

  /**
   * @brief Compute the triangles for an octahedron.
   * @param[in, out] indices The vector of triangles, consisting of groups of three vertex indices.
   */
  void FormOctahedronTriangles( Vector<unsigned short>& indices );

  /**
   * @brief Compute the vertices for a bevelled cube.
   * @param[in, out] vertices The vector of vertices.
   * @Param[in] dimensions The dimensions of the object. Scales in the same fashion as a 9-patch image.
   * @param[in] bevelPercentage The ratio of the outer face widths to the cube's width. Between 0.0 and 1.0.
   * @param[in] bevelSmoothness The smoothness of the bevelled edges. Between 0.0 and 1.0.
   */
  void ComputeBevelledCubeVertices( Vector<Vertex>& vertices, Vector3 dimensions, float bevelPercentage,
                                    float bevelSmoothness );

  /**
   * @brief Compute the triangles for a bevelled cube.
   * @param[in, out] indices The vector of triangles, consisting of groups of three vertex indices.
   */
  void FormBevelledCubeTriangles( Vector<unsigned short>& indices );

private:

  // Undefined
  PrimitiveVisual( const PrimitiveVisual& PrimitiveVisual );

  // Undefined
  PrimitiveVisual& operator=( const PrimitiveVisual& PrimitiveVisual );

private:
  Shader mShader;
  Geometry mGeometry;

  Vector4 mColor;                //Color of shape.
  Vector3 mObjectDimensions;     //Dimensions of shape, scaled to be between 0.0 and 1.0.

  Vector3 mSceneCenter;
  Vector3 mSceneSize;

  //Shader properties.
  Vector3 mLightPosition;

  //Shape properties.
  Vector3 mScaleDimensions;      ///< Scale of dimensions of bevelled cube and sub-shapes.
  float   mScaleTopRadius;       ///< Scale of radius of top circle, to use when creating certain objects.
  float   mScaleBottomRadius;    ///< Scale of radius of bottom circle, to use when creating certain objects.
  float   mScaleHeight;          ///< Scale of height, to use when creating certain objects.
  float   mScaleRadius;          ///< Scale of radius, to use when creating certain objects.
  float   mBevelPercentage;      ///< Used to determine bevel amount when creating certain objects.
  float   mBevelSmoothness;      ///< Used to determine the smoothness of bevelled edges.
  int     mSlices;               ///< Number of slices to use when creating certain objects.
  int     mStacks;               ///< Number of stacks to use when creating certain objects.

  Toolkit::PrimitiveVisual::Shape::Type mPrimitiveType;  //Shape to render, as enum.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_PRIMITIVE_VISUAL_H */
