/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include "bubble-actor.h"

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

BubbleActor::BubbleActor( unsigned int numberOfBubble,
                          const Vector2& movementArea)
: mActor(),
  mRenderer(),
  mMovementArea( movementArea ),
  mIndicesOffset(),
  mIndiceStartEndPos(),
  mIndicesPercentage(),
  mIndexGravity( Property::INVALID_INDEX ),
  mIndexDynamicScale( Property::INVALID_INDEX ),
  mIndexInvertedMovementArea( Property::INVALID_INDEX ),
  mNumBubble( numberOfBubble ),
  mRandomSeed( 0 )
{
  mActor = Actor::New();
}

void BubbleActor::MakeRenderable( Geometry geometry, TextureSet textureSet, Shader shader  )
{
  if( mRenderer )
  {
    // This function is supposed to be called once
    return;
  }

  mRenderer = Renderer::New( geometry, shader );
  mRenderer.SetTextures( textureSet );

  mActor.AddRenderer( mRenderer );
  mActor.SetSize( mMovementArea );
  mActor.SetParentOrigin(ParentOrigin::TOP_LEFT);

  // register uniforms
  mIndexGravity = mActor.RegisterProperty( "uGravity", 50.f );
  mIndexDynamicScale = mActor.RegisterProperty( "uDynamicScale", 1.f );

  mIndexInvertedMovementArea = mActor.RegisterProperty( "uInvertedMovementArea", Vector2(1.f,1.f) / mMovementArea );

  mIndicesOffset.resize(9);
  int offset = mMovementArea.Length() / 10.f;

  mRandomSeed = time( NULL );

  mIndicesOffset[0] = mActor.RegisterProperty( "uOffset[0]", Vector2( 0.f,0.f ) );
  mIndicesOffset[1] = mActor.RegisterProperty( "uOffset[1]", Vector2( rand_r( &mRandomSeed ) % offset,  rand_r( &mRandomSeed ) % offset ) );
  mIndicesOffset[2] = mActor.RegisterProperty( "uOffset[2]", Vector2( rand_r( &mRandomSeed ) % offset, -rand_r( &mRandomSeed ) % offset ) );
  mIndicesOffset[3] = mActor.RegisterProperty( "uOffset[3]", Vector2(-rand_r( &mRandomSeed ) % offset,  rand_r( &mRandomSeed ) % offset ) );
  mIndicesOffset[4] = mActor.RegisterProperty( "uOffset[4]", Vector2(-rand_r( &mRandomSeed ) % offset, -rand_r( &mRandomSeed ) % offset ) );
  mIndicesOffset[5] = mActor.RegisterProperty( "uOffset[5]", Vector2( rand_r( &mRandomSeed ) % offset, 0.f ) );
  mIndicesOffset[6] = mActor.RegisterProperty( "uOffset[6]", Vector2(-rand_r( &mRandomSeed ) % offset, 0.f ) );
  mIndicesOffset[7] = mActor.RegisterProperty( "uOffset[7]", Vector2( 0.f,  rand_r( &mRandomSeed ) % offset ) );
  mIndicesOffset[8] = mActor.RegisterProperty( "uOffset[8]", Vector2( 0.f, -rand_r( &mRandomSeed ) % offset ) );

  Vector4 zeroVector;
  mIndiceStartEndPos.resize( mNumBubble );
  mIndicesPercentage.resize( mNumBubble );
  for( unsigned int i=0; i<mNumBubble; i++ )
  {
    std::ostringstream ossProperty;
    ossProperty<< "uStartEndPosition["<< i << "]";
    mIndiceStartEndPos[i] = mActor.RegisterProperty( ossProperty.str(), zeroVector );

    ossProperty.str("");
    ossProperty<< "uPercentage["<< i << "]";
    mIndicesPercentage[i] = mActor.RegisterProperty( ossProperty.str(), 0.f );
  }
}

Actor BubbleActor::GetMeshActor()
{
  return mActor;
}

void BubbleActor::SetGeometry( Geometry geometry )
{
  mRenderer.SetGeometry( geometry );
}

void BubbleActor::SetMovementArea( const Vector2& movementArea )
{
  if( movementArea == mMovementArea)
  {
    return;
  }

  mMovementArea = movementArea;
  mActor.SetSize( mMovementArea );
  mActor.SetProperty( mIndexInvertedMovementArea, Vector2(1.f,1.f) / mMovementArea );

  int offset = mMovementArea.Length() / 10.f;

  mActor.SetProperty( mIndicesOffset[1], Vector2( rand_r( &mRandomSeed ) % offset,  rand_r( &mRandomSeed ) % offset ) );
  mActor.SetProperty( mIndicesOffset[2], Vector2( rand_r( &mRandomSeed ) % offset, -rand_r( &mRandomSeed ) % offset ) );
  mActor.SetProperty( mIndicesOffset[3], Vector2(-rand_r( &mRandomSeed ) % offset,  rand_r( &mRandomSeed ) % offset ) );
  mActor.SetProperty( mIndicesOffset[4], Vector2(-rand_r( &mRandomSeed ) % offset, -rand_r( &mRandomSeed ) % offset ) );
  mActor.SetProperty( mIndicesOffset[5], Vector2( rand_r( &mRandomSeed ) % offset, 0.f ) );
  mActor.SetProperty( mIndicesOffset[6], Vector2(-rand_r( &mRandomSeed ) % offset, 0.f ) );
  mActor.SetProperty( mIndicesOffset[7], Vector2( 0.f,  rand_r( &mRandomSeed ) % offset ) );
  mActor.SetProperty( mIndicesOffset[8], Vector2( 0.f, -rand_r( &mRandomSeed ) % offset ) );
}

void BubbleActor::SetStartAndEndPosition( unsigned int index, const Vector4& startAndEndPosition )
{
  mActor.SetProperty( mIndiceStartEndPos[index], startAndEndPosition );
}

void BubbleActor::SetPercentage( unsigned int index, float percentage )
{
  mActor.SetProperty( mIndicesPercentage[index], percentage );
}

void BubbleActor::SetGravity( float gravity )
{
  mActor.SetProperty( mIndexGravity, gravity );
}

void BubbleActor::SetDynamicScale( float scale )
{
  mActor.SetProperty( mIndexDynamicScale, scale );
}

Property BubbleActor::GetPercentageProperty( unsigned int index )
{
  return Property( mActor, mIndicesPercentage[index] );
}

void BubbleActor::ResetProperties()
{
  Vector4 zeroVector;
  for( unsigned int i=0; i<mNumBubble; i++ )
  {
    SetPercentage( i, 0.f);
    SetStartAndEndPosition( i, zeroVector );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
