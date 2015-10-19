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
#include "cube-transition-fold-effect-impl.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{
CubeTransitionFoldEffect::CubeTransitionFoldEffect( unsigned int numRows, unsigned int numColumns )
: CubeTransitionEffect( numRows, numColumns )
{
}

Toolkit::CubeTransitionFoldEffect CubeTransitionFoldEffect::New(unsigned int numRows, unsigned int numColumns )
{
  // Create the implementation
  IntrusivePtr< CubeTransitionFoldEffect > internalCubeTransEffect = new CubeTransitionFoldEffect( numRows, numColumns );

  // Pass ownership to CustomActor handle
  Toolkit::CubeTransitionFoldEffect cubeTransEffect( *internalCubeTransEffect );

  //Initialization
  internalCubeTransEffect->Initialize();

  return cubeTransEffect;
}

void CubeTransitionFoldEffect::OnInitialize()
{
  unsigned int idx;
  for( unsigned int y = 0; y < mRows; y++ )
  {
    idx = y*mColumns;
    for( unsigned int x = y%2; x < mColumns; x=x+2)
    {
      SetTargetLeft( idx + x );
    }
    for( unsigned int x = (y+1)%2; x < mColumns; x=x+2)
    {
      SetTargetRight( idx + x );
    }
  }
}

void CubeTransitionFoldEffect::OnStartTransition( Vector2 panPosition, Vector2 panDisplacement )
{
  float angle = Math::PI_2;

  unsigned int idx;
  if( panDisplacement.x < 0 )
  {
    for( unsigned int y = 0; y < mRows; y++ )
    {
      idx = y*mColumns;
      for( unsigned int x = y%2; x < mColumns; x=x+2)
      {
        SetTargetLeft( idx + x );
      }
      for( unsigned int x = (y+1)%2; x < mColumns; x=x+2)
      {
        SetTargetRight( idx + x );
      }
    }
  }
  else
  {
    angle = -angle;

    for( unsigned int y = 0; y < mRows; y++ )
    {
      idx = y*mColumns;
      for( unsigned int x = y%2; x < mColumns; x=x+2)
      {
        SetTargetRight( idx + x );
      }
      for( unsigned int x = (y+1)%2; x < mColumns; x=x+2)
      {
        SetTargetLeft( idx + x );
      }
    }
  }

  for( unsigned int y = 0; y < mRows; y++ )
  {
    idx = y*mColumns;
    for( unsigned int x = y%2; x < mColumns; x=x+2)
    {
      SetupAnimation( idx + x, x, angle );
    }
    for( unsigned int x = (y+1)%2; x < mColumns; x=x+2)
    {
      SetupAnimation( idx + x, x, -angle );
    }
  }

  mAnimation.Play();
  mIsAnimating = true;
}

void CubeTransitionFoldEffect::SetupAnimation( unsigned int actorIndex, unsigned int x, float angle )
{
  //rotate and translate the cube such that the edges remain in constant contact
  //calculate the maximum distance the cube has to move when it the box has rotated 45 degrees
  //ie distance from of centre of square to a vertex is given by:
  //  distance = width / sqrt(2)
  //therefore the delta distance the cube should move is given by:
  //  delta_distance = ( width / 2 ) - distance
  //re-arranging we get:
  //  delta_distance = ( width / 2 ) * ( sqrt(2) - 1 )
  //accumulating over the length of the row we get:
  //  delta_distance_at_x = x * delta_distance;

  float delta = (float)x * mTileSize.x * ( 1.4142f - 1.0f );

  Vector3 position( mBoxes[ actorIndex ].GetCurrentPosition() );
  mAnimation.AnimateTo( Property( mBoxes[ actorIndex ], Actor::Property::ORIENTATION ), Quaternion( Radian( angle ), Vector3::YAXIS ), AlphaFunction::LINEAR );
  mAnimation.AnimateTo( Property( mBoxes[ actorIndex ], Actor::Property::POSITION_X ), position.x + delta, AlphaFunction::BOUNCE );

  mAnimation.AnimateTo( Property( mCurrentTiles[ actorIndex ], Actor::Property::COLOR ), HALF_BRIGHTNESS, AlphaFunction::EASE_OUT );
  mAnimation.AnimateTo( Property( mTargetTiles[ actorIndex ], Actor::Property::COLOR ), FULL_BRIGHTNESS, AlphaFunction::EASE_IN );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
