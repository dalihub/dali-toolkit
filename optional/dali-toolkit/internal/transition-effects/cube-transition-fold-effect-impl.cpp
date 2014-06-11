//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include "cube-transition-fold-effect-impl.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{
const float CubeTransitionFoldEffect::mDisplacementRatio = 1.4142f; //sqrt(2)

CubeTransitionFoldEffect::CubeTransitionFoldEffect( unsigned int numRows, unsigned int numColumns, Size viewAreaSize )
: CubeTransitionEffect( numRows, numColumns, viewAreaSize)
{
}

Toolkit::CubeTransitionFoldEffect CubeTransitionFoldEffect::New(unsigned int numRows, unsigned int numColumns, Size viewAreaSize)
{
  // Create the implementation
  CubeTransitionFoldEffect* internalCubeTransEffect = new CubeTransitionFoldEffect( numRows, numColumns, viewAreaSize );

  // Pass ownership to CustomActor handle
  Toolkit::CubeTransitionFoldEffect cubeTransEffect( internalCubeTransEffect );

  //Initialization
  internalCubeTransEffect->Initialize();

  return cubeTransEffect;
}

void CubeTransitionFoldEffect::OnInitialize()
{
  float offset = mTileSize.width*0.5f;
  unsigned int idx;
  for( unsigned int y = 0; y < mNumRows; y++ )
  {
    idx = y*mNumColumns;
    for( unsigned int x = y%2; x < mNumColumns; x=x+2)
    {
      mTiles[0][idx+x].SetZ( offset );
      mTiles[1][idx+x].SetX( offset );
    }
    for( unsigned int x = (y+1)%2; x < mNumColumns; x=x+2)
    {
      mTiles[0][idx+x].SetZ( offset );
      mTiles[1][idx+x].SetX( -offset );
    }
  }
}

void CubeTransitionFoldEffect::OnStartTransition( Vector2 panPosition, Vector2 panDisplacement )
{
  float angle = mRotateIndex * Math::PI_2 ;
  Vector3 translation0 = mTiles[mContainerIndex][ 0 ].GetCurrentPosition()*(-2.f);
  Vector3 translation1 = mTiles[mContainerIndex][ mNumColumns ].GetCurrentPosition()*(-2.f);

  unsigned int idx;

  for( unsigned int y = 0; y < mNumRows; y++ )
  {
    for( unsigned int x = y%2; x < mNumColumns; x=x+2)
    {
      idx = y*mNumColumns + x;
      SetupAnimation( idx, -angle, translation0 );
    }
    for( unsigned int x = (y+1)%2; x < mNumColumns; x=x+2)
    {
      idx = y*mNumColumns + x;
      SetupAnimation( idx, angle, translation1 );
    }
  }

  mAnimation.Play();
  mIsAnimating = true;
}

void CubeTransitionFoldEffect::OnStopTransition()
{
  float angle = mRotateIndex * Math::PI_2 ;
  unsigned int idx;
  for( unsigned int y = 0; y < mNumRows; y++ )
  {
    idx = y*mNumColumns;
    for( unsigned int x = y%2; x < mNumColumns; x=x+2)
    {
      mBoxes[idx+x].SetRotation( Radian(angle), Vector3::YAXIS );
    }
    for( unsigned int x = (y+1)%2; x < mNumColumns; x=x+2)
    {
      mBoxes[idx+x].SetRotation( Radian(-angle), Vector3::YAXIS );
    }
  }
}

void CubeTransitionFoldEffect::SetupAnimation(unsigned int actorIndex, float angle, Vector3 resetTranslation)
{
  Actor currentCube = mBoxes[actorIndex];
  ImageActor sideTile = mTiles[mContainerIndex][actorIndex];
  ImageActor frontTile = mTiles[mContainerIndex^1][actorIndex];
  if ( mFirstTransition && (!mIsToNextImage) ) // for the first transition, it is going to previous image
  {
    sideTile.SetRotation( Radian( angle),   Vector3::YAXIS );
  }
  else if( !mChangeTurningDirection )   // reset rotation, translation and color
  {
    sideTile.MoveBy( resetTranslation );
    sideTile.SetRotation( Radian( angle),   Vector3::YAXIS );
  }
  mAnimation.RotateTo( currentCube, Radian( -angle ), Vector3::YAXIS, AlphaFunctions::Linear );
  Vector3 position(currentCube.GetCurrentPosition());
  mAnimation.MoveTo(currentCube, Vector3( position.x*mDisplacementRatio, position.y, position.z ), AlphaFunctions::Bounce);
  mAnimation.ColorTo( frontTile, HALF_BRIGHTNESS, AlphaFunctions::EaseOut );
  mAnimation.ColorTo( sideTile, FULL_BRIGHTNESS, AlphaFunctions::EaseIn );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
