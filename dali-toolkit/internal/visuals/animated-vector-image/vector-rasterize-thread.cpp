/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-rasterize-thread.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/thread-settings.h>
#include <dali/integration-api/adaptors/adaptor.h>
#include <dali/integration-api/debug.h>
#include <chrono>
#include <thread>

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

inline void ResetValue( bool& updated, uint32_t& value, uint32_t newValue, ConditionalWait& conditionalWait )
{
  ConditionalWait::ScopedLock lock( conditionalWait );
  if( !updated )
  {
    value = newValue;
    updated = true;
  }
}

} // unnamed namespace

VectorRasterizeThread::VectorRasterizeThread( const std::string& url )
: mUrl( url ),
  mVectorRenderer(),
  mConditionalWait(),
  mResourceReadyTrigger(),
  mAnimationFinishedTrigger(),
  mPlayState( PlayState::STOPPED ),
  mStopBehavior( DevelImageVisual::StopBehavior::CURRENT_FRAME ),
  mLoopingMode( DevelImageVisual::LoopingMode::RESTART ),
  mFrameDurationNanoSeconds( 0 ),
  mFrameRate( 60.0f ),
  mCurrentFrame( 0 ),
  mTotalFrame( 0 ),
  mStartFrame( 0 ),
  mEndFrame( 0 ),
  mWidth( 0 ),
  mHeight( 0 ),
  mLoopCount( LOOP_FOREVER ),
  mCurrentLoop( 0 ),
  mNeedRender( false ),
  mDestroyThread( false ),
  mResourceReady( false ),
  mCurrentFrameUpdated( false ),
  mForward( true ),
  mUpdateFrameNumber( false ),
  mLogFactory( Dali::Adaptor::Get().GetLogFactory() )
{
  Initialize();
}

VectorRasterizeThread::~VectorRasterizeThread()
{
  // Stop the thread
  {
    ConditionalWait::ScopedLock lock( mConditionalWait );
    mDestroyThread = true;
    mConditionalWait.Notify( lock );

    // This should be called in the main thread to stop waiting for the dequeuable buffer.
    mVectorRenderer.StopRender();
  }

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::~VectorRasterizeThread: Join\n" );

  Join();
}

void VectorRasterizeThread::Run()
{
  SetThreadName( "VectorImageThread" );
  mLogFactory.InstallLogFunction();

  while( !mDestroyThread )
  {
    Rasterize();
  }

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::Run: End of thread\n" );
}

void VectorRasterizeThread::SetRenderer( Renderer renderer )
{
  ConditionalWait::ScopedLock lock( mConditionalWait );

  mVectorRenderer.SetRenderer( renderer );

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::SetRenderer\n" );
}

void VectorRasterizeThread::SetSize( uint32_t width, uint32_t height )
{
  if( mWidth != width || mHeight != height )
  {
    ConditionalWait::ScopedLock lock( mConditionalWait );
    mVectorRenderer.SetSize( width, height );

    mWidth = width;
    mHeight = height;

    mResourceReady = false;

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::SetSize: width = %d, height = %d\n", width, height );
  }
}

void VectorRasterizeThread::PlayAnimation()
{
  ConditionalWait::ScopedLock lock( mConditionalWait );

  if( mPlayState != PlayState::PLAYING )
  {
    mNeedRender = true;
    mUpdateFrameNumber = false;
    mPlayState = PlayState::PLAYING;
    mConditionalWait.Notify( lock );

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::PlayAnimation: Play\n" );
  }
}

void VectorRasterizeThread::StopAnimation()
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  if( mPlayState != PlayState::STOPPED && mPlayState != PlayState::STOPPING )
  {
    mNeedRender = true;
    mPlayState = PlayState::STOPPING;
    mConditionalWait.Notify( lock );

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::StopAnimation: Stop\n" );
  }
}

void VectorRasterizeThread::PauseAnimation()
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  if( mPlayState == PlayState::PLAYING )
  {
    mPlayState = PlayState::PAUSED;

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::PauseAnimation: Pause\n" );
  }
}

