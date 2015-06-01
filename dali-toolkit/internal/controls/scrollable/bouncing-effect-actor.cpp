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

// CLASS HEADER
#include <dali-toolkit/internal/controls/scrollable/bouncing-effect-actor.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/renderer.h>
#include <dali/public-api/geometry/geometry.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/object/property-buffer.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/shader-effects/material.h>
#include <dali/public-api/shader-effects/shader.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
// Bouncing effect is presented by stacked three layers with same color and opacity
const Vector3 LAYER_HEIGHTS( 1.f, 27.f/42.f, 13.f/42.f);

#define MAKE_SHADER(A)#A

// Modify the vertex position according to the bounce coefficient;
const char* MESH_VERTEX_SHADER = MAKE_SHADER(
attribute mediump vec3    aPosition1;\n
attribute mediump vec3    aPosition2;\n
uniform   mediump mat4    uMvpMatrix;\n
uniform   mediump vec3    uSize;
uniform   mediump float   uBounceCoefficient;\n
\n
void main()\n
{\n
  gl_Position = uMvpMatrix * vec4(mix( aPosition1, aPosition2, abs(uBounceCoefficient) )*uSize, 1.0);\n
}
);

// use the actor color to paint every layer
const char* MESH_FRAGMENT_SHADER = MAKE_SHADER(
uniform lowp  vec4    uColor;\n
void main()\n
{\n
  gl_FragColor = uColor;\n
}\n;
);

} // namespace Anon

Actor CreateBouncingEffectActor( Property::Index& bouncePropertyIndex )
{
  // Create the bouncing mesh geometry
  struct VertexPosition
  {
    Vector3 position1;
    Vector3 position2;
  };
  // 4 vertices 2 triangles per layer. The depth interval between each layer is 0.01
  VertexPosition vertexData[12] = {
    //bottom layer
    { Vector3( -0.5f, -0.5f, 0.f ),  Vector3( -0.5f, -0.5f, 0.f )  },
    { Vector3( 0.5f, -0.5f, 0.f ),   Vector3( 0.5f, -0.5f, 0.f )   },
    { Vector3( -0.5f, -0.5f, 0.f ),  Vector3( -0.5f, -0.5f + LAYER_HEIGHTS[0], 0.f ) },
    { Vector3( 0.5f, -0.5f, 0.f ),   Vector3( 0.5f, -0.5f+ LAYER_HEIGHTS[0], 0.f )   },
    // middle layer
    { Vector3( -0.5f, -0.5f, 0.01f ),  Vector3( -0.5f, -0.5f, 0.01f )  },
    { Vector3( 0.5f, -0.5f, 0.01f ),   Vector3( 0.5f, -0.5f, 0.01f )   },
    { Vector3( -0.5f, -0.5f, 0.01f ),  Vector3( -0.5f, -0.5f + LAYER_HEIGHTS[1], 0.01f ) },
    { Vector3( 0.5f, -0.5f, 0.01f ),   Vector3( 0.5f, -0.5f+ LAYER_HEIGHTS[1], 0.01f )   },
    // top layer
    { Vector3( -0.5f, -0.5f, 0.02f ),  Vector3( -0.5f, -0.5f, 0.02f )  },
    { Vector3( 0.5f, -0.5f, 0.02f ),   Vector3( 0.5f, -0.5f, 0.02f )   },
    { Vector3( -0.5f, -0.5f, 0.02f ),  Vector3( -0.5f, -0.5f + LAYER_HEIGHTS[2], 0.02f ) },
    { Vector3( 0.5f, -0.5f, 0.02f ),   Vector3( 0.5f, -0.5f+ LAYER_HEIGHTS[2], 0.02f )   }
  };
  Property::Map vertexFormat;
  vertexFormat["aPosition1"] = Property::VECTOR3;
  vertexFormat["aPosition2"] = Property::VECTOR3;
  PropertyBuffer vertices = PropertyBuffer::New( vertexFormat, 12u );
  vertices.SetData( vertexData );

  unsigned int indexData[18] = { 0,3,1,0,2,3,4,7,5,4,6,7,8,11,9,8,10,11 };
  Property::Map indexFormat;
  indexFormat["indices"] = Property::UNSIGNED_INTEGER;
  PropertyBuffer indices = PropertyBuffer::New( indexFormat, 18u );
  indices.SetData( indexData );

  Geometry meshGeometry = Geometry::New();
  meshGeometry.AddVertexBuffer( vertices );
  meshGeometry.SetIndexBuffer( indices );

  // Create material
  Shader shader = Shader::New( MESH_VERTEX_SHADER, MESH_FRAGMENT_SHADER );
  Material material = Material::New( shader );

  // Create renderer
  Renderer renderer = Renderer::New( meshGeometry, material );

  // Create actor
  Actor meshActor= Actor::New();
  meshActor.AddRenderer( renderer );

  // Register property
  bouncePropertyIndex = meshActor.RegisterProperty("bounce-coefficient", 0.f);
  meshActor.AddUniformMapping( bouncePropertyIndex, "uBounceCoefficient" );

  return meshActor;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
