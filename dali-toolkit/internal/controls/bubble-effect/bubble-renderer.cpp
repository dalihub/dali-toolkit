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

// CLASS HEADER
#include "bubble-renderer.h"

// EXTERNAL INCLUDES
#include <cmath>
#include <sstream>

// INTERNAL INCLUDES

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

BubbleRenderer::BubbleRenderer()
: mRenderer(),
  mIndicesOffset(),
  mIndiceStartEndPos(),
  mIndicesPercentage(),
  mIndexGravity( Property::INVALID_INDEX ),
  mIndexDynamicScale( Property::INVALID_INDEX ),
  mIndexInvertedMovementArea( Property::INVALID_INDEX )
{
}

void BubbleRenderer::Initialize( unsigned int numberOfBubble, const Vector2& movementArea, Geometry geometry, TextureSet textureSet, Shader shader  )
{
  if( mRenderer )
  {
    // This function is supposed to be called once
    return;
  }

  mRenderer = Renderer::New( geometry, shader );
  mRenderer.SetTextures( textureSet );

  // register uniforms
  mIndexGravity = mRenderer.RegisterProperty( "uGravity", 50.f );
  mIndexDynamicScale = mRenderer.RegisterProperty( "uDynamicScale", 1.f );

  mIndexInvertedMovementArea = mRenderer.RegisterProperty( "uInvertedMovementArea", Vector2(1.f,1.f) / movementArea );

  mIndicesOffset.resize(9);
  int offset = movementArea.Length() / 10.f;

  unsigned int seed = time( NULL );

  mIndicesOffset[0] = mRenderer.RegisterProperty( "uOffset[0]", Vector2( 0.f,0.f ) );
  mIndicesOffset[1] = mRenderer.RegisterProperty( "uOffset[1]", Vector2( rand_r( &seed ) % offset,  rand_r( &seed ) % offset ) );
  mIndicesOffset[2] = mRenderer.RegisterProperty( "uOffset[2]", Vector2( rand_r( &seed ) % offset, -rand_r( &seed ) % offset ) );
  mIndicesOffset[3] = mRenderer.RegisterProperty( "uOffset[3]", Vector2(-rand_r( &seed ) % offset,  rand_r( &seed ) % offset ) );
  mIndicesOffset[4] = mRenderer.RegisterProperty( "uOffset[4]", Vector2(-rand_r( &seed ) % offset, -rand_r( &seed ) % offset ) );
  mIndicesOffset[5] = mRenderer.RegisterProperty( "uOffset[5]", Vector2( rand_r( &seed ) % offset, 0.f ) );
  mIndicesOffset[6] = mRenderer.RegisterProperty( "uOffset[6]", Vector2(-rand_r( &seed ) % offset, 0.f ) );
  mIndicesOffset[7] = mRenderer.RegisterProperty( "uOffset[7]", Vector2( 0.f,  rand_r( &seed ) % offset ) );
  mIndicesOffset[8] = mRenderer.RegisterProperty( "uOffset[8]", Vector2( 0.f, -rand_r( &seed ) % offset ) );

  Vector4 zeroVector;
  mIndiceStartEndPos.resize( numberOfBubble );
  mIndicesPercentage.resize( numberOfBubble );
  for( unsigned int i=0; i<numberOfBubble; i++ )
  {
    std::ostringstream ossProperty;
    ossProperty<< "uStartEndPosition["<< i << "]";
    mIndiceStartEndPos[i] = mRenderer.RegisterProperty( ossProperty.str(), zeroVector );

    ossProperty.str("");
    ossProperty<< "uPercentage["<< i << "]";
    mIndicesPercentage[i] = mRenderer.RegisterProperty( ossProperty.str(), 0.f );
  }
}

Renderer& BubbleRenderer::GetRenderer()
{
  return mRenderer;
}

void BubbleRenderer::SetGeometry( Geometry geometry )
{
  mRenderer.SetGeometry( geometry );
}

void BubbleRenderer::SetStartAndEndPosition( unsigned int index, const Vector4& startAndEndPosition )
{
  mRenderer.SetProperty( mIndiceStartEndPos[index], startAndEndPosition );
}

void BubbleRenderer::SetPercentage( unsigned int index, float percentage )
{
  mRenderer.SetProperty( mIndicesPercentage[index], percentage );
}

void BubbleRenderer::SetGravity( float gravity )
{
  mRenderer.SetProperty( mIndexGravity, gravity );
}

void BubbleRenderer::SetDynamicScale( float scale )
{
  mRenderer.SetProperty( mIndexDynamicScale, scale );
}

Property BubbleRenderer::GetPercentageProperty( unsigned int index )
{
  return Property( mRenderer, mIndicesPercentage[index] );
}

void BubbleRenderer::ResetProperties()
{
  Vector4 zeroVector;
  for( unsigned int i=0; i<mIndicesPercentage.size(); i++ )
  {
    SetPercentage( i, 0.f);
    SetStartAndEndPosition( i, zeroVector );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
