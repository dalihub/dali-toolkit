/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-task.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/math/math-utils.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/image-visual-shader-factory.h>
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-thread.h>
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-manager.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

constexpr auto LOOP_FOREVER = -1;
constexpr auto NANOSECONDS_PER_SECOND( 1e+9 );

#if defined(DEBUG_ENABLED)
Debug::Filter* gVectorAnimationLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_VECTOR_ANIMATION" );
#endif

} // unnamed namespace

VectorAnimationTask::VectorAnimationTask( VisualFactoryCache& factoryCache, const std::string& url )
: mUrl( url ),
  mVectorRenderer(),
  mAnimationData(),
  mVectorAnimationThread( factoryCache.GetVectorAnimationManager().GetVectorAnimationThread() ),
  mConditionalWait(),
  mAnimationFinishedTrigger(),
  mPlayState( PlayState::STOPPED ),
  mStopBehavior( DevelImageVisual::StopBehavior::CURRENT_FRAME ),
  mLoopingMode( DevelImageVisual::LoopingMode::RESTART ),
  mNextFrameStartTime(),
  mFrameDurationNanoSeconds( 0 ),
  mFrameRate( 60.0f ),
  mCurrentFrame( 0 ),
  mTotalFrame( 0 ),
  mStartFrame( 0 ),
  mEndFrame( 0 ),
  mWidth( 0 ),
  mHeight( 0 ),
  mAnimationDataIndex( 0 ),
  mLoopCount( LOOP_FOREVER ),
  mCurrentLoop( 0 ),
  mForward( true ),
  mUpdateFrameNumber( false ),
  mNeedAnimationFinishedTrigger( true ),
  mAnimationDataUpdated( false ),
  mDestroyTask( false )
{
  Initialize();
}

VectorAnimationTask::~VectorAnimationTask()
{
  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::~VectorAnimationTask: destructor [%p]\n", this );
}

void VectorAnimationTask::Finalize()
{
  ConditionalWait::ScopedLock lock( mConditionalWait );

  // Release some objects in the main thread
  if( mAnimationFinishedTrigger )
  {
    mAnimationFinishedTrigger.reset();
  }

  mVectorRenderer.Finalize();

  mDestroyTask = true;
}

void VectorAnimationTask::SetRenderer( Renderer renderer )
{
  ConditionalWait::ScopedLock lock( mConditionalWait );

  mVectorRenderer.SetRenderer( renderer );

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::SetRenderer [%p]\n", this );
}

void VectorAnimationTask::SetAnimationData( const AnimationData& data )
{
  ConditionalWait::ScopedLock lock( mConditionalWait );

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::SetAnimationData [%p]\n", this );

  uint32_t index = mAnimationDataIndex == 0 ? 1 : 0;  // Use the other buffer

  mAnimationData[index] = data;
  mAnimationDataUpdated = true;

  if( data.resendFlag & VectorAnimationTask::RESEND_SIZE )
  {
    // The size should be changed in the main thread.
    SetSize( data.width, data.height );
  }

  mVectorAnimationThread.AddTask( this );
}

void VectorAnimationTask::SetSize( uint32_t width, uint32_t height )
{
  if( mWidth != width || mHeight != height )
  {
    mVectorRenderer.SetSize( width, height );

    mWidth = width;
    mHeight = height;

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::SetSize: width = %d, height = %d [%p]\n", width, height, this );
  }
}

void VectorAnimationTask::PlayAnimation()
{
  if( mPlayState != PlayState::PLAYING )
  {
    mNeedAnimationFinishedTrigger = true;
    mUpdateFrameNumber = false;
    mPlayState = PlayState::PLAYING;

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::PlayAnimation: Play [%p]\n", this );
  }
}

void VectorAnimationTask::StopAnimation()
{
  if( mPlayState != PlayState::STOPPING )
  {
    mNeedAnimationFinishedTrigger = false;
    mPlayState = PlayState::STOPPING;

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::StopAnimation: Stop [%p]\n", this );
  }
}

void VectorAnimationTask::PauseAnimation()
{
  if( mPlayState == PlayState::PLAYING )
  {
    mPlayState = PlayState::PAUSED;

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::PauseAnimation: Pause [%p]\n", this );
  }
}

