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
#include "cube-transition-effect-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/common/stage.h>
#include <dali/public-api/images/buffer-image.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/render-tasks/render-task-list.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::CubeTransitionEffect, Dali::BaseHandle, NULL );

DALI_SIGNAL_REGISTRATION( CubeTransitionEffect, "transition-completed", SIGNAL_TRANSITION_COMPLETED )

DALI_TYPE_REGISTRATION_END()

}

const Vector4 CubeTransitionEffect::FULL_BRIGHTNESS( 1.0f, 1.0f, 1.0f, 1.0f );
const Vector4 CubeTransitionEffect::HALF_BRIGHTNESS( 0.5f, 0.5f, 0.5f, 1.0f );

CubeTransitionEffect::CubeTransitionEffect( unsigned int numRows, unsigned int numColumns, Size viewAreaSize )
: mNumRows( numRows ),
  mNumColumns( numColumns ),
  mViewAreaSize( viewAreaSize ),
  mRotateIndex( 0 ),
  mContainerIndex( 0 ),
  mChangeTurningDirection( false ),
  mIsToNextImage( true ),
  mIsImageLoading( false ),
  mAnimationDuration( 1.f ),
  mIsAnimating( false ),
  mIsPaused( false ),
  mCubeDisplacement( 0.f ),
  mFirstTransition( true ),
  mBufferIndex( 0 )
{
}

CubeTransitionEffect::~CubeTransitionEffect()
{
}

void CubeTransitionEffect::Initialize()
{
  //create root actor for the cube transition effect, only visible during the transition
  mRoot = Actor::New();
  mRoot.SetParentOrigin( ParentOrigin::CENTER );
  mRoot.SetAnchorPoint( AnchorPoint::CENTER );
  mRoot.SetVisible(false);

  // create two groups of tiles,
  // and one group of actors (cubes) serving as parents of every two tiles (one from each image).
  unsigned int totalNum = mNumColumns* mNumRows;
  mBoxes.resize( totalNum );
  mTiles[0].resize( totalNum );
  mTiles[1].resize( totalNum );
  mTileSize = Vector2( mViewAreaSize.width / mNumColumns, mViewAreaSize.height / mNumRows );
  const Vector3 basePosition( (-mViewAreaSize.width + mTileSize.width) * 0.5f,
                              (-mViewAreaSize.height + mTileSize.height) * 0.5f,
                              -mTileSize.width * 0.5f );

  Image placeHolder = BufferImage::WHITE();
  for( unsigned int y = 0; y < mNumRows; y++ )
  {
    float positionY = y * mTileSize.height + basePosition.y;
    for( unsigned int x = 0; x < mNumColumns; x++)
    {
      unsigned int idx = y*mNumColumns + x;
      Actor actor( Actor::New() );
      mBoxes[idx] = actor;
      actor.SetParentOrigin( ParentOrigin::CENTER );
      actor.SetAnchorPoint( AnchorPoint::CENTER );
      actor.SetPosition( x * mTileSize.width + basePosition.x,
                         positionY,
                         basePosition.z );
      mRoot.Add( actor );

      mTiles[ 0 ][idx] = CreateTile( placeHolder, FULL_BRIGHTNESS );
      actor.Add( mTiles[ 0 ][idx] );

      mTiles[ 1 ][idx] = CreateTile( placeHolder, HALF_BRIGHTNESS );
      actor.Add( mTiles[ 1 ][idx] );
    }
  }

  // helper actor to create a off-screen image using shader effect
  mEmptyImage = ImageActor::New( placeHolder );
  mEmptyImage.SetSize(Stage::GetCurrent().GetSize());
  mEmptyImage.SetParentOrigin( ParentOrigin::CENTER );
  mEmptyImage.SetAnchorPoint( AnchorPoint::CENTER );
  mFullImageCreator = FullAreaImageCreator::New();
  mEmptyImage.SetShaderEffect( mFullImageCreator );
  Stage::GetCurrent().Add(mEmptyImage);

  // set up off-screen render task
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  mOffScreenTask = taskList.CreateTask();
  mOffScreenTask.SetSourceActor(mEmptyImage);
  mOffScreenTask.SetExclusive(true);
  mOffScreenBuffer[0] = FrameBufferImage::New(mViewAreaSize.x, mViewAreaSize.y);
  mOffScreenBuffer[1] = FrameBufferImage::New(mViewAreaSize.x, mViewAreaSize.y);
  mOffScreenTask.SetTargetFrameBuffer(mOffScreenBuffer[mBufferIndex]);
  mOffScreenTask.SetRefreshRate(RenderTask::REFRESH_ONCE);

  OnInitialize();
}

