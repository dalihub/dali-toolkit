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
#include <math.h>
#include <dali/public-api/actors/mesh-actor.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/geometry/animatable-mesh.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/public-api/math/vector3.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
// Bouncing effect is presented by stacked three layers with same color and opacity
const size_t NUM_LAYERS( 3 );
const Vector3 LAYER_HEIGHTS( 1.f, 27.f/42.f, 13.f/42.f);

// use the actor color to paint every layer
const char* MESH_FRAGMENT_SHADER =
"void main()\n"
"{\n"
"  gl_FragColor = uColor;\n"
"}\n";

// Constraint to move the vertices vertically
struct VertexPositionConstraint
{
  VertexPositionConstraint( float initialY, float range )
  : mInitialY( initialY ),
    mRange( range )
  {
  }

  void operator()( Vector3& current, const PropertyInputContainer& inputs )
  {
    current.y = mInitialY + mRange * fabsf( inputs[0]->GetFloat() );
  }

  float mInitialY;
  float mRange;
};

} // namespace Anon

Actor CreateBouncingEffectActor( Property::Index& bouncePropertyIndex )
{
  Dali::AnimatableMesh             mesh;
  Dali::MeshActor                  meshActor;

  Dali::AnimatableMesh::Faces faces;
  faces.reserve( NUM_LAYERS * 6 ); // 2 triangles per layer
  for( size_t i=0; i<NUM_LAYERS; i++ )
  {
    size_t j=i*4;
    faces.push_back(j); faces.push_back(j+3); faces.push_back(j+1);
    faces.push_back(j); faces.push_back(j+2); faces.push_back(j+3);
  }

  mesh = Dali::AnimatableMesh::New(NUM_LAYERS*4, faces); // 4 vertices per layer
  for( size_t i=0;i<NUM_LAYERS;i++ )
  {
    size_t j=i*4;
    float positionZ = 0.01f*static_cast<float>( i ); // the interval between each layer is 0.01
    mesh[j  ].SetPosition( Vector3( -0.5f, -0.5f, positionZ ) );
    mesh[j+1].SetPosition( Vector3( 0.5f, -0.5f, positionZ ) );
    mesh[j+2].SetPosition( Vector3( -0.5f, -0.5f, positionZ ) );
    mesh[j+3].SetPosition( Vector3( 0.5f, -0.5f, positionZ ) );
  }

  meshActor = Dali::MeshActor::New(mesh);

  Dali::ShaderEffect shaderEffect = Dali::ShaderEffect::New( "", MESH_FRAGMENT_SHADER,
                                                             GEOMETRY_TYPE_UNTEXTURED_MESH,
                                                             Dali::ShaderEffect::HINT_BLENDING );
  meshActor.SetShaderEffect(shaderEffect);

  // To control the movement of all vertices with one custom property
  bouncePropertyIndex = meshActor.RegisterProperty("BounceCoeffcient", 0.f);
  for( size_t i=0;i<NUM_LAYERS;i++ )
  {
    size_t j=i*4;
    Constraint constraint = Constraint::New<Vector3>( mesh, mesh.GetPropertyIndex(j+2, AnimatableVertex::Property::POSITION ), VertexPositionConstraint(-0.5f, LAYER_HEIGHTS[i]) );
    constraint.AddSource( Source(meshActor, bouncePropertyIndex) );
    constraint.Apply();

    constraint = Constraint::New<Vector3>( mesh, mesh.GetPropertyIndex(j+3,  AnimatableVertex::Property::POSITION), VertexPositionConstraint(-0.5f, LAYER_HEIGHTS[i]) );
    constraint.AddSource( Source(meshActor, bouncePropertyIndex) );
    constraint.Apply();
  }

  return meshActor;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