void VectorAnimationTask::SetAnimationFinishedCallback( EventThreadCallback* callback )
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  if( callback )
  {
    mAnimationFinishedTrigger = std::unique_ptr< EventThreadCallback >( callback );
  }
}

void VectorAnimationTask::SetLoopCount( int32_t count )
{
  if( mLoopCount != count )
  {
    mLoopCount = count;
    mCurrentLoop = 0;

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::SetLoopCount: [%d] [%p]\n", count, this );
  }
}

void VectorAnimationTask::SetPlayRange( const Property::Array& playRange )
{
  bool valid = false;
  uint32_t startFrame = 0, endFrame = 0;
  size_t count = playRange.Count();

  if( count >= 2 )
  {
    int32_t start = 0, end = 0;
    if( playRange.GetElementAt( 0 ).Get( start ) && playRange.GetElementAt( 1 ).Get( end ) )
    {
      startFrame = static_cast< uint32_t >( start );
      endFrame = static_cast< uint32_t >( end );
      valid = true;
    }
    else
    {
      std::string startMarker, endMarker;
      if( playRange.GetElementAt( 0 ).Get( startMarker ) && playRange.GetElementAt( 1 ).Get( endMarker ) )
      {
        if( mVectorRenderer )
        {
          uint32_t frame;   // We don't use this later
          if( mVectorRenderer.GetMarkerInfo( startMarker, startFrame, frame ) && mVectorRenderer.GetMarkerInfo( endMarker, frame, endFrame ) )
          {
            valid = true;
          }
        }
      }
    }
  }
  else if( count == 1 )
  {
    std::string marker;
    if( playRange.GetElementAt( 0 ).Get( marker ) )
    {
      if( mVectorRenderer )
      {
        mVectorRenderer.GetMarkerInfo( marker, startFrame, endFrame );
        valid = true;
      }
    }
  }

  if( !valid )
  {
    DALI_LOG_ERROR( "VectorAnimationTask::SetPlayRange: Invalid range [%p]\n", this );
    return;
  }

  // Make sure the range specified is between 0 and the total frame number
  if( startFrame < mTotalFrame && endFrame < mTotalFrame )
  {
    // If the range is not in order swap values
    if( startFrame > endFrame )
    {
      uint32_t temp = startFrame;
      startFrame = endFrame;
      endFrame = temp;
    }

    if( startFrame != mStartFrame || endFrame != mEndFrame )
    {
      mStartFrame = startFrame;
      mEndFrame = endFrame;

      // If the current frame is out of the range, change the current frame also.
      if( mStartFrame > mCurrentFrame )
      {
        mCurrentFrame = mStartFrame;
      }
      else if( mEndFrame < mCurrentFrame )
      {
        mCurrentFrame = mEndFrame;
      }

      DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::SetPlayRange: [%d, %d] [%p]\n", mStartFrame, mEndFrame, this );
    }
  }
  else
  {
    DALI_LOG_ERROR( "VectorAnimationTask::SetPlayRange: Invalid range (%d, %d) [%p]\n", startFrame, endFrame, this );
    return;
  }
}

void VectorAnimationTask::GetPlayRange( uint32_t& startFrame, uint32_t& endFrame )
{
  startFrame = mStartFrame;
  endFrame = mEndFrame;
}

void VectorAnimationTask::SetCurrentFrameNumber( uint32_t frameNumber )
{
  if( mCurrentFrame == frameNumber )
  {
    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::SetCurrentFrameNumber: Set same frame [%d] [%p]\n", frameNumber, this );
    return;
  }

  if( frameNumber >= mStartFrame && frameNumber <= mEndFrame )
  {
    mCurrentFrame = frameNumber;
    mUpdateFrameNumber = false;

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::SetCurrentFrameNumber: frame number = %d [%p]\n", mCurrentFrame, this );
  }
  else
  {
    DALI_LOG_ERROR( "Invalid frame number [%d (%d, %d)]\n", frameNumber, mStartFrame, mEndFrame );
  }
}

uint32_t VectorAnimationTask::GetCurrentFrameNumber() const
{
  return mCurrentFrame;
}

uint32_t VectorAnimationTask::GetTotalFrameNumber() const
{
  return mTotalFrame;
}

void VectorAnimationTask::GetDefaultSize( uint32_t& width, uint32_t& height ) const
{
  mVectorRenderer.GetDefaultSize( width, height );
}