ImageActor CubeTransitionEffect::CreateTile( Image image, const Vector4& color )
{
  ImageActor tile = ImageActor::New( image );
  tile.SetParentOrigin( ParentOrigin::CENTER );
  tile.SetAnchorPoint( AnchorPoint::CENTER );
  tile.SetSize( mTileSize );
  tile.SetColorMode( Dali::USE_OWN_COLOR );
  tile.SetColor( color );

  return tile;
}

void CubeTransitionEffect::SetTransitionDuration( float duration )
{
  mAnimationDuration = duration;
}

float CubeTransitionEffect::GetTransitionDuration( ) const
{
  return mAnimationDuration;
}

void CubeTransitionEffect::SetCubeDisplacement( float displacement )
{
  mCubeDisplacement = displacement;
}

float CubeTransitionEffect::GetCubeDisplacement() const
{
  return mCubeDisplacement;
}

Actor CubeTransitionEffect::GetRoot()
{
  return mRoot;
}

bool CubeTransitionEffect::IsTransiting()
{
  return mIsImageLoading || mIsAnimating;
}

void CubeTransitionEffect::SetCurrentImage( ImageActor imageActor )
{
  mContainerIndex = std::abs(mRotateIndex) % 2;
  SetImage( imageActor );
}

void CubeTransitionEffect::SetTargetImage( ImageActor imageActor )
{
  mContainerIndex = std::abs( mRotateIndex+1 ) % 2;
  SetImage( imageActor );
}

void CubeTransitionEffect::SetImage( ImageActor imageActor )
{
  mCurrentImage = imageActor;

  Image image = imageActor.GetImage();
  ResourceImage resourceImage = ResourceImage::DownCast( image );
  mBufferIndex = mBufferIndex^1;

  //must make sure the image is already loaded before using its attributes
  if( resourceImage && resourceImage.GetLoadingState() != ResourceLoadingSucceeded )
  {
    mIsImageLoading = true;
    resourceImage.LoadingFinishedSignal().Connect( this, &CubeTransitionEffect::OnImageLoaded );
  }
  else
  {
    mIsImageLoading = false;
    PrepareTiles( image );
  }
}

void CubeTransitionEffect::StartTransition( bool toNextImage )
{
  if( toNextImage )
  {
    StartTransition( Vector2( mViewAreaSize.width, mViewAreaSize.height*0.5f ), Vector2( -10.f, 0.f ) );
  }
  else
  {
    StartTransition( Vector2( 0, mViewAreaSize.height*0.5f ), Vector2( 10.f, 0.f ));
  }
}

void CubeTransitionEffect::StartTransition( Vector2 panPosition, Vector2 panDisplacement )
{
  mRoot.SetVisible( true );
  mCurrentImage.SetVisible( false );
  bool toNextImage = ( panDisplacement.x < 0 ) ? true : false;
  if( mIsToNextImage != toNextImage )
  {
    mChangeTurningDirection = true;
  }
  else
  {
    mChangeTurningDirection = false;
  }
  mIsToNextImage = toNextImage;

  if( mIsToNextImage )
  {
    mRotateIndex += 1.f;
  }
  else
  {
    mRotateIndex -= 1.f;
  }

  if(mAnimation)
  {
    mAnimation.Clear();
    mAnimation.Reset();
  }
  mAnimation = Animation::New( mAnimationDuration );
  mAnimation.FinishedSignal().Connect(this, &CubeTransitionEffect::OnTransitionFinished);

  OnStartTransition( panPosition, panDisplacement );
}

void CubeTransitionEffect::PauseTransition()
{
  if( mIsAnimating && !mIsPaused )
  {
    mAnimation.Pause();
    mIsPaused = true;
  }
}