void VectorRasterizeThread::RenderFrame()
{
  ConditionalWait::ScopedLock lock( mConditionalWait );

  if( !mResourceReady )
  {
    mNeedRender = true;
    mConditionalWait.Notify( lock );

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::RenderFrame: Render\n" );
  }
}

void VectorRasterizeThread::SetResourceReadyCallback( EventThreadCallback* callback )
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  mResourceReadyTrigger = std::unique_ptr< EventThreadCallback >( callback );
}

void VectorRasterizeThread::SetAnimationFinishedCallback( EventThreadCallback* callback )
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  mAnimationFinishedTrigger = std::unique_ptr< EventThreadCallback >( callback );
}

void VectorRasterizeThread::SetLoopCount( int32_t count )
{
  if( mLoopCount != count )
  {
    ConditionalWait::ScopedLock lock( mConditionalWait );

    mLoopCount = count;

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::SetLoopCount: [%d]\n", count );
  }
}

void VectorRasterizeThread::SetPlayRange( uint32_t startFrame, uint32_t endFrame )
{
  // Make sure the range specified is between 0 and the total frame number
  if( ( startFrame < mTotalFrame ) && ( endFrame < mTotalFrame ) )
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
      ConditionalWait::ScopedLock lock( mConditionalWait );

      mStartFrame = startFrame;
      mEndFrame = endFrame;

      // If the current frame is out of the range, change the current frame also.
      if( mStartFrame > mCurrentFrame )
      {
        mCurrentFrame = mStartFrame;

        mCurrentFrameUpdated = true;
        mResourceReady = false;
      }
      else if( mEndFrame < mCurrentFrame )
      {
        mCurrentFrame = mEndFrame;

        mCurrentFrameUpdated = true;
        mResourceReady = false;
      }

      DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::SetPlayRangeInFrame: [%d, %d]\n", mStartFrame, mEndFrame );
    }
  }
}

DevelImageVisual::PlayState::Type VectorRasterizeThread::GetPlayState() const
{
  DevelImageVisual::PlayState::Type state = DevelImageVisual::PlayState::STOPPED;

  switch( mPlayState )
  {
    case PlayState::PLAYING:
    {
      state = DevelImageVisual::PlayState::PLAYING;
      break;
    }
    case PlayState::PAUSED:
    {
      state = DevelImageVisual::PlayState::PAUSED;
      break;
    }
    case PlayState::STOPPING:
    case PlayState::STOPPED:
    {
      state = DevelImageVisual::PlayState::STOPPED;
      break;
    }
  }

  return state;
}

bool VectorRasterizeThread::IsResourceReady() const
{
  return mResourceReady;
}

void VectorRasterizeThread::SetCurrentFrameNumber( uint32_t frameNumber )
{
  ConditionalWait::ScopedLock lock( mConditionalWait );

  if( frameNumber >= mStartFrame && frameNumber <= mEndFrame )
  {
    mCurrentFrame = frameNumber;
    mCurrentFrameUpdated = true;

    mResourceReady = false;

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::SetCurrentFrameNumber: frame number = %d\n", mCurrentFrame );
  }
  else
  {
    DALI_LOG_ERROR( "Invalid frame number [%d (%d, %d)]\n", frameNumber, mStartFrame, mEndFrame );
  }
}

uint32_t VectorRasterizeThread::GetCurrentFrameNumber() const
{
  return mCurrentFrame;
}

uint32_t VectorRasterizeThread::GetTotalFrameNumber() const
{
  return mTotalFrame;
}

void VectorRasterizeThread::GetDefaultSize( uint32_t& width, uint32_t& height ) const
{
  mVectorRenderer.GetDefaultSize( width, height );
}

void VectorRasterizeThread::SetStopBehavior( DevelImageVisual::StopBehavior::Type stopBehavior )
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  mStopBehavior = stopBehavior;

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::SetStopBehavior: stop behavor = %d\n", mStopBehavior );
}

void VectorRasterizeThread::SetLoopingMode( DevelImageVisual::LoopingMode::Type loopingMode )
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  mLoopingMode = loopingMode;

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::SetLoopingMode: looping mode = %d\n", mLoopingMode );
}

