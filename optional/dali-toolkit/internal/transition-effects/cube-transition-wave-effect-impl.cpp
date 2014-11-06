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
#include "cube-transition-wave-effect-impl.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

CubeTransitionWaveEffect::CubeTransitionWaveEffect( unsigned int numRows, unsigned int numColumns, Size viewAreaSize )
: CubeTransitionEffect( numRows, numColumns, viewAreaSize),
  mSaddleAA( 1.f ),
  mSaddleBB( 1.f ),
  mSaddleB( 1.f )
{
}

Toolkit::CubeTransitionWaveEffect CubeTransitionWaveEffect::New(unsigned int numRows, unsigned int numColumns, Size viewAreaSize)
{
  // Create the implementation
  CubeTransitionWaveEffect* internalCubeTransEffect = new CubeTransitionWaveEffect( numRows, numColumns, viewAreaSize );

  // Pass ownership to CustomActor handle
  Toolkit::CubeTransitionWaveEffect cubeTransEffect( internalCubeTransEffect );

  //Initialization
  internalCubeTransEffect->Initialize();

  return cubeTransEffect;
}

void CubeTransitionWaveEffect::OnInitialize()
{
  float offset = -mTileSize.width * 0.5f;
  unsigned int totalNum = mNumColumns* mNumRows;
  for( unsigned int idx = 0; idx < totalNum; idx++ )
  {
    mTiles[ 0 ][idx].SetZ( -offset );
    mTiles[ 1 ][idx].SetX( offset );
  }
}

void CubeTransitionWaveEffect::OnStartTransition( Vector2 panPosition, Vector2 panDisplacement )
{
  float direc = mIsToNextImage ? 1.f : -1.f;
  CalculateSaddleSurfaceParameters( panPosition, panDisplacement*direc );

  float angle = mRotateIndex * 90.0f ;
  Vector3 translation = mTiles[mContainerIndex][ 0 ].GetCurrentPosition()*(-2.f);

  unsigned int idx;
  unsigned int totalNum = mNumColumns* mNumRows;
  if( mFirstTransition && (!mIsToNextImage) ) // the first transition is transiting to previous image
  {
    for( unsigned int idx = 0; idx < totalNum; idx++ )
    {
      mTiles[mContainerIndex][idx].SetRotation( Degree( angle),  Vector3::YAXIS );
    }
  }
  else if(!mChangeTurningDirection)  // reset rotation, translation
  {
    for( unsigned int idx = 0; idx < totalNum; idx++ )
    {
      mTiles[mContainerIndex][idx].MoveBy( translation );
      mTiles[mContainerIndex][idx].SetRotation( Degree( angle),  Vector3::YAXIS );
    }
  }

  float thirdAnimationDuration = mAnimationDuration / 3.f;
  unsigned int anotherIndex = mContainerIndex^1;

  for( unsigned int y = 0; y < mNumRows; y++ )
  {
    for( unsigned int x = 0; x < mNumColumns; x++)
    {
      idx = y*mNumColumns + x;
      // the delay value is within 0.f ~ 2.f*thirdAnimationDuration
      float delay = thirdAnimationDuration * CalculateDelay(x*mTileSize.width,y*mTileSize.height);

      mAnimation.RotateTo( mBoxes[idx], Degree( -angle ), Vector3::YAXIS,
                           AlphaFunctions::EaseOutSine, delay, thirdAnimationDuration );
      mAnimation.MoveBy( mBoxes[idx], Vector3(0.f,0.f,-mCubeDisplacement),
                         AlphaFunctions::Bounce, delay, thirdAnimationDuration );
      mAnimation.ColorTo( mTiles[anotherIndex][idx], HALF_BRIGHTNESS,
                          AlphaFunctions::EaseOut, delay, thirdAnimationDuration );
      mAnimation.ColorTo( mTiles[mContainerIndex][idx], FULL_BRIGHTNESS,
                          AlphaFunctions::EaseIn, delay, thirdAnimationDuration );
    }
  }

  mAnimation.Play();
  mIsAnimating = true;
}

void CubeTransitionWaveEffect::OnStopTransition()
{
  float angle = - mRotateIndex * 90.0f ;
  unsigned int totalNum = mNumRows * mNumColumns;
  for( unsigned int idx = 0; idx < totalNum; idx++ )
  {
    mBoxes[idx].SetRotation( Degree( angle ), Vector3::YAXIS );
  }
}