void VectorAnimationTask::SetStopBehavior( DevelImageVisual::StopBehavior::Type stopBehavior )
{
  mStopBehavior = stopBehavior;

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::SetStopBehavior: stop behavor = %d [%p]\n", mStopBehavior, this );
}

void VectorAnimationTask::SetLoopingMode( DevelImageVisual::LoopingMode::Type loopingMode )
{
  mLoopingMode = loopingMode;

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::SetLoopingMode: looping mode = %d [%p]\n", mLoopingMode, this );
}

void VectorAnimationTask::GetLayerInfo( Property::Map& map ) const
{
  mVectorRenderer.GetLayerInfo( map );
}

VectorAnimationTask::UploadCompletedSignalType& VectorAnimationTask::UploadCompletedSignal()
{
  return mVectorRenderer.UploadCompletedSignal();
}

void VectorAnimationTask::Initialize()
{
  mVectorRenderer = VectorAnimationRenderer::New( mUrl );

  mTotalFrame = mVectorRenderer.GetTotalFrameNumber();

  mEndFrame = mTotalFrame - 1;

  mFrameRate = mVectorRenderer.GetFrameRate();
  mFrameDurationNanoSeconds = NANOSECONDS_PER_SECOND / mFrameRate;

  uint32_t width, height;
  mVectorRenderer.GetDefaultSize( width, height );

  SetSize( width, height );

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::Initialize: file = %s [%d frames, %f fps] [%p]\n", mUrl.c_str(), mTotalFrame, mFrameRate, this );
}

bool VectorAnimationTask::Rasterize()
{
  bool stopped = false;
  uint32_t currentFrame;

  {
    ConditionalWait::ScopedLock lock( mConditionalWait );
    if( mDestroyTask )
    {
      // The task will be destroyed. We don't need rasterization.
      return false;
    }
  }

  ApplyAnimationData();

  if( mPlayState == PlayState::PLAYING && mUpdateFrameNumber )
  {
    mCurrentFrame = mForward ? mCurrentFrame + 1 : mCurrentFrame - 1;
    Dali::ClampInPlace( mCurrentFrame, mStartFrame, mEndFrame );
  }

  currentFrame = mCurrentFrame;

  mUpdateFrameNumber = true;

  if( mPlayState == PlayState::STOPPING )
  {
    mCurrentFrame = GetStoppedFrame( mStartFrame, mEndFrame, mCurrentFrame );
    currentFrame = mCurrentFrame;
    stopped = true;
  }
  else if( mPlayState == PlayState::PLAYING )
  {
    bool animationFinished = false;

    if( currentFrame >= mEndFrame )  // last frame
    {
      if( mLoopingMode == DevelImageVisual::LoopingMode::AUTO_REVERSE )
      {
        mForward = false;
      }
      else
      {
        if( mLoopCount < 0 || ++mCurrentLoop < mLoopCount )   // repeat forever or before the last loop
        {
          mCurrentFrame = mStartFrame;
          mUpdateFrameNumber = false;
        }
        else
        {
          animationFinished = true;   // end of animation
        }
      }
    }
    else if( currentFrame == mStartFrame && !mForward )  // first frame
    {
      if( mLoopCount < 0 || ++mCurrentLoop < mLoopCount )   // repeat forever or before the last loop
      {
        mForward = true;
      }
      else
      {
        animationFinished = true;   // end of animation
      }
    }

    if( animationFinished )
    {
      if( mStopBehavior == DevelImageVisual::StopBehavior::CURRENT_FRAME )
      {
        stopped = true;
      }
      else
      {
        mPlayState = PlayState::STOPPING;
      }
    }
  }

  // Rasterize
  bool renderSuccess = false;
  if( mVectorRenderer )
  {
    renderSuccess = mVectorRenderer.Render( currentFrame );
    if( !renderSuccess )
    {
      DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::Rasterize: Rendering failed. Try again later.[%d] [%p]\n", currentFrame, this );
      mUpdateFrameNumber = false;
    }
  }

  if( stopped && renderSuccess )
  {
    mPlayState = PlayState::STOPPED;
    mForward = true;
    mCurrentLoop = 0;

    // Animation is finished
    {
      ConditionalWait::ScopedLock lock( mConditionalWait );
      if( mNeedAnimationFinishedTrigger && mAnimationFinishedTrigger )
      {
        mAnimationFinishedTrigger->Trigger();
      }
    }

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::Rasterize: Animation is finished [current = %d] [%p]\n", currentFrame, this );
  }

  bool keepAnimation = true;
  if( mPlayState == PlayState::PAUSED || mPlayState == PlayState::STOPPED )
  {
    keepAnimation = false;
  }

  return keepAnimation;
}