void VectorRasterizeThread::Initialize()
{
  mVectorRenderer = VectorAnimationRenderer::New( mUrl );

  mTotalFrame = mVectorRenderer.GetTotalFrameNumber();

  mEndFrame = mTotalFrame - 1;

  mFrameRate = mVectorRenderer.GetFrameRate();
  mFrameDurationNanoSeconds = NANOSECONDS_PER_SECOND / mFrameRate;

  uint32_t width, height;
  mVectorRenderer.GetDefaultSize( width, height );

  SetSize( width, height );

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::Initialize: file = %s [%d frames, %f fps]\n", mUrl.c_str(), mTotalFrame, mFrameRate );
}

void VectorRasterizeThread::Rasterize()
{
  bool resourceReady, stopped = false;
  uint32_t currentFrame, startFrame, endFrame;
  int32_t loopCount;

  {
    ConditionalWait::ScopedLock lock( mConditionalWait );

    if( ( mPlayState == PlayState::PAUSED || mPlayState == PlayState::STOPPED ) && !mNeedRender && !mDestroyThread )
    {
      DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::Rasterize: Wait\n" );
      mConditionalWait.Wait( lock );
    }

    if( mPlayState == PlayState::PLAYING && mUpdateFrameNumber )
    {
      mCurrentFrame = mForward ? mCurrentFrame + 1 : mCurrentFrame - 1;
    }

    resourceReady = mResourceReady;
    currentFrame = mCurrentFrame;
    startFrame = mStartFrame;
    endFrame = mEndFrame;
    loopCount = mLoopCount;

    mNeedRender = false;
    mResourceReady = true;
    mCurrentFrameUpdated = false;
    mUpdateFrameNumber = true;
  }

  auto currentFrameStartTime = std::chrono::system_clock::now();

  if( mPlayState == PlayState::STOPPING )
  {
    currentFrame = GetStoppedFrame( startFrame, endFrame, currentFrame );
    ResetValue( mCurrentFrameUpdated, mCurrentFrame, currentFrame, mConditionalWait );

    stopped = true;
  }
  else if( mPlayState == PlayState::PLAYING )
  {
    bool animationFinished = false;

    if( currentFrame >= endFrame )  // last frame
    {
      if( mLoopingMode == DevelImageVisual::LoopingMode::AUTO_REVERSE )
      {
        mForward = false;
      }
      else
      {
        if( loopCount < 0 || ++mCurrentLoop < loopCount )   // repeat forever or before the last loop
        {
          ResetValue( mCurrentFrameUpdated, mCurrentFrame, startFrame, mConditionalWait );  // If the current frame is changed in the event thread, don't overwrite it.
          mUpdateFrameNumber = false;
        }
        else
        {
          animationFinished = true;   // end of animation
        }
      }
    }
    else if( currentFrame == startFrame && !mForward )  // first frame
    {
      if( loopCount < 0 || ++mCurrentLoop < loopCount )   // repeat forever or before the last loop
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
  mVectorRenderer.Render( currentFrame );

  if( !resourceReady )
  {
    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::Rasterize: Resource ready trigger\n" );

    mResourceReadyTrigger->Trigger();
  }

  if( stopped )
  {
    mPlayState = PlayState::STOPPED;
    mForward = true;
    mCurrentLoop = 0;

    // Animation is finished
    mAnimationFinishedTrigger->Trigger();

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::Rasterize: Animation is finished\n" );
  }

  auto timeToSleepUntil = currentFrameStartTime + std::chrono::nanoseconds( mFrameDurationNanoSeconds );

#if defined(DEBUG_ENABLED)
  auto sleepDuration = std::chrono::duration_cast< std::chrono::milliseconds >( timeToSleepUntil - std::chrono::system_clock::now() );

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::Rasterize: [current = %d, sleep duration = %lld]\n", currentFrame, sleepDuration.count() );
#endif

  std::this_thread::sleep_until( timeToSleepUntil );
}

uint32_t VectorRasterizeThread::GetStoppedFrame( uint32_t startFrame, uint32_t endFrame, uint32_t currentFrame )
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

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