void CubeTransitionEffect::ResumeTransition()
{
  if( mIsAnimating && mIsPaused)
  {
    mAnimation.Play();
    mIsPaused = false;
  }
}

void CubeTransitionEffect::StopTransition()
{
  if( mIsAnimating )
  {
    mAnimation.Clear();
    mAnimation.Reset();
    mIsPaused = false;

    //reset the position of the cubes
    //reset the color of the tiles
    //all these status should be the same as the final state when the transition animation is finished completely
    const Vector3 basePosition( (-mViewAreaSize.width + mTileSize.width) * 0.5f,
                                (-mViewAreaSize.height + mTileSize.height) * 0.5f,
                                 -mTileSize.width * 0.5f );
    unsigned int anotherIndex = mContainerIndex^1;
    for( unsigned int y = 0; y < mNumRows; y++ )
    {
      float positionY = y * mTileSize.height + basePosition.y;
      for( unsigned int x = 0; x < mNumColumns; x++)
      {
        unsigned int idx = y*mNumColumns + x;
        mBoxes[idx].SetPosition( x * mTileSize.width + basePosition.x,
                                 positionY,
                                 basePosition.z );
        mTiles[mContainerIndex][idx].SetColor( FULL_BRIGHTNESS );
        mTiles[anotherIndex][idx].SetColor( HALF_BRIGHTNESS);
      }
    }

    // reset the rotation of the cubes, which is different process for different derived classes
    OnStopTransition();

    mRoot.SetVisible(false);
    mCurrentImage.SetVisible(true);
    mIsAnimating = false;
    mFirstTransition = false;
  }
}

void CubeTransitionEffect::OnImageLoaded(ResourceImage image)
{
  mIsImageLoading = false;
  PrepareTiles( image );
}

/**
 * Set sub-image to each tile.
 * @param[in] image The image content of the imageActor for transition
 */
void CubeTransitionEffect::PrepareTiles( Image image )
{
  // Fit the image to view area, while keeping the aspect; FitKeepAspectRatio(imageSize, viewAreaSize)
  float scale = std::min(  mViewAreaSize.width / image.GetWidth(), mViewAreaSize.height / image.GetHeight() );
  Vector2 imageSize(image.GetWidth()*scale, image.GetHeight()*scale);

  mFullImageCreator.SetEffectImage(image);
  mFullImageCreator.SetRegionSize(mViewAreaSize, imageSize);

  mOffScreenTask.SetTargetFrameBuffer(mOffScreenBuffer[mBufferIndex]);
  mOffScreenTask.SetRefreshRate(RenderTask::REFRESH_ONCE);

  ImageActor::PixelArea pixelArea( 0, 0, mViewAreaSize.x / mNumColumns, mViewAreaSize.y / mNumRows);

  for( unsigned int y = 0; y < mNumRows; y++ )
  {
    pixelArea.y = y * pixelArea.height;
    for( unsigned int x = 0; x < mNumColumns; x++)
    {
      pixelArea.x = x * pixelArea.width;
      unsigned int idx = y*mNumColumns + x;
      mTiles[mContainerIndex][idx].SetImage( mOffScreenBuffer[mBufferIndex]);
      mTiles[mContainerIndex][idx].SetPixelArea( pixelArea );
    }
  }
}


void CubeTransitionEffect::OnTransitionFinished(Animation& source)
{
  mRoot.SetVisible(false);
  mCurrentImage.SetVisible(true);
  mIsAnimating = false;
  mFirstTransition = false;

  //Emit signal
  Toolkit::CubeTransitionEffect handle( this );
  mTransitionCompletedSignal.Emit( handle, mCurrentImage );
}

Toolkit::CubeTransitionEffect::TransitionCompletedSignalType& CubeTransitionEffect::TransitionCompletedSignal()
{
  return mTransitionCompletedSignal;
}

bool CubeTransitionEffect::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::CubeTransitionEffect cubeTransitionEffect = Toolkit::CubeTransitionEffect::DownCast( handle );

  if( 0 == strcmp( signalName.c_str(), SIGNAL_TRANSITION_COMPLETED ) )
  {
    cubeTransitionEffect.TransitionCompletedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