void  CubeTransitionWaveEffect::CalculateSaddleSurfaceParameters( Vector2 position, Vector2 displacement )
{
  // the line passes through 'position' and has the direction of 'displacement'
  float coefA, coefB, coefC; //line equation: Ax+By+C=0;
  coefA = displacement.y;
  coefB = -displacement.x;
  coefC = -displacement.y*position.x + displacement.x*position.y;

  float inversedAABB = 1.f / (coefA*coefA+coefB*coefB);
  float inversedSqrtAABB = sqrtf(inversedAABB);
  float saddleA;

  if(displacement.y > 0)
  {
    //distance from (0,0) to the line
    float distanceTopLeft =  fabsf(coefC) * inversedSqrtAABB;
    //distance from (viewAreaSize.x, viewAreaSize.y) to the line
    float distanceBottomRight = fabsf(coefA*mViewAreaSize.x+coefB*mViewAreaSize.y+coefC) * inversedSqrtAABB;
    saddleA = std::max( distanceTopLeft, distanceBottomRight );

    //foot of a perpendicular: (viewAreaSize.x,0) to the line
    float footX1 = ( coefB*coefB*mViewAreaSize.x - coefA*coefC) * inversedAABB;
    float footY1 = (-coefA*coefB*mViewAreaSize.x - coefB*coefC) * inversedAABB;
    //foot of a perpendicular: (0,viewAreaSize.y) to the line
    float footX2 = (-coefA*coefB*mViewAreaSize.y - coefA*coefC) * inversedAABB;
    float footY2 = ( coefA*coefA*mViewAreaSize.y - coefB*coefC) * inversedAABB;
    mSaddleBB = (footX1-footX2)*(footX1-footX2) + (footY1-footY2)*(footY1-footY2);
    mTranslation = Vector2(-footX2,-footY2);
  }
  else
  {
    //distance from(viewAreaSize.x,0) to the line
    float distanceTopRight = fabsf(coefA*mViewAreaSize.x+coefC) * inversedSqrtAABB;
    //distance from(0,viewAreaSize.y) to the line
    float distanceBottomLeft = fabsf(coefB*mViewAreaSize.y+coefC) * inversedSqrtAABB;
    saddleA = std::max( distanceTopRight, distanceBottomLeft );
    //foot of a perpendicular: (0,0) to the line
    float footX3 = (-coefA*coefC) * inversedAABB;
    float footY3 = (-coefB*coefC) * inversedAABB;
    //foot of a perpendicular: (viewAreaSize.x,viewAreaSize.y) to the line
    float footX4 = ( coefB*coefB*mViewAreaSize.x - coefA*coefB*mViewAreaSize.y - coefA*coefC) * inversedAABB;
    float footY4 = (-coefA*coefB*mViewAreaSize.x + coefA*coefA*mViewAreaSize.y - coefB*coefC) * inversedAABB;
    mSaddleBB = (footX3-footX4)*(footX3-footX4) + (footY3-footY4)*(footY3-footY4);
    mTranslation = Vector2(-footX3, -footY3);
  }

  mSaddleB = sqrtf(mSaddleBB);
  //prevent high curve shape
  if(mSaddleB > 2.f * saddleA)
  {
    saddleA = mSaddleB * 0.5f;
  }
  else if(mSaddleB < saddleA)
  {
    mSaddleB = saddleA;
    mSaddleBB = mSaddleB*mSaddleB;
  }
  mSaddleAA = saddleA*saddleA;
  mRotation = Vector2(-displacement.x, displacement.y);
  mRotation.Normalize();
}

float CubeTransitionWaveEffect::CalculateDelay(float x, float y)
{
  float tx = x + mTranslation.x;
  float ty = y + mTranslation.y;
  float valueX = mRotation.x * tx - mRotation.y * ty;
  float valueY = mRotation.y * tx + mRotation.x * ty;
  if(!mIsToNextImage) // to previous image
  {
    valueX = mSaddleB - valueX;
  }
  //the return value is a float number between 0.f and 2.f
  return (1.f + valueY*valueY / mSaddleAA - valueX*valueX / mSaddleBB);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