uint32_t VectorAnimationTask::GetStoppedFrame( uint32_t startFrame, uint32_t endFrame, uint32_t currentFrame )
{
  uint32_t frame = currentFrame;

  switch( mStopBehavior )
  {
    case DevelImageVisual::StopBehavior::FIRST_FRAME:
    {
      frame = startFrame;
      break;
    }
    case DevelImageVisual::StopBehavior::LAST_FRAME:
    {
      if( mLoopingMode == DevelImageVisual::LoopingMode::AUTO_REVERSE )
      {
        frame = startFrame;
      }
      else
      {
        frame = endFrame;
      }
      break;
    }
    case DevelImageVisual::StopBehavior::CURRENT_FRAME:
    {
      frame = currentFrame;
      break;
    }
  }

  return frame;
}

std::chrono::time_point< std::chrono::system_clock > VectorAnimationTask::CalculateNextFrameTime( bool renderNow )
{
  // std::chrono::time_point template has second parameter duration which defaults to the std::chrono::system_clock supported
  // duration. In some C++11 implementations it is a milliseconds duration, so it fails to compile unless mNextFrameStartTime
  // is casted to use the default duration.
  mNextFrameStartTime =  std::chrono::time_point_cast< std::chrono::time_point< std::chrono::system_clock >::duration >(
      mNextFrameStartTime + std::chrono::nanoseconds( mFrameDurationNanoSeconds ) );
  auto current = std::chrono::system_clock::now();
  if( renderNow || mNextFrameStartTime < current )
  {
    mNextFrameStartTime = current;
  }
  return mNextFrameStartTime;
}

std::chrono::time_point< std::chrono::system_clock > VectorAnimationTask::GetNextFrameTime()
{
  return mNextFrameStartTime;
}

void VectorAnimationTask::ApplyAnimationData()
{
  uint32_t index;

  {
    ConditionalWait::ScopedLock lock( mConditionalWait );

    if( !mAnimationDataUpdated || mAnimationData[mAnimationDataIndex].resendFlag != 0 )
    {
      // Data is not updated or the previous data is not applied yet.
      return;
    }

    mAnimationDataIndex = mAnimationDataIndex == 0 ? 1 : 0;  // Swap index
    mAnimationDataUpdated = false;

    index = mAnimationDataIndex;
  }

  if( mAnimationData[index].resendFlag & VectorAnimationTask::RESEND_LOOP_COUNT )
  {
    SetLoopCount( mAnimationData[index].loopCount );
  }

  if( mAnimationData[index].resendFlag & VectorAnimationTask::RESEND_PLAY_RANGE )
  {
    SetPlayRange( mAnimationData[index].playRange );
  }

  if( mAnimationData[index].resendFlag & VectorAnimationTask::RESEND_STOP_BEHAVIOR )
  {
    SetStopBehavior( mAnimationData[index].stopBehavior );
  }

  if( mAnimationData[index].resendFlag & VectorAnimationTask::RESEND_LOOPING_MODE )
  {
    SetLoopingMode( mAnimationData[index].loopingMode );
  }

  if( mAnimationData[index].resendFlag & VectorAnimationTask::RESEND_CURRENT_FRAME )
  {
    SetCurrentFrameNumber( mAnimationData[index].currentFrame );
  }

  if( mAnimationData[index].resendFlag & VectorAnimationTask::RESEND_PLAY_STATE )
  {
    if( mAnimationData[index].playState == DevelImageVisual::PlayState::PLAYING )
    {
      PlayAnimation();
    }
    else if( mAnimationData[index].playState == DevelImageVisual::PlayState::PAUSED )
    {
      PauseAnimation();
    }
    else if( mAnimationData[index].playState == DevelImageVisual::PlayState::STOPPED )
    {
      StopAnimation();
    }
  }

  mAnimationData[index].resendFlag = 0;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
