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
#include "cube-transition-cross-effect-impl.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

CubeTransitionCrossEffect::CubeTransitionCrossEffect( unsigned int numRows, unsigned int numColumns )
: CubeTransitionEffect( numRows, numColumns ),
  mDisplacementSpreadFactor( 0.008f )
{
}

Toolkit::CubeTransitionCrossEffect CubeTransitionCrossEffect::New(unsigned int numRows, unsigned int numColumns )
{
  // Create the implementation
  IntrusivePtr< CubeTransitionCrossEffect > internalCubeTransEffect = new CubeTransitionCrossEffect( numRows, numColumns );

  // Pass ownership to CustomActor handle
  Toolkit::CubeTransitionCrossEffect cubeTransEffect( *internalCubeTransEffect );

  //Initialization
  internalCubeTransEffect->Initialize();

  return cubeTransEffect;
}

void CubeTransitionCrossEffect::OnInitialize()
{
  unsigned int idx;
  for( unsigned int y = 0; y < mRows; y++ )
  {
    for( unsigned int x = y % 2; x < mColumns; x += 2 )
    {
      idx = y * mColumns + x;
      SetTargetTop( idx );
    }
    for( unsigned int x = ( y + 1 ) % 2; x < mColumns; x += 2 )
    {
      idx = y * mColumns + x;
      SetTargetRight( idx );
    }
  }
}

void CubeTransitionCrossEffect::OnStartTransition( Vector2 panPosition, Vector2 panDisplacement )
{
  float angle = Math::PI_2;
  unsigned int idx;

  if( panDisplacement.x < 0 )
  {
    for( unsigned int y = 0; y < mRows; y++ )
    {
      for( unsigned int x = y % 2; x < mColumns; x += 2 )
      {
        idx = y * mColumns + x;
        SetTargetTop( idx );
      }
      for( unsigned int x = ( y + 1 ) % 2; x < mColumns; x += 2 )
      {
        idx = y * mColumns + x;
        SetTargetRight( idx );
      }
    }
  }
  else
  {
    angle = -angle;

    for( unsigned int y = 0; y < mRows; y++ )
    {
      for( unsigned int x = y % 2; x < mColumns; x += 2 )
      {
        idx = y * mColumns + x;
        SetTargetBottom( idx );
      }
      for( unsigned int x = ( y + 1 ) % 2; x < mColumns; x += 2 )
      {
        idx = y * mColumns + x;
        SetTargetLeft( idx );
      }
    }
  }

  const Vector2& size = Self().GetCurrentSize().GetVectorXY();
  Vector2 halfSize = size * 0.5f;
  //the centre to "explode" the tiles outwards from
  Vector3 centre( halfSize.x, halfSize.y, -1.0f / mDisplacementSpreadFactor );

  for( unsigned int y = 0; y < mRows; y++ )
  {
    for( unsigned int x = y%2; x < mColumns; x=x+2) // rotate vertically
    {
      idx = y*mColumns + x;
      SetupAnimation( idx, x, y, -angle, Vector3::XAXIS, centre );
    }
    for( unsigned int x = (y+1)%2; x < mColumns; x=x+2) // rotate horizontally
    {
      idx = y*mColumns + x;
      SetupAnimation( idx, x, y, angle, Vector3::YAXIS, centre );
    }
  }

  mAnimation.Play();
  mIsAnimating = true;
}

void CubeTransitionCrossEffect::SetupAnimation( unsigned int actorIndex, unsigned int x, unsigned int y, float angle, const Vector3 axis, const Vector3& displacementCentre )
{
  const Vector2& size = Self().GetCurrentSize().GetVectorXY();
  Vector2 halfSize = size * 0.5f;

  //the position of the centre of the front face tile
  Vector3 position( halfSize.x * (2.0f * x + 1.0f) / mColumns, halfSize.y * (2.0f * y + 1.0f ) / mRows, 0.0f );

  Vector3 direction = position - displacementCentre;
  float length = direction.Length();
  direction.Normalize();

  float deltaLength = mCubeDisplacement / direction.z; //the length along the direction vector such that the projected direction onto the z axis is equal to mCubeDisplacement

  Vector3 newPosition = ( direction * (length + deltaLength ) ) + displacementCentre;
  Vector3 newLocalPosition = newPosition - position;

  mAnimation.AnimateTo( Property( mBoxes[ actorIndex ], Actor::Property::ORIENTATION ), Quaternion( Radian( -angle ), axis ), AlphaFunction::EASE_IN_OUT_SINE );
  mAnimation.AnimateTo( Property( mBoxes[ actorIndex ], Actor::Property::POSITION ), newLocalPosition, AlphaFunction::BOUNCE );

  mAnimation.AnimateTo( Property( mCurrentTiles[ actorIndex ], Actor::Property::COLOR ), HALF_BRIGHTNESS, AlphaFunction::EASE_OUT );
  mAnimation.AnimateTo( Property( mTargetTiles[ actorIndex ], Actor::Property::COLOR ), FULL_BRIGHTNESS, AlphaFunction::EASE_IN );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
