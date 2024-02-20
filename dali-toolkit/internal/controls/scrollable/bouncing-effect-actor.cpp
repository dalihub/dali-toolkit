/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali-toolkit/internal/controls/scrollable/bouncing-effect-actor.h>

// EXTERNAL INCLUDES
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/shader.h>
#include <dali/public-api/rendering/texture-set.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
// Bouncing effect is presented by stacked three layers with same color and opacity
const float LAYER_HEIGHTS[5] =
  {
    1.f,
    26.f * 4.f / 130.f,
    26.f * 3.f / 130.f,
    26.f * 2.f / 130.f,
    26.f / 130.f};

} // namespace

Actor CreateBouncingEffectActor(Property::Index& bouncePropertyIndex)
{
  // Create the bouncing mesh geometry
  struct VertexPosition
  {
    Vector3 position1;
    Vector3 position2;
  };
  // 4 vertices 2 triangles per layer. The depth interval between each layer is 0.01
  VertexPosition vertexData[20] = {
    // bottom layer
    {Vector3(-0.5f, -0.5f, 0.f), Vector3(-0.5f, -0.5f, 0.f)},
    {Vector3(0.5f, -0.5f, 0.f), Vector3(0.5f, -0.5f, 0.f)},
    {Vector3(-0.5f, -0.5f, 0.f), Vector3(-0.5f, -0.5f + LAYER_HEIGHTS[0], 0.f)},
    {Vector3(0.5f, -0.5f, 0.f), Vector3(0.5f, -0.5f + LAYER_HEIGHTS[0], 0.f)},
    // mid-bottom layer
    {Vector3(-0.5f, -0.5f, 0.01f), Vector3(-0.5f, -0.5f, 0.01f)},
    {Vector3(0.5f, -0.5f, 0.01f), Vector3(0.5f, -0.5f, 0.01f)},
    {Vector3(-0.5f, -0.5f, 0.01f), Vector3(-0.5f, -0.5f + LAYER_HEIGHTS[1], 0.01f)},
    {Vector3(0.5f, -0.5f, 0.01f), Vector3(0.5f, -0.5f + LAYER_HEIGHTS[1], 0.01f)},
    // middle layer
    {Vector3(-0.5f, -0.5f, 0.02f), Vector3(-0.5f, -0.5f, 0.02f)},
    {Vector3(0.5f, -0.5f, 0.02f), Vector3(0.5f, -0.5f, 0.02f)},
    {Vector3(-0.5f, -0.5f, 0.02f), Vector3(-0.5f, -0.5f + LAYER_HEIGHTS[2], 0.02f)},
    {Vector3(0.5f, -0.5f, 0.02f), Vector3(0.5f, -0.5f + LAYER_HEIGHTS[2], 0.02f)},
    // mid-top layer
    {Vector3(-0.5f, -0.5f, 0.03f), Vector3(-0.5f, -0.5f, 0.03f)},
    {Vector3(0.5f, -0.5f, 0.03f), Vector3(0.5f, -0.5f, 0.03f)},
    {Vector3(-0.5f, -0.5f, 0.03f), Vector3(-0.5f, -0.5f + LAYER_HEIGHTS[3], 0.03f)},
    {Vector3(0.5f, -0.5f, 0.03f), Vector3(0.5f, -0.5f + LAYER_HEIGHTS[3], 0.03f)},
    // top layer
    {Vector3(-0.5f, -0.5f, 0.04f), Vector3(-0.5f, -0.5f, 0.04f)},
    {Vector3(0.5f, -0.5f, 0.04f), Vector3(0.5f, -0.5f, 0.04f)},
    {Vector3(-0.5f, -0.5f, 0.04f), Vector3(-0.5f, -0.5f + LAYER_HEIGHTS[4], 0.04f)},
    {Vector3(0.5f, -0.5f, 0.04f), Vector3(0.5f, -0.5f + LAYER_HEIGHTS[4], 0.04f)}};
  Property::Map vertexFormat;
  vertexFormat["aPosition1"] = Property::VECTOR3;
  vertexFormat["aPosition2"] = Property::VECTOR3;
  VertexBuffer vertices      = VertexBuffer::New(vertexFormat);
  vertices.SetData(vertexData, 20u);

  unsigned short indexData[30] = {0, 3, 1, 0, 2, 3, 4, 7, 5, 4, 6, 7, 8, 11, 9, 8, 10, 11, 12, 15, 13, 12, 14, 15, 16, 19, 17, 16, 18, 19};

  Geometry meshGeometry = Geometry::New();
  meshGeometry.AddVertexBuffer(vertices);
  meshGeometry.SetIndexBuffer(indexData, sizeof(indexData) / sizeof(indexData[0]));

  // Create the shader
  Shader shader = Shader::New(SHADER_BOUNCING_EFFECT_MESH_SHADER_VERT, SHADER_BOUNCING_EFFECT_MESH_SHADER_FRAG, Shader::Hint::NONE, "BOUNCING_EFFECT");

  // Create renderer
  Renderer renderer = Renderer::New(meshGeometry, shader);

  // Create actor
  Actor meshActor = Actor::New();
  meshActor.AddRenderer(renderer);

  // Register property
  bouncePropertyIndex = meshActor.RegisterProperty("uBounceCoefficient", 0.f);

  return meshActor;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
