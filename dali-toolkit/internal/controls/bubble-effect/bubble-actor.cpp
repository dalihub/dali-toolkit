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

namespace
{

/**
 * Register a property and map it as uniform.
 * @param[in] handle The handle to register new property.
 * @param[in] propertyName The name of the property.
 * @param[in] unifoemName The name of the uniform.
 * @param[in] value The initial value of the property.
 * @return The index of theproperty
 */
Dali::Property::Index RegisterUniform( Dali::Handle& handle,
                                       const std::string& propertyName,
                                       const std::string& uniformName,
                                       Dali::Property::Value value)
{
  Dali::Property::Index propertyIndex = handle.RegisterProperty( propertyName, value );
  handle.AddUniformMapping( propertyIndex, uniformName );

  return propertyIndex;
}

}// end LOCAL_STUFF

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

BubbleActor::BubbleActor( unsigned int numberOfBubble,
                          const Vector2& movementArea)
: mMovementArea( movementArea ),
  mNumBubble( numberOfBubble )
{
  mActor = Actor::New();
}

void BubbleActor::MakeRenderable( Geometry geometry, Material material  )
{
  if( mRenderer )
  {
    // This function is supposed to be called once
    return;
  }

  mRenderer = Renderer::New( geometry, material );

  mActor.AddRenderer( mRenderer );
  mActor.SetSize( mMovementArea );
  mActor.SetParentOrigin(ParentOrigin::TOP_LEFT);

  // register uniforms
  mIndexGravity = RegisterUniform( mActor, "gravity", "uGravity", 50.f );
  mIndexDynamicScale = RegisterUniform( mActor, "dynamic-scale", "uDynamicScale", 1.f );

  mIndexInvertedMovementArea = RegisterUniform( mActor, "inverted-movement-area", "uInvertedMovementArea", Vector2(1.f,1.f) / mMovementArea );

  srand(time(NULL));
  mIndicesOffset.resize(9);
  int offset = mMovementArea.Length() / 10.f;
  mIndicesOffset[0] = RegisterUniform(mActor, "offset-0", "offset[0]", Vector2(0.f,0.f));
  mIndicesOffset[1] = RegisterUniform(mActor, "offset-1", "offset[1]", Vector2(rand()%offset,rand()%offset) );
  mIndicesOffset[2] = RegisterUniform(mActor, "offset-2", "offset[2]", Vector2(rand()%offset,-rand()%offset) );
  mIndicesOffset[3] = RegisterUniform(mActor, "offset-3", "offset[3]", Vector2(-rand()%offset,rand()%offset) );
  mIndicesOffset[4] = RegisterUniform(mActor, "offset-4", "offset[4]", Vector2(-rand()%offset,-rand()%offset) );
  mIndicesOffset[5] = RegisterUniform(mActor, "offset-5", "offset[5]", Vector2(rand()%offset,0.f));
  mIndicesOffset[6] = RegisterUniform(mActor, "offset-6", "offset[6]", Vector2(-rand()%offset,0.f));
  mIndicesOffset[7] = RegisterUniform(mActor, "offset-7", "offset[7]", Vector2(0.f,rand()%offset));
  mIndicesOffset[8] = RegisterUniform(mActor, "offset-8", "offset[8]", Vector2(0.f,-rand()%offset));

  Vector4 zeroVector;
  mIndiceStartEndPos.resize( mNumBubble );
  mIndicesPercentage.resize( mNumBubble );
  for( unsigned int i=0; i<mNumBubble; i++ )
  {
    std::ostringstream ossProperty;
    ossProperty<< "start-end-position-"<< i;

    std::ostringstream ossUniform;
    ossUniform<< "uStartAndEndPos["<< i << "]";
    mIndiceStartEndPos[i] = RegisterUniform( mActor, ossProperty.str(), ossUniform.str(), zeroVector );

    ossProperty.str("");
    ossProperty<< "percentage-"<< i;

    ossUniform.str("");
    ossUniform<< "uPercentage["<< i << "]";
    mIndicesPercentage[i] = RegisterUniform( mActor, ossProperty.str(), ossUniform.str(), 0.f );
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
  mActor.SetProperty( mIndicesOffset[1], Vector2(rand()%offset,rand()%offset) );
  mActor.SetProperty( mIndicesOffset[2], Vector2(rand()%offset,-rand()%offset) );
  mActor.SetProperty( mIndicesOffset[3], Vector2(-rand()%offset,rand()%offset) );
  mActor.SetProperty( mIndicesOffset[4], Vector2(-rand()%offset,-rand()%offset) );
  mActor.SetProperty( mIndicesOffset[5], Vector2(rand()%offset,0.f));
  mActor.SetProperty( mIndicesOffset[6], Vector2(-rand()%offset,0.f));
  mActor.SetProperty( mIndicesOffset[7], Vector2(0.f,rand()%offset));
  mActor.SetProperty( mIndicesOffset[8], Vector2(0.f,-rand()%offset));
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
